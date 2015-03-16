**libcstr** is a C library for string minipulation.
It implements a string data type which aims to:
  * significantly simply string manipulation in C;
  * make the usage of strings in C as powerful as with C++, Java, Python, and other high-level languages;
  * avoid buffer overflow/overrun problems and that result from erroneous usage of the typical C char `*` arrays;
  * maintain some degree of compatibility with code that uses '\0' terminated char arrays;
  * implement a user-friendly interface which makes operations such as search, copy, concatenation, replace, find sub-strings, etc. readily available.


The code is stable and tested under Linux and OpenSolaris with several versions of GCC (3.4 - 4.3).

It has been heavily used on projects involving database connections, parsing SQL queries, concatenating strings, replacing chars, etc.