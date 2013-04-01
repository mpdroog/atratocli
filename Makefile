#
# AtratoCLI Makefile
#

PROG		:= atraticli

DEBUGOPTS	:=
OBJDIR		:= .obj
CC		:= gcc
AR		:= ar
PKGLIBS		:= sqlite3 libcurl

#WARNINGS	:= -Wcast-qual -Wwrite-strings -Wall -Wshadow -Wuninitialized
CFLAGS		:= `pkg-config --cflags $(PKGLIBS)` $(DEBUGOPTS) $(WARNINGS) -std=c99
LDFLAGS		:= `pkg-config --libs $(PKGLIBS)`

export PROG PROGLIB CFLAGS LDFLAGS CC AR

.PHONY: all clean ${PROG}-clean ${PTOG}

all:		${OBJDIR} ${PROG}

${OBJDIR}:
		-@mkdir .obj >/dev/null 2>&1

${OBJDIR}/%.o:	%.c
		${CC} ${CFLAGS} $^ -c -o $@

clean:		${PROG}-clean
		- @rm -rf test$(PROG) core ${OBJDIR} >/dev/null 2>&1

tags:		lib/*.[ch] nnd/*.[ch] nnd/classes/*.[ch]*
		@ctags lib/* nnd/* nnd/classes/* 2>/dev/null

${PROG}:	AtratoCLI/*.[ch]
		@cd AtratoCLI ; $(MAKE)

${PROG}-clean:
		@cd AtratoCLI ; $(MAKE) clean

