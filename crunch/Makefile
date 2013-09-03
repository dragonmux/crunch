include ../Makefile.inc

EXTRA_CFLAGS = -Wall -pedantic -pthread -std=gnu99
#$(shell pkg-config --cflags )
CFLAGS = -c $(OPTIM_FLAGS) $(EXTRA_CFLAGS) -D__libTest__ -o $@ $<
EXTRA_LIBS = -pthread
#$(shell pkg-config --libs )
LIBS = -lstdc++ $(EXTRA_LIBS)
SO_LFLAGS = -shared $(O) $(LIBS) $(OPTIM_FLAGS) -Wl,-soname,$(SOMAJ) -o $(SO)
OUT_LFLAGS = $< $(shell pkg-config --libs libTest) -ldl -pthread -o lib$@

quiet_cmd_testMake = -n
cmd_testMake = $(LIBTESTMAKE) $(2)
quiet_cmd_test = -n
cmd_test = $(LIBTEST) $(2)

LIBDIR ?= $(PREFIX)/lib
PKGDIR = $(LIBDIR)/pkgconfig
INCDIR = $(PREFIX)/include
BINDIR = $(PREFIX)/bin

SO_STRIP = -x
OUT_STRIP = -s
SED = sed -e 's:@LIBDIR@:$(LIBDIR):g' -e 's:@PREFIX@:$(PREFIX):g'

O = ArgsParser.o Memory.o StringFuncs.o Logger.o Core.o
TESTS = testLibTest.so

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

all: $(PC) $(SO) $(EXE)

$(BINDIR):
	$(call run-cmd,install_dir,$(BINDIR))

$(LIBDIR):
	$(call run-cmd,install_dir,$(LIBDIR))

$(INCDIR):
	$(call run-cmd,install_dir,$(INCDIR))

install: all $(BINDIR)
	$(call run-cmd,install_bin,$(EXE),$(BINDIR))

install-so: $(LIBDIR) $(INCDIR)
	$(call run-cmd,install_file,$(SO),$(LIBDIR))
	$(call run-cmd,install_file,$(PC),$(PKGDIR))
	$(call run-cmd,install_file,$(H),$(INCDIR))
	$(call run-cmd,ln,$(LIBDIR)/$(SO),$(LIBDIR)/$(SOREV))
	$(call run-cmd,ln,$(LIBDIR)/$(SOREV),$(LIBDIR)/$(SOMIN))
	$(call run-cmd,ln,$(LIBDIR)/$(SOMIN),$(LIBDIR)/$(SOMAJ))
	$(call ldconfig)

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
	$(Q)if [[ $(UID) -ne 0 && "$(PREFIX)" =~ ^/usr\.*\$$ ]]; then \
		sudo PREFIX=$(PREFIX) LIBDIR=$(LIBDIR) $(MAKE) install-so; \
	else \
		$(MAKE) install-so; \
	fi

$(PC): $(IN)
	$(call run-cmd,sed,$(IN),$(PC))

check: $(TESTS)
	$(call run-cmd,test,$(subst .so,,$(TESTS)))

clean:
	$(call run-cmd,rm,$(EXE),$(EXE) $(patsubst %,%.o,$(OUT)))
	$(call run-cmd,rm,libTest,$(O) $(SOMAJ)* $(A) $(PC))
	$(call run-cmd,rm,tests,$(TESTS))

.o:
	$(call run-cmd,ccld,$(OUT_LFLAGS))
	$(call debug-strip, $(OUT_STRIP) lib$@)

.c.o:
	$(call run-cmd,cc,$(CFLAGS))

%.so: %.c
	$(call run-cmd,testMake,$<)

.PHONY: clean install all uninstall default .cpp.o $(PC)

libTestMake: TestMake
libTest: Test

TestMake: TestMake.o
Test: Test.o

TestMake.o: TestMake.c
TestMake.o: CFLAGS += -DlibTest_GCC="\"$(GCC)\""
Test.o: Test.c
