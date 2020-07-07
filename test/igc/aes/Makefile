include ../Makefile.rules

LEVEL1 = libsbox_us00.so libaes_rcon.so libaes_mix_col_0.so
LEVEL2 = libaes_key_expand_128.so
LEVEL3 = libaes_top.so libtestbench.so

all: $(LEVEL1) $(LEVEL2) $(LEVEL3)

sim: aes.fdl
	$(BUILD)/bin/fdlsim aes.fdl 50

$(LEVEL1): aes.fdl
	$(create_dynlib)

sim1: $(LEVEL1)
	$(BUILD)/bin/fdlsim aes_l1.fdl 50

$(LEVEL2): aes_l1.fdl
	$(create_dynlib)

sim2: $(LEVEL2)
	$(BUILD)/bin/fdlsim aes_l2.fdl 50

$(LEVEL3): aes_l2.fdl
	$(create_dynlib)

sim3: $(LEVEL3)
	$(BUILD)/bin/fdlsim aes_l3.fdl 50


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
	$(LEVEL3)    \
	$(subst lib,,$(subst .so,.fdl,$(LEVEL3))) \
	$(subst lib,,$(subst .so,.cxx,$(LEVEL3))) \
	$(subst lib,,$(subst .so,.h,$(LEVEL3)))   \
	gmon.out

