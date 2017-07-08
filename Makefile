include Makefile.inc
all:
	(cd src; $(MAKE) )

clean:
	(rm -f *~; cd src; $(MAKE) clean )
