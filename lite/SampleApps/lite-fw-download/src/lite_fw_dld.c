#include "lite_fw_dld.h"

#define debug 0

int g_devMode;
unsigned g_xid = 1;
/*
 * To indicate device nodes are created manually
 */
static bool bFixedNode = false;
static bool g_crashStateIgnore = false;
static bool bDMreset = false;
static FILE *fpLogFile = NULL;
static int  iUARTMode = -1;
char carriers[][20] =
{
    {"VODAFONE"},
    {"VERIZON"},
    {"ATT"},
    {"SPRINT"},
    {"TMOBILE"},
    {"GENERIC"},
    {"TELEFONICA"},
    {"ITALIA"},
    {"ORANGE"},
    {"ORANGE-EU"},
    {"TELSTRA"},
    {"BELL"},
    {"TELUS"},
    {"ROGERS"},
    {"DOCOMO"},
    {"SWISSCOM"},
    {"AERIS"},
    {"PTCRB"},
};

static void print_FW_file_details(struct _userOptions *pUserOptData);

void PrintUsage()
{
    printf( "\r\n" );
    printf( "App usage: \r\n\r\n" );
    printf( "  <appName> -c <QMI/MBIM mode> -d <QDL Port> -p <QMI/MBIM Device path> -f <FW path> -h \n\n" );       
    printf( "  -c  --devmode \n ");
    printf( "        Specifies the mode of the device, MBIM or QMI\n\n");
    printf( "        Defaults to QMI if not specified\n\n");     
    printf( "  -d  --qdl \n ");
    printf( "        Specifies the QDL port when modem switches to BOOT and HOLD mode to download firmware.\n\n");
    printf( "        For example: -d /dev/ttyUSB0\n\n");
    printf( "        Defaults to /dev/ttyUSB0 if not specified\n\n");
    printf( "  -p  --devpath \n ");
    printf( "        Specifies the QMI or MBIM device\n\n");
    printf( "        Defaults to /dev/qcqmi0 for QMI and /dev/cdc-wdm0 for MBIM\n\n");
    printf( "  -f  --fwpath [folder to firmware images]\n" );
    printf( "        This specifies the folder location of the firmware images. This option is mandatory. \n" );
    printf( "        - 9x30: Specify the path containing a carrier FW package (.cwe and .nvu) or an OEM PRI (.nvu)\n\n" );
    printf( "  -q --fwtoread or folder with .CWE+.NVU combination\n" );
    printf( "        Use this option to read a firmware file (or folder with .CWE+.NVU) info. \n" );
    printf( "        App will exit after printing the details \n" );
    printf( "  -i  --ignore crash state checking or not.Default value is 0 means crash state checking is required\n" );
    printf( "          - 0: crash state checking required (default value)\n" );
    printf( "          - 1: ignore crash state checking\n\n" );
    printf( "  -l  --logfile  \n" );
    printf( "        Specific custom log path.\n\n" );
    printf( "  -b  --blocksize  \n" );
    printf( "        File Read Block Size.\n\n" );
    printf( "  -m  --modelfamily  \n" );
    printf( "          - 0: Auto Detect (default value)\n" );
    printf( "          - 1: 9x15 Family\n" );
    printf( "          - 2: WP9x15 Family\n" );    
    printf( "          - 3: 9x30 Family\n" );
    printf( "          - 4: 9x07 or 9x50 Family\n" );
    printf( "          - 5: 9x06 Family\n\n" );
    printf( "  -h  --help  \n" );
    printf( "        This option prints the usage instructions.\n\n" );
    printf( "  -e  --enable/disable debug logs  \n" );
    printf( "           - 0 : Debug logs disable.\n" );
    printf( "           - 1 : Debug logs enable.\n\n" );
    printf( " -r --dmreset reset modem using DM command \n" );
    printf( " -u --enable/disable UART mode on firehose protocol  \n" );
    printf( "           - 0 : Disable.\n" );
    printf( "           - 1 : Enable.\n\n" );
    printf( " This operation only support when modem in App mode.\n" );
    printf( " Don't use this option when modem is already in QDL mode.\n" );
    printf( " This option should not be used in normal download operation.\n" );
    printf( " Modem will not be reset on QDL mode\n\n" );
}

/* Command line options to firmware download tool */
const char * const short_options = "c:d:p:f:q:l:i:b:m:e:u:hr";

/* Command line long options for firmware download tool */
const struct option long_options[] = {
    {"help",   0, NULL, 'h'},      /* Provides terse help to users */
    {"devmode",1, NULL, 'c'},      /* Device mode QMI or MBIM */
    {"qdl",    1, NULL, 'd'},      /* QDL port path */
    {"devpath",    1, NULL, 'p'},      /* QMI/MBIM device path */
    {"fwpath", 1, NULL, 'f'},      /* FW files path */
    {"fwtoread", 1, NULL, 'q'},      /* FW files path */
    {"logfile",   1, NULL, 'l'},      /* custom log file path */
    {"blocksize",   1, NULL, 'b'},      /* Read Block Size */
    {"crashStateChecking",   1, NULL, 'i'},      /* crash state checking */
    {"modelfamily",1,NULL,'m'},/* model family */
    {"debuglogsflag",1,NULL,'e'},/* debug logs flag */
    {"dmreset",0,NULL,'r'},/* dmreset flag */
    {"uartmode",0,NULL,'u'},/* uartmode flag */
    {NULL,     0, NULL,  0 }       /* End of list */
};

#define SYS_FS_TTY_PATH "/sys/class/tty/"
#define SYS_FS_TTY_SERIAL_PATH "/device/GobiSerial/ttyUSB"
#define SYS_FS_TTY_DEVICE_MODULE "/device/driver/"

int UpdateTTYUSBPath(char *qdl_dev_path)
{
    DIR *dir;
    struct dirent *ent;
    struct stat statbuf;
    char buf[1024];
    char linkpath[1024];
    ssize_t len;
    if ((dir = opendir (SYS_FS_TTY_PATH)) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            snprintf(linkpath,1023,"%s%s",SYS_FS_TTY_PATH,ent->d_name);
            if (lstat(linkpath, &statbuf) != 0)
                continue;
            if ((len = readlink(linkpath, buf, sizeof(buf)-1)) != -1)
            {
                buf[len] = '\0';
                if(strstr(buf,".0/ttyUSB"))
                {
                    snprintf(linkpath,1023,"%s%s%s%s/driver",SYS_FS_TTY_PATH,ent->d_name,SYS_FS_TTY_DEVICE_MODULE, ent->d_name);
                    if (lstat(linkpath, &statbuf) != 0)
                         continue;
                    if ((len = readlink(linkpath, buf, sizeof(buf)-1)) != -1)
                    {
                        buf[len] = '\0';
                        if(strstr(buf,"GobiSerial"))
                        {
                            snprintf(qdl_dev_path,MAX_PATH_LEN-1,"/dev/%s",ent->d_name);
                            if (debug)
                                fprintf(stderr,"---Auto TTY PATH:%s---\n",qdl_dev_path);
                            break;
                        }
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

int GetTTYUSBPath(char *qdl_dev_path ,char *qdl_usb_path, unsigned int sizeofusbpath)
{
    DIR *dir;
    struct dirent *ent;
    struct stat statbuf;
    char pathbuf[1024];
    char buf[1024];
    char linkpath[1024];
    ssize_t len;
    char *pUSBPATH = NULL;
    char *ppathbuf = NULL;
    ppathbuf = (char *)&pathbuf;
    memset(qdl_usb_path,0,sizeofusbpath);
    if ((dir = opendir (SYS_FS_TTY_PATH)) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            snprintf(linkpath,1023,"%s%s",SYS_FS_TTY_PATH,ent->d_name);
            if (lstat(linkpath, &statbuf) != 0)
                continue;
            if ((len = readlink(linkpath, pathbuf, sizeof(pathbuf)-1)) != -1)
            {
                pathbuf[len] = '\0';
                pUSBPATH = strstr(pathbuf,".0/ttyUSB");
                if(pUSBPATH)
                {
                    snprintf(linkpath,1023,"%s%s%s%s/driver",SYS_FS_TTY_PATH,ent->d_name,SYS_FS_TTY_DEVICE_MODULE, ent->d_name);
                    if (lstat(linkpath, &statbuf) != 0)
                         continue;
                    if ((len = readlink(linkpath, buf, sizeof(buf)-1)) != -1)
                    {
                        buf[len] = '\0';
                        if(strstr(buf,"GobiSerial"))
                        {
                            unsigned int length = 0;
                            snprintf(qdl_dev_path,MAX_PATH_LEN-1,"/dev/%s",ent->d_name);
                            if (debug)
                                fprintf(stderr,"---DEV TTY PATH:%s---\n",qdl_dev_path);
                            pUSBPATH = strstr(pathbuf,ent->d_name);
                            length = pUSBPATH - ppathbuf;
                            if(sizeofusbpath>length)
                            {
                                snprintf(qdl_usb_path,length+strlen(SYS_FS_TTY_PATH),"%s%s",SYS_FS_TTY_PATH, pathbuf);
                                if (debug)
                                    fprintf(stderr,"---DEV USB PATH:%s---\n",qdl_usb_path);
                            }
                            break;
                        }
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

int CheckTTYUSBPath(char *qdl_dev_path)
{
    DIR *dir;
    struct dirent *ent;
    struct stat statbuf;
    char buf[1024];
    char linkpath[1024];
    char devicename[64];
    int iDeviceExist = 0;
    ssize_t len;
    char *ttyName = strstr(qdl_dev_path,"/");
    if(ttyName==NULL)
        return -1;
    snprintf(devicename,63,".0/%s",ttyName+1);
    if ((dir = opendir (SYS_FS_TTY_PATH)) != NULL)
    {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL)
        {
            snprintf(linkpath,1023,"%s%s",SYS_FS_TTY_PATH,ent->d_name);
            if (lstat(linkpath, &statbuf) != 0)
                continue;
            if ((len = readlink(linkpath, buf, sizeof(buf)-1)) != -1)
            {
                buf[len] = '\0';
                if(strstr(buf,devicename))
                {
                    iDeviceExist = 1;
                    break;
                }
            }
        }
        closedir (dir);
    }
    else
    {
        return -1;
    }
    return iDeviceExist;
}

int parseSwitches( int argc, char **argv, struct _userOptions *puserOptData)
{
    if (NULL == puserOptData)
        return eLITE_FWDWL_APP_ERR_INVALID_ARG;
    int next_option;
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
                exit (eLITE_FWDWL_APP_OK);
                break;

           case 'c':
                /* caller specifies the mode*/
                strncpy (puserOptData->dev_mode_str,optarg,MAX_PATH_LEN-1);
                break;

            case 'd':
                /* caller specifies the QDL port*/
                strncpy (puserOptData->qdl_dev_path,optarg,MAX_PATH_LEN-1);
                break;

            case 'p':
                /* caller specifies the QMI/MBIM device*/
                strncpy (puserOptData->dev_path,optarg,MAX_PATH_LEN-1);
                break;

            case 'f':
                /* caller specifies the FW path*/
                strncpy (puserOptData->fw_image_path,optarg,MAX_PATH_LEN-1);
                break;

            case 'q':
                /* caller specifies the FW path*/
                strncpy (puserOptData->fw_image_path,optarg,MAX_PATH_LEN-1);
                print_FW_file_details(puserOptData);
                exit(eLITE_FWDWL_APP_OK);
                break;

            case '?':
                /* Caller has used an invalid option */
                printf("\nInvalid option\n" );

                /* Print the usage info and exit */
                PrintUsage();
                exit(eLITE_FWDWL_APP_OK);
                break;

            case 'i':
                g_crashStateIgnore = (bool) atoi(optarg);
                break;
            case 'l':
                /* Log file path*/
                strncpy (puserOptData->log_file_path,optarg,MAX_PATH_LEN-1);
                break;
            case 'b':
                puserOptData->lBlockSize= atol(optarg);
                break;
            case 'm':
                puserOptData->modelfamily = atoi(optarg);
                break;
            case 'e':
                litefw_logsenable(atoi(optarg));
                break;
            case 'r':
                bDMreset = true;
                break;
            case 'u':
                iUARTMode = atoi(optarg);
                if(iUARTMode!=1)
                    iUARTMode = 0;
                if(iUARTMode == 1)
                {
                    iUARTMode = eQDL_MODE_UART0;
                }
                break;
            case -1:
                /* Done with options list */
                break;

            default:
                exit(eLITE_FWDWL_APP_OK);
                break;
        }
    }
    while( next_option != -1 );
    return eLITE_FWDWL_APP_OK;
}

int ResetModem (char *szDevPath)
{
    int rc = eLITE_FWDWL_APP_OK;

    if (g_devMode == eLITE_MODEM_MODE_QMI) {
        return ResetModem_QMI (szDevPath);
    }
    else if (g_devMode == eLITE_MODEM_MODE_MBIM) {
        return ResetModem_MBIM (szDevPath);
    }
    return rc;
}

enum fwdwl_modem_states GetDeviceMode (char *szDevPort,char *szQDLPort)
{
    if (0 == access(szDevPort, F_OK) &&  (0 == access(szQDLPort, F_OK)))
    {
        // Modem still in online mode as both qcqmiX and ttyUSBX are enumerated
        return eFWDWL_STATE_ONLINE;
    }
    if ((-1 == access(szDevPort,F_OK)) && (0 == access(szQDLPort, R_OK|W_OK)))
    {
        // only DM port is present. Modem in BOOT and HOLD mode
        return eFWDWL_STATE_BOOT_AND_HOLD;
    }
    if ((-1 == access(szDevPort,F_OK)) && (-1 == access(szQDLPort, F_OK)))
    {
        // Both ports absent. Modem disconnected from host
        return eFWDWL_STATE_DISCONNECTED;
    }    
    // Unknown state. Should never reach here
    printf ("%s: ERROR! Unknown modem state\n", __func__);
    return eFWDWL_STATE_UNKNOWN;
}

void WaitDevicesMode(char *szDevPort,char *szQDLPort,int Mode)
{
    bool bExitLoop = false;
    bool bQDLPort = false;
    FILE *fp= NULL;
    do 
    {
        /* update the device path as it may change sometimes during reset */
        UpdateTTYUSBPath(szQDLPort);

        if (0 == access(szDevPort, F_OK) &&  (0 == access(szQDLPort, F_OK)))
        {
            // Modem still in online mode as both qcqmiX and ttyUSBX are enumerated
            // printf ("Modem in ONLINE mode....\n");
            if(eFWDWL_STATE_ONLINE==Mode)
            {
                if(bFixedNode==false)
                    bExitLoop = true;
                //fprintf (stderr,"Modem Online\n");
            }
        }
        else if ((-1 == access(szDevPort,F_OK)) && (0 == access(szQDLPort, R_OK|W_OK)))
        {
            // only DM port is present. Modem in BOOT and HOLD mode
            if(eFWDWL_STATE_BOOT_AND_HOLD==Mode)
            {
                bExitLoop = true;
                //fprintf (stderr,"Modem in BOOT and HOLD mode\n");
            }
        }
        else if ((-1 == access(szDevPort,F_OK)) && (-1 == access(szQDLPort, F_OK)))
        {
            // Both ports absent. Modem disconnected from host
            if(eFWDWL_STATE_DISCONNECTED==Mode)
            {
                bExitLoop = true;
                //fprintf (stderr,"Modem disconnected from host\n");
            }
        }

        if(bExitLoop==false)
        {
            if (0 == access(szDevPort, F_OK) && (0 == access(szQDLPort, F_OK)))
            {
                fp = fopen(szQDLPort, "r+" );
                if(fp==NULL)
                {
                    bQDLPort = false;
                    bFixedNode = true;
                }
                else
                {
                    bQDLPort = true;
                    fclose(fp);
                }
            }
            else
            {
                bFixedNode = false;
            }
            if(bFixedNode==true)
            {
                if(eFWDWL_STATE_ONLINE==Mode)
                {
                    if((bQDLPort==true))
                    {
                        bExitLoop = true;
                        sleep(60);// Wait modem ready..
                        //fprintf (stderr,"Modem Online\n");
                    }
                }
                else if(eFWDWL_STATE_BOOT_AND_HOLD==Mode)
                {
                    if((bQDLPort==true))
                    {
                        bExitLoop = true;
                        //fprintf (stderr,"Modem in BOOT and HOLD mode\n");
                    }
                }
                else if(eFWDWL_STATE_DISCONNECTED == Mode)
                {
                    if((bQDLPort==false))
                    {
                        bExitLoop = true;
                        //fprintf (stderr,"Modem disconnected from host\n");
                    }
                }
            }
        }
        if(Mode==eFWDWL_STATE_DISCONNECTED)
        {
             usleep(1000);
        }
        else
        {
            // Sleep 
            usleep (HALF_A_SECOND);
        }
    } while (false == bExitLoop);
}

bool WaitDeviceDisconnect(char *szDevPort,char *szQDLPort,bool bTimeout)
{
    bool bExitLoop = false;
    bool bQDLPort = false;
    bool bSuccess = true;
    unsigned short count = 0;
    FILE *fp= NULL;
    do 
    {
        /* update the device path as it may change sometimes during reset */
        UpdateTTYUSBPath(szQDLPort);

        if ((-1 == access(szDevPort,F_OK)) && (-1 == access(szQDLPort, F_OK)))
        {
            // Both ports absent. Modem disconnected from host
            bExitLoop = true;
        }

        if(bExitLoop==false)
        {
            if (0 == access(szDevPort, F_OK) && (0 == access(szQDLPort, F_OK)))
            {
                fp = fopen(szQDLPort, "r+" );
                if(fp==NULL)
                {
                    bQDLPort = false;
                    bFixedNode = true;
                }
                else
                {
                    bQDLPort = true;
                    fclose(fp);
                }
            }
            else
            {
                bFixedNode = false;
            }

            if(bFixedNode==true)
            {
                if((bQDLPort==false))
                {
                    bExitLoop = true;
                }
            }
        }
        if(bExitLoop == false)
        {
            /* sleep for 1 ms */
            usleep(1000);             
            if (bTimeout) {
                count++;

                /* If host not disconnected within 25 seconds, return */
                if (count == 25000)
                {
                    bExitLoop = true;
                    bSuccess = false;
                }
            }
        }
    } while (false == bExitLoop);
    return bSuccess;
}

void InitParams(struct _userOptions *puserOptData)
{
    if (0==strlen(puserOptData->fw_image_path))
    {
        printf ("Firmware path is mandatory\n");
        PrintUsage();
        exit(eLITE_FWDWL_APP_ERR_INVALID_ARG);
    }

    if (0==strlen(puserOptData->dev_mode_str))
    {
        g_devMode = eLITE_MODEM_MODE_QMI;
    } 
    else
    {
        if (!strcmp(puserOptData->dev_mode_str,"QMI"))
            g_devMode = eLITE_MODEM_MODE_QMI;
        else if (!strcmp(puserOptData->dev_mode_str,"MBIM"))
            g_devMode = eLITE_MODEM_MODE_MBIM;
        else {
            printf ("Modem mode not correct\n");
            PrintUsage();
            exit(eLITE_FWDWL_APP_ERR_INVALID_ARG);
        }
    }
    
    if (0==strlen(puserOptData->qdl_dev_path))
    {
        strncpy (puserOptData->qdl_dev_path,DEFAULT_DM_PORT0, strlen(DEFAULT_DM_PORT0)+1);
    }    
    if (0==strlen(puserOptData->dev_path))
    {
        if (g_devMode == eLITE_MODEM_MODE_QMI)
            strncpy (puserOptData->dev_path,DEFAULT_QMI_PORT0, strlen(DEFAULT_QMI_PORT0)+1);
        else if (g_devMode == eLITE_MODEM_MODE_MBIM)
            strncpy (puserOptData->dev_path,DEFAULT_MBIM_PORT0, strlen(DEFAULT_MBIM_PORT0)+1);
    }
    else
    {
        if ((g_devMode == eLITE_MODEM_MODE_QMI) && 
            ((strstr(puserOptData->dev_path,"qcqmi") == NULL)))
        {
                printf ("Dev path not correct\n");
                PrintUsage();
                exit(eLITE_FWDWL_APP_ERR_INVALID_ARG);
        }
        if ((g_devMode == eLITE_MODEM_MODE_MBIM) && 
            ((strstr(puserOptData->dev_path,"cdc-wdm") == NULL)))
        {
                printf ("Dev path not correct\n");
                PrintUsage();
                exit(eLITE_FWDWL_APP_ERR_INVALID_ARG);
        }
    }
   
    if(strlen(puserOptData->log_file_path)>0)
    {
        fpLogFile = fopen(puserOptData->log_file_path, "a+" );
        if(fpLogFile==NULL)
        {
            fprintf(stderr,"Log File Error %s!",puserOptData->log_file_path);
        }
    }
    return;
}

#if 0
int GetFirmwareInfo(unpack_dms_GetFirmwareRevision_t *pResp, char *szQMIPath)
{
    pack_qmi_t req_ctx;
    uint8_t rsp[QMI_MSG_MAX];
    uint8_t req[QMI_MSG_MAX];
    uint16_t rspLen;
    uint16_t reqLen;
    int rtn;
    memset(&req_ctx, 0, sizeof(req_ctx));
    g_xid++;
    req_ctx.xid = g_xid;

    if (NULL == pResp)
    {
        fprintf (stderr, "%s: pResp is NULL\n", __func__);
        return eLITE_FWDWL_APP_ERR_INVALID_ARG;
    }
    int dms = client_fd(eDMS,szQMIPath);
    if(dms<0)
    {
        fprintf (stderr, "%s: dms<0\n", __func__);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    memset(&rsp,0,QMI_MSG_MAX);
    memset(&req,0,QMI_MSG_MAX);
    if(pack_dms_GetFirmwareRevision(&req_ctx, req, &reqLen,NULL)!=0)
    {
        printf("pack_dms_GetFirmwareRevision error\n");
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    
    rtn = write(dms, req, reqLen);
    if (rtn!=reqLen)
    {
        printf("write %d wrote %d\n", reqLen, rtn);
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    rspLen = read(dms, rsp, QMI_MSG_MAX);
    if(unpack_dms_GetFirmwareRevision(rsp, rspLen,pResp)!=0)
    {
        printf("unpack_dms_GetFirmwareRevision failed\n");
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    close(dms);
    return eLITE_FWDWL_APP_OK;
}

int set_modem_Crash_Action (int crashAction, char *szQMIPath, char *szDMPort)
{
    pack_qmi_t req_ctx;
    uint8_t rsp[QMI_MSG_MAX];
    uint8_t req[QMI_MSG_MAX];
    uint16_t rspLen;
    uint16_t reqLen;
    pack_dms_SetCrashAction_t packReq;    
    int rtn;
    int dms = -1;
    if (NULL == szQMIPath || NULL==szDMPort)
        return eLITE_FWDWL_APP_ERR_INVALID_ARG;
    dms = client_fd(eDMS,szQMIPath);
    if(dms<0)
    {
        return eLITE_FWDWL_APP_ERR_QMI;
    }    

    packReq.crashAction = crashAction;
    g_xid++;
    memset(&req_ctx, 0, sizeof(req_ctx));
    req_ctx.xid = g_xid;        
    memset(&rsp,0,QMI_MSG_MAX);
    memset(&req,0,QMI_MSG_MAX);             
    if(pack_dms_SetCrashAction(&req_ctx, req, &reqLen,packReq)!=0)
    {
        printf ("pack_dms_SetCrashAction error\n");
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }    
    rtn = write(dms, req, reqLen);
    if (rtn!=reqLen)
    {
        printf("write %d wrote %d\n", reqLen, rtn);
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    rspLen = read(dms, rsp, QMI_MSG_MAX);    
    if(unpack_dms_SetCrashAction(rsp, rspLen,NULL)!=0)
    {
        printf("unpack_dms_SetCrashAction error\n");
        fprintf (stderr, "Modem not set to reset in case of a crash. Please set the modem to reset with the following commands:\n");
        fprintf (stderr, "AT!ENTERCND=\"A710\"\n");
        fprintf (stderr, "AT!EROPTION=1\n");
        fprintf (stderr, "AT!RESET\n");
        close(dms);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    //Reset the modem and wait for it to come back online
    ResetModem(szQMIPath);
    close(dms);    
    printf ("Crash action set to reset. Waiting for modem to reset ...\n");
    WaitDevicesMode(szQMIPath,szDMPort,eFWDWL_STATE_DISCONNECTED);
    WaitDevicesMode(szQMIPath,szDMPort,eFWDWL_STATE_ONLINE);    
    printf ("Modem online. Continue with FW download\n");    
    return eLITE_FWDWL_APP_OK;
}
#endif

int verify_modem_Crash_Action (char *szDevPath)
{
    int rc = eLITE_FWDWL_APP_OK;

    if (g_devMode == eLITE_MODEM_MODE_QMI) {
        return verify_modem_Crash_Action_QMI (szDevPath);
    }
    else if (g_devMode == eLITE_MODEM_MODE_MBIM) {
        return verify_modem_Crash_Action_MBIM (szDevPath);
    }
    return rc; 
}

int FWVersionAfterDownload (char *szDevPath)
{
    int rc = eLITE_FWDWL_APP_OK;

    if (g_devMode == eLITE_MODEM_MODE_QMI) {
        return FWVersionAfterDownload_QMI (szDevPath);
    }    
    else if (g_devMode == eLITE_MODEM_MODE_MBIM) {
        return FWVersionAfterDownload_MBIM (szDevPath);
    }
    return rc;
}

void HandleOnlineSPK(struct _userOptions userOptData)
{
    // No specific preference to set. But command is to be sent to reset the modem in QDL mode
    unpack_dms_SetFirmwarePreference_t response;
    memset (&response,0,sizeof(response));

    if (g_devMode == eLITE_MODEM_MODE_QMI) {        
        SendSetImagePreferenceToModem_QMI(&response, userOptData.dev_path);
    }
    else if (g_devMode == eLITE_MODEM_MODE_MBIM) {
        SendSetImagePreferenceToModem_MBIM(&response, userOptData.dev_path);
    }    
}

int HandleOnlineCWEAndNVU(struct _userOptions userOptData,int  file_type,
                          bool *bValidateFWUpgrade,litefw_FirmwareInfo *info,
                          bool *bIsFWDownloadNeeded,int *image_mask,int *imgListLen)
{
    int iCount = 0;
    int  eCode = -1;
    unpack_fms_SetImagesPreference_t SetPrefRspFromModem;
    // Build the image preference request to be sent to the modem 
    if(eFW_TYPE_CWE == file_type)
        
    {
        if((litefw_CheckValidFirmwareInfo(*info)==0))
            fprintf(stderr,"CWE(With NVU)\n");
        else
            fprintf(stderr,"CWE\n");
    }
    else
    {
        *bValidateFWUpgrade = true;
        fprintf(stderr,"CWE+NVU\n");
    }
    pack_fms_SetImagesPreference_t pack_request;
    FMSPrefImageList ImageList;
    if(imgListLen==NULL)
    {
        fprintf (stderr, "Image List Length NULL\n");
        return eLITE_FWDWL_APP_ERR_LITEFW;
    }
    memset(&pack_request,0,sizeof(pack_fms_SetImagesPreference_t));
    memset(&ImageList,0,sizeof(FMSPrefImageList));
    pack_request.pImageList = &ImageList;
    eCode = litefw_BuildImagesPreferenceRequest(*info,&pack_request);
    if(0 != eCode)
    {
        fprintf (stderr, "litefw_BuildImagesPreferenceRequest failed: %d\n", eCode);
        return eLITE_FWDWL_APP_ERR_LITEFW;
    }
    
    // Send the image preference to the modem and get modem's response    
    memset (&SetPrefRspFromModem,0,sizeof(SetPrefRspFromModem));

    if (g_devMode == eLITE_MODEM_MODE_QMI) {
        eCode = SendImagePreferenceToModem_QMI(pack_request, &SetPrefRspFromModem, 
                                               userOptData.dev_path);
    }
    else if (g_devMode == eLITE_MODEM_MODE_MBIM) {
        eCode = SendImagePreferenceToModem_MBIM(pack_request, &SetPrefRspFromModem,
                                                userOptData.dev_path);
    }

    if(0 != eCode)
    {
        fprintf (stderr, "SendImagePreferenceToModem failed: %d\n", eCode);
        return eCode;
    }

    // Based on modem's response to SetImagesPref, check if modem needs FW to be downloaded
    *imgListLen = SetPrefRspFromModem.ImageTypesSize;
    printf ("Set pref ImageTypesSize: %d\n", SetPrefRspFromModem.ImageTypesSize);
    if (0 == *imgListLen)
    {
        // FW does not need to be downloaded. Modem will perform a FW switch. 
        // Simply reset the modem and wait for it to come up in ONLINE mode
        printf ("Modem does not need FW to be downloaded\n");
        *bIsFWDownloadNeeded = false;
    }
    else
    {
        if(eFW_TYPE_CWE != file_type)
        {
            // Modem needs FW to be downloaded.     
            // Calculate the mask for CWE OR PRI or BOTH        
            *image_mask = litefw_CalculateImageMask (SetPrefRspFromModem);
            for (iCount=0;iCount<*imgListLen;iCount++)
            {
                fprintf (stderr, "Modem Needs %s\n", SetPrefRspFromModem.ImageTypes[iCount] ? "PRI" : "FW");
            }
        }
    }
    return eLITE_FWDWL_APP_OK;
}

int HandleOnline9x07(struct _userOptions userOptData,int  file_type,
                     bool *bValidateFWUpgrade,litefw_FirmwareInfo *info,
                     bool *bIsFWDownloadNeeded,int *image_mask,int *imgListLen)
{
    
    int eCode = 0;
    UNUSEDPARAM(info);
    UNUSEDPARAM(image_mask);
    UNUSEDPARAM(userOptData);
    if(eFW_TYPE_CWE == file_type)
        
    {
        if((litefw_CheckValidFirmwareInfo(*info)==0))
            fprintf(stderr,"CWE(With NVU)\n");
        else
            fprintf(stderr,"CWE\n");
        *bValidateFWUpgrade = true;
    }
    else
    {
        *bValidateFWUpgrade = true;
        fprintf(stderr,"CWE+NVU\n");
    }
    
    *imgListLen = 0;
    *bIsFWDownloadNeeded = true;
    
    eCode = litefw_switch_9x07_to_downloadmode(userOptData.qdl_dev_path);
    if(eCode!=0)
    {
        return eLITE_FWDWL_APP_ERR_QDL;
    }
    return eLITE_FWDWL_APP_OK;
        
}

int SetImagePrefForSavingToSlot(struct _userOptions userOptData, int file_type,
                                 libSDP_FirmwareInfo info, int *image_mask)
{
    unpack_fms_SetImagesPreference_t SetPrefRspFromModem;
    pack_fms_SetImagesPreference_t pack_request;
    FMSPrefImageList ImageList;
    int bIsFWDownloadNeeded = 1;
    int  iCount     = 0;
    int  eCode = -1;
    int  imgListLen = 0;

    memset(&pack_request,0,sizeof(pack_fms_SetImagesPreference_t));
    memset(&ImageList,0,sizeof(FMSPrefImageList));
    pack_request.pImageList = &ImageList;
    eCode = litefw_BuildImagesPreferenceRequest(info,&pack_request);
    if(0 != eCode)
    {
        fprintf (stderr, "libSDP_BuildImagesPreferenceRequest failed: %d\n", eCode);
        return eLITE_FWDWL_APP_ERR_QMI;
    }
    
    // Send the image preference to the modem and get modem's response    
    memset (&SetPrefRspFromModem,0,sizeof(SetPrefRspFromModem));

    if (g_devMode == eLITE_MODEM_MODE_QMI) {
        eCode = SendImagePreferenceToModem_QMI(pack_request, &SetPrefRspFromModem,
                                              userOptData.dev_path);
    }
    else if (g_devMode == eLITE_MODEM_MODE_MBIM) {
        eCode = SendImagePreferenceToModem_MBIM(pack_request, &SetPrefRspFromModem,
                                               userOptData.dev_path);
    }

    if(0 != eCode)
    {
        fprintf (stderr, "SendImagePreferenceToModemQMI failed: %d\n", eCode);
        return eCode;
    }

    // Based on modem's response to SetImagesPref, check if modem needs FW to be downloaded
    imgListLen = SetPrefRspFromModem.ImageTypesSize;
    printf ("Set pref ImageTypesSize: %d\n", SetPrefRspFromModem.ImageTypesSize);
    if (0 == imgListLen)
   {
        // FW does not need to be downloaded. Modem will perform a FW switch. 
        // Simply reset the modem and wait for it to come up in ONLINE mode
        printf ("Modem does not need FW to be downloaded\n");
       bIsFWDownloadNeeded = 0;
    }
    else
    {
        if(eFW_TYPE_CWE != file_type)
        {
            // Modem needs FW to be downloaded.     
            // Calculate the mask for CWE OR PRI or BOTH        
            *image_mask = litefw_CalculateImageMask (SetPrefRspFromModem);
            for (iCount=0; iCount < imgListLen; iCount++)
            {
                fprintf (stderr, "Modem Needs %s\n", SetPrefRspFromModem.ImageTypes[iCount] ? "PRI" : "FW");
            }
        }
    }

    return bIsFWDownloadNeeded;
}

int lite_fw_download (struct _userOptions userOptData)
{
    int  eCode = -1;
    int  image_mask = IMG_MASK_PRI;
    int  file_type  = -1;
    int  imgListLen = 0;
    bool bIsFWDownloadNeeded = true;
    int  iSetImagePrefernceloadNeeded = 0;
    bool bValidateFWUpgrade  = false;
    bool matched = false;
    bool bDisconnect = false;
    bool bQmiResetNeeded = true;
    enum fwdwl_modem_states modem_state = eFWDWL_STATE_UNKNOWN;
    int iModelFamily = eModel_Unknown;
    
    printf ("INFO: QDL Port  : %s\n", userOptData.qdl_dev_path);
    printf ("INFO: Device Path: %s\n", userOptData.dev_path);
    printf ("INFO: FW  Path  : %s\n", userOptData.fw_image_path);
    
    // Get FW info from the FW files
    litefw_FirmwareInfo info;
    litefw_FirmwarePartNo partNoinfo;
    memset(&info,0,sizeof(litefw_FirmwareInfo));
    memset(&partNoinfo, 0, sizeof(partNoinfo));

    // Get the file type. SPK or CWE/NVU
    file_type = litefw_getFileType(userOptData.fw_image_path);
    
    eCode = litefw_ExtractFirmwareParametersByPath(userOptData.fw_image_path,&info);
    if(eCode==0)
    {
        fprintf(stderr,"Package Info:\n");
        fprintf(stderr,"Carrier    :%s\n",info.szCarrier_str);
        fprintf(stderr,"FW Version :%s\n",info.szFwversion_str);
        fprintf(stderr,"Model ID   :%s\n",info.szModelid_str);
        fprintf(stderr,"Package ID :%s\n",info.szPackageid_str);
        fprintf(stderr,"PRI Version:%s\n",info.szCarrierPriversion_str);
        fprintf(stderr,"SKU        :%s\n",info.szSku_str);        
    }
    else
    {
        // Not be fatal for CWE and NVU alone cases
        if(file_type != eFW_TYPE_CWE)
        {            
            fprintf(stderr,"litefw_ExtractFirmwareParametersByPath Error: %d\n", eCode);
            eCode = eLITE_FWDWL_APP_ERR_LITEFW;
            return eCode;
        }
    }

    eCode = litefw_ExtractFirmwarePartNoByPath(userOptData.fw_image_path,&partNoinfo);
    if(eCode==0)
    {
        fprintf(stderr,"Part Number:%s\n",partNoinfo.szPartno_str);
    }
    else
    {
        // Not be fatal for CWE and NVU alone cases
        if(file_type != eFW_TYPE_CWE)
        {
            fprintf(stderr,"litefw_ExtractFirmwarePartNoByPath Error: %d\n", eCode);
            eCode = eLITE_FWDWL_APP_ERR_LITEFW;
            return eCode;
        }
    }

    switch(userOptData.modelfamily)
    {
        case 1:
            iModelFamily = eModel_9X15;
            break;
        case 2:
            iModelFamily = eModel_WP9X15;
            break;
        case 3:
            iModelFamily = eModel_9X30;
            break;
        case 4:
        case 5:
            iModelFamily = eModel_9x07;
            break;
        case 0:
        default:
            // Get the modem family the FW is for
            iModelFamily = litefw_GetModelFamily (info.szModelid_str);
            break;
    }
    
    if(iModelFamily == eModel_Unknown)
    {
        fprintf(stderr,"Unknown Model Family\n");
        fprintf(stderr,"Please specific model family by using argument -m/--modelfamily\n");
        return eLITE_FWDWL_APP_ERR_INVALID_MODEL_FAMILY;
    }
    if(iUARTMode == eQDL_MODE_UART0)
    {
        fprintf(stderr,"Set UART Mode\n");
        litefw_SetQTLDownloadMode(eQDL_MODE_UART0);
        fprintf(stderr,"UART Mode:%d\n",litefw_GetQTLDownloadMode());
    }
    // Get the modem's state
    modem_state = GetDeviceMode (userOptData.dev_path,userOptData.qdl_dev_path);
    if (eFWDWL_STATE_UNKNOWN == modem_state)
    {
        printf ("Modem not in correct state\n");
        return eLITE_FWDWL_APP_ERR_MODEM_STATE;
    }

    fprintf(stderr, "file_type : %d\n", file_type);    

    if (eFWDWL_STATE_BOOT_AND_HOLD == modem_state)
    {
        // Modem is already in QDL mode. Simply set the flags to start FW download
        printf ("Modem is QDL mode. Skipping FW preference setting\n");
        bIsFWDownloadNeeded = true; 
        if (eFW_TYPE_SPK == file_type)
        {
            image_mask = IMG_MASK_GENERIC;
        }
        else if (eFW_TYPE_CWE_NVU == file_type)
        {
            // Set flag to download both CWE and NVU
            image_mask |= IMG_MASK_MDM;
            image_mask |= IMG_MASK_PRI;
        }
    } // if (eFWDWL_STATE_BOOT_AND_HOLD == modem_state)

    if ((eFWDWL_STATE_BOOT_AND_HOLD == modem_state)||
         (eFWDWL_STATE_ONLINE == modem_state))
    {
        if(bDMreset)
        {
            printf ("Switch Modem to QDL mode.\n");
            litefw_switch_to_BootHoldMode(userOptData.qdl_dev_path);
            modem_state = eFWDWL_STATE_BOOT_AND_HOLD;
            g_crashStateIgnore = true;
        }
    }

    if (eFWDWL_STATE_ONLINE == modem_state)
    {
        // Check if the modem is set to reset in case of a crash
        if ( !g_crashStateIgnore )
        {
            eCode = verify_modem_Crash_Action (userOptData.dev_path);
            if (eLITE_FWDWL_APP_OK != eCode)
            {
                    printf ("Error in modem crash state checking!error code = %d\n", eCode);
                    return eLITE_FWDWL_APP_ERR_INVALID_CRASH_ACTION_STATE;
            }
        }
        
        if (eFW_TYPE_SPK == file_type )
        {
            if ( (iModelFamily == eModel_WP9X15) || (iModelFamily == eModel_9x07) || (iModelFamily == eModel_9x06))
            {
                if ( !strstr(info.szFwversion_str, "?") && !strstr(info.szCarrier_str, "?"))
                {
                    unsigned int i;
                    for ( i = 0; i < sizeof(carriers)/sizeof(carriers[0]); i++)
                    {
                        if ( strcmp(info.szCarrier_str, carriers[i]) == 0)
                        {
                            matched = true;
                            break;
                        }
                    }
                    if ( matched == true )
                    {
                        fprintf(stderr,"SPK Parsing: download to slot\n");   
                        /* firmware needs to be stored in slot in this case */
                        iSetImagePrefernceloadNeeded = SetImagePrefForSavingToSlot(userOptData, file_type,
                                                                          info, &image_mask);
                        if(iSetImagePrefernceloadNeeded==0)
                        {
                            bIsFWDownloadNeeded = false;
                        }
                        /* if set image preference returns fail and user did not select skip checking image preference, return fail for
                           firmware download and output the message */
                        if ( iSetImagePrefernceloadNeeded == eLITE_FWDWL_APP_ERR_QMI )
                        {
                            fprintf(stderr,"\nSet Image Preference error, run lite-fw-download again with command line option -r to force firmware download\n");
                            return eLITE_FWDWL_APP_ERR_QMI;
                        }
                    }
                }
            }
            if ( matched == false )
            {
                // If it's a NON WP SPK file, we do not need to do anything special. Just set FW preference and reset the modem
                // SPK will set the preference etc
                fprintf(stderr,"SPK case\n");
                image_mask = IMG_MASK_GENERIC;
                bValidateFWUpgrade = true;
                HandleOnlineSPK(userOptData);

                // HandleOnlineSPK invoke SetFirmwarePreference, that reset the modem automatcially
                bQmiResetNeeded = false;
            }
        }
        else if (eFW_TYPE_NVU == file_type)
        {
            // NVU only case. Mostly a OEM PRI
            // Check if the nvu is an OEM PRI and handle it differently
            if (0 != strncmp (info.szSku_str, LITEFW_CARRIER_PACKAGE_SKU /*"9999999"*/, LITEFW_SKU_STRING_LENGTH/*7*/))
            {
                fprintf(stderr,"OEM PRI for SKU :%s.\n",info.szSku_str);
                
                // Set the image mask to PRI
                image_mask = IMG_MASK_PRI;
            }
            else
            {
                // We should not be here. Carrier package should be CWE+NVU in a directory
                fprintf(stderr,"Carrier PRI alone scenario is not valid\n");
                return eLITE_FWDWL_APP_ERR_GEN;
            }
        }        
        else if ( (eFW_TYPE_CWE_NVU == file_type) || 
                   (eFW_TYPE_CWE == file_type)  )
        {
            if((iModelFamily == eModel_9x07) || (iModelFamily == eModel_9x06))
            {
                if(eFW_TYPE_CWE_NVU == file_type)
                {
                    HandleOnlineCWEAndNVU(userOptData,file_type,&bValidateFWUpgrade,&info,&bIsFWDownloadNeeded,&image_mask,&imgListLen);
                }
                else
                {
                    HandleOnline9x07(userOptData,file_type,&bValidateFWUpgrade,&info,&bIsFWDownloadNeeded,&image_mask,&imgListLen);
                    //DM reset command is sent by HandleOnline9x07
                    modem_state = eFWDWL_STATE_BOOT_AND_HOLD; 
                }
            }//if(iModelFamily == eModel_9x07)
            else
            {
                if((eModel_WP9X15==iModelFamily) &&(eFW_TYPE_CWE == file_type))
                {
                    printf("Reset Modem\n");
                    HandleOnlineSPK(userOptData);

                    // HandleOnlineSPK invoke SetFirmwarePreference, that reset the modem automatcially
                    bQmiResetNeeded = false;
                    eCode=eLITE_FWDWL_APP_OK;
                }
                else
                {
                    eCode = HandleOnlineCWEAndNVU(userOptData,file_type,&bValidateFWUpgrade,&info,&bIsFWDownloadNeeded,&image_mask,&imgListLen);
                }
                if(eCode!=eLITE_FWDWL_APP_OK)
                {
                    return eCode;
                }
            }//else (iModelFamily == eModel_9x07)
        } //else if (eFW_TYPE_CWE_NVU == file_type)
    } //if (eFWDWL_STATE_ONLINE == modem_state)

    if (eFWDWL_STATE_BOOT_AND_HOLD != modem_state)
    {
        if (eFW_TYPE_NVU != file_type )
        {
            // Reset modem and wait for correct state depending on whether FW download is needed or not
            if (bQmiResetNeeded)
                ResetModem (userOptData.dev_path);
            printf ("Waiting for modem to disconnect from the host after reset command is issued ...\n");

            // Wait with timeout
            bDisconnect = WaitDeviceDisconnect(userOptData.dev_path,
                                      userOptData.qdl_dev_path,
                                      true);
            if (bDisconnect == false) {
                printf ("QMI reset failure, reset by DM cmd\n");
                litefw_switch_to_BootHoldMode(userOptData.qdl_dev_path);
            }
            else
                bDisconnect = true;                
        }
        else
        {
            // Reset the modem by DM command in case of OEM PRI.
            printf ("Switch Modem to QDL mode.\n");
            litefw_switch_to_BootHoldMode(userOptData.qdl_dev_path);
        }

        if (bDisconnect == false) {
            printf ("Waiting for modem to disconnect from the host after reset command is issued ...\n");
            // Wait for modem to disconnect from the host USB bus
            WaitDeviceDisconnect(userOptData.dev_path,
                                      userOptData.qdl_dev_path,
                                      false);
        }
        printf ("Modem disconnected from host.\n");
    }

    if (true == bIsFWDownloadNeeded)
    {
        // wait for modem to come up in BOOT and HOLD mode and perform FW download
        printf ("Waiting for modem to come up in BOOT and HOLD mode ...\n");
        WaitDevicesMode(userOptData.dev_path,userOptData.qdl_dev_path,eFWDWL_STATE_BOOT_AND_HOLD);
        printf ("BOOT and HOLD Mode. Downloading firmware ...\n");
        eCode = litefw_DownloadFW(userOptData.fw_image_path,userOptData.qdl_dev_path,file_type,image_mask,iModelFamily);
        if (0!=eCode)
        {
            fprintf (stderr, "litefw_DownloadFW failed: %d\n", eCode);
            return eLITE_FWDWL_APP_ERR_LITEFW;
        }
        fprintf (stderr, "litefw_DownloadFW succeeded.\n");    
    }
    if(iUARTMode != eQDL_MODE_UART0)
    {
        fprintf (stderr, "Waiting for modem to come up in ONLINE mode ...\n");

        WaitDevicesMode(userOptData.dev_path,userOptData.qdl_dev_path,eFWDWL_STATE_ONLINE);
    }
    if (bValidateFWUpgrade)
    {
        // TBD
    }
    //Print FW version after FW upgrade
    FWVersionAfterDownload (userOptData.dev_path);

    fprintf (stderr, "Firmware Download Process completed\n");

    return eLITE_FWDWL_APP_OK;
}

void *litefw_syslog_proxy(uint8_t lvl, const char *buff)
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

static void print_FW_file_details (struct _userOptions *pUserOptData)
{
    uint8_t numOfFileInfo = 0, index = 0, eCode = -1;
    litefw_FirmwareFileInfo fminfo[20];
    memset(fminfo, 0, 20*sizeof(litefw_FirmwareFileInfo));
    eCode = litefw_SLQSGetFirmwareFileInfo(pUserOptData->fw_image_path, fminfo, &numOfFileInfo);
    if(eCode==0)
    {
        for (index = 0; index < numOfFileInfo; index++ )
        {
            printf ("Index: %d\n", index+1);
            printf ("\tFileName     : %s\n", basename(fminfo[index].fullPath));
            printf ("\tImage Mask   : %d, ", fminfo[index].imageMask);
            switch (fminfo[index].imageMask)
            {
                //Bit 0
                case 1:
                {
                    printf ("CWE\n");
                    break;
                }
                // Bit 1
                case 2:
                {
                    printf ("NVU\n");
                    break;
                }
                case 3:
                {
                    printf ("CWE+NVU\n");
                    break;
                }
                // Bit 2
                case 4:
                {
                    printf ("SPK\n");
                    break;
                }
                default:
                {
                    printf ("Unkown image mask\n");
                    break;
                }
            }
            printf ("\tHeader Type  : %d, ", fminfo[index].headerType);
            switch (fminfo[index].headerType)
            {
                case eIMAGE_TYPE_BOOT: printf ("BOOT\n");break;
                case eIMAGE_TYPE_APPL: printf ("APPL\n");break;
                case eIMAGE_TYPE_APPS: printf ("APPS\n");break;
                case eIMAGE_TYPE_FILE: printf ("FILE\n");break;
                case eIMAGE_TYPE_SPKG: printf ("SPKG\n");break;
                case eIMAGE_TYPE_MODM: printf ("MODM\n");break;
                case eIMAGE_TYPE_USER: printf ("USER\n");break;
                case eIMAGE_TYPE_KEYS: printf ("KEYS\n");break;
                default: printf ("Unknown Header Type\n");break;
            }
            printf ("\tImage Type   : %d, ", fminfo[index].imageType);
            switch (fminfo[index].imageType)
            {
                case eFILE_TYPE_CAR_PRI  : printf ("Carrier PRI\n");break;
                case eFILE_TYPE_OEM_PRI  : printf ("OEM PRI\n");break;
                case eFILE_TYPE_COMPO_PRI: printf ("Composite PRI\n");break;
                default: printf ("Unknown PRI type\n");break;
            }
            printf ("\tModel        : %s\n", fminfo[index].modelIdStr);
            printf ("\tPart No      : %s\n", fminfo[index].partNoStr);
            printf ("\tSKU          : %s\n", fminfo[index].skuStr);
            printf ("\tPacakge ID   : %s\n", fminfo[index].packageIdStr);
            printf ("\tCarrier ID   : %s\n", fminfo[index].carrierStr);
            printf ("\tPRI Version  : %s\n", fminfo[index].priVersionStr);
            printf ("\tFW Version   : %s\n", fminfo[index].versionStr);
            printf ("\tRelease Date : %s\n", fminfo[index].releaseDate);
            printf ( "\n");
        }
    }
    else
    {
        printf ("litefw_SLQSGetFirmwareFileInfo Error: 0x%04x\n", eCode);
        eCode = eLITE_FWDWL_APP_ERR_LITEFW;
        return;
    }
    return;
}

int main( int argc, char *argv[])
{
    /* Parse the command line switches  */
    struct _userOptions userOptData;
    int ret = 0;
    setbuf(stderr,NULL);
    memset (&userOptData,0,sizeof(userOptData));
    parseSwitches( argc, argv, &userOptData);
    UpdateTTYUSBPath((char*)&userOptData.qdl_dev_path);
    GetTTYUSBPath((char*)&userOptData.qdl_dev_path,(char*)&userOptData.qdl_usb_path,MAX_USB_PATH_LEN);
    InitParams(&userOptData);
    litefw_set_log_func((litefwlogger*)&litefw_syslog_proxy);
    fprintf(stderr,"litefw version :%s \n",litefw_GetVersion());
    litefw_SetReadBlockSize(userOptData.lBlockSize);
    ret = lite_fw_download (userOptData);
    fprintf( stdout, "Exiting Application!!!\n");
    if(fpLogFile!=NULL)
    {
        fclose(fpLogFile);
    }
    return ret; 
}
