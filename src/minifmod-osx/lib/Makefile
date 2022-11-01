# ============================================================================
#  Makefile for Linux version of MiniFMOD
# ============================================================================

# = LIBRARY DEFINITIONS ======================================================

LIBPATH = .
LIBGCC = $(LIBPATH)/libminifmod$(EXT).a

ifeq ($(PLATFORM),)
 PLATFORM = MACOSX
endif

OBJS = fmusic.o \
	mixer_fpu_ramp.o \
	mixer_clipcopy.o \
	music_formatxm.o 

ifeq ($(PLATFORM),W32)
 CC     = mingw32-gcc
 AR     = mingw32-ar

 P      = -D__WIN32__ -I/usr/local/mingw32/mingw32/include
 NFLAGS	= -f win32 $(P) 
 EXT    = -w32
 
 OBJS += fsound-winmm.o mixer_fpu_ramp_nasm.o 
 
 CFLAGS = -Os
endif
ifeq ($(PLATFORM),LNX)
 CC     = gcc
 AR     = ar

 P      = -D__LINUX__
 NFLAGS = -f elf $(P) 
 EXT    = -lnx

 OBJS += fsound-oss.o mixer_fpu_ramp_nasm.o
 CFLAGS = -Os
endif
ifeq ($(PLATFORM),MACOSX)
 CC     = gcc
 AR     = ar

 P      = -D__MACOSX__
 EXT    = -macosx

 OBJS += fsound-openal.o mixer_fpu_ramp_asm.o 
# OBJS += fsound-disk.o
# CFLAGS = -arch ppc
 CFLAGS = -arch i386 -Oz \
	-fno-common
endif

CFLAGS	+= -g -c $(P) -D__COMPILE_MINIFMOD__ -Wall \
	-DNO_FREE \
	-DNO_ERRORCHECK \
	-DNO_SAMPLECALLBACK \
	-DNO_PLAYCALLBACK \
	-DNO_STOPSONG \
	-DVOLUMERAMPING \
	-UROLLED

#CFLAGS += \
#	-DNO_GETORDER \
#	-DNO_GETROW \
#	-DNO_GETTIME

AFLAGS   = rcs

# = MAKEFILE =================================================================

.SUFFIXES: .c .o .cpp .asm .s

.c.o:
	$(CC) $(CFLAGS) -o $@ $<

.asm.o:
	nasm $(NFLAGS) -o $@ $<

.s.o:
	$(CC) -c $(CFLAGS) -o $@ $<

%.o : %.cpp
	$(CC) $(CFLAGS) $<

all : $(LIBGCC)

# build library using GNU AR
$(LIBGCC) : $(OBJS)
	rm -f $@
	$(AR) $(AFLAGS) $@ $(OBJS)

test : $(LIB) main.o
	$(CC) main.o $(LFLAGS) -o $(TEST)

#ifeq ($(PLATFORM),LNX)
#mixer_fpu_ramp_asm.o : mixer_fpu_ramp_asm.s 
#	$(CC) -c -o mixer_fpu_ramp_asm.o mixer_fpu_ramp_asm.s -g
#else
#mixer_fpu_ramp_asm.o : mixer_fpu_ramp_asm.s
#	$(CC) -c -o mixer_fpu_ramp_asm.o mixer_fpu_ramp_asm.s -g
#endif

#ifeq ($(PLATFORM),W32)
#mixer_fpu_ramp_asm.inc : genasminc
#	./genasminc > mixer_fpu_ramp_asm.inc
#	
#genasminc : genasminc.c
#	gcc $(CFLAGS) -s -O3 genasminc.c -o genasminc.o
#	gcc -s genasminc.o -o genasminc
#endif

clean:
	rm -f *.o *.a genasminc
