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
// Get slab info from sysfs 
#include <regex.h>

#include "packingdemo.h"

#include "dms.h"
#include "wds.h"
#include "nas.h"
#include "uim.h"
#include "msgid.h"
#include "loc.h"
#include "cat.h"
#include "pds.h"
#include "sar.h"
#include "voice.h"
#include "audio.h"
#include "swiaudio.h"
#include "ims.h"

#include "qmerrno.h"

#include <signal.h>
#include <execinfo.h>
#include <stdio.h>
#include <sys/sysinfo.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>

int g_skip_swiavms = 0;
uint8_t g_modem_index = 0;
uint8_t g_qmi_req[QMI_MSG_MAX];
uint8_t g_rsp[QMI_MSG_MAX];
int g_fork = 0;
int g_runqmap_demo = 0;
int g_runoem_demo = 0;
int g_avc2_test = 0;
int g_qos_started = 0;
unsigned int g_num_of_rmnet_supported = MAX_QMAP_INSTANCE;
char g_eth_name[255] = {0};
int g_ip_version[MAX_QMAP_INSTANCE] ={0};
uint8_t g_profile_index;
int g_qos[MAX_QMAP_INSTANCE] ={0};
pid_t pids[MAX_TEST_PROCESS];
pid_t cmd_pids[MAX_TEST_PROCESS];
int g_cmd_pid_index = 0;
char szFunction[MAX_TEST_PROCESS][255];
char g_szEthName[64]={0};
int g_auto_test_enable = 0;
int g_mtu_auto_update_enable = 0;
int g_ip_auto_assign = 0;


static int process_counter=0;
static int iCompleted = 0;


// QOS
void qos_loop_exit(void);
void qos_loop(void);
extern void QosSetMuxID(uint16_t MuxID);
//SWIOMA
void swioma_loop_exit(void);
void swioma_loop(void);
void swiomaext_loop_exit(void);
void swiomaext_loop(void);
extern void swioma_pack_unpack_loop();
//WDS
extern void wds_test_pack_unpack_loop_invalid_unpack();
extern void wds_test_ind();
extern void wds_test_ind_exit();
extern void wds_test_pack_unpack_loop();
extern void wds_setID(uint8_t newID);
extern void wds_qmap_pack_unpack_loop();
extern void wds_qmap_test_ind();
extern void wds_qmap_test_ind_exit();
extern void wds_setEthName(char *newName);
extern void wds_setIPFamily(int iIPVersion);
extern void wds_setprofile(uint8_t iProfileIndex);
extern void wds_test_thread();
extern void wds_test_thread_stop();
//LOC
extern void loc_test_pack_unpack_loop();
extern void loc_test_pack_unpack_loop_invalid_unpack();
extern void loc_test_ind();
extern void loc_test_ind_exit();
//NAS
extern void nas_test_pack_unpack_loop_invalid_unpack();
extern void nas_test_pack_unpack_loop();
extern void nas_test_ind();
extern void nas_test_ind_exit();
//DMS
extern void dms_test_pack_unpack_loop();
extern void dms_test_pack_unpack_loop_invalid_unpack();
extern void dms_dummy_unpack();
extern void dms_test_ind();
extern void dms_test_ind_exit();

//UIM
extern void uim_test_pack_unpack_loop();
extern void uim_test_pack_unpack_loop_invalid_unpack();
extern void uim_test();
extern void uim_read_IMSI();
extern void uim_read_IMSI_LTE();

//SMS
extern void sms_test_pack_unpack_loop_invalid_unpack();
extern void sms_test_pack_unpack_loop();
extern void sms_test_ind();
extern void sms_test_ind_exit();
extern void sms_test_msg_encode_decode();
extern int send_long_sms();
extern int send_wcdma_sms_different_encodings();

//SWI LOC
extern void swiloc_test_pack_unpack_loop();
extern void swiloc_test_pack_unpack_loop_invalid_unpack();

// TMD
extern void tmd_test_pack_unpack_loop_invalid_unpack();
extern void tmd_test_pack_unpack_loop();

//CAT
extern void cat_test_pack_unpack_loop();
extern void cat_test_pack_unpack_loop_invalid_unpack();
extern void cat_test_ind();
extern void cat_test_ind_exit();

//PDS
extern void pds_test_pack_unpack_loop_invalid_unpack();
extern void pds_test_pack_unpack_loop();
extern void pds_test_ind();
extern void pds_test_ind_exit();

//SAR
extern void sar_test_pack_unpack_loop_invalid_unpack();
extern void sar_test_pack_unpack_loop();

//RMS
extern void rms_test_pack_unpack_loop_invalid_unpack();
extern void rms_test_pack_unpack_loop();

//VOICE
extern void voice_test_pack_unpack_loop_invalid_unpack();
extern void voice_test_pack_unpack_loop();
extern void voice_test_ind();
extern void voice_test_ind_exit();

//SWIAVMS
extern void swiavms_loop();

//IMSA
void imsa_unit_test();
void imsa_loop();

//AUDIO
extern void audio_test_pack_unpack_loop_invalid_unpack();
extern void audio_test_pack_unpack_loop();

//SWIAUDIO
extern void swiaudio_test_pack_unpack_loop();
extern void swiaudio_test_pack_unpack_loop_invalid_unpack();

//IMS
extern void ims_test_pack_unpack_loop();
extern void ims_test_pack_unpack_loop_invalid_unpack();
extern void ims_test_ind();
extern void ims_test_ind_exit();

//SWIDMS
extern void swidms_test_pack_unpack_loop_invalid_unpack();
extern void swidms_test_pack_unpack_loop();

//SWIAVMS-AVC2
extern void swiavms_loop_avc2();

int iCheckQMIDevice();

int GetIPAliasMode();
int local_fprintf (const char *format, ...)
{
    va_list arg;
    int done =0;
    FILE *stream = NULL;

    stream = fopen("./TestResults/testResult.csv", "a");
    if ( stream ){
        va_start (arg, format);
        done = vfprintf (stream, format, arg);
        va_end (arg);
        fclose(stream);
    }
    return done;
}

int
client_fd(
        uint8_t svc
        )
{
    //TODO support first PDN only for the moment
    char szDevicePath[32]={0};
    int fd = -1; 
    snprintf(szDevicePath,31,"/dev/qcqmi%d",g_modem_index);
    fd = open(szDevicePath, O_RDWR);
    if(fd>=0)
    {
        if(ioctl(fd, QMI_GET_SERVICE_FILE_IOCTL, svc)!=0)
        {
            if(fd>=0)
                close(fd);
            return -1;
        }
    }
    else
    {
        fprintf(stderr,"Fail Open:%s\n",szDevicePath);
    }
    return fd;
}

int file_fd()
{
    //TODO support first PDN only for the moment
    char szDevicePath[32]={0};
    int fd = -1; 
    snprintf(szDevicePath,31,"/dev/qcqmi%d",g_modem_index);
    fd = open(szDevicePath, O_RDWR);
    if(fd>=0)
    {
        
    }
    else
    {
        fprintf(stderr,"Fail Open:%s\n",szDevicePath);
    }
    return fd;
}

void check_running_pids()
{
    int i = 0;
    for(i=0;i<MAX_TEST_PROCESS;i++)
    {
        if(__getpgid(pids[i]) < 0)
        {
            pids[i]=0;
        }
    }
    
}

void print_running_pids()
{
    int i = 0;
    for(i=0;i<MAX_TEST_PROCESS;i++)
    {
        if( pids[i]!=0)
        {
            fprintf(stderr,"%d. pid:%d %s\n",i,pids[i],szFunction[i]);
        }
    }
}

void kill_running_pids()
{
    int i = 0;
    for(i=0;i<MAX_TEST_PROCESS;i++)
    {
        if( pids[i]!=0)
        {
            fprintf(stderr,"kill %d\n",pids[i]);
            kill(pids[i], SIGKILL);
        }
        if( cmd_pids[i]!=0)
        {
            fprintf(stderr,"kill %d\n",cmd_pids[i]);
            kill(cmd_pids[i], SIGKILL);
        }
    }
    if(GetIPAliasMode()==0)
    {
        system("ps xl | grep gobi- | grep -v grep | awk '{print $3}' |  xargs -r -t kill");
    }
}

void childHandler(int signum)
{
    pid_t childpid;
    int status;
    int i=0;
    UNUSEDPARAM(signum);
    while ((childpid = waitpid( -1, &status, WNOHANG)) > 0)
    {    
        if (WIFEXITED(status))
            printf("Child %d exited naturally\n", childpid);

        if (WIFSIGNALED(status))
            printf("Child %d exited because of signal\n", childpid);
    }

    if (childpid == -1 && errno != ECHILD)
    {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }
    
    for(i=0;i<MAX_TEST_PROCESS;i++)
    {
        if(pids[i]==childpid)
        {
            pids[i] = 0;
            break;
        }
    }
    iCompleted += 1;
}

void run_function(void *(func_ptr)(),char* szName)
{
    strncpy(szFunction[process_counter],szName,254);
    if(g_fork==1)
    {
        pid_t pid;
        pid = fork();
        if(pid==0)
        {
            (*func_ptr)();
            exit(0);
        }
        else if (pid > 0)
        {
            if(process_counter>=MAX_TEST_PROCESS)
            {
                fprintf(stderr,"!Run out of process ID!\n");
                exit(0);
            }
            pids[process_counter] = pid;
            printf("%d. pid:%d\n", ++process_counter,pid );

        }
        else
        {
            // fork failed
            printf("fork() failed!\n");
            return ;
        }
    }
    else
    {
       (*func_ptr)();
    }
    sleep(1);
}

void cat_tests()
{
    cat_test_pack_unpack_loop_invalid_unpack();
    cat_test_pack_unpack_loop();
    cat_test_ind();
    cat_test_ind_exit();
}

void sms_tests()
{
   int rtn;
   sms_test_pack_unpack_loop_invalid_unpack();
   sms_test_pack_unpack_loop();
   sms_test_ind_exit();
   sms_test_ind();
   sms_test_ind_exit();

   /* Both send_long_sms() and  send_wcdma_sms_different_encodings() 
    * create a new read thread, kill that  with sms_test_ind_exit() 
    */
   rtn = send_long_sms();
   if (rtn !=0)
   {
     printf("[%s] fail! rtn = %d\n", __func__, rtn );
   }
   sms_test_ind_exit();
   send_wcdma_sms_different_encodings();
   sms_test_ind_exit();
   sms_test_msg_encode_decode();
}

void dms_tests()
{
    if (g_runoem_demo==0)
    {
        dms_test_pack_unpack_loop_invalid_unpack();
        printf("========START dms_dummy_unpack========\n");
        dms_dummy_unpack();
        printf("========END dms_dummy_unpack========\n");
    }
    dms_test_pack_unpack_loop();
    dms_test_ind();
    dms_test_ind_exit();
}

void loc_tests()
{
   loc_test_pack_unpack_loop_invalid_unpack();
   loc_test_pack_unpack_loop();
   loc_test_ind();
   loc_test_ind_exit();
}

void swioma_tests()
{
    swioma_loop();
    sleep(10);
    swioma_loop_exit();
}

void swiomaext_tests()
{
   swiomaext_loop();
   sleep(10);
   swiomaext_loop_exit();
}

void qos_tests()
{
    qos_loop();
    qos_loop_exit();
}

void uim_tests()
{
    uim_test_pack_unpack_loop_invalid_unpack();
    uim_read_IMSI();
    uim_read_IMSI_LTE();
    uim_test();
    uim_test_pack_unpack_loop();
    uim_read_IMSI();
}

void rms_tests()
{
    rms_test_pack_unpack_loop_invalid_unpack();
    rms_test_pack_unpack_loop();
}

void voice_tests()
{
    voice_test_pack_unpack_loop_invalid_unpack();
    voice_test_pack_unpack_loop();
    voice_test_ind();
    voice_test_ind_exit();
}

void sar_tests()
{
    sar_test_pack_unpack_loop_invalid_unpack();
    sar_test_pack_unpack_loop();
}

void wds_tests()
{
    wds_test_pack_unpack_loop_invalid_unpack();
    wds_test_pack_unpack_loop();
    wds_test_ind();
    wds_test_ind_exit();
}

void wds_qmap_tests()
{
    wds_qmap_pack_unpack_loop();
    wds_test_ind_exit();
}

void tmd_tests()
{
    tmd_test_pack_unpack_loop_invalid_unpack();
    tmd_test_pack_unpack_loop();
}

void nas_tests()
{
    #if _ASYNC_READ_ENABLE_
    if(g_auto_test_enable)
    {
        for(;;sleep(5))
        {
            nas_test_pack_unpack_loop();
            if(iCheckQMIDevice()!=1)
                break;
        }
        return ;
    }
    #endif
    nas_test_pack_unpack_loop_invalid_unpack();
    nas_test_pack_unpack_loop();
}

void pds_tests()
{
    if (g_runoem_demo==0)
    {
        pds_test_pack_unpack_loop_invalid_unpack();
    }
    pds_test_pack_unpack_loop();
    pds_test_ind();
    pds_test_ind_exit();
}

void audio_tests()
{
    audio_test_pack_unpack_loop_invalid_unpack();
    audio_test_pack_unpack_loop();
}

void swiaudio_tests()
{
    swiaudio_test_pack_unpack_loop_invalid_unpack();
    swiaudio_test_pack_unpack_loop();
}

void ims_tests()
{
    ims_test_pack_unpack_loop_invalid_unpack();
    ims_test_pack_unpack_loop();
    ims_test_ind();
    ims_test_ind_exit();
}

void swiloc_tests()
{
    swiloc_test_pack_unpack_loop_invalid_unpack();
    swiloc_test_pack_unpack_loop();
}

void swidms_tests()
{
    swidms_test_pack_unpack_loop_invalid_unpack();
    swidms_test_pack_unpack_loop();
}

int GetIPAliasMode()
{
    int iMode = 1;
    int result = -1;
    char szDevicePath[32]={0};
    int fd = -1; 
    snprintf(szDevicePath,31,"/dev/qcqmi%d",g_modem_index);
    fd = open(szDevicePath, O_RDWR);
    if(fd>=0)
    {
       result = ioctl(fd, 0x8BE0 + 18, &iMode);
       close(fd);
    }
    else
    {
       fprintf(stderr,"fd failed:%d\n",fd); 
    }
    if(result!=0)
    {
       iMode = 1;
    }
    return iMode;
}

int GetNumberOfQmapSupported()
{
    unsigned int iNumber = MAX_QMAP_INSTANCE;
    int result = -1;
    char szDevicePath[32]={0};
    int fd = -1; 
    snprintf(szDevicePath,31,"/dev/qcqmi%d",g_modem_index);
    fd = open(szDevicePath, O_RDWR);
    if(fd>=0)
    {
       //Get Max RMNet supported 
       result = ioctl(fd, QMI_GET_QMAP_SUPPORT, &iNumber);
       close(fd);
       if (result != 0)
       {
           fprintf(stderr,"QMI_GET_QMAP_SUPPORT ioctl failed: %d\n",result);
           iNumber = 0;
       }
    }
    else
    {
       fprintf(stderr,"fd failed:%d\n",fd); 
    }
    if(result!=0)
    {
       iNumber = MAX_QMAP_INSTANCE;
    }
    fprintf(stderr,"iNumber:%d\n",iNumber); 
    return iNumber;
}

int GetAvmsVersionSupported()
{
    int result = -1;
    char szDevicePath[32]={0};
    int fd = -1;
    int svctype = -1;
    uint8_t totalSvc;
    uint8_t i,j;
    uint8_t svcVersion[255*5]= {0};

    snprintf(szDevicePath,31,"/dev/qcqmi%d",g_modem_index);
    fd = open(szDevicePath, O_RDWR);
    if(fd>=0)
    {
       //Get Version Info Supported
       result = ioctl(fd, QMI_GET_SVC_VERSION_IOCTL, &svcVersion[0]);
       close(fd);
       if(result >=0)
       {
            totalSvc = svcVersion[0];
            j = 1;
            /* set default service type as 0XF3 */
            svctype = eQMI_SVC_SWI_M2M_CMD;

            /* TLV 0x01 of QMI_CTL_GET_VERSION_INFO is :
             * byte[0] = numInstances, for each instance
             * svcType: 1 byte, svc major version: 2 byte, svc minor version: 2 byte
             */
            for (i=0; i < totalSvc; i++) {
                if  (svcVersion[j] == eQMI_SVC_SWI_M2M_CMD_2)
                {
                    svctype = svcVersion[j];
                    break;
                }
                else
                {
                    j = j + 5;
                }
            }
       }
       else
       {
           fprintf(stderr,"ioctl QMI_GET_SVC_VERSION_IOCTL error :%d\n",result);
       }
    }
    else
    {
       fprintf(stderr,"fd failed:%d\n",fd);
    }

    fprintf(stderr,"AVMS service type:%d\n",svctype);
    return svctype;
}



int iCheckQMInode(uint8_t index, char *szNetPath)
{
    char qmipath[1024];
    struct stat statbuf;
    snprintf(qmipath,1023,"%s%s%d",szNetPath,SYS_FS_NET_QMI_PATH,index);
    printf("QMI Folder Path:%s\n",qmipath);
    if (lstat(qmipath, &statbuf) != 0)
    {
        return 0;
    }
    return 1;
}

int getEthName()
{
    DIR *dir;
    struct dirent *ent;
    struct stat statbuf;
    char buf[1024];
    char linkpath[1024];
    ssize_t len;
    if ((dir = opendir (SYS_FS_NET_PATH)) != NULL) 
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) 
        {
            snprintf(linkpath,1023,"%s%s",SYS_FS_NET_PATH,ent->d_name);
            if (lstat(linkpath, &statbuf) != 0)
                continue;
            if ((len = readlink(linkpath, buf, sizeof(buf)-1)) != -1)
            {
                buf[len] = '\0';
                if(strstr(buf,".8/"))
                {
                    if(iCheckQMInode(g_modem_index,linkpath))
                    {
                        wds_setEthName(ent->d_name);
                        strncpy(g_szEthName,ent->d_name,sizeof(g_szEthName));
                        printf("Eth Device Name:%s\n",ent->d_name);
                        break;
                    }
                }
                else if(strstr(buf,".10/"))
                {
                    if(iCheckQMInode(g_modem_index,linkpath))
                    {
                        wds_setEthName(ent->d_name);
                        strncpy(g_szEthName,ent->d_name,sizeof(g_szEthName));
                        printf("Eth Device Name:%s\n",ent->d_name);
                        break;
                    }
                }
            }
        
        }
        closedir (dir);
    }
    else 
    {
        return -1;
    }
    return 0;
}

int iCheckQMIDevice()
{
    int fd = file_fd();
    if(fd<0)
        return 0;
    close(fd);
        return 1;
}

void SetEthName(unsigned int index,int IPVersion)
{
    if(GetIPAliasMode()==0)
    {
        char szEthName[255]="gobi-";
        snprintf(szEthName,254,"gobi-%d-%d",g_modem_index,index);
        fprintf(stderr,"========Update EthName %s===========\n",szEthName);
        wds_setEthName(szEthName);
    }
    else
    {
        char szEthName[255]="";
        if((index==0)||(IPVersion==6))
        {
            snprintf(szEthName,sizeof(szEthName),"%s",g_szEthName);
        }
        else
        {
            snprintf(szEthName,sizeof(szEthName),"%s:%d",g_szEthName,index-1);
        }
        fprintf(stderr,"========Update EthName %s===========\n",szEthName);
        wds_setEthName(szEthName);
    }
}

void start_wds_qmap_test(unsigned int index,int IPVersion)
{
    if(g_num_of_rmnet_supported!=0)
    {
        SetEthName(index,IPVersion);
        wds_setID(0x80+index);
    }
    else
    {
        wds_setprofile(index);
    }
    wds_setIPFamily(IPVersion);
    run_function((void *)&wds_qmap_tests,FUNCTION_NAME(wds_qmap_tests));
    sleep(5);
}

void packingdemo()
{
    unsigned int i = 0;
    signal(SIGCHLD, childHandler);
    if(g_runqmap_demo)
    {
        unsigned int index = 0;
        getEthName();
        if(g_num_of_rmnet_supported==0)
        {
            if(g_profile_index<MAX_QMAP_INSTANCE)
                index = g_profile_index;
            else
                index=0;
            
            fprintf(stderr,"========Profile ID:%d===========\n",g_profile_index+1);
            if(g_ip_version[index]==7)
            {
                start_wds_qmap_test(g_profile_index,4);
                start_wds_qmap_test(g_profile_index,6);
            }
            else
            {
                start_wds_qmap_test(g_profile_index,g_ip_version[index]);
            }
            if(g_auto_test_enable)
            {
               run_function((void *)&qos_tests,FUNCTION_NAME(qos_tests));
               sleep(5);
               #if _ASYNC_READ_ENABLE_
               run_function((void *)&nas_tests,FUNCTION_NAME(nas_tests));
               sleep(5);
               #endif
            }
        }
        for(index=0;index<g_num_of_rmnet_supported;index++)
        {
            if(g_ip_version[index])
            {
                fprintf(stderr,"========%d/%d. MUXID:0x%02X IPv%d===========\n",index+1,g_num_of_rmnet_supported,0x80+index,g_ip_version[index]);
                
                if(g_ip_version[index]==7)
                {
                    start_wds_qmap_test(index,4);
                    start_wds_qmap_test(index,6);
                }
                else
                {
                    start_wds_qmap_test(index,g_ip_version[index]);
                }
                if(g_auto_test_enable)
                {
                   QosSetMuxID(0x80+index);
                   run_function((void *)&qos_tests,FUNCTION_NAME(qos_tests));
                   sleep(5);
                   #if _ASYNC_READ_ENABLE_
                   run_function((void *)&nas_tests,FUNCTION_NAME(nas_tests));
                   sleep(5);
                   #endif
                }
            }       
        }
        while(iCompleted!=process_counter)
        {
            sleep(1);  
            check_running_pids();
            if(iCheckQMIDevice()!=1)
            {
                fprintf(stderr,"Killing process\n");
                kill_running_pids();
                sleep(1);
                exit(1);
            }
        };
        exit(0) ;
    }
    for(i=0;i<MAX_TEST_PROCESS;i++)
    {
        pids[i] = 0;
        memset(szFunction[i],0,255);
    }
    run_function((void *)&ims_tests,FUNCTION_NAME(ims_tests));

    run_function((void *)&swiaudio_tests,FUNCTION_NAME(swiaudio_tests));

    run_function((void *)&audio_tests,FUNCTION_NAME(audio_tests));

    run_function((void *)&imsa_loop,FUNCTION_NAME(imsa_loop));
    if (g_skip_swiavms == 0)
    {
        if ( GetAvmsVersionSupported() == eQMI_SVC_SWI_M2M_CMD_2 )
        {
            run_function((void *)&swiavms_loop_avc2,FUNCTION_NAME(swiavms_loop_avc2));
        }
        else
        {
            run_function((void *)&swiavms_loop,FUNCTION_NAME(swiavms_loop));
        }
    }
    run_function((void *)&rms_tests,FUNCTION_NAME(rms_tests));
    run_function((void *)&voice_tests,FUNCTION_NAME(voice_tests));
    run_function((void *)&sar_tests,FUNCTION_NAME(sar_tests));
    run_function((void *)&wds_tests,FUNCTION_NAME(wds_tests));
    run_function((void *)&tmd_tests,FUNCTION_NAME(tmd_tests));
    run_function((void *)&dms_tests,FUNCTION_NAME(dms_tests));
    run_function((void *)&nas_tests,FUNCTION_NAME(nas_tests));
    run_function((void *)&uim_tests,FUNCTION_NAME(uim_tests));
    run_function((void *)&pds_tests,FUNCTION_NAME(pds_tests));
    run_function((void *)&cat_tests,FUNCTION_NAME(cat_tests));
    run_function((void *)&sms_tests,FUNCTION_NAME(sms_tests));
    run_function((void *)&loc_tests,FUNCTION_NAME(loc_tests));
    run_function((void *)&swioma_tests,FUNCTION_NAME(swioma_tests));
    run_function((void *)&swiomaext_tests,FUNCTION_NAME(swiomaext_tests));
    run_function((void *)&qos_tests,FUNCTION_NAME(qos_tests));
    run_function((void *)&swiloc_tests,FUNCTION_NAME(swiloc_tests));
    run_function((void *)&swidms_tests,FUNCTION_NAME(swidms_tests));
    if(g_fork==1)
    {
        int counter = 0;
        print_running_pids();
        while(iCompleted!=process_counter)
        {
            sleep(1);  
            check_running_pids();
            if(counter++>60*5)
            {
               fprintf(stderr,"%d/%d\n",iCompleted,process_counter);
               print_running_pids();
               kill_running_pids();
               exit(1);
            }
        };
    }
}

void start_wds_api_test(unsigned int index,int IPversion)
{
    if ( g_runqmap_demo == 1)
    {
        fprintf(stderr,"========%d. MUXID:0x%02X IPv%d===========\n",index+1,0x80+index,g_ip_version[index]);
        SetEthName(index,IPversion);
        wds_setID(0x80+index);
    }
    wds_setIPFamily(IPversion);
    run_function((void *)&wds_qmap_tests,FUNCTION_NAME(wds_qmap_tests));
}

void oemApiTest(void)
{
    unsigned int index = 0;
    int cnt = 0;

    signal(SIGCHLD, childHandler);
    getEthName();

    sleep(5);
    pds_tests();
    sleep(5);
    nas_test_pack_unpack_loop();
    sleep(5);
    dms_tests();
    sleep(5);
    sms_test_ind();
    sms_test_ind_exit();
    sleep(5);
    loc_test_pack_unpack_loop();
    sleep(5);
    swiloc_test_pack_unpack_loop();
    sleep(5);
    if ( GetAvmsVersionSupported() == eQMI_SVC_SWI_M2M_CMD_2 )
    {
        swiavms_loop_avc2();
    }
    else
    {
        swiavms_loop();
    }
    sleep(5);
    swioma_pack_unpack_loop();
    sleep(5);
    swioma_loop_exit();
    uim_test_pack_unpack_loop();
    uim_test();
    uim_read_IMSI();
    uim_read_IMSI_LTE();
    sleep(5);

    if((g_runqmap_demo==0)&&
        (g_num_of_rmnet_supported==0))
    {
        g_num_of_rmnet_supported = 1;
    }
    for(index=0;index<g_num_of_rmnet_supported;index++)
    {
        if(g_ip_version[index])
        {
            
            if(g_ip_version[index]==7)
            {
                start_wds_api_test(index,4);
                start_wds_api_test(index,6);
            }
            else
            {
                start_wds_api_test(index,g_ip_version[index]);
            }            
            /* wait 60 seconds so that the wds child process has enough time to do its job */
            for ( cnt = 0; cnt < 90; cnt+=2)
            {
                if (iCheckQMIDevice() == 0)
                {
                    /* exit the loop since qcqmi disappears */
                    fprintf(stderr,"exit loop since qcqmi disappears\n");
                    break;
                }
                sleep(2);
            }
            if(g_runqmap_demo==1)
            {
                QosSetMuxID(0x80+index);
                g_qos_started = 1;
            }
            run_function((void *)&qos_tests,FUNCTION_NAME(qos_tests));
            /* wait 30 seconds so that the qos child process has enough time to do its job */
            for ( cnt = 0; cnt < 30; cnt+=2)
            {
                if (iCheckQMIDevice() == 0)
                {
                    /* exit the loop since qcqmi disappears */
                    fprintf(stderr,"exit loop since qcqmi disappears\n");
                    break;
                }
                sleep(2);
            }

            if(g_fork==1)
            {
                int counter = 0;
                print_running_pids();
                while(iCompleted!=process_counter)
                {
                    sleep(1);  
                    check_running_pids();
                    if(counter++>60)
                    {
                       fprintf(stderr,"%d/%d\n",iCompleted,process_counter);
                       print_running_pids();
                       kill_running_pids();
                       break;
                    }
                };
            }
        }
    }

}

int ReadDevcieFDWithTimeout(int fd,uint8_t *prsp, int iLen,int iTimeout)
{
   fd_set readfds;
   int rc =0;
   int rtn;
   struct timeval timeout;
   FD_ZERO(&readfds);
   FD_SET(fd, &readfds);
   if((iTimeout<0)||(iLen<0)||(prsp==NULL))
   {
      return -1;
   }
   if(iTimeout!=0)
   {
       timeout.tv_sec = iTimeout;
       timeout.tv_usec = 0;
       rc = select(fd+1, &readfds, NULL, NULL, &timeout);
       if (-1==rc)
       {
          liteqmi_log(eLOG_DEBUG,"Error with select(): %s\n", strerror(errno));
          return -1;
       }
       else if (0==rc)
       {
          liteqmi_log(eLOG_DEBUG,"Timeout\n");
          return 0;
       }
       else
       {
          liteqmi_log(eLOG_DEBUG,"select() returned: %d\n", rc);
          liteqmi_log(eLOG_DEBUG,"FD_ISSET(wds, &readfds): %d\n", FD_ISSET(fd, &readfds));
       }
   }
   rtn = read(fd, prsp, iLen);
   liteqmi_log(eLOG_DEBUG, "rtn: %d\n", rtn);
   return rtn;
}

int CheckDevcieState(int fd)
{
   int rtn;
   uint8_t rsp[255];
   rtn = ReadDevcieFDWithTimeout(fd, (uint8_t*)&rsp, 255,1);
   liteqmi_log(eLOG_DEBUG, "rtn: %d\n", rtn);
   if(rtn>=0)
      return 1;
   else
      return -1;
}

void dump_hex(uint16_t dataLen,uint8_t *hexData )
{
#ifdef HEXDUMP
    uint16_t j = 0;

    for (j = 0; j < dataLen; j++)
    {
        printf("%02x ", hexData[j]);
        if ((j+1)%16 == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
#else
    UNUSEDPARAM(dataLen);
    UNUSEDPARAM(hexData);
#endif
}

#define MAX_BIN_PATH_LENGTH 256
int checkPath(const char *szBinFile)
{
    char *PATH = getenv("PATH");
    char *DefaultBinPath = "/bin/:/sbin/:/usr/bin/:/usr/local/bin:/usr/sbin/:/usr/local/sbin:";
    char *pSplit = NULL ;
    char szBIN[MAX_BIN_PATH_LENGTH] ;
    int i=0;
    int j=0;
    int copylength = 0;
    int fd = -1;
    if(PATH==NULL)
    {
        PATH = DefaultBinPath;
    }
    pSplit = strstr(PATH,":");
    while(pSplit)
    {
        j = pSplit -PATH;
        if(j<1)
        {
            break;
        }
        copylength = (j-i)+1;
        if(copylength>MAX_BIN_PATH_LENGTH)
        {
            fprintf(stderr,"PATH too long :%s\n",pSplit);
            break;
        }
        memset(szBIN,0,MAX_BIN_PATH_LENGTH);
        snprintf(szBIN,copylength,"%s",PATH+i);
        copylength = strlen(szBinFile)+2;
        if(copylength>MAX_BIN_PATH_LENGTH)
        {
            fprintf(stderr,"PATH too long :%s\n",pSplit);
            break;
        }
        if((strlen(szBIN)+1)>=MAX_BIN_PATH_LENGTH)
        {
            fprintf(stderr,"BIN too long :%s\n",szBIN);
            break;
        }
        snprintf(szBIN+strlen(szBIN),copylength,"/%s",szBinFile);
        fd = open(szBIN, O_RDONLY);
        if( fd>=0 )
        {
            // file exists
            close(fd);
            return 1;
        } 
        pSplit = strstr(pSplit+1,":");
        i=j+1;
        
    }
    return 0;
}

#if _ASYNC_READ_ENABLE_

void CloseFD(int *pFd)
{
    if(pFd)
    {
        if(*pFd>0)
        {
            close(*pFd);
        }
        *pFd = -1;
    
}
}
int iAsyncRead(struct aiocb *paiocb,int *pFd, size_t size,uint8_t *pBuf)
{
    
    memset(paiocb, 0, sizeof(struct aiocb));
    paiocb->aio_nbytes = size;
    paiocb->aio_lio_opcode = LIO_READ;     
    paiocb->aio_buf = pBuf;
    paiocb->aio_fildes = *pFd;
    if (aio_read(paiocb) == -1) {
        printf(" Error at aio_read(): %s\n",
               strerror(errno));
        CloseFD(pFd);
        return -1;
    }
    return 0;
}

int iWaitAsyncReadDataReady(struct aiocb *paiocb,int *pFd,int iTimeoutInSec)
{
    int iTimeout = 0;
    int iSecondCount = 0;
    int err=-1;
    int ret = 0;
    while ((err = aio_error (paiocb)) == EINPROGRESS)
    {
        usleep(1000);
        if(iTimeout++>1000)
        {
            if(iSecondCount++>iTimeoutInSec)
            {
                return -ETIME;
            }
            iTimeout = 0;
        }
    };
    err = aio_error(paiocb);
    if(err!=0)
    {
        printf("aio_error: %d\n", err);
        CloseFD(pFd);
        return -1;
    }
    ret = aio_return(paiocb);
    return ret;
}
#endif

static unsigned long read_obj(const char *name,char *slab_buffer,unsigned slab_size)
{
    FILE *f = fopen(name, "r");

    if (!f)
        slab_buffer[0] = 0;
    else {
        if (!fgets(slab_buffer, slab_size-1, f))
            slab_buffer[0] = 0;
        fclose(f);
        if (slab_buffer[strlen(slab_buffer)] == '\n')
            slab_buffer[strlen(slab_buffer)] = 0;
    }
    return strlen(slab_buffer);
}

static int slab_mismatch(char *slab,regex_t *pattern)
{
    return regexec(pattern, slab, 0, NULL, 0);
}
 /*
 * Get the contents of an attribute
 */
static unsigned long get_obj(const char *name, char *slab_buffer,unsigned slab_size)
{
    if (!read_obj(name,slab_buffer,slab_size))
        return 0;

    return atol(slab_buffer);
}

#define KMALLOC_PATTHEN "kmalloc*"
#define HANDLE_DT_DIR \
if (chdir(de->d_name))\
{\
    fprintf(stderr,"Unable to access slab %s\n", de->d_name);\
    goto read_slab_dir_clsoe_dir;\
}\
slab->objects = get_obj("objects",&slab_buffer[0],sizeof(slab_buffer));\
chdir("..");\
slab++;\

int read_slab_dir(slabinfo *Slabinfo)
{
    DIR *dir = NULL;
    struct dirent *de = NULL;
    slabinfo *slab = Slabinfo;
    int count = 0;
    int err =-1;
    char slab_buffer[4096]={0};
    char *pattern_source = KMALLOC_PATTHEN;
    regex_t pattern;
    memset(&slab_buffer,0,4096);
    err = regcomp(&pattern, pattern_source, REG_ICASE|REG_NOSUB);
    if (err)
    {
        fprintf(stderr,"%s: Invalid pattern '%s' code %d\n",
            KMALLOC_PATTHEN, pattern_source, err);
        return err;
    }
    if (chdir("/sys/kernel/slab") && chdir("/sys/slab"))
    {
        fprintf(stderr,"SYSFS support for SLUB not active\n");
        err = -1;
        return err;
    }
    dir = opendir(".");
    if(dir==NULL)
    {
        return 0;
    }
    while ((de = readdir(dir))) {
        if (de->d_name[0] == '.' ||
            (slab_mismatch(de->d_name,&pattern)))
                continue;
        switch (de->d_type) {
           case DT_LNK:
            count = readlink(de->d_name, slab_buffer, sizeof(slab_buffer));
            if (count < 0)
            {
                err = -1;
                goto read_slab_dir_clsoe_dir;
            }
            slab_buffer[count] = 0;
            HANDLE_DT_DIR;
            break;
           case DT_DIR:
            HANDLE_DT_DIR;
            break;
           default :
            fprintf(stderr,"Unknown file type \n");
            err = -1;
            goto read_slab_dir_clsoe_dir;
            break;
        }
    }
read_slab_dir_clsoe_dir:
    closedir(dir);
    return err;
}

unsigned long get_total_slab_activity()
{
    unsigned long total = 0;
    unsigned i = 0;
    slabinfo Slabinfo[MAX_SLABS];
    memset(&Slabinfo, 0, sizeof(slabinfo)*MAX_SLABS);
    if(read_slab_dir(&Slabinfo[0])!=0)
    {
        return -1;
    }
    for (; i < MAX_SLABS; i++)
    {
        total += Slabinfo[i].objects;
    }
    return total;
}

void dump_common_resultcode (void *ptr)
{
    unpack_result_t *result =
            (unpack_result_t*) ptr;
    CHECK_DUMP_ARG_PTR_IS_NULL
    if(swi_uint256_get_bit (result->ParamPresenceMask, 2))
        printf("%s Unpack response result Code: %d\n",__FUNCTION__, result->Tlvresult);
}

bool check_uint256_whitelist_mask(
    swi_uint256_t whitelistmask,
    swi_uint256_t mask)
{
    uint8_t i = 0;
    i=(SWI_UINT256_WORD_COUNT*SWI_UINT256_BITS_PER_WORD)-1;
    do
    {
        if(swi_uint256_get_bit(mask,i))
        {
            if(!swi_uint256_get_bit(whitelistmask,i))
            {
                return false;
            }
        }
        i--;
    }while(i);
    return true;
}
