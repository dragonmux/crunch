# This file is part of crunch
# Copyright © 2014-2017 Rachel Mant (dx-mon@users.sourceforge.net)
#
# crunch is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# crunch is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

include ../Makefile.inc

PKG_CONFIG_PKGS = crunch
EXTRA_CFLAGS = -Wall -Wextra -pedantic -pipe -std=c++11
CFLAGS = -c $(OPTIM_FLAGS) $(EXTRA_CFLAGS) $(EXTRA_FLAGS) -I../crunch -D__crunch_lib__ -o $@ $<
LIBS = $(shell pkg-config --libs crunch++)
ifneq ($(MAC), 1)
	EXTRA_FLAGS = -pthread
	SO_LFLAGS = $(OPTIM_FLAGS) -shared $(O) -pthread -lstdc++ -Wl,-soname,$(SOMAJ) -o $(SO)
else
	EXTRA_FLAGS =
	SO_LFLAGS = $(OPTIM_FLAGS) -shared $(O) -lstdc++ -o $(SO) -Wl,-U,_args
endif
OUT_LFLAGS = $(OPTIM_FLAGS) $< $(LIBS) -pthread -ldl -lstdc++ -lm -o $@

CRUNCHMAKE = crunchMake $(shell pkg-config --cflags --libs crunch++)
ifeq ($(BUILD_VERBOSE), 0)
	CRUNCHMAKE += -q
endif
quiet_cmd_crunchMake = -n
cmd_crunchMake = $(CRUNCHMAKE) -lcrunch++ $(2)
quiet_cmd_crunch = -n
cmd_crunch = $(CRUNCHpp) $(2)

LIBDIR ?= $(PREFIX)/lib
PKGDIR = $(LIBDIR)/pkgconfig
INCDIR = $(PREFIX)/include
BINDIR = $(PREFIX)/bin

SO_STRIP = -x
ifneq ($(MAC), 1)
	OUT_STRIP = -s
else
	OUT_STRIP = -Sx
endif
SED = sed -e 's:@LIBDIR@:$(LIBDIR):g' -e 's:@PREFIX@:$(PREFIX):g'

O = ArgsParser.o StringFuncs.o Logger.o Tester.o Core.o
TESTS = testArgsParser.so testCrunch++.so

VERMAJ = .0
VERMIN = .1
VERREV = .0
ifneq ($(MAC), 1)
	SOMAJ = libcrunch++.so
else
	SOMAJ = libcrunch++.dylib
endif
SOMIN = $(SOMAJ)$(VERMAJ)
SOREV = $(SOMIN)$(VERMIN)
SO = $(SOREV)$(VERREV)
A = libcrunch++.a
PC = crunch++.pc
IN = crunch++.pc.in
EXE = crunch++
H = crunch++.h

default: all

all: lib priv-install-so $(EXE)

$(BINDIR):
	$(call run-cmd,install_dir,$(BINDIR))

$(LIBDIR):
	$(call run-cmd,install_dir,$(LIBDIR))

$(INCDIR):
	$(call run-cmd,install_dir,$(INCDIR))

install: all $(BINDIR)
	$(call run-cmd,install_bin,$(EXE),$(BINDIR))

install-so: $(PC) $(LIBDIR) $(INCDIR)
	$(call run-cmd,install_file,$(SO),$(LIBDIR))
	$(call run-cmd,install_file,$(PC),$(PKGDIR))
	$(call run-cmd,install_file,$(H),$(INCDIR))
	$(call run-cmd,ln,$(LIBDIR)/$(SO),$(LIBDIR)/$(SOREV))
	$(call run-cmd,ln,$(LIBDIR)/$(SOREV),$(LIBDIR)/$(SOMIN))
	$(call run-cmd,ln,$(LIBDIR)/$(SOMIN),$(LIBDIR)/$(SOMAJ))
	$(call ldconfig)

priv-install-so:
	$(Q)if [[ $(UID) -ne 0 && "$(PREFIX)" =~ ^/usr\.*\$$ ]]; then \
		sudo PREFIX=$(PREFIX) LIBDIR=$(LIBDIR) $(MAKE) install-so; \
	else \
		$(MAKE) install-so; \
	fi

uninstall:
	rm $(LIBDIR)/$(SOMAJ)*
	rm $(LIBDIR)/$(A)
	rm $(PKGDIR)/$(PC)

lib: $(SO)

$(SO): $(O)
	$(call run-cmd,ar,$(A),$(O))
	$(call run-cmd,ranlib,$(A))
	$(call run-cmd,ccld,$(SO_LFLAGS))
	$(call debug-strip,$(SO_STRIP) $(SO))

$(PC): $(IN)
	$(call run-cmd,sed,$(IN),$(PC))

test: $(TESTS)

check: test
	$(call run-cmd,crunch,$(subst .so,,$(TESTS)))

clean:
	$(call run-cmd,rm,$(EXE),$(EXE) $(patsubst %,%.o,$(EXE)))
	$(call run-cmd,rm,crunch,$(O) $(SOMAJ)* $(A) $(PC))
	$(call run-cmd,rm,tests,$(TESTS))

.o:
	$(call run-cmd,ccld,$(OUT_LFLAGS))
	$(call debug-strip,$(OUT_STRIP) $@)

.cpp.o:
	$(call run-cmd,cxx,$(CFLAGS))

%.so: %.cpp
	$(call run-cmd,crunchMake,$<)

.PHONY: test clean install priv-install-so install-so all uninstall default .cpp.o $(PC)

crunch++: crunch++.o

crunch++.o: crunch++.cpp
