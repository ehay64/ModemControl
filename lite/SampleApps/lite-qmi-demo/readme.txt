This document provides information about the SMS helper functions that can be 
used to decode and encode SMS PDUs.
- Application developers can use the SMS helper functions by integrating
  helper_sms.c and helper_sms.h with their applications.
- The list of SMS helper functions and the respective SDK functions (for documentation) 
  are below

    1. sms_SLQSCDMAEncodeMOTextMsg()    SLQSCDMAEncodeMOTextMsg()
    2. sms_SLQSWCDMAEncodeMOTextMsg()   SLQSWCDMAEncodeMOTextMsg()
    3. sms_SLQSCDMADecodeMTTextMsg()    SLQSCDMADecodeMTTextMsg()
    4. sms_SLQSWCDMADecodeMTTextMsg()   SLQSWCDMADecodeMTTextMsg()

**************************
Compilation of packingdemo
**************************
- By default, the application is compiled for i86 platform.
  "make"

- The application can be compiled for different platform using the CPU option:
  "make CPU=<platform>"
  where <platform> = arm9 in case 0f ARM and
                     ppc  in case of Power PC.
- For arm9 and powerpc, platform specific toolchains are required. These must be
  installed to the $(HOME) directory once, and can be used for all SDK workspaces.
  If they do not exist, 
  "make CPU=arm9" and
  "make CPU=ppc"  will simply fail.
 
- To enable backtrace function name, application can compiled as
  "make DEBUG=yes"

- The default build option will run the auto test oemapitest array, it only includes
  some lite APIs used by the OEM customer, if the user wants to run the full API list, please
  modify the makefile located in the same directory of this readme file, please ommit the
  following build flag in the makefile.
  -DOEM_API_TEST -DQMAP_ENABLED
  hence the CFLAGS should be look like below:
  CFLAGS += -I ../../lite-qmi/inc -I ./inc -static
