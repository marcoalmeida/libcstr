/* Copyright (C) 2006-2010 Marco Almeida malmeida@netc.pt */

/* This file is part of libcstr. */

/* libcstr is free software; you can redistribute it and/or modify it */
/* under the terms of the GNU General Public License as published by */
/* the Free Software Foundation; either version 2 of the License, or */
/* (at your option) any later version. */

/* libcstr is distributed in the hope that it will be useful, but WITHOUT */
/* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY */
/* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public */
/* License for more details. */

/* You should have received a copy of the GNU General Public License */
/* along with libcstr; if not, write to the Free Software Foundation, */
/* Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA */


/*! \file cstr.h
    \brief cstr header file
*/

#ifndef CSTR
#define CSTR

#define MIN(X, Y) (X < Y ? X : Y) /**< simple, obvious minimum between two integers */

#define CSTR_NICE_FREE(X) free(X); X=NULL /**<  safe free(). makes the pointer NULL */

/* will become 64 during init */
#define CSTR_INIT_SIZE 63 /**< default string size */

/** \brief The cstr_t data type
 * 
 */
typedef struct cstr {
    unsigned long int size; /**<  maximum string length */
    unsigned long int len; /**<  current string length (not including \\0) */
    char *str; /**<  the string itself */
} *cstr_t; 

#define CSTR_SIZE(X) ((X)->size) /**< struct cstr buffer size field */
#define CSTR_LEN(X) ((X)->len) /**< struct cstr string length field */
#define CSTR_STR(X) ((X)->str) /**< struct cstr string buffer field */

/** \brief Case change choice enum
 * 
 */
typedef enum {UPPER, LOWER, SWAP} CASE;


/** \brief Get the next power of 2
 *
 * This function computes the smallest k, such that k is a power of 2
 * and k > n \n If n is already a power of two, it returns 2*n 
 \param n integer (not a power of two) 
 \return smallest power of 2 greater than n
*/
unsigned long int powerup(unsigned long int n);

/** \brief Create a cstr_t instance
 *
 * Create a cstr_t instance with the default maximum string length (64 chars)
 * This value will be reajusted when needed
 \return cstr_t instance
*/
cstr_t cstrInit(void);

/** \brief Create a cstr_t instance defining the buffer size
 *
 * Create a cstr_t instance defining an initial maximum string length
 * This is only the initial maximum. This value will be reajusted when needed
 \param n minimum string size (will be rounded to the next power of 2)
 \return cstr_t instance
*/
cstr_t cstrInit2(unsigned long int n);

/** \brief Create and initialize a cstr_t instance
 *
 * Create a cstr_t instance and initialize it with a C string (char *)
 * The buffer size will be powerup(strlen(s))
 \param s string to initialize the cstr_t instance with
 \return cstr_t instance
*/
cstr_t cstrInitChar(char *s);

/** \brief Destroy a cstr_t instance
 *
 * Destroy and free all allocated memory used by str
 \param str cstr_t instance to be freed
*/
void cstrDel(cstr_t str);

/** \brief Get string length
 *
 * This function just calls the CSTR_LEN() macro. It is the same as 
 * strlen(cstrlibGetChar(str))
 \param str cstr_t instance
 \return the length of the string (number of characters, without \\0)
*/
unsigned long int cstrLength(cstr_t str);

/** \brief Get buffer size
 *
 * This function just calls the CSTR_SIZE() macro, which gives the
 * ammount of currently allocated memory. In other words, it returns
 * the length of the biggest string that is possible to store in str
 \param str cstr_t instance
 \return the size of the currently allocated string buffer
*/
unsigned long int cstrSize(cstr_t str);

/** \brief Get a pointer to the string
 *
 * This function just calls the CSTR_STR() macro, which gives the pointer
 * to the string
 * It returns a const char * because you should only alter the string
 * using the cstr functions
 \param str cstr_t instance
 \return a pointer to the string
*/
const char * cstrGetChar(cstr_t str);

/** \brief Re-calculate minimum buffer size
 *
 * This function re-calculates the minimum buffer size that is needed to
 * keep the current string. If it is less than the currently allocated, the 
 * extra memory will be freed\n
 * This is the only way to make the buffer smaller. All the other functions
 * will only increase its size when needed
 \param str cstr_t instance to be resized
*/
void cstrResize(cstr_t str);

/** \brief Store a new string in a cstr instance
 *
 * This function stores the string s in the cstr instance \n
 * The buffer will not be resized. If needed, it will be increased, but
 * to make smaller (thus freeing some memory) you must call cstrResize()
 \param str cstr_t instance to store a new string
 \param s char array to be stored in str
 \return 0 or 1 if it fails or succeeds, respectively
*/
int cstrUpdate(cstr_t str, const char *s);

/** \brief Store a new string in a cstr instance according to a format
 *
 * This function stores a new string in the cstr instance according to a 
 * format. The format specifiers are the following:\n
 * \%s - a C null terminated string\n
 * \%d - a C int\n
 * \%A - a cstr_t instance\n\n
 * An example: suppose you want to create the string "select * from t where u='x' and p='y'".
 * Both values x and y are strings. You have one as a cstr_t instance, a1, and the other
 * as a C null terminated string, s1. The following code will do it:\n\n
 * cstrUpdate(str1, "select * from t where u='%A' and p='%s'", a1, s1);
 \param str cstr_t instance to store a new string
 \param fmt  the format string
 \param ... the remainder arguments, in the same order as they appear in fmt
 \return 0 or 1 if it fails or succeeds, respectively
*/
int cstrUpdateFormat(cstr_t str, char *fmt, ...);

/** \brief Get the i-th character
 *
 * This function returns the character at the position i of the stored string\n
 * The first position is 0
 \param str cstr_t instance 
 \param i the index
 \return the i-th character or EOF if limits are exceeded
*/
int cstrCharAt(cstr_t str, unsigned long int i);

/** \brief Get the (first) index of a character
 *
 * This function returns the index of the first occurence (left to right) of the
 * character c in the stored string
 \param str cstr_t instance 
 \param c the char
 \return the index of the first occurence of c or EOF
*/
unsigned long int cstrIndexOf(cstr_t str, char c);

/** \brief Get the (last) index of a character
 *
 * This function returns the index of the last occurence (first, if read right to left) 
 * of the character c in the stored string
 \param str cstr_t instance 
 \param c the char
 \return the index of the last occurence of c or EOF
*/
unsigned long int cstrLastIndexOf(cstr_t str, char c);

/** \brief String comparison
 *
 * This function compares the strings stored at str1 and str2
 \param str1 cstr_t instance 
 \param str2 cstr_t instance 
 \return 1 if the strings are equal. 0 otherwise
*/
int cstrEquals(cstr_t str1, cstr_t str2);

/** \brief String comparison (case insensitive)
 *
 * This function compares the strings stored at str1 and str2 but ignores
 * the case
 \param str1 cstr_t instance 
 \param str2 cstr_t instance 
 \return 1 if the strings are equal (modulo case). 0 otherwise
*/
int cstrEqualsI(cstr_t str1, cstr_t str2);

/** \brief cstr instance comparison
 *
 * This function compares two cstr instances\n
 * It returns true if both objects are exactly the same: equal strings and 
 * equal buffer size
 \param str1 cstr_t instance 
 \param str2 cstr_t instance 
 \return 1 if the objects are equal. 0 otherwise
*/
int cstrIsClone(cstr_t str1, cstr_t str2);

/** \brief String concatenation
 *
 * This function concatenates str1 with str2 and puts the result in str\n
 * The buffer size of str will be increased if necessary
 \param dst cstr_t instance which will hold the result of the concatenation
 \param str1 cstr_t instance 
 \param str2 cstr_t instance 
 \return the length of str or 0 if the buffer could not be increased
*/
unsigned long int cstrConcat(cstr_t dst, cstr_t str1, cstr_t str2);

/** \brief String concatenation (in place)
 *
 * This function concatenates str1 with str2 and keeps the result in str1\n
 * The buffer size of str1 will be increased if necessary
 \param str1 cstr_t instance 
 \param str2 cstr_t instance 
 \return the new length of str1 or 0 if the buffer could not be increased
*/
unsigned long int cstrConcatInPlace(cstr_t str1, cstr_t str2);

/** \brief String concatenation (in place)
 *
 * Same as cstrConcatInPlace() but taking a char* as second argument
 \param str1 cstr_t instance 
 \param str2 char* to be appended to the string stored int str1
 \return the new length of str1 or 0 if the buffer could not be increased
*/
unsigned long int cstrConcatInPlaceChar(cstr_t str1, char *str2);

/** \brief Concatenate lots of strings
 *
 * This function concatenates all the strings given as argument and
 * places the result in the first argument, str. The strings which are to be
 * concatenated have to be of type char*\n
 * The last argument must be NULL
 \param str cstr_t instance which will hold the result
 \param ... lots of char* to be concatenated. The last one must be NULL!!
 \return the new length of str or 0 if the buffer could not be increased
*/
unsigned long int cstrConcatMany(cstr_t str, ...);

/** \brief String copy
 *
 * This function places a copy of str2 in str1. The buffer size of str1 
 * will be increased if necessary.
 \param str1 cstr_t instance
 \param str2 cstr_t instance
*/
void cstrCopy(cstr_t str1, cstr_t str2);

/** \brief String reverse
 *
 * This function reverses the string stored in str. For example, "abc" is
 * changed to "cba".
 \param str cstr_t instance
*/
void cstrReverse(cstr_t str);

/** \brief Remove extra space
 *
 * This function replaces sequences of white space with a single space. For example,
 * "a   b     c d" is transformed into "a b c d"
 \param str cstr_t instance
*/
void cstrMakeSingleSpace(cstr_t str);

/** \brief Make the string lowercase
 *
 \param str cstr_t instance
*/
void cstrLower(cstr_t str);

/** \brief Make the string uppercase
 *
 \param str cstr_t instance
*/
void cstrUpper(cstr_t str);

/** \brief Capitalize the string
 *
 * This function makes the first character after a sequence of white space
 * upper case
 \param str cstr_t instance
*/
void cstrCapitalize(cstr_t str);

/** \brief Switch upper with lower case and vice-versa
 *
 * This function replaces every lower case character with the correspondent 
 * upper case and vice-versa
 \param str cstr_t instance
*/
void cstrSwapCase(cstr_t str);

/** \brief Remove white space from the beginning of the string
 *
 \param str cstr_t instance
*/
void cstrStripL(cstr_t str);

/** \brief Remove white space from the end of the string
 *
 \param str cstr_t instance
*/
void cstrStripR(cstr_t str);

/** \brief Remove white space from both the beginning and the end of the string
 *
 \param str cstr_t instance
*/
void cstrStrip(cstr_t str);

/** \brief Import from file
 *
 * This function reads the contents of fp (until EOF) and puts it in str
 \param str cstr_t instance
 \param fp input source
*/
void cstrImport(cstr_t str, FILE *fp);

/** \brief Write to file
 *
 * This function writes the entire string to fp
 \param str cstr_t instance
 \param fp output destination
 \return the number of items successfully read or written not the number of characters
*/
size_t cstrDump(cstr_t str, FILE *fp);

/** \brief Write n characters to file
 *
 * This function writes the first n characters of the string to fp
 \param str cstr_t instance
 \param fp output destination
 \param n number of characters to write
 \return the number of items successfully read or written not the number of characters
*/
size_t cstrDumpN(cstr_t str, FILE *fp, unsigned long int n);

/** \brief Read n characters from file
 *
 * This function reads the first n characters from fp and places them in str
 \param str cstr_t instance
 \param fp input source
 \param n number of characters to read
*/
void cstrImportN(cstr_t str, FILE *fp, unsigned long int n);

/** \brief Read n characters from a byte sequence
 *
 * This function reads n characters (bytes) from buffer and places them in str
 \param str cstr_t instance
 \param buffer input source
 \param n number of characters to read
*/
void cstrImportCharN(cstr_t str, const char *buffer, unsigned long int n);

/** \brief Get a substring
 *
 * This function extracts a substring from str and places it in dst
 \param dst cstr_t instance which will hold the result
 \param str cstr_t instance from which the substring will be extracted
 \param a left bound
 \param b right bound
 \return 0 or 1 if if fails ou succeeds, respectively
*/
int cstrGetSubstring(cstr_t dst, cstr_t str, unsigned long int a, unsigned long int b);

/** \brief Get a substring starting at the beginning
 *
 * This function extracts a substring from str and places it in dst
 \param dst cstr_t instance which will hold the result
 \param str cstr_t instance from which the substring will be extracted
 \param i the size of the substring
 \return 0 or 1 if if fails ou succeeds, respectively
*/
int cstrLeft(cstr_t dst, cstr_t str, unsigned long int i);

/** \brief Get a substring starting at the end
 *
 * This function extracts a substring from str and places it in dst
 \param dst cstr_t instance which will hold the result
 \param str cstr_t instance from which the substring will be extracted
 \param i the size of the substring
 \return 0 or 1 if if fails ou succeeds, respectively
*/
int cstrRight(cstr_t dst, cstr_t str, unsigned long int i);

/** \brief Search for a substring
 *
 * This function searches for the first occurence of s in the string stored
 * at str\n
 * Currently uses the KMP algorithm
 \param str cstr_t instance 
 \param s the substring to find
 \return the index of str where the match occured or CSTR_LEN(str) if s was not found
*/
unsigned long int cstrSearch(cstr_t str, char *s);

/** \brief Replace a substring with another
 *
 * This function searches for the first occurence of s1 in the string stored
 * at str and replaces it with s2\n
 * The buffer size will be increased if necessary
 \param str cstr_t instance 
 \param s1 the substring to find
 \param s2 the substring which will replace s1
 \return 1 or 0 if it succeeds or fails, respectively
*/
int cstrReplace(cstr_t str, const char *s1, const char *s2);

/** \brief Replace all occurences of a substring with another
 *
 * This function searches for all the occurences of s1 in the string stored
 * at str and replaces them with s2\n
 * The buffer size will be increased if necessary
 \param str cstr_t instance 
 \param s1 the substring to find
 \param s2 the substring which will replace s1
 \return the number of replaced substrings
*/
unsigned long int cstrReplaceAll(cstr_t str, const char *s1, const char *s2);

/** \brief Decode an encoded URL
 *
 * This function decodes the URL in str2 and saves the result
 * in str1
 * The buffer size will be increased if necessary
 \param str1 cstr_t instance with the result
 \param str2 cstr_t instance with an encoded URL
*/
void cstrDecodeURL(cstr_t str1, cstr_t str2);

/** \brief Decode an encoded URL (in place)
 *
 * This function decodes the URL in str
 * The buffer size will be increased if necessary
 \param str cstr_t instance with an encoded URL
*/
void cstrDecodeURLInPlace(cstr_t str);

/* TODO ? */
/* split */
/* join */

#endif /* CSTR */
