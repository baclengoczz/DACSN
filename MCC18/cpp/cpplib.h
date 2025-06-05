/* $Id: cpplib.h,v 1.18 2007/01/15 23:13:25 moshtaa Exp $ */
#ifndef CPPLIB_H
#define CPPLIB_H

/* define the tokens the scanner will use to communicate with the rest of
 * the world.
 */
enum toks
{ IDENT = 256,
  ELLIPSIS,
  VAL,
  SHL,
  SHR,
  LE,
  GE,
  EQ,
  NE,
  ANDAND,
  OROR,
  S_CHAR_SEQ,
  CONCAT,
  TOK                  /* generic catch-all token */
};

/** constant values */
struct constant
{
  enum
  { SINT, UINT, FLOAT }
  type;
  union
  {
    signed long sint;
    unsigned long uint;
    long double fl;
  }
  data;
};

/** macro data structure */
struct macro
{
  const char *name;
  unsigned has_params:1;
  size_t num_params;
  const char **params;
  unsigned has_ellipsis:1;
  const char *body;
  size_t num_replacements;
  struct replacement *replacements;

  /* some of the predefined macros have special expansions which can't
   * be represented by just a body text.
   */
#define M_NOSPECIAL  0x0   /* no special expasion required */
#define M_LINE       0x1   /* __LINE__ expansion */
#define M_FILE       0x2   /* __FILE__ expansion */
  unsigned special;

  //###FIXME -- add info about where the macro was defined for better
  //    diagnostics when an invocation doesn't match the definition
};
struct replacement
{
  size_t idx;                   /* index into the body where the replacement takes place */
  size_t param;                 /* which argument gets expanded here? */
};

extern FILE *errfptr;
extern char *source_file_dir;


/** define a new simple macro, aka, no parameters or replacements 
 * @param name name of the macro
 * @param body macro body text
 * @param special special replacement flags, if any
 */
struct macro *macro_new_simple (const char *name, const char *body, unsigned special);
struct macro *macro_new (const char *name);
void macro_delete (const char *name);
struct macro *macro_find (const char *name);



int cpp_input (void);
void cpp_startmacrobody (void);
void cpp_lineno (void);
void cpp_add_sysinclude_dir_list (const char *hnames);
void cpp_add_sysinclude_dir (const char *hname);
void cpp_new_include_file (FILE * fptr, const char *name);
int cpp_include (void);
void cpp_define (void);
void cpp_undef (void);
void cpp_line (void);
void cpp_error (void);
void cpp_pragma (void);
void cpp_nul (void);
unsigned cpp_if (void);
unsigned cpp_elif (void);
unsigned cpp_ifdef (void);
unsigned cpp_else (void);       /* return -1 on underflow, 0 otherwise */
unsigned cpp_endif (void);      /* return -1 on underflow, 0 otherwise */
int cpp_lex (void);
int include_user_file (char *hname); /*used from inline include as well as commandline include*/
const char *makeCommandLineIncFileName (const char*hname);

#endif
