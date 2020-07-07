include ../Makefile.rules

LEVEL1 = libeuclid.so libdrive_euclid.so
LEVEL2 = libsyseuclid.so

all: $(LEVEL1) $(LEVEL2)

sim: euclidsys.fdl
	$(BUILD)/bin/fdlsim euclidsys.fdl 500

$(LEVEL1): euclidsys.fdl
	$(create_dynlib)

sim1: $(LEVEL1)
	$(BUILD)/bin/fdlsim euclidsys_l1.fdl 500

$(LEVEL2): euclidsys_l1.fdl
	$(create_dynlib)

sim2: $(LEVEL2)
	$(BUILD)/bin/fdlsim euclidsys_l2.fdl 500


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
	$(subst lib,,$(subst .so,.h,$(LEVEL2)))   \
	gmon.out
