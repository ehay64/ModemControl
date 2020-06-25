# makefile

include ../SampleApps.mak

_OBJ  = lite_fw_dld.o lite_fw_util_qmi.o lite_fw_util_mbim.o
OBJ  := $(patsubst %, $(ODIR)%, $(_OBJ))

CFLAGS += -I ../../lite-fw/inc -I../../lite-qmi/inc -I ./inc

LIBS  = -L ../../lite-fw/lib/$(CPU)/ \
	-L ../../lite-qmi/lib/$(CPU) -llite-qmi -llite-fw -lrt -lpthread

ifneq ($(GCC_SYS_ROOT),)
LIBS  += --sysroot=$(GCC_SYS_ROOT)
endif

$(ODIR)%.o:	$(SRCDIR)%.c
	@if [ -e $(ODIR) ] ;\
        then echo "$(ODIR) exists - good." ;\
        else mkdir -p $(ODIR);\
        fi;
	@echo Compiling $<

	$(CC) -g -c -o $@ $< $(CFLAGS)

$(BINDIR)fwdwl-lite$(CPU): $(OBJ)
	@if [ -e $(BINDIR) ] ;\
        then echo "$(BINDIR) exists - good." ;\
        else mkdir -p $(BINDIR);\
        fi;
	$(CC) -g -o $@ $^ $(LDFLAGS) $(LIBS)

.PHONY clean:
	rm -rf $(DDIR)
	rm -rf $(BINDIR)fwdwl-lite$(CPU)
