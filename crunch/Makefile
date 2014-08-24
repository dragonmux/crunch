# This file is part of crunch
# Copyright © 2013 Rachel Mant (dx-mon@users.sourceforge.net)
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

EXTRA_CFLAGS = -Wall -Wextra -pedantic -pthread -std=gnu99
#$(shell pkg-config --cflags )
CFLAGS = -c $(OPTIM_FLAGS) $(EXTRA_CFLAGS) -D__crunch_lib__ -o $@ $<
EXTRA_LIBS = -pthread
#$(shell pkg-config --libs )
LIBS = -lstdc++ $(EXTRA_LIBS)
SO_LFLAGS = -shared $(O) $(LIBS) $(OPTIM_FLAGS) -Wl,-soname,$(SOMAJ) -o $(SO)
OUT_LFLAGS = $< $(shell pkg-config --libs crunch) -ldl -pthread -o $@

quiet_cmd_crunchMake = -n
cmd_crunchMake = $(CRUNCHMAKE) $(2)
quiet_cmd_crunch = -n
cmd_crunch = $(CRUNCH) $(2)

LIBDIR ?= $(PREFIX)/lib
PKGDIR = $(LIBDIR)/pkgconfig
INCDIR = $(PREFIX)/include
BINDIR = $(PREFIX)/bin

SO_STRIP = -x
OUT_STRIP = -s
SED = sed -e 's:@LIBDIR@:$(LIBDIR):g' -e 's:@PREFIX@:$(PREFIX):g'

O = ArgsParser.o Memory.o StringFuncs.o Logger.o Core.o
TESTS = testCrunch.so

VERMAJ = .0
VERMIN = .0
VERREV = .1
SOMAJ = libcrunch.so
SOMIN = $(SOMAJ)$(VERMAJ)
SOREV = $(SOMIN)$(VERMIN)
SO = $(SOREV)$(VERREV)
A = libcrunch.a
PC = crunch.pc
IN = crunch.pc.in
OUT = Test TestMake
EXE = crunch crunchMake
H = crunch.h

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
	$(call run-cmd,crunch,$(subst .so,,$(TESTS)))

clean:
	$(call run-cmd,rm,$(EXE),$(EXE) $(patsubst %,%.o,$(OUT)))
	$(call run-cmd,rm,crunch,$(O) $(SOMAJ)* $(A) $(PC))
	$(call run-cmd,rm,tests,$(TESTS))

.o:
	$(call run-cmd,ccld,$(OUT_LFLAGS))
	$(call debug-strip,$(OUT_STRIP) $@)

.c.o:
	$(call run-cmd,cc,$(CFLAGS))

%.so: %.c
	$(call run-cmd,crunchMake,$<)

.PHONY: clean install all uninstall default .c.o $(PC)

crunchMake: crunchMake.o
crunch: crunch.o

crunchMake.o: crunchMake.c
crunchMake.o: CFLAGS += -Dcrunch_GCC="\"$(GCC)\""
crunch.o: crunch.c
