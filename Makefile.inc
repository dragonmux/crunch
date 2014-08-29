# Makefile include for crunch. Set V to something other than 0 for more detailed build output.

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

ifeq ($(MAKELEVEL), 0)

# Silence the tools
ifeq ("$(origin V)", "command line")
	BUILD_VERBOSE = $(V)
endif
ifndef BUILD_VERBOSE
	BUILD_VERBOSE = 0
endif

MAKEFLAGS += -w

ifeq ($(BUILD_VERBOSE), 0)
	quiet=quiet_
	Q=@
else
	quiet=
	Q=
endif
ifneq ($(findstring s,$(MAKEFLAGS)),)
	quiet=silent_
endif

export quiet Q BUILD_VERBOSE

endif

quiet_cmd_cc = " CC    $@"
cmd_cc = $(GCC) $(2)
quiet_cmd_cxx = " CXX   $@"
cmd_cxx = $(CC) $(2)
quiet_cmd_ccld = " CCLD  $@"
cmd_ccld = $(CC) $(2)
quiet_cmd_strip = " STRIP $@"
cmd_strip = $(STRIP) $(1)
quiet_cmd_ar = " AR    $(2)"
cmd_ar = $(AR) $(2) $(3)
quiet_cmd_ranlib = " RANLIB $(2)"
cmd_ranlib = $(RANLIB) $(2)
quiet_cmd_ln = " LN    $(3) => $(2)"
cmd_ln = $(LN) $(2) $(3)
quiet_cmd_rm = " CLEAN $(2)"
cmd_rm = $(RM) $(3)
quiet_cmd_chmod = " CHMOD $(2)"
cmd_chmod = $(CHMOD) $(2)
quiet_cmd_install = " INSTALL $(2)"
cmd_install = $(INSTALL) $(2)
quiet_cmd_install_dir = " INSTALL $(2)"
cmd_install_dir = $(INSTALL_DIR) $(2)
quiet_cmd_install_file = " INSTALL $(2) => $(3)"
cmd_install_file = $(INSTALL_FILE) $(2) $(3)
quiet_cmd_install_bin = " INSTALL $(2) => $(3)"
cmd_install_bin = $(INSTALL_BIN) $(2) $(3)
quiet_cmd_install_rename = " INSTALL $(2) => $(3)"
cmd_install_rename = $(INSTALL_RENAME) $(2) $(3)
quiet_cmd_sed = " GEN   $(3)"
cmd_sed = $(SED) $(2) > $(3)
quiet_cmd_ldconfig = " LDCONFIG"
cmd_ldconfig = ldconfig

define run-cmd
	@echo $($(quiet)cmd_$(1))
	@$(cmd_$(1))
endef

define debug-strip
	$(Q)if [ $(DEBUG) -eq 0 ]; then \
		echo $($(quiet)cmd_strip); \
		$(cmd_strip); \
	fi
endef

define ldconfig
	$(Q)if [ $(UID) -eq 0 ]; then \
		echo $($(quiet)cmd_ldconfig); \
		$(cmd_ldconfig); \
	fi
endef

# Set up build engine variables
GCC ?= gcc
GXX ?= g++
ifeq ($(strip $(DEBUG)), 1)
	OPTIM_FLAGS = -ggdb
	GCC_FLAGS = 
else
	GCC_VER = $(shell gcc -dumpversion | cut -d . -f 1)
	ifeq ($(shell if [ $(GCC_VER) -ge 4 ]; then echo 1; else echo 0; fi), 1)
		GCC_FLAGS = -fvisibility=hidden -fvisibility-inlines-hidden
	else
		GCC_FLAGS = 
	endif
	OPTIM_FLAGS = -O2
	DEBUG = 0
endif
CC = $(GCC) $(GCC_FLAGS)
CXX = $(GXX) $(GCC_FLAGS)
CHMOD = chmod 0755
STRIP = strip
RM = rm -f
AR = ar cr
RANLIB = ranlib
LN = ln -sfT
INSTALL = install
INSTALL_FILE = $(INSTALL) -m644
INSTALL_BIN = $(INSTALL) -m755
INSTALL_DIR = $(INSTALL) -m755 -d
CRUNCHMAKE = crunchMake $(shell pkg-config --cflags --libs crunch)
CRUNCH = crunch
UID = $(shell id -u)
PREFIX ?= /usr

export GCC GXX DEBUG UID PREFIX
