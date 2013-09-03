include Makefile.inc

default: all

all:
	@cd crunch && $(MAKE)

install:
	@cd crunch && $(MAKE) install

check:
	@cd crunch && $(MAKE) check

clean:
	@cd crunch && $(MAKE) clean
