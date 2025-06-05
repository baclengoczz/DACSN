/* $Id: cpplib.c,v 1.67 2007/01/15 23:13:25 moshtaa Exp $ */

#include "config.h"
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cpplib.h"
#include "cerror.h"
#include "strutil.h"
#include "portability.h"

/** We'll maintain a set of locations to search for system include files.
 * It's important that this list be modifiable at run-time so that the
 * command-line options and such can add to it. We'll expose an API to
 * build the set.
 *
 * Internally, we'll build it as a resizeable array. When it gets full,
 * we'll realloc() it to make more room.
 */
#define SYSINCDIR_BLOCK_SZ  128
char **sysinclude_dir = NULL;
size_t num_sysinclude_dirs = 0;


/** keep track of where the .c file is located so we can check there
 * for user include files.
 */
char *source_file_dir = NULL;

/** stream to output error messages to, in addition to stderr. NULL
 * if none. Set via the --error command line option.
 */
FILE *errfptr = NULL;

int yylex (void);

struct constant val;

/** Allocate space for a header name in blocks of this many bytes. We should
 * never need to alloc additional space on any reasonable system, but if we
 * don't do this we risk buffer overflow, and those are a Bad Thing(tm), 
 * mmmkay? Once we've got the whole thing, we'll free up any extra 
 * memory taken up by the string, so we won't be wasting huge amount of 
 * memory by this.
 *
 * It's also possible to run into this situation when there's a missing header
 * name terminator token.
 */
#define HNAME_ALLOC_BLOCK_SZ  4096
/** Allocate space for a macro body in blocks of this many bytes.
 */
#define MACRO_ALLOC_BODY_BLOCK_SZ  4096

#define MAX_PARAMS 4096
#define MAX_REPLACEMENTS 4096

/* for now, we'll just keep the macros in a list. Not efficient at all, 
 * but it'll work. We'll replace it with a hash table or some such later.
 */
struct _macro_
{
  struct macro macro;
  struct _macro_ *next;
  struct _macro_ *prev;
};
static struct _macro_ *s_macro_list = NULL;


static struct _macro_ *s_macro_find_in_list (const char *name);
static char *s_get_header_name (char term);
static void s_include_system_header (const char *hname);


static struct constant s_expr (unsigned eval);
static struct constant s_compound (unsigned eval);
static struct constant s_conditional (unsigned eval);
static struct constant s_lor (unsigned eval);
static struct constant s_land (unsigned eval);
static struct constant s_bor (unsigned eval);
static struct constant s_bxor (unsigned eval);
static struct constant s_band (unsigned eval);
static struct constant s_equality (unsigned eval);
static struct constant s_relational (unsigned eval);
static struct constant s_shift (unsigned eval);
static struct constant s_additive (unsigned eval);
static struct constant s_multiplicative (unsigned eval);
static struct constant s_unary (unsigned eval);
static struct constant s_primary (unsigned eval);
static void s_arithmetic_conversions (struct constant *c1,
                                      struct constant *c2);
static unsigned s_is_constant_zero (struct constant v);
static void s_advance (void);
static int s_match (int tok);
static int s_curtok (void);


extern char yytext[];
extern char yyfilename[];
extern long yylineno;
extern FILE *yyout;

static size_t level = 0;

/* macro definitions */
struct macro *
macro_new (const char *name)
{
  struct _macro_ *macro;
  /* if this is a duplicate of one already defined, we've got an error.
   * that should have been checked higher up. We'll assert it here for
   * consistency's sake. program by contract and all that.
   */
  assert (!macro_find (name));

  macro = calloc (1, sizeof (struct _macro_));
  assert (macro);
  macro->macro.name = malloc (strlen (name) + 1);
  assert (macro->macro.name);
  strcpy ((char *) macro->macro.name, name);
  macro->next = s_macro_list;
  if (s_macro_list)
    s_macro_list->prev = macro;
  s_macro_list = macro;
  return &macro->macro;
}

struct macro *
macro_new_simple (const char *name, const char *body, unsigned special)
{
  struct macro *macro, *prevdef;

  if ((prevdef = macro_find (name)))
    {
      /* check to see if the previous definition matches our definition.
       * this is typically seen for predefined macros redefined on the
       * command line or the same macro defined more than once on the
       * same command line.
       */
      if (prevdef->special != special
          || prevdef->has_params
          || prevdef->has_ellipsis || strcmp (prevdef->body, body))
        {
          post_error (ERR_ERROR_PREPROCMACROREDEFINITIONMISMATCH, name);
        }
      /* whether it's a valid redefinition or not, we still don't want
       * to try to insert a new copy.
       */
      return prevdef;
    }

  macro = macro_new (name);
  assert (macro);
  if (body)
    {
      macro->body = malloc (strlen (body) + 1);
      assert (macro->body);
      strcpy ((char *) macro->body, body);
    }
  macro->special = special;
  return macro;
}

void
macro_delete (const char *name)
{
  struct _macro_ *macro;
  macro = s_macro_find_in_list (name);
  /* trying to delete a non-existent macro is a no-no. Should be handled
   * higher up than this. assert our contract.
   */
  assert (macro);
  /* remove it from the list */
  if (macro->next)
    macro->next->prev = macro->prev;
  if (macro->prev)
    macro->prev->next = macro->next;
  /* handle removing the head of the list */
  if (s_macro_list == macro)
    s_macro_list = macro->next;
  free ((void *) macro->macro.name);
  free (macro);
  /* note that since we didn't alloc the space for the params and the body
   * at this level, we won't free them here either. We don't know for sure
   * they were alloced from the heap directly, and we don't know that
   * the storage for them is really going out of use.
   */
}

struct macro *
macro_find (const char *name)
{
  struct _macro_ *macro;
  macro = s_macro_find_in_list (name);
  return macro ? &macro->macro : NULL;
}

static struct _macro_ *
s_macro_find_in_list (const char *name)
{
  struct _macro_ *macro;
  for (macro = s_macro_list;
       macro && strcmp (macro->macro.name, name); macro = macro->next)
    ;
  return macro;
}


/************************************************************
 * added for letting the preprocessor to search system directories
 * to find a file that is included from commandline
 */
const char *
makeCommandLineIncFileName (const char*hname)
{
  size_t idx;
  char *buf;
  FILE *fptr;


  /* First we check the current directory, if the file is not there,
   * we'll check each of the system include locations in turn until we
   * either find a file which matches the header name or we run out
   * of places to check, in which case we'll issue an error diagnostic.
   */
  if ((fptr = fopen (hname, "rt")) != NULL)
    {
      fclose (fptr);
      return hname;
    }

  for (idx = 0; idx < num_sysinclude_dirs; idx++)
    {
      buf = malloc (strlen (hname) + strlen (sysinclude_dir[idx]) + 1);
      strcpy (buf, sysinclude_dir[idx]);
      strcat (buf, hname);
      /* If we can open it, we're done and can just set up for the new
       * include file and quit, otherwise we need to keep checking.
       */
      if ((fptr = fopen (buf, "rt")) != NULL)
        {
	  fclose(fptr);
          return buf;
        }
      free (buf);
    }
  /* If we fell all the way through, then we weren't able to find a matching
   * file, which is an error.
   */
  post_error (ERR_ERROR_PREPROCCANNOTFINDHEADERFILE, hname);

  return NULL;
}


void
cpp_lineno (void)
{
  /* output a #line */
  fprintf (yyout, "#line %ld \"%s\"\n", yylineno, yyfilename);
}

void
cpp_add_sysinclude_dir_list (const char *hnames)
{
  char *hlist;
  char *p, *dir;
  /* get a copy we can diddle with */
#if HAVE_ALLOCA
  hlist = alloca (strlen (hnames) + 2);
#else
  hlist = malloc (strlen (hnames) + 2);
#endif
  assert (hlist);
  strcpy (hlist, hnames);
  /* we want our copy to have a '\0\0' terminator, not just '\0' */
  hlist[strlen (hlist) + 1] = '\0';
  p = dir = hlist;
  /* allow either ';' or ':' as a delimiter, at least on non-windows platforms. On
   * windows, it's ';' only.
   */
  while (*dir)
    {
      if ((p = strchr (dir, ';')) != NULL
#if !defined(__NT__) && !defined(__MINGW32__)
          || (p = strchr (dir, ':')) != NULL
#endif
        )
        *p = '\0';
      cpp_add_sysinclude_dir (dir);
      dir += strlen (dir) + 1;
    }
#ifndef HAVE_ALLOCA
  free (hlist);
#endif
}


void
cpp_add_sysinclude_dir (const char *hname)
{
  char *p;
  /* First, figure out if we need to alloc more space */
  if (!sysinclude_dir
      || (num_sysinclude_dirs % SYSINCDIR_BLOCK_SZ) == 0)
    sysinclude_dir =
      realloc (sysinclude_dir,
               (((num_sysinclude_dirs / SYSINCDIR_BLOCK_SZ) + 1) * SYSINCDIR_BLOCK_SZ * sizeof (char *)));
  assert (sysinclude_dir);
  /* we'll assume that the name is *not* space that'll stick around, so
   * we need to allocate our own copy of the string.
   *
   * By convention, we will always put a trailing directory separator
   * on each directory name if it's not already there. We'll always use
   * '/' as the directory separator internally. We'll replace any '\'
   * separators with '/' for consistency. That will also allow us to
   * compile programs which use MSFT conventions on real platforms as well.
   */
  sysinclude_dir[num_sysinclude_dirs] = malloc (strlen (hname) + 2);
  assert (sysinclude_dir[num_sysinclude_dirs]);
  strcpy (sysinclude_dir[num_sysinclude_dirs++], hname);

  /* replace any '\' with '/' */
  for (p = sysinclude_dir[num_sysinclude_dirs - 1]; *p; p++)
    if (*p == '\\')
      *p = '/';
  /* add a terminating '/' if there's not one already */
  if (*(p - 1) != '/')
    *p++ = '/', *p = '\0';
}

static int
s_include_system (void)
{
  char *hname;
  /* Get the header name into a C string */
  /* the header name consists of an h-char-sequence, which is a sequence of
   * characters in the source character set which include any characters
   * except a new-line or a '>'.
   */
  hname = s_get_header_name ('>');
  if (hname)
    {
      s_include_system_header (hname);
      return 0;
    }
  return 1; /* error in hname means EOF */
}

int
include_user_file (char *hname)
{
  FILE *fptr;
  char *name;
  
  if (hname)
    {
  #if HAVE_ALLOCA      
      name = alloca (strlen (hname) + strlen (source_file_dir) + 2);
#else
      name = malloc (strlen (hname) + strlen (source_file_dir) + 2);
#endif

      /* For us, a "" style header file means check the directory in which
       * the source file is located for the header file. If it's not found 
       * there, then we check as if the file were named in a <> style 
       * include, as per the standard. We'll also try it as-is and see if
       * that works.
       */
      strcpy (name, source_file_dir);
      strcat (name, "/");
      strcat (name, hname);
      if (((fptr = fopen (name, "rt")) != NULL) 
	  || ((fptr = fopen (hname, "rt")) != NULL))
        {
          cpp_new_include_file (fptr, name);
        }
      else
        {
          /* Try it as a <> style include... */
          s_include_system_header (hname);
        }
#if ! HAVE_ALLOCA      
      free (name);
#endif
      free (hname);
      return 0;
    }
  return 1; /* error in hname means EOF */
}

static int
s_include_user (void)
{
  char *hname;
  /* Get the header name into a C string */
  /* the header name consists of a q-char-sequence, which is a sequence of
   * characters in the source character set which include any characters
   * except a new-line or a '"'. Note that this is *not* a string constant.
   */
  hname = s_get_header_name ('\"');

  return include_user_file (hname);
}

int
cpp_include (void)
{
  int c;
  switch (cpp_lex())
    {
    case '<':
      return s_include_system();
      break;
    case '\"':
      return s_include_user();
      break;
    default:
      post_error (ERR_ERROR_PREPROCMALFORMEDINCLUDE);
      break;
    }
  return 0;
}


void
cpp_define (void)
{
  struct macro *macro, *prevdef;
  char *name = NULL;
  char **params = NULL;
  char *body = NULL;
  struct replacement *replacements = NULL;
  size_t num_blocks, num_params = 0, num_replacements = 0;
  int c, has_ellipsis = 0;
  unsigned isid = 0;
  unsigned done = 0;
  unsigned has_params = 0;
  unsigned concat = 0;

  /* first we get the name of the macro */
  if (cpp_lex () != IDENT)
    {
      /* syntax error. Anything but an identifier following a #define
       * is invalid.
       */
      post_error (ERR_ERROR_PREPROCMACRONAMENOTIDENTIFIER, yytext);
      goto _done_;
    }

  /* copy the name into a new buffer */
  name = malloc (strlen (yytext) + 1);
  assert (name);
  strcpy (name, yytext);

  /* the next thing we need to do is decide if we're in a function
   * style macro definition or not. That's determined entirely by
   * whether the identifier is followed immediately by a '(' character.
   */
  if ((c = cpp_input ()) == '(')
    {
      int tok;

      /* We need to keep track of being a function style macro separately 
       * from the parameter count to handle macros w/o parameters but 
       * which expect () on them.
       */
      has_params = 1;

      /* parse out the parameter names until we reach the ')' closing things
       * out. The storage of the macro body itself is no different for
       * function style and simple macros, so we'll save that part until
       * later.
       *
       * This is a pretty simple task, really:
       *
       *   arg_list: (IDENT (',' arg_list)*)?
       */

      params = malloc (MAX_PARAMS * sizeof (char*));
      num_params = 0;
      num_blocks = 0;
      while ((tok = cpp_lex ()) == IDENT || tok == ELLIPSIS)
        {
          size_t i;

          if (tok == ELLIPSIS)
            {
              has_ellipsis = 1;

              /* for expansion purposes, we treat ... as a macro named
               * __VA_ARGS__. The only additional special treatment needed
               * is when reading the substitution text.
               */
              params[num_params] = strdup ("__VA_ARGS__");
              num_params++;
	      assert (num_params < MAX_PARAMS);

              /* after the '...' we must have a ')' token */
              if ((tok = cpp_lex ()) == ')')
                break;
              else
                {
                  post_error (ERR_ERROR_PREPROCMISSINGPARENINMACRODEFINE);
                }
            }
          else
            {
              /* Check for duplicate names in the same argument list */
              for (i = 0; i < num_params; i++)
                if (strcmp (yytext, params[i]) == 0)
                  {
                    post_error (ERR_ERROR_PREPROCDUPLICATEMACROPARAMETER,
                                yytext, name);
                  }
              /* alloc a copy of the argument name and store it */
              params[num_params] = strdup (yytext);
              num_params++;
	      assert (num_params < MAX_PARAMS);
            }

          /* Now we'll either have a ')' or a ',' token */
          if ((tok = cpp_lex ()) == ')')
            break;
          else if (tok != ',' || has_ellipsis)
            {
              post_error (ERR_ERROR_PREPROCMACROSYNTAXERROR);
              /* eat data until the '\n' */
              while ((c = cpp_input ()) != '\n' && c != EOF)
                ;
              goto _done_;
            }
        }
      /* Now that we've got all the params, we can free up any extra memory
       * we ended up allocating.
       */
      //            params = realloc (params, num_params * sizeof (char *));
      assert (!num_params || params);

    }
  else
    {
      /* non-function style macro definition. The character we just read
       * should either be horizontal whitespace or a '\n' character or
       * EOF. Anything else is an error.
       */
      if (c == ' ' || c == '\t')
        {
        }
      else if (c == '\n' || c == EOF)
        {
	  /* skip the whole function body processing */
	  goto _nobody;
        }
      else
        {
          post_error (ERR_ERROR_PREPROCILLEGALMACRONAMECHAR, c);
          /* eat data until the '\n' or EOF */
          while ((c = cpp_input ()) != '\n' && c != EOF)
            ;
          goto _done_;
        }

    }

  /* Anything left on the line is part of the macro body. */

  body = strcatdyn (0, "");
  assert (body);
  cpp_startmacrobody ();

  /* alloc space for the replacements */
  replacements = malloc (MAX_REPLACEMENTS * sizeof (struct replacement));
  assert (replacements);

  while ((c = cpp_lex()) == ' ')
    ;
  for ( concat = 0 ; c ; c = cpp_lex())
    {
      if (c == 0 || c == '\n')
        {
          body = strcatdyn (body, " ");
          break;
        }


      /* Throw an error here if a # or ## shows up w/o parameters. By doing 
       * this check here, we can do all the actual processing of those 
       * guys inside the stuff that only gets run for function-style macros.
       */
      if (c == '#' && !num_params)
        {
          post_error (ERR_ERROR_PREPROCHASHFOUNDINSIMPLEMACRO, name);
        }

      if (num_params)
        {
          size_t param;
          /* handle the stringize and token concatenation operators */
          if (c == '#')
            {
			  concat = 0;
              /* the next token should be an IDENT and it should match
               * a parameter name. 
               */
              if (cpp_lex () == IDENT)
                {
                  for (param = 0; param < num_params; param++)
                    {
                      if (strcmp (params[param], yytext) == 0)
                        {
                          /* we have a winner! mark this fellow for 
                           * replacement. put in the quotes around it
                           * too, since we're stringizing.
                           */
                          body = strcatdyn (body, "\"\"");

                          /* stringizing, so we need to put the 
                           * replacement index between the quotes.
                           */
                          replacements[num_replacements].idx = strlen (body)-1;
                          replacements[num_replacements].param = param;
                          num_replacements++;

						  assert (num_replacements < MAX_REPLACEMENTS);
                          break;
                        }
                    }
                }
              else
                {
                  post_error (ERR_ERROR_PREPROCNONPARAMSTRINGIZATION);
                }
              continue;
            }
          else if (c == CONCAT)
            {
              /* just make sure that for the expansion there's no whitespace
               * between the previous token and the next one. The space
               * in the input is combined into a single ' ', and our
               * lexing for the ## operator eats up both leading and
               * trailing whitespace around it, so we don't have to do
               * anything extra here. we do need to make sure we don't
               * do any further processing of the token, though.
               *
			   * There may be a space as the last character of the
			   * body text currently. Remove it so the previous token will
			   * get pasted with the next one. If there's not a replacement
			   * there, anyway. If there is, it'll fill in and there
			   * won't be any whitespace...
			   *
			   * We need to tell the next iteration through the case that
			   * we just hit a ##, though.
               */
			  //while (body[strlen(body)-1] == ' ')
			  //  body[strlen(body)-1] = '\0';

			  concat = 1;
              continue;
            }
          else if (c == IDENT)
            {
              /* if this is a parameter name, then handle replacement,
               * otherwise just let it fall through to be handled normally.
			   * if the previous token in the body is not a ## token, put in
			   * a 0xfd marker to enforce macro expansion in weird nested
			   * macro stuff w/ the ## operator being deeply nested. We'll
			   * cheat and only check for a single # character. if it's the
			   * # operator, we'll already have quotes inserted and so 
			   * won't see the # here. fugly stuff, but then this whole
			   * thing is a nasty hack. not the Right Thing to do.
               */
              for (param = 0; param < num_params; param++)
                {
                  if (strcmp (params[param], yytext) == 0)
                    {
                      /* we have a winner! mark this fellow for 
                       * replacement.
                       */
					  if (*body && !concat)
						{
						  body = strcatdyn (body, "\xfd");
						}
                      replacements[num_replacements].idx =
                        strlen (body);
                      replacements[num_replacements].param = param;
                      num_replacements++;
					  
					  assert (num_replacements < MAX_REPLACEMENTS);
					  
					  concat = 0;
                      break;
                    }
                }
              /* if it was a parameter, we don't want to do anything
               * else with it.
               */
              if (param != num_params)
				{
				  //body = strcatdyn (body, " ");
				  concat = 0;
				  continue;
				}
            }
        }

      switch (c)
        {
        case '#':
          /* we handled this above, so if it occurs here, it's an error */
          assert (0);
          break;
        case CONCAT:
          break;
		case IDENT:
		  /* if this identifier is the same as the name of the macro, we
		   * need to prepend a 0xfe marker character to it in the expansion
		   * text so that it won't be recursively expanded.
		   */
		  if (strcmp (yytext, name)==0)
			{
			  body = strcatdyn (body, "\xfe");
			}
		  body = strcatdyn (body, yytext);
		  break;
		case ' ':
		  /* only append whitespace if there's not a space on the
		   * end already. Note that we need to make sure that
		   * there's not an argument expansion between that space
		   * and this token.
		   */
		  if (body[strlen(body)-1] == ' ' 
			  && !(num_replacements 
				   && replacements[num_replacements-1].idx==strlen(body)))
			break;
		  /* fall through */
        default:
          body = strcatdyn (body, yytext);
          break;
        }
	  concat = 0;
    }
  /* now that we've got the body, strip all trailing space */
  while ((strlen (body)) && (isspace (body[strlen(body)-1])))
    body[strlen(body)-1] = '\0';

  body = strcatdyn (body, " ");

 _nobody:

  /* add the macro to the definitions */
  if ((prevdef = macro_find (name)))
    {
      if (prevdef->num_params != num_params
          || prevdef->has_ellipsis != has_ellipsis
          || prevdef->num_replacements != num_replacements
	  || ((prevdef->body==NULL) ? 
	      (body!=NULL) : 
	      (!body || strcmp (prevdef->body, body))))
        {
          post_error (ERR_ERROR_PREPROCMACROREDEFINITIONMISMATCH, name);
        }
      goto _done_;
    }

  /* shrink the space we allocated for the replacements to be the appropriate
   * size.
   *
   * don't trust realloc(). it's bogus as all hell...
   */
  replacements = realloc (replacements, 
  			  num_replacements * sizeof (struct replacement));
  assert (!num_replacements || replacements);

  macro = macro_new (name);
  macro->num_params = num_params;
  macro->params = (const char **) params;
  macro->has_ellipsis = has_ellipsis;
  macro->num_replacements = num_replacements;
  macro->replacements = replacements;
  macro->body = body;
  macro->has_params = has_params;
  free (name);                  /* the macro constructor will copy it */

  /* yes, a goto is the most appropriate way to handle this... It prevents all
   * sorts of nested 'if' issues which would make the code harder to read.
   *
   * We need to keep the output stream in sync with the input stream and it's
   * proper to do it here since here's where we're parsing the input. We need
   * to use a #line and not just output a '\n' because the line continuation
   * character may have mucked with things, and we won't see that one here.
   */
_done_:
  /* keep the output in sync with the input line numbers */
  cpp_lineno ();
}

void
cpp_undef (void)
{
  /* Following the undef there should be an IDENT and a '\n', nothing else */
  if (cpp_lex () != IDENT)
    {
      post_error (ERR_ERROR_PREPROCIDENTIFIEREXPECTED, "#undef");
    }
  else
    {
      /* we only need to do anything if there is a macro of this name currently
       * defined.
       */
      if (macro_find (yytext))
        macro_delete (yytext);

      if (cpp_lex () != '\n')
        {
          post_error (ERR_ERROR_PREPROCEXTRAINPUT, "#undef");
        }
    }
  /* keep the output in sync with the input line numbers */
  cpp_lineno ();
}

void
cpp_line (void)
{
  unsigned long lineno;
  /* this one uses lexical states to parse the input, so we can call cpp_lex()
   * to get the info we need.
   */
  s_advance ();
  if (s_match (VAL))
    {
      /* floats should not be recognized here */
      assert (val.type == SINT || val.type == UINT);
      if (val.type == SINT)
        lineno = val.data.sint;
      else
        lineno = val.data.uint;

      /* now we've got either a filename or the newline */
      s_advance ();
      if (s_match (S_CHAR_SEQ))
        {
          /* yytext has the enclosing quotes still on it.
           */
          if ((strlen (yytext) - 2) > MAX_FILENAME_PATH_LEN)
            post_error (ERR_ERROR_PREPROCLINEFILENAMETOOLONG, "#line");
          strncpy (yyfilename, yytext + 1, strlen (yytext) - 2);
          yyfilename[strlen (yytext) - 2] = '\0';
	  /* advance to the terminating '\n' */
	  s_advance();
        }

      if (!s_match ('\n'))
        {
          post_error (ERR_ERROR_PREPROCEXTRAINPUT, "#line");
        }

      /* assign to yylineno after we check for the \n since that pattern
       * will have incremented yylineno.
       */
      yylineno = lineno;
    }
  else
    {
      post_error (ERR_ERROR_PREPROCLINEINTEGEREXPECTED);
    }
  /* pass along the info to the rest of the world too */
  cpp_lineno ();
}

void
cpp_error (void)
{
  char * errtext = NULL;
  /* we can't just grab characters because we have to do macro expansion. :( */
  s_advance ();
  while (!s_match ('\n'))
    {
      errtext = strcatdyn (errtext, yytext);
      s_advance ();
    }
  post_error (ERR_ERROR_PREPROCERRORDIRECTIVE, errtext);
  free (errtext);
}

void
cpp_pragma (void)
{
  /* process the ones we know about and pass the rest along */

  fprintf (yyout, "_Pragma (");
  while (!s_match ('\n'))
    {
      fprintf (yyout, "%s ", yytext);
    }
  fprintf (yyout, ")");
}

void
cpp_nul (void)
{
  /* everything for this is already handled in the scanner itself */
}

unsigned
cpp_ifdef (void)
{
  level++;
  s_advance ();
  if (s_match (IDENT))
    return (macro_find (yytext) != NULL);
  else
    {
      post_error (ERR_ERROR_PREPROCIDENTIFIEREXPECTED, "#ifdef");
    }
  return 0;
}


unsigned
cpp_if (void)
{
  struct constant v;

  level++;

  v = s_expr (1);
  if (!s_match ('\n'))
    {
      post_error (ERR_ERROR_PREPROCMALFORMEDEXPRESSION, "#if");
    }
  /* keep the output in sync with the input line numbers */
  cpp_lineno ();
  switch (v.type)
    {
    case SINT:
      return v.data.sint != 0;
    case UINT:
      return v.data.uint != 0;
    case FLOAT:
      return v.data.fl != 0;
    }
  return 0;
}

unsigned
cpp_elif (void)
{
  struct constant v;

  /* If this is a dangling #else, i.e., no matching #if, then level will
   * already be zero.
   */
  if (!level)
    {
      post_error (ERR_ERROR_PREPROCMISSINGIF, "#elif");
      return -1;
    }


  v = s_expr (1);
  if (!s_match ('\n'))
    {
      post_error (ERR_ERROR_PREPROCMALFORMEDEXPRESSION, "#elif");
    }
  /* keep the output in sync with the input line numbers */
  cpp_lineno ();
  switch (v.type)
    {
    case SINT:
      return v.data.sint != 0;
    case UINT:
      return v.data.uint != 0;
    case FLOAT:
      return v.data.fl != 0;
    }
  return 0;
}

unsigned
cpp_endif (void)
{
  /* we should see a '\n' as the next token, or there's an error */
  if (cpp_lex() != '\n')
    {
      post_error (ERR_ERROR_PREPROCEXTRAINPUT, "#endif");
    }
  /* keep the output in sync with the input line numbers */
  cpp_lineno ();

  /* If this is a dangling #endif, i.e., no matching #if, then level will
   * already be zero.
   */
  if (!level)
    {
      post_error (ERR_ERROR_PREPROCMISSINGIF, "#endif");
      return -1;
    }
  else
    --level;
  return 0;
}

unsigned
cpp_else (void)
{
  /* we should see a '\n' as the next token, or there's an error */
  if (cpp_lex() != '\n')
    {
      post_error (ERR_ERROR_PREPROCEXTRAINPUT, "#else");
    }
  /* keep the output in sync with the input line numbers */
  cpp_lineno ();

  /* If this is a dangling #else, i.e., no matching #if, then level will
   * already be zero.
   */
  if (!level)
    {
      post_error (ERR_ERROR_PREPROCMISSINGIF, "#else");
      return -1;
    }
  return 0;
}

static char *
s_get_header_name (char term)
{
  char *hname = NULL;
  size_t idx, num_blocks;
  int c;
  long start_lineno = yylineno;

  idx = 0;
  num_blocks = 1;
  while ((c = cpp_lex ()) != term && c != 0)
    {
      hname = strcatdyn (hname, yytext);
    }
  /* Ok, we've either got the whole header name, or we've reached the
   * end of the file. If EOF, then we issue an error, otherwise we realloc
   * the memory block down to the minimum required size and process the
   * include file.
   */
  if (c == 0)
    {
      /* EOF reached. */
      long oldlineno = yylineno;
      yylineno = start_lineno;
      post_error (ERR_ERROR_PREPROCEOFINHEADERFILENAME, term);
      yylineno = oldlineno;
      free (hname);
      hname = NULL;
    }
  return hname;
}

static void
s_include_system_header (const char *hname)
{
  size_t idx;

  /* we'll check each of the system include locations in turn until we
   * either find a file which matches the header name or we run out
   * of places to check, in which case we'll issue an error diagnostic.
   */
  for (idx = 0; idx < num_sysinclude_dirs; idx++)
    {
      char *buf;
      FILE *fptr;

      buf = malloc (strlen (hname) + strlen (sysinclude_dir[idx]) + 1);
      strcpy (buf, sysinclude_dir[idx]);
      strcat (buf, hname);

      /* If we can open it, we're done and can just set up for the new
       * include file and quit, otherwise we need to keep checking.
       */
      if ((fptr = fopen (buf, "rt")) != NULL)
        {
          cpp_new_include_file (fptr, buf);
          free (buf);
          return;
        }
      free (buf);
    }
  /* If we fell all the way through, then we weren't able to find a matching
   * file, which is an error.
   */
  post_error (ERR_ERROR_PREPROCCANNOTFINDHEADERFILE, hname);
}

/** expression handling.
 */
static struct constant
s_expr (unsigned eval)
{
  struct constant v;
  s_advance ();
  v = s_compound (eval);
  return v;
}

static struct constant
s_compound (unsigned eval)
{
  /* we don't need to do anything with 'eval' here other than pass it on
   * down since there are no potentially harmful side effects of evaluating the
   * expression anyway in this case.
   */
  struct constant v = { SINT, {0} };
  v = s_conditional (eval);
  while (s_match (','))
    {
      s_advance ();
      v = s_conditional (eval);
    }
  return v;
}

static struct constant
s_conditional (unsigned eval)
{
  struct constant v = { SINT, {0} };
  v = s_lor (eval);
  while (s_match ('?'))
    {
      s_advance ();
      /* There's a sequence point after the first operand, so we've got the
       * check it here. If the first operand is true, then the second operand
       * is evaluated and its value is the value of the expression, else the
       * third operand is evaluated and its value is the value of the 
       * expression.
       */
      if (s_is_constant_zero (v))
        eval = 0;
      v = s_expr (eval);
      if (s_match (':'))
        {
          s_advance ();
          /* the ?: operator is convenient for evaluating the ?: operator.
           * How tautologically appropriate.
           * If we evaluated the second operand, keep that value but parse
           * the third operand, else evaluate the third operand.
           */
          if (eval)
            s_conditional (0);
          else
            v = s_conditional (1);
        }
      else
        post_error (ERR_ERROR_PREPROCSYNTAXERROREXPECTING, ":");
    }
  return v;
}

static struct constant
s_lor (unsigned eval)
{
  struct constant v = { SINT, {0} };
  v = s_land (eval);
  while (s_match (OROR))
    {
      s_advance ();
      /* There's a sequence point after the first operand, so we've got the
       * check it here. If the first operand is true, then the result of
       * the whole expression is true, and we need to short-circuit the
       * evaluation of the second operand. This is required by the standard, 
       * and is the point of the 'eval' argument to all of this stuff.
       */
      if (!s_is_constant_zero (v))
        eval = 0;
      v = s_land (eval);
      /* note that '!eval' effectively contains the boolean result of the first
       * operand at this point, which is why we don't use a second value to
       * get the second operand.
       */
      if (!eval || !s_is_constant_zero (v))
        v.type = SINT, v.data.sint = 1;
      else
        v.type = SINT, v.data.sint = 0;
    }
  return v;
}

static struct constant
s_land (unsigned eval)
{
  struct constant v = { SINT, {0} };
  v = s_bor (eval);
  while (s_match (ANDAND))
    {
      s_advance ();
      /* There's a sequence point after the first operand, so we've got the
       * check it here. If the first operand is false, then the result of
       * the whole expression is false, and we need to short-circuit the
       * evaluation of the second operand. This is required by the standard, 
       * and is the point of the 'eval' argument to all of this stuff.
       */
      if (s_is_constant_zero (v))
        eval = 0;
      v = s_bor (eval);
      /* note that 'eval' effectively contains the boolean result of the first
       * operand at this point, which is why we don't use a second value to
       * get the second operand.
       */
      if (eval && !s_is_constant_zero (v))
        v.type = SINT, v.data.sint = 1;
      else
        v.type = SINT, v.data.sint = 0;
    }
  return v;
}

static struct constant
s_bor (unsigned eval)
{
  /* we don't need to do anything with 'eval' here other than pass it on
   * down since there are no potentially harmful side effects of evaluating the
   * expression anyway in this case.
   */
  struct constant v = { SINT, {0} };
  v = s_bxor (eval);
  while (s_match ('|'))
    {
      struct constant v2 = { SINT, {0} };

      s_advance ();
      v2 = s_bxor (eval);
      s_arithmetic_conversions (&v, &v2);

      switch (v.type)
        {
        case SINT:
          v.data.sint |= v2.data.sint;
          break;
        case UINT:
          v.data.uint |= v2.data.uint;
          break;
        case FLOAT:
          post_error (ERR_ERROR_TYPEMISMATCHBOR);
          break;
        }
    }
  return v;
}

static struct constant
s_bxor (unsigned eval)
{
  /* we don't need to do anything with 'eval' here other than pass it on
   * down since there are no potentially harmful side effects of evaluating the
   * expression anyway in this case.
   */
  struct constant v = { SINT, {0} };
  v = s_band (eval);
  while (s_match ('^'))
    {
      struct constant v2 = { SINT, {0} };

      s_advance ();
      v2 = s_band (eval);
      s_arithmetic_conversions (&v, &v2);

      switch (v.type)
        {
        case SINT:
          v.data.sint ^= v2.data.sint;
          break;
        case UINT:
          v.data.uint ^= v2.data.uint;
          break;
        case FLOAT:
          post_error (ERR_ERROR_TYPEMISMATCHBXOR);
          break;
        }
    }
  return v;
}

static struct constant
s_band (unsigned eval)
{
  /* we don't need to do anything with 'eval' here other than pass it on
   * down since there are no potentially harmful side effects of evaluating the
   * expression anyway in this case.
   */
  struct constant v = { SINT, {0} };
  v = s_equality (eval);
  while (s_match ('&'))
    {
      struct constant v2 = { SINT, {0} };

      s_advance ();
      v2 = s_equality (eval);
      s_arithmetic_conversions (&v, &v2);

      switch (v.type)
        {
        case SINT:
          v.data.sint &= v2.data.sint;
          break;
        case UINT:
          v.data.uint &= v2.data.uint;
          break;
        case FLOAT:
          post_error (ERR_ERROR_TYPEMISMATCHBAND);
          break;
        }
    }
  return v;
}

static struct constant
s_equality (unsigned eval)
{
  /* we don't need to do anything with 'eval' here other than pass it on
   * down since there are no potentially harmful side effects of evaluating the
   * expression anyway in this case.
   */
  struct constant v = { SINT, {0} };
  v = s_relational (eval);
  while (s_match (EQ) || s_match (NE))
    {
      int tok = s_curtok ();
      struct constant v2 = { SINT, {0} };

      s_advance ();
      v2 = s_relational (eval);
      s_arithmetic_conversions (&v, &v2);

      switch (tok)
        {
        case EQ:
          switch (v.type)
            {
            case SINT:
              v.data.sint = v.data.sint == v2.data.sint;
              break;
            case UINT:
              v.data.sint = v.data.uint == v2.data.uint;
              break;
            case FLOAT:
              v.data.sint = v.data.fl == v2.data.fl;
              break;
            }
          break;
        case NE:
          switch (v.type)
            {
            case SINT:
              v.data.sint = v.data.sint != v2.data.sint;
              break;
            case UINT:
              v.data.sint = v.data.uint != v2.data.uint;
              break;
            case FLOAT:
              v.data.sint = v.data.fl != v2.data.fl;
              break;
            }
          break;
        default:
          assert (0);
        }
      v.type = SINT;
    }
  return v;
}

static struct constant
s_relational (unsigned eval)
{
  /* we don't need to do anything with 'eval' here other than pass it on
   * down since there are no potentially harmful side effects of evaluating the
   * expression anyway in this case.
   */
  struct constant v = { SINT, {0} };
  v = s_shift (eval);
  while (s_match ('<') || s_match ('>') || s_match (LE) || s_match (GE))
    {
      int tok = s_curtok ();
      struct constant v2 = { SINT, {0} };

      s_advance ();
      v2 = s_shift (eval);
      s_arithmetic_conversions (&v, &v2);

      switch (tok)
        {
        case '<':
          switch (v.type)
            {
            case SINT:
              v.data.sint = v.data.sint < v2.data.sint;
              break;
            case UINT:
              v.data.sint = v.data.uint < v2.data.uint;
              break;
            case FLOAT:
              v.data.sint = v.data.fl < v2.data.fl;
              break;
            }
          break;
        case '>':
          switch (v.type)
            {
            case SINT:
              v.data.sint = v.data.sint > v2.data.sint;
              break;
            case UINT:
              v.data.sint = v.data.uint > v2.data.uint;
              break;
            case FLOAT:
              v.data.sint = v.data.fl > v2.data.fl;
              break;
            }
          break;
        case LE:
          switch (v.type)
            {
            case SINT:
              v.data.sint = v.data.sint <= v2.data.sint;
              break;
            case UINT:
              v.data.sint = v.data.uint <= v2.data.uint;
              break;
            case FLOAT:
              v.data.sint = v.data.fl <= v2.data.fl;
              break;
            }
          break;
        case GE:
          switch (v.type)
            {
            case SINT:
              v.data.sint = v.data.sint >= v2.data.sint;
              break;
            case UINT:
              v.data.sint = v.data.uint >= v2.data.uint;
              break;
            case FLOAT:
              v.data.sint = v.data.fl >= v2.data.fl;
              break;
            }
          break;

        default:
          assert (0);
        }
      v.type = SINT;
    }
  return v;
}

static struct constant
s_shift (unsigned eval)
{
  /* we don't need to do anything with 'eval' here other than pass it on
   * down since there are no potentially harmful side effects of evaluating the
   * expression anyway in this case.
   */
  struct constant v = { SINT, {0} };
  v = s_additive (eval);
  while (s_match (SHL) || s_match (SHR))
    {
      int tok = s_curtok ();
      struct constant v2 = { SINT, {0} };

      s_advance ();
      v2 = s_additive (eval);
      s_arithmetic_conversions (&v, &v2);

      switch (tok)
        {
        case SHL:
          switch (v.type)
            {
            case SINT:
              v.data.sint <<= v2.data.sint;
              break;
            case UINT:
              v.data.uint <<= v2.data.uint;
              break;
            case FLOAT:
              post_error (ERR_ERROR_TYPEMISMATCHSHIFT);
              break;
            }
          break;
        case SHR:
          switch (v.type)
            {
            case SINT:
              v.data.sint >>= v2.data.sint;
              break;
            case UINT:
              v.data.uint >>= v2.data.uint;
              break;
            case FLOAT:
              post_error (ERR_ERROR_TYPEMISMATCHSHIFT);
              break;
            }
          break;
        default:
          assert (0);
        }
    }
  return v;
}

static struct constant
s_additive (unsigned eval)
{
  /* we don't need to do anything with 'eval' here other than pass it on
   * down since there are no potentially harmful side effects of evaluating the
   * expression anyway in this case.
   */
  struct constant v = { SINT, {0} };
  v = s_multiplicative (eval);
  while (s_match ('+') || s_match ('-'))
    {
      int tok = s_curtok ();
      struct constant v2 = { SINT, {0} };

      s_advance ();
      v2 = s_multiplicative (eval);
      s_arithmetic_conversions (&v, &v2);

      switch (tok)
        {
        case '+':
          switch (v.type)
            {
            case SINT:
              v.data.sint += v2.data.sint;
              break;
            case UINT:
              v.data.uint += v2.data.uint;
              break;
            case FLOAT:
              v.data.fl += v2.data.fl;
              break;
            }
          break;
        case '-':
          switch (v.type)
            {
            case SINT:
              v.data.sint -= v2.data.sint;
              break;
            case UINT:
              v.data.uint -= v2.data.uint;
              break;
            case FLOAT:
              v.data.fl -= v2.data.fl;
              break;
            }
          break;
        default:
          assert (0);
        }
    }
  return v;
}

static struct constant
s_multiplicative (unsigned eval)
{
  /* We have to be careful of 'eval' in the '/' and '%' cases because we don't
   * want to let a divide by zero error occur if 'eval' is false, since in that
   * case we're not supposed to notice that sort of thing.
   * e.g., #define A 0
   *       #if A && 1/0 // no error, the && short circuits the expression
   */
  struct constant v = { SINT, {0} };
  v = s_unary (eval);
  while (s_match ('*') || s_match ('/') || s_match ('%'))
    {
      int tok = s_curtok ();
      struct constant v2 = { SINT, {0} };

      s_advance ();
      v2 = s_unary (eval);
      s_arithmetic_conversions (&v, &v2);

      /* we'll deal with 'eval' by skipping the actual evaluation if it's false.
       */
      if (!eval)
        continue;

      switch (tok)
        {
        case '*':
          switch (v.type)
            {
            case SINT:
              v.data.sint *= v2.data.sint;
              break;
            case UINT:
              v.data.uint *= v2.data.uint;
              break;
            case FLOAT:
              v.data.fl *= v2.data.fl;
              break;
            }
          break;
        case '/':
          switch (v.type)
            {
            case SINT:
              if (v2.data.sint == 0)
                {
                  post_error (ERR_ERROR_DIVIDEBYZERO);
                  break;
                }
              v.data.sint /= v2.data.sint;
              break;
            case UINT:
              if (v2.data.uint == 0)
                {
                  post_error (ERR_ERROR_DIVIDEBYZERO);
                  break;
                }
              v.data.uint /= v2.data.uint;
              break;
            case FLOAT:
              if (v2.data.fl == 0)
                {
                  post_error (ERR_ERROR_DIVIDEBYZERO);
                  break;
                }
              v.data.fl /= v2.data.fl;
              break;
            }
          break;
        case '%':
          switch (v.type)
            {
            case SINT:
              if (v2.data.sint == 0)
                {
                  post_error (ERR_ERROR_DIVIDEBYZERO);
                  break;
                }
              v.data.sint %= v2.data.sint;
              break;
            case UINT:
              if (v2.data.uint == 0)
                {
                  post_error (ERR_ERROR_DIVIDEBYZERO);
                  break;
                }
              v.data.uint %= v2.data.uint;
              break;
            case FLOAT:
              post_error (ERR_ERROR_TYPEMISMATCHMODULUS);
              v.type = SINT;
              v.data.sint = 0;
              break;
            }
          break;
        default:
          assert (0);
        }
    }
  return v;
}

static struct constant
s_unary (unsigned eval)
{
  /* we don't need to do anything with 'eval' here other than pass it on
   * down since there are no potentially harmful side effects of evaluating the
   * expression anyway in this case.
   */
  struct constant v = { SINT, {0} };
  switch (s_curtok ())
    {
    case '+':
      s_advance ();
      v = s_unary (eval);
      break;
    case '-':
      s_advance ();
      v = s_unary (eval);
      switch (v.type)
        {
        case SINT:
          v.data.sint = -v.data.sint;
          break;
        case UINT:
          v.data.uint = -v.data.uint;
          break;
        case FLOAT:
          v.data.fl = -v.data.fl;
          break;
        }
      break;
    case '~':
      s_advance ();
      v = s_unary (eval);
      switch (v.type)
        {
        case SINT:
          v.data.sint = ~v.data.sint;
          break;
        case UINT:
          v.data.uint = ~v.data.uint;
          break;
        case FLOAT:
          post_error (ERR_ERROR_TYPEMISMATCHBNOT);
          v.type = SINT;
          v.data.sint = 0;
          break;
        }
      break;
    case '!':
      s_advance ();
      v = s_unary (eval);
      switch (v.type)
        {
        case SINT:
          v.data.sint = v.data.sint == 0;
          break;
        case UINT:
          v.data.sint = v.data.uint == 0;
          break;
        case FLOAT:
          v.data.sint = v.data.fl == 0;
          break;
        }
      v.type = SINT;
      break;
    default:
      v = s_primary (eval);
      break;
    }
  return v;
}

static struct constant
s_primary (unsigned eval)
{
  /* We don't need to do anything with 'eval' here other than pass it
   * on through any parenthesized expressions.
   */
  struct constant v = { SINT, {0} };
  switch (s_curtok ())
    {
    case '(':
      v = s_expr (eval);
      if (!s_match (')'))
        post_error (ERR_ERROR_PREPROCSYNTAXERROREXPECTING, "')'");
      break;
    case VAL:
      v = val;
      break;
    case IDENT:
      break;
    default:
      post_error (ERR_ERROR_PREPROCSYNTAXERROREXPECTING, "constant");
    }
  s_advance ();
  return v;
}

static void
s_arithmetic_conversions (struct constant *c1, struct constant *c2)
{
  /* we'll just brute force this. convert to the higher of the two
   * types.
   */
  switch (c1->type)
    {
    case SINT:
      switch (c2->type)
        {
        case SINT:
          break;
        case UINT:
          c1->data.uint = c1->data.sint;
          c1->type = UINT;
          break;
        case FLOAT:
          c1->data.fl = c1->data.sint;
          c1->type = FLOAT;
          break;
        }
      break;
    case UINT:
      switch (c2->type)
        {
        case SINT:
          c2->data.uint = c2->data.sint;
          c2->type = UINT;
          break;
        case UINT:
          break;
        case FLOAT:
          c1->data.fl = c1->data.uint;
          c1->type = FLOAT;
        }
      break;
    case FLOAT:
      switch (c2->type)
        {
        case SINT:
          c2->data.fl = c2->data.sint;
          c2->type = FLOAT;
          break;
        case UINT:
          c2->data.fl = c2->data.uint;
          c2->type = FLOAT;
          break;
        case FLOAT:
          break;
        }
      break;
    }
}

static unsigned
s_is_constant_zero (struct constant v)
{
  switch (v.type)
    {
    case SINT:
      return (v.data.sint == 0);
    case UINT:
      return (v.data.uint == 0);
    case FLOAT:
      return (v.data.fl == 0);
    default:
      assert (0);
    }
  return 0;
}


static int curtok = -1;
static void
s_advance (void)
{
  curtok = cpp_lex ();
}

static int
s_match (int tok)
{
  return curtok == tok;
}

static int
s_curtok (void)
{
  return curtok;
}
