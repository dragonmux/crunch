GCC ?= gcc
GCC_VER = $(shell gcc -dumpversion | cut -d . -f 1)
ifeq ($(shell if [ $(GCC_VER) -ge 4 ]; then echo 1; else echo 0; fi), 1)
GCC_FLAGS = -fvisibility=hidden -fvisibility-inlines-hidden
else
GCC_FLAGS = 
endif
CC = $(GCC) $(GCC_FLAGS)
ifeq ($(strip $(DEBUG)), 1)
OPTIM_FLAGS = -ggdb
DEBUG = 1
else
OPTIM_FLAGS = -O2
DEBUG = 0
endif
EXTRA_CFLAGS = -pthread
#$(shell pkg-config --cflags )
CFLAGS = -c $(OPTIM_FLAGS) -Wall $(EXTRA_CFLAGS) -D__libTest__ -o $@
EXTRA_LIBS = -pthread
#$(shell pkg-config --libs )
LIBS = -lstdc++ $(EXTRA_LIBS)
SO_LFLAGS = -shared $(O) $(LIBS) $(OPTIM_FLAGS) -Wl,-soname,$(SOMAJ) -o $(SO)
OUT_LFLAGS = $< -lTest -ldl -o lib$@
AR = ar cr
RANLIB = ranlib
LN = ln -sfT
LIBDIR ?= /usr/lib
PKGDIR = $(LIBDIR)/pkgconfig
INCDIR = /usr/include
BINDIR = /usr/bin
INSTALL = install -m644
INSTALL_BIN = install -m755
SO_STRIP = strip -x
OUT_STRIP = strip --strip-unneeded

O = ArgsParser.o Memory.o StringFuncs.o Logger.o Core.o

VERMAJ = .0
VERMIN = .0
VERREV = .1
SO = $(SOREV)$(VERREV)
SOREV = $(SOMIN)$(VERMIN)
SOMIN = $(SOMAJ)$(VERMAJ)
SOMAJ = libTest.so
A = libTest.a
LA = libTest.la
PC = libTest.pc
IN = libTest.pc.in
OUT = Test TestMake
EXE = libTest libTestMake
H = libTest.h

default: all

all: $(SO) $(EXE)

install: all
	$(INSTALL_BIN) $(OUT) $(EXE)
	$(INSTALL_BIN) $(OUT) $(BINDIR)/$(EXE)

install-so:
	$(INSTALL) $(SO) $(LIBDIR)
	#$(INSTALL) $(PC) $(PKGDIR)
	$(INSTALL) $(H) $(INCDIR)
	$(LN) $(LIBDIR)/$(SO) $(LIBDIR)/$(SOREV)
	$(LN) $(LIBDIR)/$(SOREV) $(LIBDIR)/$(SOMIN)
	$(LN) $(LIBDIR)/$(SOMIN) $(LIBDIR)/$(SOMAJ)
	ldconfig

uninstall:
	rm $(LIBDIR)/$(SOMAJ)*
	rm $(LIBDIR)/$(A)
	rm $(LIBDIR)/$(LA)
	rm $(PKGDIR)/$(PC)

$(SO): $(O)
	rm -f $(PC)
	$(AR) $(A) $(O)
	$(RANLIB) $(A)
	$(GCC) $(SO_LFLAGS)
	if [ $(DEBUG) -eq 0 ]; then $(SO_STRIP) $(SO); fi
	sudo LIBDIR=$(LIBDIR) $(MAKE) install-so

$(PC): $(IN)
	sed -e 's:@LIBDIR@:$(LIBDIR):g' $(IN) > $(PC)

clean:
	rm -f *.o *.so* *.a *.pc $(EXE)

.o:
	$(GCC) $(OUT_LFLAGS)
	if [ $(DEBUG) -eq 0 ]; then $(OUT_STRIP) lib$@; fi

.c.o:
	$(GCC) $(CFLAGS) $<

.PHONY: clean install all uninstall default .cpp.o

libTestMake: TestMake
libTest: Test

TestMake: TestMake.o
Test: Test.o

TestMake.o: TestMake.c
Test.o: Test.c

Logger.o: Logger.c
StringFuncs.o: StringFuncs.c
Memory.o: Memory.c
ArgsParser.o: ArgsParser.c
Core.o: Core.c
