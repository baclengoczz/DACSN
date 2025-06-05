/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define if you have a working `mmap' system call.  */
#define HAVE_MMAP 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
/* #undef STDC_HEADERS */

/* Absolute path to popt top_sourcedir. */
#undef POPT_SOURCE_PATH

/* Define if you have the __argz_count function.  */
/* #undef HAVE___ARGZ_COUNT */

/* Define if you have the __argz_next function.  */
/* #undef HAVE___ARGZ_NEXT */

/* Define if you have the __argz_stringify function.  */
/* #undef HAVE___ARGZ_STRINGIFY */

/* Define if you have the dcgettext function.  */
/* #undef HAVE_DCGETTEXT */

/* Define if you have the feof_unlocked function.  */
/* #undef HAVE_FEOF_UNLOCKED */

/* Define if you have the fgets_unlocked function.  */
/* #undef HAVE_FGETS_UNLOCKED */

/* Define if you have the getcwd function.  */
#define HAVE_GETCWD 1

#if defined (__NT__) || defined(__MINGW32__)
/* Define if you have the getegid function.  */
#define HAVE_GETEGID 0

/* Define if you have the geteuid function.  */
#define HAVE_GETEUID 0

/* Define if you have the getgid function.  */
#define HAVE_GETGID 0

/* Define if you have the getuid function.  */
#define HAVE_GETUID 0
#else
/* Define if you have the getegid function.  */
#define HAVE_GETEGID 1

/* Define if you have the geteuid function.  */
#define HAVE_GETEUID 1

/* Define if you have the getgid function.  */
#define HAVE_GETGID 1

/* Define if you have the getuid function.  */
#define HAVE_GETUID 1
#endif

/* Define if you have the getpagesize function.  */
#define HAVE_GETPAGESIZE 1

/* Define if you have the mempcpy function.  */
/* #undef HAVE_MEMPCPY */

/* Define if you have the mtrace function.  */
/* #undef HAVE_MTRACE */

/* Define if you have the munmap function.  */
#define HAVE_MUNMAP 1

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the setenv function.  */
/* #undef HAVE_SETENV */

/* Define if you have the setlocale function.  */
#define HAVE_SETLOCALE 1

/* Define if you have the stpcpy function.  */
/* #undef HAVE_STPCPY */

/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strchr function.  */
#define HAVE_STRCHR 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strtoul function.  */
#define HAVE_STRTOUL 1

/* Define if you have the tsearch function.  */
#define HAVE_TSEARCH 1

/* Define if you have the <alloca.h> header file.  */
#define HAVE_ALLOCA_H 1

/* Define if you have the <argz.h> header file.  */
/* #undef HAVE_ARGZ_H */

/* Define if you have the <dlfcn.h> header file.  */
#define HAVE_DLFCN_H 1

/* Define if you have the <float.h> header file.  */
#define HAVE_FLOAT_H 1

/* Define if you have the <libintl.h> header file.  */
#define HAVE_LIBINTL_H 0

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <locale.h> header file.  */
#define HAVE_LOCALE_H 1

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <mcheck.h> header file.  */
/* #undef HAVE_MCHECK_H */

/* Define if you have the <nl_types.h> header file.  */
#define HAVE_NL_TYPES_H 1

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <sys/param.h> header file.  */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Name of package */
#define PACKAGE "popt"

/* Version number of package */
#define VERSION "1.6.3"

/* Define if compiler has function prototypes */
#define PROTOTYPES 1

/* Define if you have the iconv() function. */
#define HAVE_ICONV 1

/* Define as const if the declaration of iconv() needs const. */
#define ICONV_CONST const

/* Define if you have <langinfo.h> and nl_langinfo(CODESET). */
#define HAVE_LANGINFO_CODESET 1

/* Define if your <locale.h> file defines LC_MESSAGES. */
#define HAVE_LC_MESSAGES 1

/* Define to 1 if translation of program messages to the user's native language
   is requested. */
#define ENABLE_NLS 1

/* Define if the GNU gettext() function is already present or preinstalled. */
/* #undef HAVE_GETTEXT */

