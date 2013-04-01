#
# AtratoCLI Makefile
#

CFILES		:= $(wildcard *.c)
OBJECTS		:= $(CFILES:.c=.o)
OBJDIR		:= ../.obj
INCDEPS		:= $(wildcard *.h)
OBJS		:= $(patsubst %,$(OBJDIR)/%,$(OBJECTS))

.PHONY: all clean

${OBJDIR}/%.o:	%.c ${INCDEPS} ${MAININCDEPS}
		@echo CC $<
		@${CC} ${CFLAGS} $< -c -o $@

${PROG}:	${OBJS}
		@echo LINK $@
		@${CC} $^ ${LDFLAGS} -g -o ../$@

all:		${PROG}

clean:
		- @rm ../${PROG} ${OBJS} >/dev/null 2>&1

