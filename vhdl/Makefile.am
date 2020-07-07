bin_PROGRAMS = fdlvhd
fdlvhd_SOURCES = fdlvhd.cxx \
		vhdassign.cxx   \
		vhdlut.cxx      \
		vhdreport.cxx   \
		vhdunop.cxx     \
		vsupport.cxx    \
		velements.cxx   \
		vhdbinop.cxx    \
		vhdop.cxx       \
		vhdsfg.cxx      \
		vhdvalldp.cxx   \
		vtype.cxx	\
		vformat.cxx     \
		vhdclonedp.cxx  \
		vhdout.cxx      \
		vhdsys.cxx      \
		vhdvar.cxx	\
		vgen2.cxx       \
		vhdctl.cxx      \
		vhdportnet.cxx  \
		vhdterop.cxx    \
		vhdvdp.cxx      \
		velements.h  	\
		vhdassign.h	\
		vhdctl.h 	\
		vhdout.h        \
		vhdsfg.h	\
		vhdunop.h	\
		vhdvdp.h	\
		vformat.h 	\
		vhdbinop.h      \
		vhdlut.h        \
		vhdportnet.h    \
		vhdsys.h        \
		vhdvalldp.h     \
		vsupport.h      \
		vgen2.h		\
		vhdclonedp.h    \
		vhdop.h         \
		vhdreport.h     \
		vhdterop.h      \
		vhdvar.h        \
		vtype.h

if LINKLIBDL
  LDDYNLIB = -ldl
else
  LDDYNLIB =
endif

fdlvhd_LDADD = -lgmp -lipconfig -lfdl $(LDDYNLIB)

