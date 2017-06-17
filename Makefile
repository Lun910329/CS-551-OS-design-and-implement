#
# Refined by syscl/lighting/Yating Zhou for assignment 1
#

#
# default compiler on minix 3.2.x is clang instead of gcc
#
CC=clang
CFLAGS=-I.
OBJECTS=main.o mshell.o mstack.o malias.o mprofile.o errdef.o syscl_lib.o history.o
#
# final product
#
PRODUCT=msh

install:
	$(CC) -o main.o -c main.c
	$(CC) -o mshell.o -c mshell.c
	$(CC) -o mstack.o -c mstack.c
	$(CC) -o malias.o -c malias.c
	$(CC) -o mprofile.o -c mprofile.c
	$(CC) -o errdef.o -c errdef.c
	$(CC) -o syscl_lib.o -c syscl_lib.c
	$(CC) -o history.o -c history.c
	$(CC) $(CFLAGS) -o $(PRODUCT) $(OBJECTS)

clean:
	rm $(PRODUCT) $(OBJECTS)
