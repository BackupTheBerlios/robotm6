/**
 * @file robotString.cpp  
 * @brief
 *
 * Description :
 * This  file contains  what is  needed to  have printf  recognize and
 * interpret '%b' caracter  as indicating a int number  to printed out
 * in its binary rpresentation
 *
 * @date 16/03/03
 * @author Alexis Nikichine
 *
 * @see <a href="http://www.delorie.com/gnu/docs/glibc/libc_103.html">printf 
 * extension example</a>, glibc doc
 */

#include "robotString.h"

#ifdef __cplusplus
#include <cstdio>
#include <cassert>
#else
#include <stdio.h>
#include <assert.h>
#endif

#include <string.h>

#ifdef linux
#include <printf.h>


#define PRINTF_BIN_MAX_WIDTH (sizeof(char)*8)

// -------------------------------------------------------------------------
// RobotString_bPrintfBin
// -------------------------------------------------------------------------
// the function that will actually be called by printf to print out
// the binary form
// -------------------------------------------------------------------------
static int RobotString_bPrintfBin( FILE* stream,
                                   const struct printf_info *info,
                                   const void *const *args )
{
  int length;
  int min_width;  
  int to_print;
  int i = PRINTF_BIN_MAX_WIDTH-1;


  to_print=*(char*)(*args);

  
  while( ! (to_print & (1<<i--)) && (i != -1) );
  
  i++; // there was one i-- too much

  length = i+1;
  min_width=length;

  if (info->width)
  { 
    if (info->width > length) {
/*      int j;
      for (j=info->width; j>length ; --j)
       {
      fprintf(stream,"%c",info->pad);
	}*/
      i=info->width-1;
    }
    else if (info->width <length )
    {
      i = info->width-1;
    }
    length = info->width;
  }
      
  for (; i>=0; --i)
  {
    if (i > (min_width-1) )
    {
      fprintf(stream,"%c",info->pad);
      continue;
    }
    if (to_print & (1 << i) )
    {
      fprintf(stream, "1");
    }
    else
    {
      fprintf(stream, "0");
    }
  }
  
  return length;

}

// -------------------------------------------------------------------------
// RobotString_bPrintfBinArginfo
// -------------------------------------------------------------------------
// Another function needed by the pritnf extension mechanism 
// -------------------------------------------------------------------------
static int RobotString_bPrintfBinArginfo( const struct printf_info *info,
			size_t n,
			int *argtypes )
{
  if (n>0)
  {
    argtypes[0] = PA_CHAR;
  }
  return 1;
}

#endif // linux

// -------------------------------------------------------------------------
// RString::bPrintfInit
// -------------------------------------------------------------------------
// This is the one function you need to call from the outside of this 
// file, in order to register the '%b' pattern for printf
//
// @return 0 on success, or -1 on failure.
// -------------------------------------------------------------------------
int RString::bPrintfInit() 
{
#ifdef linux
  int ret= register_printf_function( 'b',
				     RobotString_bPrintfBin,
				     RobotString_bPrintfBinArginfo);  
  
  if (ret != 0) {
    return -1;
  }
#endif
  return 0;
  
}

// -------------------------------------------------------------------------
// RString::chop
// -------------------------------------------------------------------------
// the function that emulates Perl's chop
// -------------------------------------------------------------------------
int RString::chop(char* str)
{
  int length = strlen(str);
  assert(str[length] == 0);

  if (length) {
    if (str[length-1] == '\n')
    {
      str[length-1] = 0;
      return 1;
    }
  }
  return 0;
}
