include Makefile.inc

default: all

all:
	@cd libTest && $(MAKE)

install:
	@cd libTest && $(MAKE) install

clean:
	@cd libTest && $(MAKE) clean
