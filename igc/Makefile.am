bin_PROGRAMS = igc
igc_SOURCES = igc.cxx icgen.cxx icgen.h

if LINKLIBDL
  LDDYNLIB = -ldl
else
  LDDYNLIB =
endif

igc_LDADD = -lgmp -lipconfig -lfdl $(LDDYNLIB)
