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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "cstr.h"

static void kmp_table(const char *W, unsigned long int *T)
{
    unsigned long int i = 2;
    unsigned long int j = 0;

    T[0] = -1, T[1] = 0;
    while (W[i] != '\0')
    {
        if (W[i - 1] == W[j]) {
            T[i] = j + 1;
            ++j;
            ++i;
        } else 
	    if (j > 0) {
		j = T[j];
	    } else {
		T[i] = 0;
		i++;
		j = 0;
	    }
    }
}

static unsigned long int kmp_search(const char *W, const char *S, unsigned long int *T)
{
    unsigned long int m = 0;
    unsigned long int i = 0;
    
    while (S[m+i] != '\0' && W[i] != '\0')
    {
        if (S[m + i] == W[i]) {
            i++;
        }
	else {
            m += i-T[i];
            if (i > 0)
		i = T[i];
        }
    }
    
    if (W[i] == '\0')
        return m;
    else
        return m + i;
}

static unsigned long int cstrGrow(cstr_t str, unsigned long int n)
{
    unsigned int m = CSTR_SIZE(str);

    while (m < n)
	m <<= 1;

    if (m != CSTR_SIZE(str))
    {
	if (! (CSTR_STR(str)=(char*)realloc(CSTR_STR(str), sizeof(char)*m)))
	    return 0;
	CSTR_SIZE(str) = m;
	return m;
    }
    return CSTR_SIZE(str);
}

unsigned long int powerup(unsigned long int n)
{
    unsigned long int m;
    
    while (m = n, n &= (n-1))
        ;
    return m*2;
}

cstr_t cstrInit(void)
{
    return cstrInit2(CSTR_INIT_SIZE);
}

cstr_t cstrInit2(unsigned long int n)
{
    cstr_t str;
    unsigned long int m = powerup(n);

    str = (cstr_t)malloc(sizeof(struct cstr));    
    if (str) {
	CSTR_SIZE(str) = m;
	CSTR_LEN(str) = 0;
	CSTR_STR(str) = (char*)malloc(sizeof(char)*m);
	if (CSTR_STR(str))
	    *CSTR_STR(str) = 0;
    }

    return str;
}

cstr_t cstrInitChar(char *s)
{
    cstr_t str;

    str = cstrInit2(strlen(s)+1);
    if (str)
	cstrUpdate(str, s);
    return str;
}

void cstrDel(cstr_t str)
{
    CSTR_NICE_FREE(CSTR_STR(str));
    CSTR_NICE_FREE(str);
}

unsigned long int cstrLength(cstr_t str)
{
    return CSTR_LEN(str);
}

unsigned long int cstrSize(cstr_t str)
{
    return CSTR_SIZE(str);
}

const char * cstrGetChar(cstr_t str)
{
    return (const char*)CSTR_STR(str);
}

int cstrUpdate(cstr_t str, const char *s)
{
    unsigned long int n = strlen(s);
    
    if (n > CSTR_SIZE(str))
	if (! cstrGrow(str, n+1))
	    return 0;
    
    strcpy(CSTR_STR(str), s);
    CSTR_LEN(str) = n;
    return 1;
}

int cstrUpdateFormat(cstr_t str, char *fmt, ...)
{
    va_list ap;
    char *s, c[2] = {0, 0};
    char ds[32]; /* very, very ugly... */
    int d;
    cstr_t A;

    if (! cstrUpdate(str, ""))
	return 0;

    va_start(ap, fmt);
    while (*fmt) {
	if (*fmt == '%') {
	    fmt++;
	    switch(*fmt)
	    {
	    case 's':
		s = va_arg(ap, char *);
		if (! cstrConcatInPlaceChar(str, s))
		    return 0;
		break;
	    case 'd':
		d = va_arg(ap, int);
		sprintf(ds, "%d", d);
		if (! cstrConcatInPlaceChar(str, ds))
		    return 0;
		break;
	    case 'A':
		A = (cstr_t)va_arg(ap, cstr_t);
		if (! cstrConcatInPlace(str, A))
		    return 0;
		break;
	    case '%':
		c[0] = '%';
		if (! cstrConcatInPlaceChar(str, c))
		    return 0;
		break;
	    }
	}
	else {
	    c[0] = *fmt;
	    if (! cstrConcatInPlaceChar(str, c))
		return 0;
	}
	fmt++;
    }
    va_end(ap);
    CSTR_LEN(str) = strlen(CSTR_STR(str));
    return 1;
}

/* reajusts the size of the allocated memory to the current needs */
void cstrResize(cstr_t str)
{
    unsigned long int p = powerup(CSTR_LEN(str)+1), m = CSTR_SIZE(str);
    
    if (m > (p>>1))
    {
	if (! (CSTR_STR(str)=(char*)realloc(CSTR_STR(str), sizeof(char)*p)))
	    return;
	CSTR_SIZE(str) = p;
    }
}

unsigned long int cstrConcat(cstr_t dst, cstr_t str1, cstr_t str2)
{
/*     cstrConcatMany(dst, CSTR_STR(str1), CSTR_STR(str2), NULL); */
    unsigned long int n = CSTR_LEN(str1) + CSTR_LEN(str2);
    
    if (CSTR_SIZE(dst) < n+1)
	if (! cstrGrow(dst, n+1))
	    return 0;
    strcpy(CSTR_STR(dst), CSTR_STR(str1));
    strcpy(CSTR_STR(dst)+CSTR_LEN(str1), CSTR_STR(str2));
    CSTR_LEN(dst) = n;
    return n;
}

unsigned long int cstrConcatInPlace(cstr_t str1, cstr_t str2)
{
    /* talvez uma função comum para evitar strlen(CSTR_STR(str2)) */
    return cstrConcatInPlaceChar(str1, CSTR_STR(str2));
/*     unsigned long int m = CSTR_LEN(str1)+CSTR_LEN(str2)+1; */

/*     if (CSTR_SIZE(str1) < m) */
/* 	cstrGrow(str1, m); */
/*     strcpy(CSTR_STR(str1)+CSTR_LEN(str1), CSTR_STR(str2)); */
/*     CSTR_LEN(str1) = m-1; */
}

unsigned long int cstrConcatInPlaceChar(cstr_t str, char *s)
{
    unsigned long int m = CSTR_LEN(str)+strlen(s)+1;

    if (CSTR_SIZE(str) < m)
	if (! cstrGrow(str, m))
	    return 0;
    strcpy(CSTR_STR(str)+CSTR_LEN(str), s);
    CSTR_LEN(str) = m-1;
    return CSTR_LEN(str);
}

unsigned long int cstrConcatMany(cstr_t str, ...)
{
    va_list ap;
    char *s;
    
    va_start(ap, str);
    while ((s=va_arg(ap, char *)) != NULL)
	if (! cstrConcatInPlaceChar(str, s))
	    return 0;
    va_end(ap);
    CSTR_LEN(str) = strlen(CSTR_STR(str));
    return CSTR_LEN(str);
}

int cstrCharAt(cstr_t str, unsigned long int index)
{
    if (index >= CSTR_LEN(str))
	return EOF;
    return CSTR_STR(str)[index];
}

unsigned long int cstrIndexOf(cstr_t str, char c)
{
    unsigned long int i;
    
    for (i = 0; i < CSTR_LEN(str); i++)
	if (CSTR_STR(str)[i] == c)
	    return i;
    return EOF;
}

unsigned long int cstrLastIndexOf(cstr_t str, char c)
{
    unsigned long int i = CSTR_LEN(str);
    
    for (i = CSTR_LEN(str); i >= 0; i--)
	if (CSTR_STR(str)[i] == c)
	    return i;
    return EOF;
}

/* returns cstrLength() when s is not found */
unsigned long int cstrSearch(cstr_t str, char *s)
{
    unsigned long int i, *table = (unsigned long int*)malloc(strlen(s)*sizeof(unsigned long int));
    
    if (! table)
	return CSTR_LEN(str);
    kmp_table(s, table);
    i = kmp_search(s, CSTR_STR(str), table);
    CSTR_NICE_FREE(table);
    return i;
}

int cstrGetSubstring(cstr_t dst, cstr_t str, unsigned long int i, unsigned long int j)
{
    if (j <= i)
	return 0;

    if (! cstrGrow(dst, j-i+1))
	return 0;

    if (CSTR_STR(dst) != CSTR_STR(str))
	memcpy(CSTR_STR(dst), CSTR_STR(str)+i, j-i);
    else
	memmove(CSTR_STR(dst), CSTR_STR(str)+i, j-i);
    CSTR_STR(dst)[j-i] = 0;
    CSTR_LEN(dst) = j-i;
    return 1;
}

int cstrLeft(cstr_t dst, cstr_t str, unsigned long int i)
{
    return cstrGetSubstring(dst, str, 0, i);
}

int cstrRight(cstr_t dst, cstr_t str, unsigned long int i)
{
    return cstrGetSubstring(dst, str, CSTR_LEN(str)-i, CSTR_LEN(str));
}

static int cstrLocalReplace(cstr_t str, const char *s1, const char *s2, 
			       unsigned long int *table)
{
    unsigned long int i;
    int l1 = strlen(s1), l2 = strlen(s2);
    
    if (table)
	i = kmp_search(s1, CSTR_STR(str), table); /* cstrSearch2(str, (char*)s1, table); */
    else
	i = cstrSearch(str, (char*)s1);

    if (i < CSTR_LEN(str))
    {
	cstrGrow(str, CSTR_LEN(str)+l2-l1+1);
	memmove(CSTR_STR(str)+i+l2, CSTR_STR(str)+i+l1, (CSTR_LEN(str)-i-l1+1)*sizeof(char));
	memcpy(CSTR_STR(str)+i, s2, l2*sizeof(char));
	CSTR_LEN(str) = CSTR_LEN(str) - l1 + l2;
	return 1;
    }
    return 0;
}

int cstrReplace(cstr_t str, const char *s1, const char *s2)
{
    return cstrLocalReplace(str, s1, s2, NULL);
}

unsigned long int cstrReplaceAll(cstr_t str, const char *s1, const char *s2)
{
    unsigned long int i = 0,
	*table = (unsigned long int*)malloc(CSTR_LEN(str)*sizeof(unsigned long int));
    
    if (table) {
	kmp_table(CSTR_STR(str), table);
	while(cstrLocalReplace(str, s1, s2, table))
	    i++;
	CSTR_NICE_FREE(table);
    }
    return i;
}

int cstrEquals(cstr_t str1, cstr_t str2)
{
    return (strcmp(CSTR_STR(str1), CSTR_STR(str2)) == 0);
}

int cstrEqualsI(cstr_t str1, cstr_t str2)
{
    unsigned long int i;
    char c1, c2;

    if (CSTR_LEN(str1) != CSTR_LEN(str2))
	return 0;

    for (i = 0; i < CSTR_LEN(str1); i++) {
	c1 = CSTR_STR(str1)[i];
	c2 = CSTR_STR(str2)[i];
	if (c1 != c2) {
	    if ((c1 < c2 && c1 != c2-32) || (c1 > c2 && c1 != c2+32))
		return 0;
	}
    }
    return 1;
}

int cstrIsClone(cstr_t str1, cstr_t str2)
{
    if ((CSTR_SIZE(str1) != CSTR_SIZE(str2)) ||
	(CSTR_LEN(str1) != CSTR_LEN(str2)) ||
	(! cstrEquals(str1, str2)))
	return 0;
    return 1;
}

void cstrCopy(cstr_t str1, cstr_t str2)
{
    cstrUpdate(str1, CSTR_STR(str2));
}

void cstrReverse(cstr_t str)
{
    unsigned long int i, n = CSTR_LEN(str);
    char c;

    for (i = 0; i < n/2; i++) {
	c = CSTR_STR(str)[n-i-1];
	CSTR_STR(str)[n-i-1] = CSTR_STR(str)[i];
	CSTR_STR(str)[i] = c;
    }
}

void cstrMakeSingleSpace(cstr_t str)
{
    unsigned long int i, j;
    
    for (i = 0, j = 0; i < CSTR_LEN(str); i++) {
	if (CSTR_STR(str)[i] == ' ') {
	    while (CSTR_STR(str)[++i] == ' ')
		;
	    i--;
	}
	CSTR_STR(str)[j++] = CSTR_STR(str)[i];
    }
    CSTR_STR(str)[j] = 0;
    CSTR_LEN(str) = j;
}

/* case transformations */
void cstrUpdateCase(cstr_t str, CASE cs)
{
    char *s = CSTR_STR(str);
    int low = 0;

    while (*s)
    {
	switch (cs)
	{
	case SWAP:
	    low = 1;
	case UPPER:
	    if (*s >= 'a' && *s <= 'z') {
		*s -= 32;
		low = 0;
	    }
	    if (! low)
		break;
	case LOWER:
	    if (*s >= 'A' && *s <= 'Z')
		*s += 32;
	    break;
	}
	s++;
    }
}

void cstrLower(cstr_t str)
{
    cstrUpdateCase(str, LOWER);
}

void cstrUpper(cstr_t str)
{
    cstrUpdateCase(str, UPPER);
}

void cstrSwapCase(cstr_t str)
{
    cstrUpdateCase(str, SWAP);
}

void cstrCapitalize(cstr_t str)
{
    char *s = CSTR_STR(str);
    while (*s++)
	if (*s == ' ' && *(s+1) >= 'a' && *(s+1) <= 'z')
	    *(s+1) -= 32;
}

void cstrStripL(cstr_t str)
{
    char *s = CSTR_STR(str);
    unsigned long int i = 0;

    while (s[i] == ' ')
	i++;
    memmove(CSTR_STR(str), CSTR_STR(str)+i, sizeof(char)*(CSTR_LEN(str)-i+1));
    CSTR_LEN(str) -= i;
}

void cstrStripR(cstr_t str)
{
    char *s = CSTR_STR(str);
    unsigned long int i;

    for (i = CSTR_LEN(str)-1; i >= 0 && s[i] == ' '; i--)
	;
    s[i+1] = 0;
    CSTR_LEN(str) = i+1;
}

void cstrStrip(cstr_t str)
{
    cstrStripL(str);
    cstrStripR(str);
}

size_t cstrDump(cstr_t str, FILE *fp)
{
    return fwrite(CSTR_STR(str), sizeof(char), CSTR_LEN(str), fp);
}

size_t cstrDumpN(cstr_t str, FILE *fp, unsigned long int n)
{
    return fwrite(CSTR_STR(str), sizeof(char), n, fp);
}

void cstrImport(cstr_t str, FILE *fp)
{
    char buffer[CSTR_INIT_SIZE];
    int n;

    cstrUpdate(str, "");
    while ((n=fread(buffer, sizeof(char), CSTR_INIT_SIZE-1, fp))) {
	buffer[n] = 0; /* cstrConcatInPlaceChar expects a \0 terminated string */
	cstrConcatInPlaceChar(str, buffer);
    }
}

void cstrImportN(cstr_t str, FILE *fp, unsigned long int n)
{
    char buffer[CSTR_INIT_SIZE];

    cstrUpdate(str, "");
    while (fgets(buffer, CSTR_INIT_SIZE-1, fp) != NULL && cstrLength(str) <= n)
	cstrConcatInPlaceChar(str, buffer);
    if (cstrLength(str) > n)
	cstrLeft(str, str, n);
}

void cstrImportCharN(cstr_t str, const char *s, unsigned long int n)
{
    if (! cstrGrow(str, n+1))
	return;
    memcpy(CSTR_STR(str), s, n);
    CSTR_STR(str)[n] = 0;
    CSTR_LEN(str) = n;
}

void cstrDecodeURL(cstr_t str1, cstr_t str2)
{
    cstrCopy(str1, str2);
    cstrDecodeURLInPlace(str1);
}

void cstrDecodeURLInPlace(cstr_t url)
{
    char *str = CSTR_STR(url);
    char *in = str, *out = str, c = 0;
    char decode_buffer[5] = { '0', 'x', 0, 0, 0 };

    if (! str)
	return;

    while ((c = *in++)) 
    {
	if (c == '%' && *in && *(in + 1)) {
	    decode_buffer[2] = *in++;
	    decode_buffer[3] = *in++;
	    c = (char)strtod(decode_buffer, (char**) NULL);
	} 
	else 
	    if (c == '+')
		c = ' ';
	*out++ = c;
    }
    
    *out = 0;
}

