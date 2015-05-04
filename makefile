# Change these to suit your system
CC      	= g++
IFLAGS  	= -I/usr/include/
LFLAGS   = -L/usr/lib
CFLAGS   = -Wall -Wno-reorder -Wno-sign-compare

# Flags for optimized compilation
CFLAGS_O = -O3 -fopenmp $(CFLAGS)
LIBS_O   = -lnetcdf_c++

# Flags for debug compilation
CFLAGS_D = -g -pg -rdynamic -fprofile-arcs -ftest-coverage -coverage -DDEBUG $(CFLAGS)
LIBS_D   = -lnetcdf_c++ -L build/gtest -lgtest


# Don't change below here
SRCDIR   = src/
BUILDDIR = build
BUILDDIR_O = build/opt
BUILDDIR_D = build/debug
EXE_O    = gridpp
EXE_D    = gridpp_debug

PREFIX = $(DESTDIR)/usr
BINDIR = $(PREFIX)/bin

CORESRC 	= $(wildcard src/*.cpp)
CALSRC  	= $(wildcard src/Calibrator/*.cpp)
FILESRC 	= $(wildcard src/File/*.cpp)
DOWNSRC 	= $(wildcard src/Downscaler/*.cpp)
DRVSRC  	= src/Driver/Gpp.cpp
DRVOBJ_O = $(BUILDDIR_O)/Driver/Gpp.o
DRVOBJ_D	= $(BUILDDIR_D)/Driver/Gpp.o
SRC     	= $(CORESRC) $(CALSRC) $(FILESRC) $(DOWNSRC)
HEADERS 	= $(SRC:.cpp=.h)
OBJ0_O   = $(patsubst src/%,$(BUILDDIR_O)/%,$(SRC))
OBJ0_D  	= $(patsubst src/%,$(BUILDDIR_D)/%,$(SRC))
OBJ_O   	= $(OBJ0_O:.cpp=.o)
OBJ_D   	= $(OBJ0_D:.cpp=.o)
TESTSRC 	= $(wildcard src/Testing/*.cpp)
TESTS0  	= $(patsubst src/Testing%,testing%,$(TESTSRC))
TESTS   	= $(TESTS0:.cpp=.exe)
INCS    	= makefile $(HEADERS)

.PHONY: tags count coverage doxygen test

default: $(EXE_O)

debug: $(EXE_D)

$(BUILDDIR):
	@mkdir build build/Calibrator build/Downscaler build/File build/Driver build/Testing

$(BUILDDIR_O)/%.o : src/%.cpp $(INCS)
	$(CC) $(CFLAGS_O) $(IFLAGS) -c $< -o $@

$(BUILDDIR_D)/%.o : src/%.cpp $(INCS)
	$(CC) $(CFLAGS_D) $(IFLAGS) -c $< -o $@

$(BUILDDIR_D)/%.E : src/%.cpp $(INCS)
	$(CC) $(CFLAGS_D) $(IFLAGS) -c $< -o $@ -E

gridpp: $(OBJ_O) $(DRVOBJ_O) makefile
	$(CC) $(CFLAGS_O) $(LFLAGS) $(OBJ_O) $(DRVOBJ_O) $(LIBS_O) -o $@

gridpp_debug: $(OBJ_D) $(DRVOBJ_D) makefile gtest
	$(CC) $(CFLAGS_D) $(LFLAGS) $(OBJ_D) $(DRVOBJ_D) $(LIBS_D) -o $@

test: gtest $(TESTS)
	./runAllTests.sh

testing/%.exe: $(BUILDDIR_D)/Testing/%.o $(INCS) $(OBJ_D) gtest
	$(CC) $(CFLAGS_D) $(OBJ_D) $< $(LFLAGS) $(LIBS_D) -o $@

count:
	@wc src/*.h src/*.cpp src/*/*.h src/*/*.cpp -l | tail -1

clean: 
	rm -rf build/*/*.o build/*/*/*.o build/*/*.E build/*/*/*.E gmon.out $(EXE) testing/*.exe\
		*.gcno build/*/*.gcda build/*/*.gcno build/*/*/*.gcda build/*/*/*.gcno\
		coverage/* coverage.* build/gtest gridpp

tags:
	ctags -R --c++-kinds=+pl --fields=+iaS --extra=+q -f tags ./*.h ./*.cpp */*.h */*.cpp

install:
	install -D gridpp $(BINDIR)/gridpp

doxygen:
	doxygen doxygen/config

coverage: test
	#rm -f build/*.gcno build/*.gcda build/*/*.gcno build/*/*.gcda
	lcov -b . -c -i -d . -o coverage.init
	./runAllTests.sh
	lcov -b . -c -d . -o coverage.run
	lcov -a coverage.init -a coverage.run -o coverage.total
	lcov -e coverage.total "`pwd`/*" -o coverage.total.filtered
	lcov -r coverage.total.filtered "`pwd`/*Testing*" -o coverage.info
	genhtml -o ./coverage/ coverage.info

depend:
	makedepend -- $(CFLAGS) -- $(SRC)

gtest: build/gtest/libgtest.a

build/gtest/libgtest.a: /usr/src/gtest
	mkdir -p build/gtest
	cd build/gtest && cmake /usr/src/gtest && cmake --build .
