if BUILDIGC
  IGCDIR = igc
else
  IGCDIR = 
endif

if BUILDVHDL
  VHDLDIR = vhdl
else
  VHDLDIR = 
endif

if BUILDVERILOG
  VERILOGDIR = verilog
else
  VERILOGDIR =
endif

SUBDIRS = $(IGCDIR) $(VERILOGDIR) $(VHDLDIR)

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath \
	BUGS \
	po/Makefile.in.in \
	test/igc/aes/aes.fdl \
	test/igc/aes/Makefile \
	test/igc/aes/aes_l1.fdl \
	test/igc/aes/aes_l2.fdl \
	test/igc/aes/aes_l3.fdl \
	test/igc/euclid/Makefile \
	test/igc/euclid/euclidsys.fdl \
	test/igc/euclid/euclidsys_l1.fdl \
	test/igc/euclid/euclidsys_l2.fdl \
	test/igc/ipchecksum/Makefile \
	test/igc/ipchecksum/ipchecksum.fdl \
	test/igc/ipchecksum/ipchecksum_l1.fdl \
	test/igc/ipchecksum/ipchecksum_l2.fdl \
	test/igc/ipchecksum/checksum.fdl \
	test/igc/Makefile \
	test/igc/Makefile.rules \
	test/Makefile
