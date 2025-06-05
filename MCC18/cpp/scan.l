/*-*-C-*-*/
/* $Id: scan.l,v 1.107 2007/01/15 23:13:25 moshtaa Exp $ */
/*
 * The C preprocessor is a rather complicated beast at some points and
 * is quite subtle. In the past, i.e. MPLAB-C17 and MPLAB-C18, the cpp
 * functionality was all handled in the lexer as the main compiler went
 * along. This complicates things even more and it's not completely
 * clear that significant performance is really gained.
 *
 * For this trial implementation the preprocessor will be implemented
 * as a truly separate pass over the source code and will produce as
 * output a preprocessed stream on standard output.
 *
 * The translation of preprocessing tranlation units into translation units
 * consists of the following:
 *
 * WG14/N869 5.1.1.2 Translation Phases
 *
 * The precedence among the syntax rules of translation is specified
 * by the following phases.
 *   1) Physical source file multibyte characters are mapped to the source
 *   character set (introducing new-line characters for end-of-line indicators)
 *   if necessary. Trigraph sequences are replaced by corresponding
 *   singled-character internal representations.
 *
 *   2) Each instance of a backslash character (\) immediately followed by a
 *   new-line character is deleted, splicing physical source-lines to form
 *   logical source lines. If, as a result, a character sequence is produced
 *   that matches the syntax of a universal character name, the behaviour
 *   is undefined. Only the last backslash on any physical source line shall
 *   be eligible for being part of such a splice. A source file that
 *   is not empty shall end in a new-line character, which shall not be
 *   immediately preceded by a backslash character before any such splicing
 *   takes place.
 *
 *   3) The source file is decomposed into preprocessing tokens and sequences
 *   of white-space characters (including comments). A source file shall not
 *   end in a partial preprocessing token or in a partial comment. Each comment
 *   is replaced by one space character. New-line characters are retained.
 *   Whether each nonempty sequence of white-space characters other than
 *   new-line is retained or replaced by one space character is implementation
 *   defined.
 *
 *   4) Preprocessing directives are executed, macro invocations are expanded,
 *   and _Pragma unary operator expressions are executed. If a character
 *   sequence that matches the syntax of a universal character name is produced
 *   by token concatenation (6.10.3.3), the behaviour is undefined. A #include
 *   preprocessing directive causes the named header or source file to be
 *   processed from phase 1 through phase 4, recursively. All preprocessing
 *   directives are then deleted.
 */

%{
#include "config.h"
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <popt.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include <time.h>
#include "cpplib.h"
#include "xplatform.h"
#include "cerror.h"

extern struct constant val;
extern int error_level;

static void c_style_comment (void);
static void cxx_style_comment (void);
static void s_character_constant (void);
static int yy_input (char *buf, size_t max_size);

/* To process identifiers, we need to check if they're a macro we need to
 * expand or not.
 */
static int s_identifier (void);
static void s_integer (const char *s, unsigned is_decimal);
static void s_float (const char *s);
static void s_defined (void);
static void s_command_line_macro (const char *s);
static int _yygetc (FILE *fptr);

char yydate[14];   /* date of the translation as required for __DATE__ */
char yytime[11];   /* time of the translation as required for __TIME__ */
char yyfilename[MAX_FILENAME_PATH_LEN + 1];
char *cmdlineIncFile = NULL; /* include file name entered using -z flag */
long yylineno;
unsigned s_need_lineno = 0 ; // tracking debug with line continuation is tricky
int error_cnt = 0;
int verbose = 0;

/* We redefine YY_INPUT() so that we can handle translation phases 1 and 2
 * before the lexer itself ever sees anything. Things get a bit tricky in
 * here. Hopefully the code isn't *too* obnoxious. It's the backing up that's
 * the killer.
 */
#define YY_INPUT(buf,result,max_size) {(result)=yy_input(buf,max_size);}
/* redefine YY_FATAL_ERROR so flex will use our standard error reporting
 * mechanisms.
 */
#define YY_FATAL_ERROR(err) post_error (ERR_ERROR, err)


/* For both include files and macro expansion, we need to maintain a stack
 * of input buffers. Thankfully, flex(1) provides the mechanisms for
 * creating and switching between the input buffers, so we don't have
 * to hack the internals of the scanner, but we do need to maintain the
 * stack.
 *
 * In C++, we'd just use a container template here to create a stack
 * object, but in C we have to create a wrapper structure to contain
 * the information and provide access functions to the stack.
 *
 * The input buffers are added to the stack when the input is processed which
 * triggers them. Input buffers are removed from the stack in yywrap().
 */
struct cpp_input_source
{
  enum { MAIN_SOURCE, INCLUDE_SOURCE, MACRO_SOURCE } type;
  union
  {
    struct
    {
      const char *name;
      char *sourcefile; /* where was the file included from? */
      long sourceline;
    } include;
    struct
    {
      /* ###FIXME -- add info about where the macro was defined
       *    useful for debugging through macros if we ever support that
       */
      const struct macro *macro;
      const char **args;
      char *string;
    } macro;
  }data;
  YY_BUFFER_STATE buffer;
  struct cpp_input_source *prev;
};
static struct cpp_input_source *s_input_source_stack = NULL;

/** When we're allocating space for macro argument replacement lists, we'll
 * allocate room in blocks to avoid calling malloc() stuff all the time.
 * 4k makes a good block size since that's a typical page size. We'll free
 * up any extra after parsing all of the replacement anyway, so there's no
 * waste.
 */
#define MACRO_ALLOC_ARG_BLOCK_SZ 4096

/* note that we have to tokenize integer and floating point constants
 * even when not parsing expressions because otherwise the suffixes
 * may get recognized as a macro name, and that's not a good thing.
 */
%}

D           [0-9]
L           [a-zA-Z_]
H           [a-fA-F0-9]
E           [Ee][+-]?{D}+
FS          (f|F|l|L)
IS          (u|U|l|L)*
white       [\t\v ]


/* we need %array since we modify yytext ourselves, which really mucks with
 * things. check s_identifier()
 */
%array
%option stack
/* conditionals are a bit tricky and require two start states.
 * 'conditional' is used when skipping the contents of a group
 *   in a series of #if and #elif directives and none have yet
 *   evaluated to 'true' and so each #elif condition should be
 *   checked, and if a #else is encountered, the group following
 *   it should be included.
 * 'conditional2' is used just as 'conditional' after a group has
 *   been included, and therefore #elif and #else directives should
 *   be kept track of in order to control nesting levels only.
 */
%x define definebody expr conditional conditional2 defnd line error macroarg
/* <inc> scans an include directive;
 * <incfile> scans the included file name between quotes (" ") or
 * brackets (< >).
 * Macro expansion may take place in <inc>, but not in <incfile>.
 */
%x inc incfile

%x identifier_accept_paren
%%

^({white}*)#({white}*)include({white}*) {yy_push_state(inc);if (cpp_include())exit (1);yy_pop_state();}
^({white}*)#({white}*)define({white}*)  {BEGIN(define);cpp_define();BEGIN(INITIAL);}
^({white}*)#({white}*)undef({white}*)   {BEGIN(define);cpp_undef();BEGIN(INITIAL);}
^({white}*)#({white}*)line({white}*)    {BEGIN(line);cpp_line();BEGIN(INITIAL);}
^({white}*)#({white}*)error({white}*)   {BEGIN(error);cpp_error();exit(1);}
^({white}*)#({white}*)"\n"       {cpp_nul();fprintf (yyout,"\n");}
\xfd {/*nop*/}
\xfe{L}(({L}|{D})*) {fprintf(yyout,"%s", yytext+1);}
{L}(({L}|{D})*) {if (s_identifier())ECHO;}
<identifier_accept_paren>"(" {/* hackery to allow comment recognition when
                                 checking for a functional macro expansion
                              */return '(';}
<identifier_accept_paren>{white}+ {}
<identifier_accept_paren>. {return *yytext;}


\"(\\.|[^\"])*\" {/*to prevent comment recognition inside strings */ECHO;}


0[xX]{H}+{IS}?          {ECHO;}
0[bB][01]+{IS}?         {ECHO;}
0{D}+{IS}?              {ECHO;}
{D}+{IS}?               {ECHO;}
{D}+{E}{FS}?            {ECHO;}
{D}*"."{D}+({E})?{FS}?  {ECHO;}
{D}+"."{D}*({E})?{FS}?  {ECHO;}
'(\\.|[^\\'])+'         {ECHO;}
L'(\\.|[^\\'])+'        {ECHO;}


<inc,error,line>\xfd {/* nop*/}
<inc,error,line>\xfe{L}(({L}|{D})*) {memmove(yytext,yytext+1,strlen(yytext));return IDENT;}
<inc,error,line>{L}(({L}|{D})*) {if (s_identifier())return IDENT;}
<inc,error>"\n" {return '\n';}
<inc>\" {yy_push_state (incfile); return '\"';} /* scan the file name */
<inc>"<" {yy_push_state (incfile); return '<';} /* scan the file name */
<inc,error>. {return IDENT;}

 /* This exclusive state scans the included file name between
  * quotes (" ") or brackets (< >). A separate state is needed apart
  * from <inc> because macro expansion must not take place here.
  * In cpplib.c, the function 's_get_header_name' receives each
  * token, concatenating them until the closing " or > is seen.
  */
<incfile>\xfd {/* nop*/}
<incfile>\xfe{L}(({L}|{D})*) {memmove(yytext,yytext+1,strlen(yytext));return IDENT;}
 /* no expansion takes place, so we don't call 's_identifier' */
<incfile>{L}(({L}|{D})*) {return IDENT;}
<incfile>"\n" {return '\n';}
<incfile>\" {yy_pop_state ( ); return '\"';} /* the closing " */
<incfile>">" {yy_pop_state ( ); return '>';} /* the closing > */
<incfile>. {return IDENT;}

<define>({L}(({L}|{D})*)) {return IDENT;}
<definebody>(\xfd) {/* nop*/}
<definebody>(\xfe?)({L}(({L}|{D})*)) {return IDENT;}
<define>"," {return ',';}
<define>")" {return ')';}
<define>"\.\.\." {return ELLIPSIS;}
<define,definebody>"\n" {return '\n';}
  /* in a macro body we need to tokenize explicitely for everything that's
   * not a single character token so they stay single tokens.
   */
<definebody>({white}*)"##"({white}*) {return CONCAT;}
<definebody>{white}+ {strcpy (yytext, " ");return ' ';}
<definebody,line>\"(\\.|[^\"])*\" {return S_CHAR_SEQ;}
<definebody>"==" {return TOK;}
<definebody>"!=" {return TOK;}
<definebody>">=" {return TOK;}
<definebody>"<=" {return TOK;}
<definebody>"||" {return TOK;}
<definebody>"&&" {return TOK;}
<definebody>"<<" {return TOK;}
<definebody>">>" {return TOK;}
<definebody>"++" {return TOK;}
<definebody>"--" {return TOK;}
<definebody>"+=" {return TOK;}
<definebody>"-=" {return TOK;}
<definebody>"*=" {return TOK;}
<definebody>"/=" {return TOK;}
<definebody>"%=" {return TOK;}
<definebody>"<<=" {return TOK;}
<definebody>">>=" {return TOK;}
<definebody>"^=" {return TOK;}
<definebody>"|=" {return TOK;}
<definebody>"&=" {return TOK;}
<definebody>0[xX]{H}+{IS}?          {return TOK;}
<definebody>0[bB][01]+{IS}?         {return TOK;}
<definebody>0{D}+{IS}?              {return TOK;}
<definebody>{D}+{IS}?               {return TOK;}
<definebody>{D}+{E}{FS}?            {return TOK;}
<definebody>{D}*"."{D}+({E})?{FS}?  {return TOK;}
<definebody>{D}+"."{D}*({E})?{FS}?  {return TOK;}

<definebody>. {return *yytext;}


^({white}*)#({white}*)"if"({white})* {yy_push_state(expr);if(cpp_if()){yy_pop_state();yy_push_state(YYSTATE);}else{yy_pop_state();yy_push_state(conditional);}}
^({white}*)#({white}*)"ifdef"({white})* {yy_push_state(defnd);if(cpp_ifdef()){yy_pop_state();yy_push_state(YYSTATE);}else{yy_pop_state();yy_push_state(conditional);}}
^({white}*)#({white}*)"ifndef"({white})* {yy_push_state(defnd);if(!cpp_ifdef()){yy_pop_state();yy_push_state(YYSTATE);}else{yy_pop_state();yy_push_state(conditional);}}
^({white}*)#({white}*)"elif"({white}*)    {yy_push_state(expr);cpp_elif();yy_pop_state();yy_pop_state();yy_push_state(conditional2);}
^({white}*)#({white}*)"endif"  {yy_push_state(expr);if (!cpp_endif())yy_pop_state();yy_pop_state();}
^({white}*)#({white}*)"else"  {yy_push_state(expr);if (!cpp_else())yy_pop_state();yy_pop_state();yy_push_state(conditional);}


<conditional>^({white}*)#({white}*)"if"({white}*) {yy_push_state(expr);cpp_if();yy_pop_state();yy_push_state(YYSTATE);}
<conditional>^({white}*)#({white}*)"ifdef"({white}+) {yy_push_state(defnd);cpp_ifdef();yy_pop_state();yy_push_state(YYSTATE);}
<conditional>^({white}*)#({white}*)"ifndef"({white}+) {yy_push_state(defnd);cpp_ifdef();yy_pop_state();yy_push_state(YYSTATE);}
<conditional>^({white}*)#({white}*)"endif"  {yy_push_state(expr);cpp_endif();yy_pop_state();yy_pop_state();}
<conditional>^({white}*)#({white}*)"elif"({white}*) {yy_push_state(expr);if (cpp_elif()){yy_pop_state();yy_pop_state();yy_push_state(YYSTATE);}else{yy_pop_state();yy_pop_state();yy_push_state(conditional);}}
<conditional>^({white}*)#({white}*)"else"  {yy_push_state(expr);cpp_else();yy_pop_state();yy_pop_state();yy_push_state(YYSTATE);}

<conditional>.    {}
<conditional>"\n" {}

<conditional2>^({white}*)#({white}*)"if"({white}*) {yy_push_state(expr);cpp_if();yy_pop_state();yy_push_state(YYSTATE);}
<conditional2>^({white}*)#({white}*)"ifdef"({white}+) {yy_push_state(defnd);cpp_ifdef();yy_pop_state();yy_push_state(YYSTATE);}
<conditional2>^({white}*)#({white}*)"ifndef"({white}+) {yy_push_state(defnd);cpp_ifdef();yy_pop_state();yy_push_state(YYSTATE);}
<conditional2>^({white}*)#({white}*)"endif" {yy_push_state(expr);cpp_endif();yy_pop_state();yy_pop_state();}
<conditional2>^({white}*)#({white}*)"elif"({white}*) {yy_push_state(expr);cpp_elif();yy_pop_state();yy_pop_state();yy_push_state(conditional2);}
<conditional2>^({white}*)#({white}*)"else"  {yy_push_state(expr);cpp_else();yy_pop_state();yy_pop_state();yy_push_state(conditional);}

<conditional2>.    {}
<conditional2>"\n" {}


<expr>'(\\.|[^\\'])+'         {s_character_constant(); return VAL;}
<expr>L'(\\.|[^\\'])+'        {s_character_constant(); return VAL;}
<expr,line>0[xX]{H}+{IS}?     {s_integer(yytext,0);return VAL;}
<expr,line>0[bB][01]+{IS}?    {s_integer(yytext,0);return VAL;}
<expr,line>0{D}+{IS}?         {s_integer(yytext,0);return VAL;}
<expr,line>{D}+{IS}?          {s_integer(yytext,1);return VAL;}
<expr>{D}+{E}{FS}?            {s_float(yytext);return VAL;}
<expr>{D}*"."{D}+({E})?{FS}?  {s_float(yytext);return VAL;}
<expr>{D}+"."{D}*({E})?{FS}?  {s_float(yytext);return VAL;}
<expr>"defined"               {yy_push_state(defnd);s_defined();yy_pop_state();return VAL;}
<defnd>{L}(({L}|{D})*) {return IDENT;}
<defnd,expr>"(" { return '(';}
<defnd,expr>")" { return ')';}
<expr>\xfd {/*nop*/}
<expr>\xfe{L}(({L}|{D})*) {memmove(yytext,yytext+1,strlen(yytext)-1);return IDENT;}
<expr>{L}(({L}|{D})*) {if (s_identifier())return IDENT;}
<expr>"<<" {return SHL;}
<expr>">>" {return SHR;}
<expr>"<=" {return LE;}
<expr>">=" {return GE;}
<expr>"==" {return EQ;}
<expr>"!=" {return NE;}
<expr>"&&" {return ANDAND;}
<expr>"||" {return OROR;}
<expr>[+\-*/%<>!~&|^\,?:] {return yytext[0];}
<expr,line>"\n" {return '\n';}


<expr,define,line,defnd>("\r"|{white})* {/* ignore whitespace */}
<macroarg>(\xfd) {/*nop*/}
<macroarg>(\xfe?){L}(({L}|{D})*) {return IDENT;}
<macroarg>\"(\\.|[^\"])*\" {return S_CHAR_SEQ;}
<macroarg>"\n"            {if (s_need_lineno) {cpp_lineno();s_need_lineno=0;}return '\n';}
<macroarg>.               {return *yytext;}


  /* comments are a bit of a pain, as we have to recognize them in the
   * same phase as tokenization or we'll do silly things like recognize
   * comment delimiters inside string constants and stuff like that.
   *
   * We also need to do a bit of extra work since the appropriate
   * behaviour will vary based on which start state we're in.
   */
"/*" {c_style_comment(); fprintf (yyout, " ");}
"//"(.*) {}

<macroarg,error,definebody>({white}*)"/*" {c_style_comment();strcpy(yytext," ");return ' ';}
<identifier_accept_paren,expr,conditional,conditional2,defnd,define,line,inc>"/*" {c_style_comment();}
<identifier_accept_paren,expr,conditional,conditional2,defnd,define,line,definebody,error>"//"(.*) {}
%%
/* We need to be able to see the input() routine outside of the scanner file
 * proper, but it's declared static by flex. So we put a wrapper function in
 * here and use that in outside files.
 */
int
cpp_input (void)
{
  return input();
}


/* we need to be able to switch to the macrobody start state from the
 * cpp_define code, and yy_push_state and company are statics, so
 * provide a wrapper to use for the task.
 */
void
cpp_startmacrobody (void)
{
  assert (YYSTATE == define);
  BEGIN definebody;
}


int
yywrap (void)
{
  struct cpp_input_source *isource;
  /* if there's nothing on the input source stack, then we're done */
  if (!s_input_source_stack)
    {
      return 1;
    }

  /* do a consistency check on our input source stack */
  assert (YY_CURRENT_BUFFER == s_input_source_stack->buffer);

  /* remove the current buffer since it's done and restore the previous one */
  isource = s_input_source_stack->prev;
  yy_delete_buffer (s_input_source_stack->buffer);
  if (s_input_source_stack->type == MACRO_SOURCE)
    free (s_input_source_stack->data.macro.string);
  else if (s_input_source_stack->type==INCLUDE_SOURCE)
    {
      fclose (yyin);
      free ((void*)s_input_source_stack->data.include.name);
      strcpy (yyfilename, s_input_source_stack->data.include.sourcefile);
      free (s_input_source_stack->data.include.sourcefile);
      yylineno = s_input_source_stack->data.include.sourceline;
      cpp_lineno();
    }
  free (s_input_source_stack);
  s_input_source_stack = isource;
  if (s_input_source_stack)
    yy_switch_to_buffer (s_input_source_stack->buffer);

  /* If that was the last input source on the stack, then we're done, otherwise
   * we should keep going.
   */
  return !s_input_source_stack;
}

void
cpp_new_include_file (FILE *fptr, const char *name)
{
  struct cpp_input_source *isource;
  /* If this is the first time this has been called, we need to save off the
   * primary input buffer so that yywrap() can restore back to it when the
   * header file is done.
   */
  if (s_input_source_stack==NULL)
    {
      s_input_source_stack = calloc (1, sizeof(*isource));
      assert (s_input_source_stack);
      s_input_source_stack->buffer = YY_CURRENT_BUFFER;
      s_input_source_stack->type = MAIN_SOURCE;
    }
  /* Add the header file input source */
  isource = calloc (1, sizeof(*isource));
  assert (isource);
  isource->buffer = yy_new_buffer (fptr, YY_BUF_SIZE);
  isource->type=INCLUDE_SOURCE;
  isource->data.include.name = malloc (strlen (name)+1);
  strcpy ((char*)isource->data.include.name, name);
  isource->prev = s_input_source_stack;
  s_input_source_stack = isource;
  yy_switch_to_buffer (isource->buffer);
  /* set up the debugging info */
  isource->data.include.sourcefile = (char *) malloc (strlen (yyfilename) + 1);
  assert (isource->data.include.sourcefile);
  strcpy (isource->data.include.sourcefile, yyfilename);
  isource->data.include.sourceline = yylineno;
  /* track line numbering in the header file */
  assert (strlen (name) <= MAX_FILENAME_PATH_LEN);
  strcpy (yyfilename, name);
  yylineno = 1;
  cpp_lineno();
}

void
cpp_expand_macro (const struct macro *macro, const char **args)
{
  struct cpp_input_source *isource;
  char *buffer;
  size_t bsize, idx, rep;

  /* If this is the first time this has been called, we need to save off the
   * primary input buffer so that yywrap() can restore back to it when the
   * macro expansion is done.
   */
  if (s_input_source_stack==NULL)
    {
      s_input_source_stack = calloc (1, sizeof(*isource));
      assert (s_input_source_stack);
      s_input_source_stack->buffer = YY_CURRENT_BUFFER;
      s_input_source_stack->type = MAIN_SOURCE;
    }

  /* if this is a special macro (e.g., __LINE__), we need to handle things
   * a bit differently.
   */
  switch (macro->special)
    {
    case M_NOSPECIAL:

      /* We have all the arguments already and so we can go ahead and
       * substitute for them before we do anything else. That'll take
       * some load off the scanner proper and it'll allow us to handle
       * ## and # in a more elegant fashion. Specifically, the #define
       * handler will have ensured that there's sufficient whitespace
       * between tokens to prevent accidental token pasting, but will have
       * specifically removed it between two args separated by a ##,
       * and similarly will have added the surrounding quotes for the #.
       * Replacements for the # operator require a bit more twiddling
       * here to handle quotes in the argument text and things like that,
       * and so have been annotated by the definition.
       *       e.g., #define STR(a) #a
       *             STR("abc")
       *
       * Figuring out the size of the input buffer we need is a bit
       * labourious, but not really that hard. We just take the size
       * of the body and add up the sizes of all the arg expansions.
       */
      for (bsize = macro->body ? strlen (macro->body) : 0,
             idx = 0 ;
           idx < macro->num_replacements ;
           idx++)
        {
          bsize += strlen (args[macro->replacements[idx].param]);
        }
      /* use calloc for this one so we don't have to worry about keeping
       * track of putting a '\0' terminator on the end of the string every
       * time we add something.
       */
      buffer = calloc (1, bsize+1);
      assert (buffer != NULL);
      /* Now go through and copy in the data for the expansion. This is a bit
       * more of a pain. For each argument replacement we copy in however much
       * of the body proper there is between this replacement and the last one
       * and then copy in the replacement. After we've processed all the
       * replacements, we copy in however much of the body is remaining.
       *
       * idx: index to the next character from the main body to copy
       * rep: index to the next argument replacement to be performed
       *
       * Note that the simplifying case of no replacements is handled properly
       * by default.
       */
      for (idx = rep = 0 ; rep < macro->num_replacements ; rep++)
        {
          strncat (buffer,
                   &macro->body[idx],
                   macro->replacements[rep].idx - idx);
          strcat (buffer, args[macro->replacements[rep].param]);
          idx = macro->replacements[rep].idx;
        }
      /* handle an empty body. */
      if (macro->body)
        strcat (buffer, &macro->body[idx]);
      break;
    case M_LINE:
      {
        int n;
#if !defined(__NT__) && !defined(__MINGW32__)
        n = snprintf (NULL, 0, "%ld", yylineno);
#else
        n = 11;
#endif
        buffer = malloc (n + 1);
        assert (buffer);
        sprintf (buffer, "%ld", yylineno);
        break;
      }
    case M_FILE:
      buffer = malloc (strlen (yyfilename) + 3);
      assert (buffer);
      sprintf (buffer, "\"%s\"", yyfilename);
      break;
    }
  /* Add the macro body input source */
  isource = calloc (1, sizeof(*isource));
  assert (isource);
  isource->type = MACRO_SOURCE;
  isource->data.macro.macro = macro;
  isource->data.macro.args = args;
  isource->data.macro.string = buffer;
  isource->buffer = yy_scan_string (buffer);
  isource->prev = s_input_source_stack;
  s_input_source_stack = isource;
  /* unlike yy_new_buffer, yy_scan_string automatically switches input
   * buffers to scan from the string, so we don't need to call
   * yy_switch_to_buffer() here.
   */
}

/* yy_input will return YY_NULL on EOF and the number of characters read
 * into buf otherwise.
 *
 * yy_input handles trigraphs and \ line concatenation. Neither of these
 * can be done in the lexer proper because they're considered a separate
 * translation phase. To do them in the lexer could cause macros which
 * expand to character sequences matching trigraphs to get replaced and
 * other sorts of nastiness. Better to do it here, even though it clobbers
 * performance a bit.
 */
int
yy_input (char *buf, size_t max_size)
{
  int ch;

  ch = _yygetc (yyin);

  /* Handle trigraphs. We have to handle them here since they are
   * specified to be substituted for prior to lexical analysis.
   */
  if (ch == '?')
    {
      long curpos;
      curpos = ftell (yyin);
      ch = _yygetc (yyin);
      if (ch == '?')
        {
          /* The header is like a trigraph, anyway, if the next
           * character is one of the special ones, the we've got
           * a trigraph.
           */
          ch = _yygetc (yyin);
          switch (ch)
            {
            case '=':
              {
                ch = '#';
                break;
              }
            case '/':
              {
                ch = '\\';
                goto process_backslash;
              }
            case '\'':
              {
                ch = '^';
                break;
              }
            case '(':
              {
                ch = '[';
                break;
              }
            case ')':
              {
                ch = ']';
                break;
              }
            case '!':
              {
                ch = '|';
                break;
              }
            case '<':
              {
                ch = '{';
                break;
              }
            case '>':
              {
                ch = '}';
                break;
              }
            case '-':
              {
                ch = '~';
                break;
              }
            default:
              /* Well, it's not a trigraph after all. Put
               * our lookahead stuff back on the input and
               * keep going. If our buffer is big enough, we'll just
               * return all three characters, otherwise we'll seek back.
               */
                            if (max_size >= 3)
                {
                  sprintf (buf, "??%c", ch);
                  /* KRT: In this case, need to increment yylineno
                   * for debugging purposes.
                   *
                   */
                  if(ch=='\n')
                    yylineno++;
                  return 3;
                }
                else
                {
                  fseek (yyin, curpos, SEEK_SET);
                  ch = '?';
                }
              break;
            }
        }
      else
        {
          /* it's not a trigraph so return a '?'.
           */
          ungetc (ch, yyin);
          ch = '?';
        }
    }
  /* Handle the line continuation character */
  else if (ch == '\\')
    {
    process_backslash:
      ch = _yygetc (yyin);
      if (ch == '\n')         /* Assume the \r\n --> \n translation is done */
        {
          /* we do still want to update yylineno for debugging purposes */
          yylineno++;
          ch = _yygetc (yyin);
          if (ch == '\\')
            goto process_backslash;
          s_need_lineno = 1;
        }
      else
        {
          ungetc (ch, yyin);
          ch = '\\';
        }
    }

  if (ch == '\r')
    ch = ' ';

  if (ch == '\n')
    {
       if (s_need_lineno)
         {
           fprintf (yyout, "\n");
           cpp_lineno();
           s_need_lineno=0;
         }
       yylineno++;
    }

  *buf = ch;

  return ch != EOF ? 1 : YY_NULL;
}

/* Unlike in past versions, comment handling will be done during input,
 * not during the lexing itself. The good part about this is that
 * we don't have to worry about getting comments in macro expansion right
 * and little subtleties like that. Directive parsing and macro expansion
 * happens in a separate phase from comment handling, so it's easier to
 * do it here.
 */
static void
c_style_comment (void)
{
  int c;
  int got_star;
  int need_lineno;
  long start_lineno = yylineno;

  got_star = need_lineno = 0;
  while ((c=cpp_input()) != EOF)
    {
      if (c=='\n')
        (need_lineno = 1);
      if (got_star && c == '/')
        {
          break;
        }
      got_star = (c == '*');
    }
  if (c == EOF)
    {
      long oldlineno = yylineno;
      yylineno = start_lineno;
      post_error (ERR_ERROR_PREPROCUNTERMINATEDCOMMENT);
      yylineno = oldlineno;
      exit (1);
    }
  if (need_lineno)
    {
      fprintf (yyout, "\n");
      cpp_lineno();
    }
}

int
cpp_lex (void)
{
  static int ateof = 0;
  int tok = 0;
  if (!ateof)
    tok = yylex();
  if (tok == 0)
    ateof = 1;
  return tok;
}


static void
cxx_style_comment (void)
{
  int c;
  while ((c=_yygetc(yyin)) != EOF)
    {
      if (c=='\n')
        break;
    }
}

static void
s_character_constant (void)
{
  /* a character constant can be a ('\' followed by any character) or
   * (any character except '\' and apostrophe) in between two apostrophes.
   * if the first character after the first apostrophe is a '\', this is an
   * escape sequence that should be translated.  Otherwise it is a letter,
   * digit, or character representable by the character set.
   */

   /* first remove the surrounding apostrophes */
  char *temp;
  int i;
  i = 1 + (yytext[0] == 'L');
  temp = strdup(yytext+i);
  temp[strlen(temp) - 1] = '\0';

  /* is the first character a backslash */
  val.type = UINT;
  if( temp[0] == '\\' )
    {
      if( isdigit( temp[1] ) )
        {
          /* if the next character is a digit, then we have octal */
          val.data.sint = strtol( temp+1, NULL, 8 );
        }
      else if( temp[1] == 'x' )
        {
          /* if the next character is an 'x', then we have hexadecimal */
          temp = temp + 1;
          val.data.sint = strtol( temp+1, NULL, 16 );
        }
      else
        {
          switch( temp[1] )
            {
            case 'n':
              val.data.sint = 10;
              break;
            case 't':
              val.data.sint = 9;
              break;
            case 'v':
              val.data.sint = 11;
              break;
            case 'b':
              val.data.sint = 8;
              break;
            case 'r':
              val.data.sint = 13;
              break;
            case 'f':
              val.data.sint = 12;
              break;
            case 'a':
              val.data.sint = 7;
              break;
            case '\\':
              val.data.sint = 92;
              break;
            case '?':
              val.data.sint = 63;
              break;
            case '\'':
              val.data.sint = 39;
              break;
            case '"':
              val.data.sint = 34;
              break;
            default:
              val.data.sint = 0;
              post_error (ERR_ERROR_UNKNOWNESCSEQUENCE, temp);
            }
        }
    }
  else
    {
      val.type = SINT;
      val.data.sint = temp[0];
    }
}

static int
s_identifier (void)
{
  struct macro *macro;

  /* If there's no expansion to be done, we return 1, otherwise 0 */
  if ((macro=macro_find (yytext)) != NULL)
    {
      long yymacroexpandlineno; /* lineno where macro expansion began */
      struct cpp_input_source *isource;
      const char **args;

      /* set the macro expansion lineno so we can give more intelligent error
       * messages.
       */
      yymacroexpandlineno = yylineno;

      /* if this is a function style macro, we need to get its argument list */
      if (macro->has_params)
        {
          int c;
          size_t num_args;

          /* make sure we've *got* a replacement list. Note that we can't
           * call cpp_lex() and expect a '(' token here, as we're not in the
           * start state which will generate that. We need to parse out
           * whitespace and check for the '(' manually.
           * In which case we need to check for comments here also, as they
           * are considered whitespace. yucky.
           *
           * try having a non-exclusive start state for this that
           * just adds the '(' token...
           */
          yy_push_state (identifier_accept_paren);
          c = cpp_lex();
          yy_pop_state();
          if (c != '(')
            {
              /* there's no argument list. That means we shouldn't
               * recognize this as an invocation of the macro according
               * to 6.10.3. This is pretty subtle, but Plum-Hall
               * tests for it (quite rightly so).
               */
              /* We'd like to use flex's %pointer mechanism to support
               * abnormally large tokens and faster scanning, but this
               * unput() call would then nuke yytext, which would
               * really screw things up here.
               */
              unput (c);
            strcpy (yytext, macro->name);
            yyleng = strlen (macro->name);
              goto _done;
            }


          /* Only alloc space if the macro isn't just expecting () */
          if (macro->num_params)
            {
              int paren_depth;
              char *text = NULL;
              size_t idx, num_blocks;

              args = calloc (macro->num_params, sizeof (const char*));

              yy_push_state (macroarg);
              for (num_args = 0 ;
                   num_args < macro->num_params - (macro->has_ellipsis!=0);
                   ++num_args)
                {
                  /* this is really a bit of a pain to process properly
                   * since we need to handle commas inside parentheses and
                   * that sort of thing. it's not hard, just a pain. we
                   * just keep track of the nesting level of the parentheses
                   * and ignore commas unless there's no enclosing
                   * parentheses. Same for detecting the final closing
                   * parenthesis.
                   *
                   * Further complicating matters is the fact that we need
                   * to at least partially tokenize the input here since
                   * we need to expand any macros which are part of the
                   * argument(s) prior to using them as replacement text
                   * for the current macro. e.g.,
                   *    #define A(a) a
                   *    A(A(a))  // would expand to A(a), should expand to a
                   */
                  text = calloc (MACRO_ALLOC_ARG_BLOCK_SZ, sizeof (char));
                  assert (text);
                  idx = 0;
                  num_blocks = 1;

                  /* get rid of leading whitespace */
                  while (isspace (c = cpp_lex()))
                    ;

                  for (paren_depth = 0 ;
                       paren_depth != 0 ||
                         c != ((num_args==macro->num_params - 1)?')':',') ;
                       c = cpp_lex())
                    {
                      if (c==0)
                        {
                          /* we fool the error message into being on the
                           * line where expansion began.
                           */
                          long oldlineno = yylineno;
                          yylineno = yymacroexpandlineno;
                          post_error (ERR_ERROR_PREPROCEOFINMACROARGUMENT,
                                      macro->params[num_args], macro->name);
                          yylineno = oldlineno;
                          /* free up anything we've already allocated */
                          for (idx = 0 ; idx < num_args ; idx++)
                            free ((void*)args[idx]);
                          free (args);
                          free (text);
                          exit (1);
                        }
                      else if (c=='(') paren_depth++;
                      else if (c==')') paren_depth--;

                      if (paren_depth < 0)
                        {
                          long oldlineno = yylineno;
                          yylineno = yymacroexpandlineno;
                          post_error (ERR_ERROR_PREPROCMACROARGCOUNTMISMATCH,
                                      macro->name,
                                      macro->num_params,
                                      num_args + 1);
                          yylineno = oldlineno;
                          goto _done;
                        }

                      if (c == IDENT || c == S_CHAR_SEQ)
                        {
                          /* it's a literal, so copy in the whole
                           * text of it. Make sure we've got room
                           * first. This need to be >= to correctly
                           * handle the boundary case.
                           */
                          if (idx+strlen (yytext) >= MACRO_ALLOC_ARG_BLOCK_SZ)
                            {
                              /* alloc more room */
                              size_t n;
                              /* how many more blocks do we need? */
                              n = ((strlen (yytext)
                                    - (MACRO_ALLOC_ARG_BLOCK_SZ-idx))
                                   / MACRO_ALLOC_ARG_BLOCK_SZ) + 1;
                              text = realloc (text,
                                              (num_blocks+=n)
                                              * MACRO_ALLOC_ARG_BLOCK_SZ);
                              assert (text);
                              strcpy (&text[MACRO_ALLOC_ARG_BLOCK_SZ*
                                            (num_blocks-n-1)+idx],
                                      yytext);
                            }
                          else
                            {
                              strcpy (&text[MACRO_ALLOC_ARG_BLOCK_SZ *
                                            (num_blocks-1) + idx],
                                      yytext);
                            }
                          /* to handle the case where we alloced more space,
                           * we need to modulo the result to wrap properly.
                           */
                          idx += strlen (yytext);
                          idx %= MACRO_ALLOC_ARG_BLOCK_SZ;
                        }
                      else
                        {
                          /* it's just a single character then so
                           * copy it in.
                           */
                          text[MACRO_ALLOC_ARG_BLOCK_SZ*(num_blocks-1)
                               + idx++] = c;

                          if (idx >= MACRO_ALLOC_ARG_BLOCK_SZ)
                            {
                              assert (idx == MACRO_ALLOC_ARG_BLOCK_SZ);
                              text = realloc (text,
                                              ++num_blocks * MACRO_ALLOC_ARG_BLOCK_SZ);
                              assert (text);
                              idx = 0;
                            }
                        }
                    }
                  /* remove any trailing whitespace */
                   while ((strlen (text)) && (isspace(text[strlen(text)-1])))
                      text[strlen(text)-1] = '\0';
                  /* now we should have the full text of the replacement
                   * and 'c' should be the comma separating arguments or,
                   * for the last one, the closing ')'.
                   * realloc the memory for the name to the proper size and
                   * '\0' terminate the buffer so it's usable as a C string.
                   */
                  text =
                    realloc (text,
                             (num_blocks - 1) * MACRO_ALLOC_ARG_BLOCK_SZ + idx + 1);
                  assert (text);
                  text[(num_blocks - 1) * MACRO_ALLOC_ARG_BLOCK_SZ + idx] = '\0';
                  /* save off the text for later use. */
                  args[num_args] = text;
                }
              /* if this macro has ellipsis, we know that there is at least
               * one more argument to go. Note that this is a bit different
               * than varargs in C code in that there *must* be at least one
               * more argument to replace the ..., i.e., for a macro, the ...
               * specifies one or more additional arguments, while in C code,
               * the ... specifies zero or more additional arguments.
               * See WG14/N869 section 6.10.3 for details.
               *
               * We don't separate out the valist replacement as separate
               * arguments, but rather bunch 'em all up together, including
               * the separating ',' tokens, as a single replacement argument
               * which will get expanded whereever there was a __VA_ARGS__
               * token in the macro body.
               */
              if (macro->has_ellipsis)
                {
                  text = malloc (MACRO_ALLOC_ARG_BLOCK_SZ);
                  assert (text);
                  idx = 0;
                  num_blocks = 1;
                  for (paren_depth = 0, c = cpp_input() ;
                       paren_depth != 0 || c != ')' ;
                       c = cpp_input())
                    {
                      if (c==EOF)
                        {
                          long oldlineno = yylineno;
                          yylineno = yymacroexpandlineno;
                          post_error (ERR_ERROR_PREPROCEOFINMACROVALIST,
                                      macro->name);
                          yylineno = oldlineno;
                          /* free up anything we've already allocated */
                          for (idx = 0 ; idx < num_args ; idx++)
                            free ((void*)args[idx]);
                          free (args);
                          free (text);
                          return 0;
                        }
                      else if (c=='(') paren_depth++;
                      else if (c==')') paren_depth--;
                      assert (paren_depth >= 0);

                      text[idx++] = c;

                      if (idx >= MACRO_ALLOC_ARG_BLOCK_SZ)
                        {
                          assert (idx == MACRO_ALLOC_ARG_BLOCK_SZ);
                          text = realloc (text,
                                          ++num_blocks * MACRO_ALLOC_ARG_BLOCK_SZ);
                          assert (text);
                          idx = 0;
                        }
                    }
                  /* now we should have the full text of the replacement
                   * and 'c' should be the comma separating arguments or,
                   * for the last one, the closing ')'.
                   * realloc the memory for the name to the proper size and
                   * '\0' terminate the buffer so it's usable as a C string.
                   */
                  text =
                    realloc (text,
                             (num_blocks - 1) *
                             MACRO_ALLOC_ARG_BLOCK_SZ + idx + 1);
                  assert (text);
                  text[(num_blocks - 1) * MACRO_ALLOC_ARG_BLOCK_SZ + idx]
                    = '\0';
                  /* save off the text for later use. */
                  args[num_args] = text;
                }
              /* go back to whatever state we were in on entry */
              yy_pop_state ();
            }
          else
            {
              /* eat the closing ')' */
              while ((c=cpp_input()) == ' ' || c == '\t')
                ;
              if (c != ')')
                {
                  long oldlineno = yylineno;

                  yylineno = yymacroexpandlineno;
                  post_error (ERR_ERROR_PREPROCMISSINGPARENINMACROEXPAND,
                              macro->name);
                  yylineno = oldlineno;
                  return 0;
                }
            }
        }
      else
        args = NULL;

      if (strcmp (macro->name, "_CONFIG_DECL") == 0)
        {
          long oldlineno = yylineno;
          yylineno = yymacroexpandlineno;
          post_error (ERR_WARN_PREPROCCONFIGDECLDEPRECATED);
          yylineno = oldlineno;
        }

      cpp_expand_macro (macro, args);
      return 0;
    }
 _done:
  return 1;
}

static void
s_integer (const char *s, unsigned is_decimal)
{
  const char *p;
  unsigned is_unsigned;
  unsigned is_long;
  /* Convert 's' to an integer value and set up the communication to the
   * parser that will actually use the value.
   *
   * The annoying part is that we have to deal with the suffixes and pay
   * attention to signedness, and so can't just call strtol() and be done
   * with it.
   *
   * The 'is_decimal' flag tells us whether the string is a decimal
   * constant or not. We could figure that out for ourselves here, but the
   * scanner already knows that, so why duplicate the checking?
   *
   * According to 6.4.4.1, we need to fit the constant value into the
   * smallest integer type which will hold the value. For decimal constants,
   * we use only signed integer types unless there's a suffix which tells
   * us otherwise, and for octal, hex, and binary, we use both signed and
   * unsigned unless there's a suffix which tells us otherwise.
   */

  /* parse the suffixes, if there are any. If we find a unsigned suffix,
   * we mark is_unsigned to be true. If we find a long suffix, we increment
   * is_long, that way we can tell the difference between 'long' and
   * 'long long' suffixes.
   */
  is_unsigned = is_long = 0;
  for (p = &s[strlen(s)-1] ; !isxdigit(*p) ; --p)
    {
      if (toupper(*p)=='U') is_unsigned = 1;
      else if (toupper(*p)=='L') is_long++;
      else assert (0);
    }
  if (is_decimal)
    {
      if (!is_unsigned)
        {
          val.type = SINT;
          val.data.sint = strtol (s,NULL,0);
        }
      else
        {
          val.type = UINT;
          val.data.uint = strtoul (s,NULL,0);
        }
    }
  else
    {
      if (!is_unsigned)
        {
          unsigned long uval;
          signed long sval;
          uval = toupper (s[1]) == 'B' ? strtoul (s + 2, NULL, 2) :
            strtoul (s, NULL, 0);
          sval = toupper (s[1]) == 'B' ? strtol (s + 2, NULL, 2) :
            strtol (s, NULL, 0);
          /* here, we put it in a signed integer unless it won't fit, in
           * which case we put it in an unsigned.
           */
          if (uval <= LONG_MAX)
            {
              val.type = SINT;
              val.data.sint = sval;
            }
          else
            {
              val.type = UINT;
              val.data.uint = uval;
            }
        }
      else
        {
          val.type = UINT;
          val.data.uint = toupper (s[1]) == 'B' ? strtoul (s + 2, NULL, 2) :
            strtoul (s, NULL, 0);
        }
    }
}

static void
s_float (const char *s)
{
  /* convert 's' to a floating point value and set up the communication to
   * the parser that will actually use the value.
   *
   * This is significantly easier than integers, really, as we don't have to
   * deal with signedness.
   */
  val.type = FLOAT;
  val.data.fl = strtod (s,NULL);
}

static void
s_defined ()
{
  unsigned need_close = 0;
  assert (strcmp (yytext,"defined")==0);

  val.type = SINT;
  switch (cpp_lex())
    {
    case '(':
      need_close = 1;
      if (cpp_lex()!=IDENT)
        {
          post_error (ERR_ERROR_PREPROCIDENTIFIEREXPECTED, "'defined'");
          break;
        }
    case IDENT:
      val.data.sint = (macro_find (yytext)!=NULL);
      if (need_close && cpp_lex() != ')')
        {
          post_error (ERR_ERROR_PREPROCMISSINGPARENINDEFINED, ')');
        }
      break;
    default:
      post_error (ERR_ERROR_PREPROCMISSINGPARENINDEFINED, '(');
      break;
    }
}


static void
s_command_line_macro (const char *s)
{
  char *p, *p2;
  struct macro *macro;
  /* the string will be a macro name optionally followed by an '=' and
   * replacement text.
   */
  /* get a copy we can fiddle with */
  p = malloc (strlen (s)+1);
  assert (p);
  strcpy (p, s);
  p2 = strchr (p, '=');
  if (p2)
    *p2++ = '\0';
  macro = macro_new (p);
  if (p2 && *p2)
    {
      macro->body = malloc (strlen(p2)+1);
      assert (macro->body);
      /* we can override the 'const' in this instance since we just allocated
       * the buffer. It's there because we don't want anybody else mucking
       * around with it later.
       */
      strcpy ((char*)macro->body, p2);
    }
  else
    {
      macro->body = malloc (2);
      assert (macro->body);
      /* we can override the 'const' in this instance since we just allocated
       * the buffer. It's there because we don't want anybody else mucking
       * around with it later.
       */
      strcpy ((char*)macro->body, "1");
    }
}


static int
_yygetc (FILE *fptr)
{
  int c;
  c = getc (fptr);
  /* I don't want to see a '\r'. Ever. */
  if (c=='\r') c = getc (fptr);
  return c;
}


static void
show_diagnostic_counts ()
{
  if (error_level >= 1)
    fprintf (stdout, "Errors:    %d\n", get_error_count ());
  if (error_level >= 2)
    fprintf (stdout, "Warnings:  %d\n", get_warning_count ());
  if (error_level >= 3)
    fprintf (stdout, "Messages:  %d\n", get_message_count ());
}


void setSourceFile (const char *file)
{
  char *p;
  if ((p = strrchr (file, '/')) != NULL
      /* Hmmm... Try '\\' instead. ^&*%$* Micro$oft idiocy. */
      || (p = strrchr (file, '\\')) != NULL)
    {
      /* Ok, we know where the filename and directory are separated, so
       * we can extract the directory name.
       */
      source_file_dir = malloc ((p-file)+1);
      assert (source_file_dir);
      strncpy (source_file_dir, file, p - file);
      source_file_dir[p - file] = '\0'; /* null terminate it */
    }
  else
    {
      /* no path component to the filename */
      source_file_dir = malloc (3);
      assert (source_file_dir);
      strcpy (source_file_dir, ".");
    }

  strcpy (yyfilename, file); 
  if ((yyin = fopen (file, "rt")) == NULL)
    {
      post_error (ERR_ERROR_UNABLETOOPENFILE, file);
      exit (1);
    }
  
}

#define OPT_SUPPRESS_MESSAGE 256

jmp_buf abortbuf;

int main (int argc, const char *argv[])
{
  static char *opt;
  struct poptOption options[] =
  {
    {"include", 'i', POPT_ARG_STRING, &opt, 'i',
     "add to system include path", "path"},
    {"output", 'o', POPT_ARG_STRING, &opt, 'o', "set output file name", "name"},
    {"error", 'e', POPT_ARG_STRING, &opt, 'e',
     "output error messages to file 'name'", "name"},
    {"define", 'd', POPT_ARG_STRING, &opt, 'd',
     "define a macro. if no replacement text is given, the macro is defined "
     "with replacement text \"1\".", "name[=text]"},
    {"diagnostic-level", 'w', POPT_ARG_INT, &error_level, 0,
     "Set diagnostic level (default = 2)", "{ 1 | 2 | 3 }" },
    {"suppress-message", 0, POPT_ARG_INT, NULL, OPT_SUPPRESS_MESSAGE,
     "Suppress message <n>", "<n>" },
    {"verbose", 'v', POPT_ARG_NONE, &verbose, 0,
     "Show diagnostic counts", NULL },
    {"include-file", 'z', POPT_ARG_STRING, &opt, 'z',
     "Force include a file", NULL },

    POPT_AUTOHELP
    /* terminator */
    {NULL, '\0', 0, NULL, 0, NULL, NULL}
  };

  poptContext pcon; /* command line parsing context */
  int rc;
  const char *arg;
  char *ofile = NULL;
  char *efile = NULL;

  pcon = poptGetContext (NULL, argc, argv, options, 0);
  /* Process the command line arguments */
  while ((rc=poptGetNextOpt (pcon)) > 0)
    {
      switch (rc)
        {
        case 'i':
          cpp_add_sysinclude_dir_list (opt);
          break;
        case 'o':
          ofile = malloc (strlen(opt)+1);
          assert (ofile);
          strcpy (ofile, opt);
          break;
        case 'z':
          cmdlineIncFile = malloc (strlen(opt)+1);
          assert (cmdlineIncFile);
          strcpy (cmdlineIncFile, opt);
          break;
        case 'd':
          s_command_line_macro (opt);
          break;
        case 'e':
          efile = malloc (strlen(opt)+1);
          assert (efile);
          strcpy (efile, opt);
          break;
        case OPT_SUPPRESS_MESSAGE:
          {
            char * end;
            long message_number =
              strtol (poptGetOptArg (pcon), &end, 0);

            if ((!end) || (*end != '\0'))
              fprintf (stderr, "%s:  %s\n",
                       poptBadOption (pcon,
                                      POPT_BADOPTION_NOALIAS),
                       poptStrerror (POPT_ERROR_BADNUMBER));
            else
              {
                if (message_number >= MAX_ERROR_NUMBER)
                  suppress_message (message_number);
                else
                  fprintf (stderr, "%s:  %s\n",
                           poptBadOption (pcon,
                                          POPT_ERROR_BADNUMBER),
                           "error messages cannot be suppressed\n");
              }
          }

          break;
        default:
          assert (0);
        }
    }
  /* Check for error */
  if (rc < -1)
    {
      fprintf (stderr, "%s: %s\n",
               poptBadOption (pcon, POPT_BADOPTION_NOALIAS),
               poptStrerror(rc));
      return 1;
    }

  if (setjmp (abortbuf) == 0)
    {
      assert (efile);
      unlink (efile);
      if (error_init (efile))
        {
          printf ("unable to open file %s", efile);
          exit (1);
        }
      
      if (verbose)
        atexit (show_diagnostic_counts);
      
      /* set up yydate and yytime */
      {
        time_t tmt;

        time (&tmt);
        strftime (yydate, 14, "\"%b %d %Y\"", localtime (&tmt));
        strftime (yytime, 11, "\"%H:%M:%S\"", localtime (&tmt));
      }


      /* set up the pre-defined macros we know about. The compiler proper may
       * have given us some others on the command line as well.
       */
      macro_new_simple ("__LINE__", NULL, M_LINE);
      macro_new_simple ("__FILE__", NULL, M_FILE);
      macro_new_simple ("__DATE__", yydate, M_NOSPECIAL);
      macro_new_simple ("__TIME__", yytime, M_NOSPECIAL);
      macro_new_simple ("__STDC__", "1", M_NOSPECIAL);

      if (ofile != NULL)
        {
          if ((yyout = fopen (ofile, "w")) == NULL)
            {
              post_error (ERR_ERROR_UNABLETOOPENFILE, ofile);
              exit (1);
            }
        }
      else
        yyout = stdout;

      arg = poptGetArg (pcon);
      if (arg)
        {
          strcpy (yyfilename ,arg);
          yylineno = 1;
          cpp_lineno();
        }
      /* process commandline include file entered using -z flag */
      if (cmdlineIncFile)
        {
	  const char *tempCmdLineIncFName = makeCommandLineIncFileName(cmdlineIncFile);
	  if (tempCmdLineIncFName == NULL)
	    exit (1);
          
	  yylineno = 1;
          cpp_lineno();
          yyin = NULL;
          setSourceFile (tempCmdLineIncFName);
          yylineno = 1;
          cpp_lineno();
          yylex ();
          /* new return everything back to what is was initially */
          free (s_input_source_stack);
          free (source_file_dir);
          yy_current_buffer = 0;
          yy_c_buf_p = 0;
          yy_init = 1;
          yy_start = 0;
          s_need_lineno = 0;
          yy_start_stack_ptr = 0;
          yy_start_stack_depth = 0;
          yy_start_stack = 0;
          s_input_source_stack = NULL;
          source_file_dir = NULL;
          fclose (yyin);
        }

      /*read the source file */
      yyin = NULL;

      if (arg)
        {
          setSourceFile (arg);
        }
      else
        {
          yyin = stdin;
          yyfilename[0] = '\0';
        }

      yylineno = 1;
      cpp_lineno();

      cpp_lex();

      if (poptGetArg (pcon) != NULL)
        post_error (ERR_ERROR_PREPROCTOOMANYSOURCEFILES);
    }

  rc = (get_error_count() != 0);
  error_uninit ();

  return rc;
}
