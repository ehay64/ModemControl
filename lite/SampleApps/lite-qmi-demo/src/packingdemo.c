#define __STDC_FORMAT_MACROS
#define _GNU_SOURCE
#include <pthread.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <errno.h>
#include <unistd.h>
#include <execinfo.h>
#include <dirent.h>
#include <sys/sysinfo.h>
#include <getopt.h>

#include "packingdemo.h"
#include "msgid.h"
#include "qmerrno.h"

extern int g_skip_swiavms;
extern uint8_t g_modem_index;
extern uint8_t g_profile_index;

extern int g_fork;
extern int g_avc2_test;
extern unsigned int g_num_of_rmnet_supported;
extern char g_eth_name[255];
extern int g_ip_version[MAX_QMAP_INSTANCE];
extern pid_t pids[MAX_TEST_PROCESS];
extern pid_t cmd_pids[MAX_TEST_PROCESS];
extern int g_cmd_pid_index;
extern char szFunction[MAX_TEST_PROCESS][255];

static FILE *fpLogFile = NULL;
static char buffer[255]={0};
static wdsConfig wdsConfigValues;
static nasConfig nasConfigValues;
static dmsConfig dmsConfigValues;
static smsConfig smsConfigValues;
static uimConfig uimConfigValues;


extern void nas_setOemApiTestConfig(nasConfig *nasConfigList);
extern void wds_setOemApiTestConfig(wdsConfig *wdsConfigList);
extern void dms_setOemApiTestConfig(dmsConfig *dmsConfigList);
extern void sms_setOemApiTestConfig(smsConfig *smsConfigList);
extern void uim_setOemApiTestConfig(uimConfig *uimConfigList);

extern int GetNumberOfQmapSupported();
extern void oemApiTest(void);
extern void swiavms_loop_avc2();
extern void packingdemo();
extern void kill_running_pids();
extern void check_running_pids();
#include <sys/msg.h>

extern int main_mqid;
pthread_t msgquetid = 0;
int stopmsgquetthread = 0;
void RemoveMsgID(int *iMsgid);
int CreateMsgID();
int g_client_fd_memory_check_enable = 0;
int check_serivce_fd_memory_usage();

#ifndef NO_BACKTRACE
void* tracePtrs[100];
void segfault(int signo, siginfo_t* pinfo, void *pcontext)
{
    int ii;
    UNUSEDPARAM(signo);
    UNUSEDPARAM(pinfo);
    UNUSEDPARAM(pcontext);
    setbuf(stdout,NULL);
    setbuf(stderr,NULL);
    int count = backtrace( tracePtrs, 100 );
    printf("backtrace() returned %d addresses\n", count);
    char** funcNames = backtrace_symbols( tracePtrs, count );

    if (funcNames == NULL) {
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }

    // Print the stack trace
    for( ii = 0; ii < count; ii++ )
        printf( "%s\n", funcNames[ii] );

    // Free the string pointers
    free( funcNames );
    exit(-1);
}
#endif


struct _userOptions
{
    char log_file_path[MAX_PATH_LEN];
    char config_file_path[MAX_PATH_LEN];
};

/* Command line options to firmware download tool */
const char * const short_options = "l:i:d:v:f:e:z:u:m:hspqoak";

/* Command line long options for firmware download tool */
const struct option long_options[] = {
    {"logfile",   1, NULL, 'l'},      /* custom log file path */
    {"help",   0, NULL, 'h'},      /* Provides terse help to users */
    {"skip-swiavms",   0, NULL, 's'},
    {"parallel-run",   0, NULL, 'p'},
    {"index",   0, NULL, 'i'},
    {"ethdevname",   1, NULL, 'd'},
    {"ipversion",   1, NULL, 'v'},
    {"configfile",   1, NULL, 'f'},      /* config log file path */
    {"qmap",   0, NULL, 'q'},
    {"oem",   0, NULL, 'o'},
    {"avc2",   0, NULL, 'a'},
    {"profileid",   1, NULL, 'e'},
    {"autoping",   1, NULL, 'z'},
    {"autoip",   1, NULL, 'u'},
    {"mtu",   1, NULL, 'm'},
    {"--check-serivice-fd-kernel-slab-usage",0,NULL,'k'},
    {NULL,     0, NULL,  0 }       /* End of list */
};


void PrintUsage()
{
    printf( "\r\n" );
    printf( "App usage: \r\n\r\n" );
    printf( "  <appName> -l Log File Path -h \n\n" );
    printf( "  -l  --logfile  \n" );
    printf( "        Specific custom log path.\n\n" );
    printf( "  -s  --skip-swiavms  \n" );
    printf( "        Skip SWI AVMS test.\n\n" );
    printf( "  -i  --index \n" );
    printf( "        qcqmi index number.\n\n" );
    printf( "  -q  --qmap \n");
    printf( "        Enable QMAP test.\n\n" );
    printf( "  -v  --ipversion \n" );
    printf( "        QMAP Data Conneciton IP version(s) parameter 0:skip, 4:IPv4, 6: IPv6, 7: IPv4v6 \n" );
    printf( "        e.g. MuxID:0x80 IPv4, MuxID:0x81 IPv6.\n" );
    printf( "             MuxID:0x82 Skip, MuxID:0x83 IPv4v6.\n" );
    printf( "             <appName> -q -v 4,6,0,7\n\n" );
    printf( "  -f  --config file  \n" );
    printf( "        config file path.\n\n" );
    printf( "  -o  --oem \n");
    printf( "        Enable OEM API test.\n\n" );
    printf( "  -a  --avc2  \n" );
    printf( "        Run ONLY WP760x AVMS test.\n\n" );
    printf( "  -e  --profileid \n" );
    printf( "        profile ID.\n\n" );
    printf( "  -z  --autoping \n" );
    printf( "        Auto ping test.\n\n" );
    printf( "  -u  --autoip\n" );
    printf( "        Auto assign the ip address.\n\n" );
    printf( "  -m  --mtu auto upate \n" );
    printf( "        Auto update the mtu reported by the GET_RUNTIME_SETTING.\n\n" );
    printf( "  -k  --check-serivice-fd-kernel-slab-usage \n" );
    printf( "        Check serivice fd kernel slab usage.\n\n" );
    printf( "  -h  --help  \n" );
    printf( "        This option prints the usage instructions.\n\n" );
}

void *syslog_proxy(uint8_t lvl, const char *buff)
{
    struct sysinfo s_info;
    if(sysinfo(&s_info)!=0)
    {
        s_info.uptime = 0;
    }
    if(fpLogFile!=NULL)
    {
       fprintf(fpLogFile,"[%ld][%d]%s\n",s_info.uptime,lvl, buff);
       fflush(fpLogFile);
    }
    return NULL;
}

int GetIPFormat(char ip)
{
    if(ip=='4')
        return 4;
    if(ip=='6')
        return 6;
    if(ip=='7')
        return 7;
    return 0;
}

int obtainValueField(char *buf, int index)
{
    char *pch, *pdel;

    pch = strstr(buf, "=");
    if (pch == NULL)
    {
        printf("error, can't find string \"=\", index = %d\n", index);
        return 0;
    }
    switch(index)
    {
        case E_PDN1_IP_ADDRESS:
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
            {
                /* Initial wdsConfigValues.pdnDestIp[0] pointers to NULL */
                memset(&wdsConfigValues.pdnDestIp[0],0,sizeof(wdsConfigValues.pdnDestIp[0]));
                strcpy(wdsConfigValues.pdnDestIp[0], pdel);
            }
            break;
        case E_PDN2_IP_ADDRESS:
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
            {
                /* Initial wdsConfigValues.pdnDestIp[1] pointers to NULL */
                memset(&wdsConfigValues.pdnDestIp[1],0,sizeof(wdsConfigValues.pdnDestIp[1]));
                strcpy(wdsConfigValues.pdnDestIp[1], pdel);
            }
            break;
        case E_PDN3_IP_ADDRESS:
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
            {
                /* Initial wdsConfigValues.pdnDestIp[2] pointers to NULL */
                memset(&wdsConfigValues.pdnDestIp[2],0,sizeof(wdsConfigValues.pdnDestIp[2]));
                strcpy(wdsConfigValues.pdnDestIp[2], pdel);
            }
            break;
        case E_PDN4_IP_ADDRESS:
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
            {
                /* Initial wdsConfigValues.pdnDestIp[1] pointers to NULL */
                memset(&wdsConfigValues.pdnDestIp[3],0,sizeof(wdsConfigValues.pdnDestIp[3]));
                strcpy(wdsConfigValues.pdnDestIp[3], pdel);
            }
            break;
        case E_PROFILE_ID:
            wdsConfigValues.profileId = atoi(++pch);
            break;
        case E_PREFER_NETWORK_TYPE:
            wdsConfigValues.prefNetworkType = atoi(++pch);
            break;

        case E_MCC:
            nasConfigValues.mcc = atoi(++pch);
            break;
        case E_MNC:
            nasConfigValues.mnc = atoi(++pch);
            break;
        case E_NETWORK_NAME:
            /* in the test config file, we use '#' for comment */
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
                strcpy(nasConfigValues.networkName, pdel); 
            break;
        case E_MODEL_ID:
            /* in the test config file, we use '#' for comment */
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
            {
                strcpy(dmsConfigValues.modelId, pdel);
                strcpy(wdsConfigValues.modelId, pdel);
                strcpy(nasConfigValues.modelId, pdel);
                strcpy(uimConfigValues.modelId, pdel);
            }
            break;
        case E_FIRMWARE_VERSION:
            /* in the test config file, we use '#' for comment */
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
                strcpy(dmsConfigValues.fwVersion, pdel); 
            break;
        case E_IMEI:
            /* in the test config file, we use '#' for comment */
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
                strcpy(dmsConfigValues.imei, pdel); 
            break;
        case E_ICCID:
            /* in the test config file, we use '#' for comment */
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
                strcpy(dmsConfigValues.iccid, pdel); 
            break;
        case E_SMS_STORAGE_TYPE:
            smsConfigValues.storageType = atoi(++pch);
            break;
        case E_SMS_MESSAGE_MODE:
            smsConfigValues.msgMode = atoi(++pch);
            break;
        case E_SMS_MESSAGE_FORMAT:
            smsConfigValues.msgFormat = atoi(++pch);
            break;
        case E_SMS_PHONE_NUMBER:
            /* in the test config file, we use '#' for comment */
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
                strcpy(smsConfigValues.number, pdel); 
            break;
        case E_UIM_PIN_CODE:
            /* in the test config file, we use '#' for comment */
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
                strcpy(uimConfigValues.pin, pdel); 
            break;
        case E_UIM_PUK_CODE:
            /* in the test config file, we use '#' for comment */
            pdel = strtok(++pch, "#");
            if (pdel !=NULL)
                strcpy(uimConfigValues.puk, pdel); 
            break;

        
        default:
            break;
    }

    return 1;
}


void parseConfigFile(char* filePath)
{
    FILE *fd;
    char *temBuf=NULL;
    size_t len = 255;
    ssize_t nread;
    int i = 0;

    fd = fopen(filePath, "r");
    if ( fd == NULL)
    {
        printf("error: open file failed: path:%s\n", filePath);
        return;
    }

    while ((nread = getline(&temBuf, &len, fd)) != -1)
    {   
        if(nread>=255)
        {
            printf("error: configure string too long >= 255\n");
            free(temBuf);
            fclose(fd);
            return ;
        }
        memset(buffer,0,255);
        strncpy(buffer, temBuf,(uint8_t)nread);
        buffer[254]=0;
        if (obtainValueField(buffer, i) == 0)
        {
            printf("error: can't retrieve value from the line\n");
        }
        i++;
    }
    /* set test config values at one shot after parse all the fields */
    wds_setOemApiTestConfig(&wdsConfigValues);
    nas_setOemApiTestConfig(&nasConfigValues);
    dms_setOemApiTestConfig(&dmsConfigValues);
    sms_setOemApiTestConfig(&smsConfigValues);
    uim_setOemApiTestConfig(&uimConfigValues);

    free(temBuf);
    fclose(fd);
}

int parseSwitches( int argc, char **argv, struct _userOptions *puserOptData)
{
    if (NULL == puserOptData)
        return 0;
    int next_option;
    char *pIPString = NULL;
    memset(&g_ip_version,0,sizeof(g_ip_version));
    int iIsSingleProfile = 0;
    #if _ASYNC_READ_ENABLE_
    g_auto_test_enable = 1;
    #else
    g_auto_test_enable = 0;
    #endif
    /* by default, mtu auto update is enabled */
    g_mtu_auto_update_enable = 1;
    /* by default, ip auto assignment is enabled */
    g_ip_auto_assign = 1;

    /* Parse the command line before doing anything else */
    do
    {
        /* Read the next option until there are no more */
        next_option = getopt_long( argc, argv,
                                   short_options,
                                   long_options, NULL );

        switch( next_option )
        {
            case 'h':
                /* Print usage information */
                PrintUsage();
                exit (0);
                break;
            case 'l':
                /* Log file path*/
                strncpy (puserOptData->log_file_path,optarg,MAX_PATH_LEN-1);
                break;
            case 's':
                g_skip_swiavms = 1;
                break;
            case 'p':
                g_fork = 1;
                break;
            case 'i':
                g_modem_index = (uint8_t)atoi(optarg);
                break;
            case 'e':
                iIsSingleProfile = 1;
                g_profile_index = (uint8_t)atoi(optarg);
                g_profile_index--;
                break;
            case 'q':
                g_runqmap_demo = 1 ;
                g_fork = 1;
                //Start only 1st PDN(IPv4) only if not specific IP version.
                if(g_ip_version[0]==0)
                {
                    g_ip_version[0] = 4;
                }
                break;
            case 'o':
                g_runoem_demo = 1;
                //Start only 1st PDN(IPv4) only if not specific IP version.
                if(g_ip_version[0]==0)
                {
                    g_ip_version[0] = 4;
                }
                g_fork = 1;
                break;
            case 'a':
                g_avc2_test = 1;
                break;
            case 'd':
                strncpy (g_eth_name,optarg,254);
                break;
            case 'v':
                if(strstr(optarg,","))
                {
                    int index=0;
                    g_ip_version[index++] = GetIPFormat(optarg[0]);
                    pIPString=strstr(optarg,",");
                    while(pIPString!=NULL)
                    {
                         if(index<MAX_QMAP_INSTANCE)
                         {
                             if(strlen(pIPString)<2)
                             {
                                break;
                             }
                             g_ip_version[index++] = GetIPFormat(pIPString[1]);
                         }
                         else
                         {
                            break;
                         }
                         pIPString++;
                         pIPString=strstr(pIPString,",");
                    }
                    for(index=0;index<MAX_QMAP_INSTANCE;index++)
                    {
                        if(g_ip_version[index]!=0)
                        {
                             fprintf(stderr,"%d IPv%d\n",index+1,g_ip_version[index]);
                        }
                    }
                }
                else
                {
                    g_ip_version[0] = atoi(optarg);
                    fprintf(stderr,"g_ip_version:%d\n",g_ip_version[0]);
                }
                break;
            case 'f':
                /* config file path*/
                strncpy (puserOptData->config_file_path,optarg,MAX_PATH_LEN-1);
                puserOptData->config_file_path[strlen(optarg)] = '\0';
                parseConfigFile(puserOptData->config_file_path);
                break;
            case 'z':
                if(atoi(optarg)==0)
                {
                    g_auto_test_enable = 0;
                }
                else
                {
                    g_auto_test_enable = 1;
                }
                break;
            case 'm':
                if(atoi(optarg)==0)
                {
                    g_mtu_auto_update_enable = 0;
                }
                else
                {
                    g_mtu_auto_update_enable = 1;
                }
                break;
            case 'u':
                if(atoi(optarg)==0)
                {
                    g_ip_auto_assign = 0;
                }
                else
                {
                    g_ip_auto_assign = 1;
                }
                break;
            case 'k':
               g_client_fd_memory_check_enable = 1;
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
    if(iIsSingleProfile == 1)
    {
        if(g_profile_index<MAX_QMAP_INSTANCE)
        {
            int i =0;
            int ipversion = g_ip_version[0];
            for(i=0;i<MAX_QMAP_INSTANCE;i++)
            {
                if(g_profile_index==i)
                {
                    g_ip_version[i]=ipversion;
                }
                else 
                {
                    g_ip_version[i]=0;
                }
            }
        }
    }
    g_num_of_rmnet_supported = GetNumberOfQmapSupported();
    return 0;
}

int InitParams(struct _userOptions *puserOptData)
{
    if(puserOptData==NULL)
    {
        return 0;
    }
    if(strlen(puserOptData->log_file_path)>0)
    {
        fpLogFile = fopen(puserOptData->log_file_path, "a+" );
        if(fpLogFile==NULL)
        {
            fprintf(stderr,"Log File Error %s!",puserOptData->log_file_path);
        }
        else
        {
            helper_set_log_func((logger*)&syslog_proxy);
        }
    }
    return 0;
}
#ifndef NO_BACKTRACE
struct sigaction old_action;
void break_handler(int sig_no)
{
    check_running_pids();
    kill_running_pids();
    fprintf(stderr,"\n==SIGINT=====%d=======\n",sig_no);
    RemoveMsgID(&main_mqid);
    sigaction(SIGINT, &old_action, NULL);
    kill(0, SIGINT);
}
#endif

int CreateMsgID()
{
    key_t key;
    key = ftok("/dev/random", 'c');
    return (int)msgget(key, 0666 | IPC_CREAT);
}

void RemoveMsgID(int *iMsgid)
{
    if(*iMsgid>=0)
    msgctl(*iMsgid, IPC_RMID, NULL);
    *iMsgid = -1;
}

void run_system(char *szCMD)
{
    pid_t pid = -1;
    if(g_cmd_pid_index<MAX_TEST_PROCESS)
    {
        pid = fork();
        if(pid==0)
        {
            system(szCMD);
            exit(1);
        }
        else if(pid > 0)
        {
            cmd_pids[g_cmd_pid_index++] = pid;
        }
    }    
}

void* msgquethread(void* ptr)
{
    uint8_t rsp[QMI_MSG_MAX+8];
    int result;
    msgbuf *pBuf = NULL;
    UNUSEDPARAM(ptr);
    while(!stopmsgquetthread)
    {
        int length = sizeof(msgbuf) - sizeof(long);
        result = msgrcv(main_mqid, &rsp, length, 0, IPC_NOWAIT);
        if(result>0)
        {
            pBuf = (msgbuf*)&rsp;
            switch(pBuf->type)
            {
                case 1:
                    fprintf(stderr,"CMD : %s\n",pBuf->buf);
                    if(strlen((char*)pBuf->buf)>0)
                    {
                        run_system((char*)pBuf->buf);
                    }
                    break;
                default:
                    break;
            }
            fprintf(stderr,"(%d)type: %ld : %s\n",result,pBuf->type,pBuf->buf);
        }
    }
    return NULL;
}

int
main( int argc, char *argv[])
{
    struct _userOptions UserOptions;
    pthread_attr_t attr;
    memset(pids,0,sizeof(pid_t)*MAX_TEST_PROCESS);
    memset(cmd_pids,0,sizeof(pid_t)*MAX_TEST_PROCESS);
    memset(&UserOptions,0,sizeof(struct _userOptions));
    parseSwitches(argc,argv,&UserOptions);
    InitParams(&UserOptions);
    if(g_client_fd_memory_check_enable)
    {
        if(check_serivce_fd_memory_usage()!=0)
        {
            return -1;
        }
    }
    printf("liteqmi version %s\n", liteqmi_GetVersion());
#ifndef NO_BACKTRACE
    struct sigaction sa;
    sa.sa_sigaction = segfault;
    sigemptyset(&sa.sa_mask);

    sa.sa_flags = SA_NODEFER | SA_SIGINFO;

    if (sigaction(SIGSEGV, &sa, NULL) < 0)
        printf("signal handler install failed");
    struct sigaction breakaction;
    memset(&breakaction, 0, sizeof(breakaction));
    breakaction.sa_handler = &break_handler;
    sigaction(SIGINT, &breakaction, &old_action);
#endif

    if (g_avc2_test)
    {
        swiavms_loop_avc2();
        return 0;
    }

    if(g_runoem_demo==1)
    {
        oemApiTest();
    }
    else
    {
        main_mqid = CreateMsgID();
        memset(&attr, 0, sizeof(attr));
        pthread_create(&msgquetid, &attr, msgquethread, NULL);
        usleep(500);
        packingdemo();
        stopmsgquetthread = 1;
        usleep(500);
        if(msgquetid!=0)
        pthread_cancel(msgquetid);
        RemoveMsgID(&main_mqid);
    }
    sleep(60);
    if(fpLogFile!=NULL)
    {
        fclose(fpLogFile);
    }
    return 0;
}

int check_serivce_fd_memory_usage()
{
    fd_set rfds;
    struct timeval tv;
    int retval;
    int fd = client_fd(eNAS);
    unsigned count =0;
    unsigned long oldememoryusage = 0;
    unsigned long newmemoryusage = 0;
    
    uint8_t rsp[QMI_MSG_MAX];
    if(fd<0)
    {
        return -1;
    }
    for(count=0;count<MAX_SLAB_TEST_LOOP_COUNT;count++)
    {
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);
        tv.tv_sec = 0;
        tv.tv_usec = 10;
        retval = select(fd+1, &rfds, NULL, NULL, &tv);
        /* Don't rely on the value of tv now! */

        if (retval == -1)
        {
           perror("select()");
        }
        else if (retval)
        {
            //printf("Data is available now.\n");
            read(fd,rsp,QMI_MSG_MAX-1);
        }
        else
        {
           //printf("No data within 10u seconds.\n");
        }
        if(count==0)
        {
            //Get the total active slab.
            oldememoryusage = get_total_slab_activity();
        }
    }
    //Get the total active slab after no. of select.
    newmemoryusage = get_total_slab_activity();
    close(fd);
    printf("slab usage from(%lu) -> to(%lu).\n",
        oldememoryusage,newmemoryusage);
    if((newmemoryusage==(unsigned long)-1)&&(newmemoryusage==(unsigned long)-1))
    {
        return 0;
    }
    if(newmemoryusage>(oldememoryusage + (MAX_SLAB_TEST_LOOP_COUNT/3)))
    {
        //10% higher than 1st select.
        fprintf(stderr,"slab kmalloc usage error(%lu) -> to(%lu).\n",
        oldememoryusage,newmemoryusage);
        return 1;
    }
    else if(newmemoryusage==0)
    {
        return 1;
    }
    return 0;
}

