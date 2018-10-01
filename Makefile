# This file is part of crunch
# Copyright © 2013-2017 Rachel Mant (dx-mon@users.sourceforge.net)
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

include Makefile.inc

default: all

all:
	@$(MAKE) -C crunch
	@$(MAKE) -C crunch++
	@$(MAKE) -C crunchMake

lib:
	@$(MAKE) -C crunch lib
	@$(MAKE) -C crunch++ lib

install-so:
	@$(MAKE) -C crunch install-so
	@$(MAKE) -C crunch++ install-so

install:
	@$(MAKE) -C crunch install
	@$(MAKE) -C crunch++ install
	@$(MAKE) -C crunchMake install

test:
	@$(MAKE) -C crunch test
	@$(MAKE) -C crunch++ test

check:
	@$(MAKE) -C crunch check
	@$(MAKE) -C crunch++ check

clean:
	@cd crunch && $(MAKE) clean
	@cd crunch++ && $(MAKE) clean
	@cd crunchMake && $(MAKE) clean
