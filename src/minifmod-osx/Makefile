
SRCS = example.c
OBJS = $(SRCS:.c=.o)

ifeq ($(PLATFORM),)
 PLATFORM = MACOSX
endif

export PLATFORM

ifeq ($(PLATFORM),MACOSX)
CC = gcc
CFLAGS = -Ilib -arch i386 \
	-Oz \
	-fno-common \
	\
	-DNO_FREE \
	-DNO_STOPSONG
LDFLAGS = -arch i386 \
	 \
	-Oz \
	-flat_namespace \
	-s \
	\
	lib/libminifmod-macosx.a \
	-framework OpenAL
NAME = example
endif
ifeq ($(PLATFORM),W32)
CC = mingw32-gcc
CFLAGS = -Ilib -Os \
	-DNO_FREE \
	-DNO_STOPSONG
LDFLAGS = \
	lib/libminifmod-w32.a \
	-lwinmm
NAME = example.exe
endif
ifeq ($(PLATFORM),LNX)
CC = gcc
CFLAGS = -Ilib -Os \
	-fno-common \
	-DNO_FREE \
	-DNO_STOPSONG
LDFLAGS = \
	lib/libminifmod-lnx.a \
	-lpthread -lm
NAME = example
endif

.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

all: $(OBJS)
	$(MAKE) -C lib
	$(CC) -o $(NAME) $(OBJS) $(LDFLAGS)

clean:
	$(MAKE) -C lib clean
	rm -f $(OBJS) example example.exe

pack:
	iPakk --lzma-fb-min 5 -i example -o example.packed --lzma-filter-off
