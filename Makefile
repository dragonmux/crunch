include Makefile.inc

default: all

all:
	@cd libTest && $(MAKE)

clean:
	@cd libTest && $(MAKE) clean
