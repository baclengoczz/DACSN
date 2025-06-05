#ifndef __XPLATFORM_H__
#define __XPLATFORM_H__

#ifdef __MINGW32__
  #include <stdio.h>
#else
  #include <sys/param.h>
#endif

#ifdef __MINGW32__
  #define MAX_FILENAME_PATH_LEN FILENAME_MAX

  /* Return the absolute path of F in A.  Kind of like strcpy. */
  #define GET_ABSOLUTE_PATH(A, F) _fullpath (A, F, MAX_FILENAME_PATH_LEN)
#else
  #define MAX_FILENAME_PATH_LEN MAXPATHLEN

  /* Return the absolute path of F in A.  Kind of like strcpy. */
  #define GET_ABSOLUTE_PATH(A, F) realpath (F, A)
#endif

#endif
