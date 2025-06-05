CC=gcc

UTIL_LIB_DIR=../libutil
UTIL_INC_DIR=../libutil

POPT_LIB_DIR=../libpopt
POPT_INC_DIR=../libpopt

DIAG_LIB_DIR=../libdiag
DIAG_INC_DIR=../libdiag

OPT_FLAGS=-O2
C_INCLUDES=-I $(UTIL_INC_DIR) \
           -I $(DIAG_INC_DIR) \
           -I $(POPT_INC_DIR) \
           -I .

CFLAGS = $(C_INCLUDES) $(OPT_FLAGS)
LDFLAGS = -L $(UTIL_LIB_DIR) -L $(POPT_LIB_DIR) -L $(DIAG_LIB_DIR) \
          -lpopt -ldiag -lutil

all: cpp18

cpp18: scan.c cpplib.c
	$(CC) $(CFLAGS) -o $@ scan.c cpplib.c $(LDFLAGS)

scan.c: scan.l
	flex -oscan.c scan.l

clean:
	rm -f scan.c
	rm -f *.o
	rm -f cpp18
	rm -f cpp18.exe
