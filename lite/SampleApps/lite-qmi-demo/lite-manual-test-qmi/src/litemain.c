#define __STDC_FORMAT_MACROS
#include <pthread.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <errno.h>

#include "packingdemo.h"
#include "qmerrno.h"

#include <sys/sysinfo.h>
#include <getopt.h>

#define MAX_SVC_NAME_LEN 64
#define QMI_GET_SERVICE_FILE_IOCTL  0x8BE0 + 1

unsigned g_xid = 0;
extern uint8_t g_modem_index;

extern void run_dms_tests();
extern void run_nas_tests();
extern void run_wds_tests();
extern void run_ims_tests();
extern void run_cat_tests();
extern void run_audio_tests();
extern void run_loc_tests();
extern void run_pds_tests();
extern void run_sar_tests();
extern void run_rms_tests();
extern void run_swiaudio_tests();
extern void run_swiloc_tests();
extern void run_tmd_tests();
extern void run_sms_tests();
extern void run_voice_tests();
extern void run_uim_tests();

/* Command line options to firmware download tool */
const char * const short_options_manual = "s:i:h";

/* Command line long options for firmware download tool */
const struct option long_options_manual[] = {
    {"service",   1, NULL, 's'},    
    {"help",   0, NULL, 'h'},
    {"index",   1, NULL, 'i'},
    {NULL,     0, NULL,  0 }       /* End of list */
};

void FlushStdinStream( )
{
    int inputChar;

    /* keep on reading until a <New Line> or end of file is received */
    do
    {
        inputChar = getchar();
    }
    while ( ( inputChar != ENTER_KEY ) &&
            ( inputChar != EOF ) );
}

void PrintUsageManual()
{
    printf( "\r\n" );
    printf( "App usage: \r\n\r\n" );
    printf( "  <appName> -s dms \n\n" );
    printf( "  -s  --service  \n" );
    printf( "        mandatory:service name as nas wds dms etc.\n" );
    printf( "        <svc name> dms nas wds ims cat audio loc pds\n");
    printf( "        rms sar sms swiaudio swiloc tmd uim voice \n\n" );
    printf( "  -i  --index \n" );
    printf( "        qcqmi index number, default 0\n\n" );
    printf( "  -h  --help  \n" );
    printf( "        This option prints the usage instructions.\n\n" );
}

int parseSwitchesManual( int argc, char **argv, char *pSvc)
{
    int next_option;

    if (NULL == pSvc)
        return 0;
    
    /* Parse the command line before doing anything else */
    do
    {
        /* Read the next option until there are no more */
        next_option = getopt_long( argc, argv,
                                   short_options_manual,
                                   long_options_manual, NULL );

        switch( next_option )
        {
            case 'h':
                /* Print usage information */
                PrintUsageManual();
                exit (0);
                break;
            case 's':
                /* Log file path*/
                strncpy (pSvc,optarg,MAX_SVC_NAME_LEN -1);
                break;
            case 'i':
                g_modem_index = (uint8_t)atoi(optarg);
                break;
            case -1:
                /* Done with options list */
                break;
            default:
                exit(EXIT_FAILURE);
                break;
        }
    }
    while( next_option != -1 );
    return 0;
}

int main( int argc, char *argv[])
{
    char svcname[MAX_SVC_NAME_LEN];

    memset(svcname,0,sizeof(svcname));
    parseSwitchesManual(argc,argv,svcname);
    printf("liteqmi version %s\n", liteqmi_GetVersion());

    if (!strcmp(svcname,"dms")) {
       run_dms_tests();
    }
    else if(!strcmp(svcname,"nas")) {
       run_nas_tests();
    }
    else if(!strcmp(svcname,"wds")) {
       run_wds_tests();
    }
    else if(!strcmp(svcname,"ims")) {
       run_ims_tests();
    }
    else if(!strcmp(svcname,"cat")) {
       run_cat_tests();
    }
    else if(!strcmp(svcname,"audio")) {
       run_audio_tests();
    }
    else if(!strcmp(svcname,"loc")) {
       run_loc_tests();
    }
    else if(!strcmp(svcname,"pds")) {
       run_pds_tests();
    }
    else if(!strcmp(svcname,"rms")) {
       run_rms_tests();
    }
    else if(!strcmp(svcname,"sar")) {
       run_sar_tests();
    }
    else if(!strcmp(svcname,"sms")) {
       run_sms_tests();
    }
    else if(!strcmp(svcname,"swiaudio")) {
       run_swiaudio_tests();
    }
    else if(!strcmp(svcname,"swiloc")) {
       run_swiloc_tests();
    }
    else if(!strcmp(svcname,"tmd")) {
       run_tmd_tests();
    }
    else if(!strcmp(svcname,"uim")) {
       run_uim_tests();
    }
    else if(!strcmp(svcname,"voice")) {
       run_voice_tests();
    }
    else {
        printf("unknown service name\n");
        PrintUsageManual();
    }
    return 0;
}


