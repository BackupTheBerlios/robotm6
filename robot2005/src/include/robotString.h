/**
 * @file binary_printf.cpp  
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

#ifndef __ROBOT_STRING_H__
#define __ROBOT_STRING_H__

namespace RString {
    /**
     * This is the one function you need to call from the outside of this 
     * file, in order to register the '%b' pattern for printf
     *
     * @return 0 on success, or -1 on failure.
     */
    
    int bPrintfInit() ;

    /**
     * the function that emulates Perl's chop (remove the last \n)
     */
    int chop(char* str);
};

#endif // __ROBOT_STRING_H__
