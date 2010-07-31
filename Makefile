CC = gcc
CFLAGS = -pedantic -Wall -O2 -funroll-loops
MAJOR = 0
MINOR = 1

LIB_NAME = libcstr.so.$(MAJOR).$(MINOR)

$(LIB_NAME): cstr.o
	$(CC) -shared -Wl,-soname,libcstr.so.$(MAJOR) -o libcstr.so.$(MAJOR).$(MINOR) cstr.o

cstr.o: cstr.h cstr.c
	$(CC) $(CFLAGS) -fPIC -c -o cstr.o cstr.c

install: cstr.o
	cp libcstr.so.$(MAJOR).$(MINOR) /usr/lib/
	cp cstr.h /usr/include/
	ln -fs /usr/lib/libcstr.so.$(MAJOR).$(MINOR) /usr/lib/libcstr.so.$(MAJOR)
	ln -fs /usr/lib/libcstr.so.$(MAJOR) /usr/lib/libcstr.so

uninstall:
	rm -fv /usr/lib/libcstr.so.$(MAJOR).$(MINOR)
	rm -fv /usr/lib/libcstr.so.$(MAJOR)
	rm -fv /usr/lib/libcstr.so
	rm -fv /usr/include/cstr.h

clean:
	rm -f *.o *.so* 

