.DEFAULT_GOAL := debug

include config.mk

SRC = main.cpp
LINENOISESRC = ../../thirdparty/linenoise/linenoise.c

clean:
	rm -f pavo

pavo:
	${CC} ${LINENOISESRC} -o linenoise.o -c
	${CPPC} ${RELEASEFLAGS} ${SRC} linenoise.o -o pavo ${LIBS}

debug:
	${CC} ${LINENOISESRC} -o linenoise.o -c
	${CPPC} ${DEBUGFLAGS} ${SRC} linenoise.o -o pavo ${LIBS}

.PHONY: clean pavo debug
