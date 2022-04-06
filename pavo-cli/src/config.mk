#flags
CPPSTD = -std=c++20
WFLAGS = -Wall -Wextra -Wpedantic
DEBUGFLAGS = ${CPPSTD} ${WFLAGS} -Og -march=native -fno-rtti
RELEASEFLAGS = ${CPPSTD} ${WFLAGS} -Os -march=native -flto -fno-rtti -fno-exceptions

#libs
LIBS = -lfmt

#compiler
CPPC = g++
