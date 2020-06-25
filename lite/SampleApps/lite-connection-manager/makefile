# makefile

include ../SampleApps.mak

_OBJ = displaysettings.o connectmgr.o
OBJ  := $(patsubst %, $(ODIR)%, $(_OBJ))
LIBS  = -L ../../lite-qmi/lib/$(CPU)/ -l lite-qmi -lpthread
CFLAGS += -I ../../lite-qmi/inc -I ./inc

STATICFLAG := 1
ifdef BUILDSTATICFLAG
STATICFLAG := $(BUILDSTATICFLAG)
endif

ifeq ($(STATICFLAG), 1)
LIBS  += -static-libgcc
CFLAGS += -static
endif

ifneq ($(GCC_SYS_ROOT),)
LIBS  += --sysroot=$(GCC_SYS_ROOT)
endif

$(ODIR)%.o:	$(SRCDIR)%.c
	@if [ -e $(ODIR) ] ;\
        then echo "$(ODIR) exists - good." ;\
        else mkdir -p $(ODIR);\
        fi;
	@echo Compiling $<

	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)lite-connectmgr$(CPU): $(OBJ)
	@if [ -e $(BINDIR) ] ;\
        then echo "$(BINDIR) exists - good." ;\
        else mkdir -p $(BINDIR);\
        fi;
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

.PHONY clean:
	rm -rf $(DDIR)
	rm -rf $(BINDIR)lite-connectmgr$(CPU)
