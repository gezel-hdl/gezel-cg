include ../Makefile.rules

LEVEL1 = libchecksum.so libtb.so
LEVEL2 = libipchecksum_sys.so

all: $(LEVEL1) $(LEVEL2)

sim: ipchecksum.fdl
	$(BUILD)/bin/fdlsim ipchecksum.fdl 500

$(LEVEL1): ipchecksum.fdl
	$(create_dynlib)

sim1: $(LEVEL1)
	$(BUILD)/bin/fdlsim ipchecksum_l1.fdl 500

$(LEVEL2): ipchecksum_l1.fdl
	$(create_dynlib)

sim2: $(LEVEL2)
	$(BUILD)/bin/fdlsim ipchecksum_l2.fdl 500


#-------------------------------------------------

clean:
	rm -f *~ *.o \
	$(LEVEL1)    \
	$(subst lib,,$(subst .so,.fdl,$(LEVEL1))) \
	$(subst lib,,$(subst .so,.cxx,$(LEVEL1))) \
	$(subst lib,,$(subst .so,.h,$(LEVEL1)))   \
	$(LEVEL2)    \
	$(subst lib,,$(subst .so,.fdl,$(LEVEL2))) \
	$(subst lib,,$(subst .so,.cxx,$(LEVEL2))) \
	$(subst lib,,$(subst .so,.h,$(LEVEL2)))
