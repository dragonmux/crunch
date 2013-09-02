include Makefile.inc

default: all

all:
	@cd libTest && $(MAKE)

install:
	@cd libTest && $(MAKE) install

check:
	@cd libTest && $(MAKE) check

clean:
	@cd libTest && $(MAKE) clean
