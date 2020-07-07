bin_PROGRAMS = fdlver
fdlver_SOURCES = fdlver.cxx \
		verigen.cxx \
		verigen.h   \
		verimod.cxx \
		verimod.h   \
		verivar.cxx \
		verivar.h   \
		veriprint.h \
		veriprint.cxx

if LINKLIBDL
  LDDYNLIB = -ldl
else
  LDDYNLIB =
endif

fdlver_LDADD = -lgmp -lipconfig -lfdl $(LDDYNLIB)

