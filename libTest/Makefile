include ../Makefile.inc

EXTRA_CFLAGS = -Wall -pedantic -pthread -std=gnu99
#$(shell pkg-config --cflags )
CFLAGS = -c $(OPTIM_FLAGS) $(EXTRA_CFLAGS) -D__libTest__ -o $@ $<
EXTRA_LIBS = -pthread
#$(shell pkg-config --libs )
LIBS = -lstdc++ $(EXTRA_LIBS)
SO_LFLAGS = -shared $(O) $(LIBS) $(OPTIM_FLAGS) -Wl,-soname,$(SOMAJ) -o $(SO)
OUT_LFLAGS = $< -lTest -ldl -pthread -o lib$@

LIBDIR ?= /usr/lib
PKGDIR = $(LIBDIR)/pkgconfig
INCDIR = /usr/include
BINDIR = /usr/bin

SO_STRIP = -x
OUT_STRIP = -s
SED = sed -e 's:@LIBDIR@:$(LIBDIR):g'

O = ArgsParser.o Memory.o StringFuncs.o Logger.o Core.o

VERMAJ = .0
VERMIN = .0
VERREV = .1
SOMAJ = libTest.so
SOMIN = $(SOMAJ)$(VERMAJ)
SOREV = $(SOMIN)$(VERMIN)
SO = $(SOREV)$(VERREV)
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
	$(call run-cmd,install_bin,$(EXE),$(BINDIR))

install-so:
	$(call run-cmd,install_file,$(SO),$(LIBDIR))
	@#(call run-cmd,install_file,$(PC),$(PKGDIR))
	$(call run-cmd,install_file,$(H),$(INCDIR))
	$(call run-cmd,ln,$(LIBDIR)/$(SO),$(LIBDIR)/$(SOREV))
	$(call run-cmd,ln,$(LIBDIR)/$(SOREV),$(LIBDIR)/$(SOMIN))
	$(call run-cmd,ln,$(LIBDIR)/$(SOMIN),$(LIBDIR)/$(SOMAJ))
	$(call run-cmd,ldconfig)

uninstall:
	rm $(LIBDIR)/$(SOMAJ)*
	rm $(LIBDIR)/$(A)
	rm $(LIBDIR)/$(LA)
	rm $(PKGDIR)/$(PC)

$(SO): $(O)
	$(call run-cmd,ar,$(A),$(O))
	$(call run-cmd,ranlib,$(A))
	$(call run-cmd,ccld,$(SO_LFLAGS))
	$(call debug-strip,$(SO_STRIP) $(SO))
	@sudo LIBDIR=$(LIBDIR) $(MAKE) install-so

$(PC): $(IN)
	$(call run-cmd,sed,$(IN),$(PC))

clean:
	$(call run-cmd,rm,$(EXE),$(EXE) $(patsubst %,%.o,$(OUT)))
	$(call run-cmd,rm,libTest,$(O) $(SOMAJ)* $(A) $(PC))

.o:
	$(call run-cmd,ccld,$(OUT_LFLAGS))
	$(call debug-strip, $(OUT_STRIP) lib$@)

.c.o:
	$(call run-cmd,cc,$(CFLAGS))

.PHONY: clean install all uninstall default .cpp.o

libTestMake: TestMake
libTest: Test

TestMake: TestMake.o
Test: Test.o

TestMake.o: TestMake.c
Test.o: Test.c
