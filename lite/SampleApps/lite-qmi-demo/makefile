include ../SampleApps.mak

_OBJ = packingdemo.o qosdemo.o swiomademo.o wdsdemo.o locdemo.o nasdemo.o uimdemo.o dmsdemo.o smsdemo.o \
	packingdemo_common.o swilocdemo.o helper_sms.o tmddemo.o catdemo.o pdsdemo.o swiomaextdemo.o sardemo.o \
	rmsdemo.o voicedemo.o swiavmsdemo.o imsa-unit-test.o imsademo.o audiodemo.o swiaudiodemo.o imsdemo.o \
    ping.o swiavmsdemo-avc2.o swidmsdemo.o
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

#CFLAGS += -D_ASYNC_READ_ENABLE_=1


$(ODIR)%.o:	$(SRCDIR)%.c
	@if [ -e $(ODIR) ] ;\
        then echo "$(ODIR) exists - good." ;\
        else mkdir -p $(ODIR);\
        fi;
	@echo Compiling $<

	$(CC) -c -o $@ $< $(CFLAGS)

$(BINDIR)packingdemo$(CPU): $(OBJ)
	@if [ -e $(BINDIR) ] ;\
        then echo "$(BINDIR) exists - good." ;\
        else mkdir -p $(BINDIR);\
        fi;
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS) 

.PHONY clean:
	rm -rf $(DDIR)
	rm -rf $(BINDIR)packingdemo$(CPU)

