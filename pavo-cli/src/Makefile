.DEFAULT_GOAL := debug

include config.mk

SRC = main.cpp

clean:
	rm -f pavo

pavo:
	${CPPC} ${RELEASEFLAGS} ${SRC} -o pavo ${LIBS}

debug:
	${CPPC} ${DEBUGFLAGS} ${SRC} -o pavo ${LIBS}

.PHONY: clean pavo debug
