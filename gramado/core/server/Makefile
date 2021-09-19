
# gws - Gramado Window Server.
# 2020 - Created by Fred Nora.

# todo:
# Os aplicativos em /init/core ainda usam PE, assim como o base kernel.
# É desejável que isso mude.

VERSION = 0
PATCHLEVEL = 1
SUBLEVEL = 0
EXTRAVERSION = 
#NAME = GWS

# Make variables (CC, etc...)
AS	= as
LD	= ld
CC	= gcc
AR	= ar
MAKE	= make
NASM	= nasm
OBJCOPY	= objcopy
OBJDUMP	= objdump
LEX	= flex
YACC	= bison
PERL	= perl
PYTHON	= python
PYTHON2	= python2
PYTHON3	= python3
RUBY	= ruby


# usermode cross compiler flags.
user_ccflags = -m64 \
	-std=gnu89 \
	-nodefaultlibs \
	-nostdinc \
	-nostdlib \
	-static \
	-fgnu89-inline \
	-ffreestanding \
	-fno-builtin \
	-fno-pie \
	-fleading-underscore \
	-fno-stack-protector \
	-s \
	-Werror=strict-prototypes    


# usermode linker flags.
user_ldflags = -m elf_x86_64 -T link_r0.ld


#todo
#path longo. vamos mudar  as libs de lugar para gramado/libs.

LIBC              = ../../../com/lib/rtl/include/
LIBCOBJ           = ../../../com/lib/rtl/obj
WS_INCLUDES       = include/ 
WS_INCLUDES_LOCAL = .


#
# Objects
#


myObjects = crt0.o main.o rtl.o \
ctype.o stdio.o stdlib.o string.o conio.o socket.o signal.o unistd.o \
r_render.o \
bitblt.o line.o rect.o xxxinput.o char.o dtext.o window.o gws.o comp.o wm.o \
grprim.o sprite.o demos.o bmp.o connect.o button.o font.o\
termios.o ioctl.o fcntl.o    


.PHONY: all all-c gws-link finalize clean

all: main.o all-c gws-link finalize clean
	@echo "Ok?"

main.o:

	$(CC)  -c  main.c $(user_ccflags) \
	-I $(LIBC) \
	-I $(API02) \
	-I $(WS_INCLUDES) \
	-I $(WS_INCLUDES_LOCAL) \
	-o main.o
	
	
all-c:

#fb
	$(CC)  -c  fb/bitblt.c    $(user_ccflags) -I $(LIBC) -I include/ -I . -I fb/  -o bitblt.o

#os
	$(CC)  -c  os/connect.c  $(user_ccflags) -I $(LIBC) -I include/ -I . -I os/  -o connect.o

#ws
	$(CC)  -c  ws/r_render.c  $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o r_render.o
	$(CC)  -c  ws/char.c      $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o char.o
	$(CC)  -c  ws/dtext.c     $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o dtext.o
	$(CC)  -c  ws/line.c      $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o line.o
	$(CC)  -c  ws/rect.c      $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o rect.o
	$(CC)  -c  ws/xxxinput.c  $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o xxxinput.o
	$(CC)  -c  ws/window.c    $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o window.o
	$(CC)  -c  ws/button.c    $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o button.o
	$(CC)  -c  ws/comp.c      $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o comp.o
	$(CC)  -c  ws/bmp.c       $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o bmp.o
	$(CC)  -c  ws/grprim.c    $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o grprim.o
	$(CC)  -c  ws/sprite.c    $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o sprite.o
	$(CC)  -c  ws/demos.c     $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o demos.o
	$(CC)  -c  ws/font.c     $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o font.o
	$(CC)  -c  ws/gws.c      $(user_ccflags) -I $(LIBC) -I include/ -I . -I ws/  -o gws.o


	$(CC)  -c  wm.c   $(user_ccflags) -I $(LIBC) -I include/ -I .  -o wm.o



#
# == imported objects =============================
#


	# rtl/
	cp $(LIBCOBJ)/crt0.o .
	cp $(LIBCOBJ)/rtl.o .

	cp $(LIBCOBJ)/ctype.o .
	cp $(LIBCOBJ)/stdio.o .
	cp $(LIBCOBJ)/stdlib.o .
	cp $(LIBCOBJ)/string.o .
	cp $(LIBCOBJ)/conio.o .
	cp $(LIBCOBJ)/unistd.o .
	cp $(LIBCOBJ)/socket.o .
	cp $(LIBCOBJ)/signal.o .
	cp $(LIBCOBJ)/termios.o .
	cp $(LIBCOBJ)/ioctl.o .
	cp $(LIBCOBJ)/fcntl.o .


gws-link:
	$(LD) $(user_ldflags) -o GWSSRV.BIN $(myObjects)
#	$(LD) -m elf_i386 -T ../link.ld -o GWSSRV.BIN $(myObjects)  

finalize:
	cp GWSSRV.BIN ../../bin

clean:
	-rm *.o
	-rm GWSSRV.BIN 
	
	
