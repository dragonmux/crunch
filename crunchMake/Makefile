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

EXTRA_CFLAGS = -Wall -Wextra -pedantic -pthread -std=c++11 -I../crunch++ -I../crunch
CFLAGS = -c $(OPTIM_FLAGS) $(EXTRA_CFLAGS) -D__crunch_lib__ -o $@ $<
LIBS = $(shell pkg-config --libs crunch++ crunch) -lstdc++ -ldl -pthread
LFLAGS = $(O) $(LIBS) -o $@

BINDIR = $(PREFIX)/bin
STRIP += -s
O = crunchMake.o
EXE = crunchMake

default: all

all: $(EXE)

$(BINDIR):
	$(call run-cmd,install_dir,$(BINDIR))

install: all $(BINDIR)
	$(call run-cmd,install_bin,$(EXE),$(BINDIR))

clean:
	$(call run-cmd,rm,$(EXE),$(EXE) $(O))

.o:
	$(call run-cmd,ccld,$(LFLAGS))
	$(call debug-strip,$@)

.cpp.o:
	$(call run-cmd,cxx,$(CFLAGS))

.PHONY: 

crunchMake: crunchMake.o
crunchMake.o: crunchMake.cpp
crunchMake.o: CFLAGS += -Dcrunch_GCC="\"$(GCC)\""
