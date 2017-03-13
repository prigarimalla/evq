CC=/usr/bin/gcc
CFLAGS=
CC_OPTS=-g
CC_LIBS=
CC_DEFINES=
CC_INCLUDES=
CC_ARGS=${CC_OPTS} ${CC_LIBS} ${CC_DEFINES} ${CC_INCLUDES}
DEPS = evq.h evq.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# clean is not a file
.PHONY=clean

#target "all" depends on all others
all: demo

demo: demo.c demo.o
	@${CC} ${CC_ARGS} -o demo demo.c evq.h evq.c
	
clean:
	@rm -f demo *.o

