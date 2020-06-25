/**************
 *
 * Filename:    connectmgr.c
 *
 * Purpose:     Connection Manager application
 *
 * Copyright: © 2017 Sierra Wireless Inc., all rights reserved
 *
 **************/
#define __STDC_FORMAT_MACROS
#include <pthread.h>
#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdbool.h>

#include "displaysettings.h"
#include "msgid.h"
#include "qmerrno.h"
#include "dms.h"
#include "nas.h"
#include "wds.h"
#include "qmap.h"

/****************************************************************
*                       DEFINES
****************************************************************/
#define SUCCESS                  0
#define FAIL                     1
#define ENTER_KEY                0x0A
#define ENTER_KEY_PRESSED        0
#define OPTION_LEN               5
#define IP_ADDRESS_LEN           15
#define IPADDREESS_OCTET_MASK    0x000000FF
#define PDP_IPV4                 0
#define IPv4_FAMILY_PREFERENCE   0x04
#define IPv6_FAMILY_PREFERENCE   0x06
#define IPv4v6_FAMILY_PREFERENCE 0x07
#define MAX_FIELD_SIZE           128

/* APN, User Name and Profile name size should be 3 greater than the actual
 * characters to be read. This will help to ensure that user should not enter
 * more than maximum allowed characters.
 */
#define MAX_APN_SIZE            104
#define MAX_PROFILE_NAME_SIZE   17
#define MAX_USER_NAME_SIZE      129
#define MAX_PROFILES            16
#define PROFILE_TYPE_UMTS       0
#define MIN_PROFILES            1
#define START_DATA_SEESION      1
#define STOP_DATA_SEESION       0
#define TECHNOLOGY_3GPP         1
#define TECHNOLOGY_3GPP2        2

#define CDMA_PROFILE_OFFSET     (100)
#define MAX_QMAP_INSTANCE       8
#define QMI_GET_QMAP_SUPPORT    0x8BE0 + 15
#define QMI_SET_QMAP_IP_TABLE   0x8BE0 + 16
#define QMI_SET_QMAP_IPV6_TABLE 0x8BE0 + 17

#define EM75XX                  "EM75"
#define WP76xx                  "WP76"
#define RC76xx                  "RC76"
#define GET_IP_VERSION(x) x==eLITE_WDS_FD_ARR_INDEX_0 ? "v4": "v6"

/****************************************************************
*                       DATA STRUCTURE
****************************************************************/

enum
{
    eLITE_CONNECT_APP_OK,   
    eLITE_CONNECT_APP_ERR_QMI,
};

enum
{
    eLITE_WDS_FD_ARR_INDEX_0,
    eLITE_WDS_FD_ARR_INDEX_1,
};

/* User options enumeration */
enum eUserOptions{
    eSTART_UMTS_DATA_SESSION = 1,
    eSTART_LTE_DATA_SESSION,
    eSTART_CDMA_DATA_SESSION,
    eSTART_CDMA_RUIM_DATASESSION,
    eSTOP_DATA_SESSION,
    eDISPLAY_ALL_PROFILES,
    eDISPLAY_SINGLE_PROFILE,
    eCREATE_PROFILE,
    eMODIFY_PROFILE_SETTINGS,
    eDELETE_PROFILE,
    eGET_RUNTIME_SETTINGS,
    eRESET_DEVICE,
    eSET_POWER_MODE,
    eQOS_TESTING
};

/* Profile indexes for profile existing on device */
struct profileIndexesInfo{
    uint8_t profileIndex[MAX_PROFILES];
    uint8_t totalProfilesOnDevice;
};


/* Profile Information structure */
struct profileInformation{
    uint8_t profileType;
    uint8_t PDPType;
    uint32_t IPAddress;
    uint32_t primaryDNS;
    uint32_t secondaryDNS;
    uint8_t Authentication;
    uint8_t  profileName[MAX_PROFILE_NAME_SIZE];
    uint8_t  APNName[MAX_APN_SIZE];
    uint8_t  userName[MAX_USER_NAME_SIZE];
    uint8_t  password[MAX_FIELD_SIZE];
};

/****************************************************************
*                    GLOBAL DATA
****************************************************************/
#define QMI_GET_SERVICE_FILE_IOCTL  0x8BE0 + 1
#define QMI_MSG_MAX 2048
#define QMI_CMD_TIMEOUT_SEC 20 /* 120 sec */

/* wds_fd[0] ->IPV4
 * wds_fd[1] -> IPv6
 */
int wds_fd[2] = {-1, -1};
unsigned g_xid[2] = {0,0};

uint8_t g_modem_index = 0;

uint8_t g_qmi_rsp[2][QMI_MSG_MAX];
uint8_t g_qmi_req[2][QMI_MSG_MAX];

uint16_t g_rspLen[2] = {0, 0};
uint8_t g_read_success[2] = {FALSE, FALSE};

uint8_t g_sessionv4 = 0;
uint32_t g_sessionIdv4 = 0;

uint8_t g_sessionv6 = 0;
uint32_t g_sessionIdv6 = 0;

uint8_t g_ipfamilypreference = IPv4_FAMILY_PREFERENCE;

uint8_t g_qmi_rsp_o[QMI_MSG_MAX];
uint8_t g_qmi_req_o[QMI_MSG_MAX];
unsigned g_xid_o = 0;

pthread_mutex_t con_mutex[2] = {PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER} ;
pthread_cond_t con_cond[2] = {PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER} ;

/* device connectivity */
static struct profileIndexesInfo indexInfo;

#define SETUP_REQRSP_CTX_WDS_FD(wdsFdIndex, req_ctx, rsp_ctx) \
{\
    memset(&req_ctx, 0, sizeof(req_ctx));\
    memset(&rsp_ctx, 0, sizeof(rsp_ctx));\
    if (wdsFdIndex ==  eLITE_WDS_FD_ARR_INDEX_0) \
    {\
        g_xid[0]++; \
        req_ctx.xid = g_xid[0];\
        memset(g_qmi_rsp[0],0,QMI_MSG_MAX);\
        memset(g_qmi_req[0],0,QMI_MSG_MAX);\
    }\
    else \
    {\
        g_xid[1]++;\
        req_ctx.xid = g_xid[1];\
        memset(g_qmi_rsp[1],0,QMI_MSG_MAX);\
        memset(g_qmi_req[1],0,QMI_MSG_MAX);\
    }\
}

#define VALIDATE_WDS_FD_INDEX(wdsFdIndex) \
{\
    if((wdsFdIndex != eLITE_WDS_FD_ARR_INDEX_0) && \
       (wdsFdIndex != eLITE_WDS_FD_ARR_INDEX_1)) \
    { \
        fprintf(stderr, "wrong wds fd index!\n"); \
        return eLITE_CONNECT_APP_ERR_QMI;\
    }\
}

/****************************************************************
*              FORWARD FUNCTION DECLARATION
****************************************************************/
static void display_device_info( void );
static void display_profile_info( void );
static void display_all_profiles( void );
static void IPUlongToDot( uint32_t IPAddress, char* pIPAddress );
static int get_model_id(unpack_dms_GetModelID_t *pModelId);

/****************************************************************
*                       FUNCTIONS
****************************************************************/

/****************************************************************
*                    COMMON FUNCTIONS
****************************************************************/
/*
 * Name:     FlushStdinStream
 *
 * Purpose:  Flush the stdin stream
 *
 * Params:   None
 *
 * Return:   None
 *
 * Notes:    fflush does not work for input stream.
 */
void FlushStdinStream( )
{
    int inputChar;

    /* keep on reading until a <New Line> or end of file is received */
    do
    {
        inputChar = getchar();

        #ifdef DBG
        fprintf( stderr,  "inputChar: 0x%x\n", inputChar );
        #endif
    }
    while ( ( inputChar != ENTER_KEY ) &&
            ( inputChar != EOF ) );
}

/*
 * Name:     client_fd
 *
 * Purpose:  open QMI svc file descriptor
 *
 * Params:   svc type
 *
 * Return:   file descriptor
 *
 * Notes:    none.
 */

int client_fd(uint8_t svc)
{
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

/*
 * Name:     ValidateIPAddressOctet
 *
 * Purpose:  Validates the received  octet of the IP Address.
 *
 * Params:   pIPAddressOctet - Pointer to the buffer containing IP Address
 *                             octet to be validated.
 *           len             - length of the passed buffer
 *
 * Return:   SUCCESS   - In case valid digits are there in the received octet of
 *                       the IP Address
 *           FAIL      - If invalid octet is received
 *
 * Notes:    None
 */
uint8_t ValidateIPAddressOctet( char* pIPAddressOctet, int len )
{
    if( len == 3)
    {
        /* If length of the octet is 3, first digit should be between 0 and 2 */
        if( ( '0' > pIPAddressOctet[0] ) ||
            ( '2' < pIPAddressOctet[0] ) )
        {
            return FAIL;
        }

        /* If first digit is 2 then second digit should not be greater than 5 */
        if( '2' == pIPAddressOctet[0] )
        {
            if( '5' < pIPAddressOctet[1] )
            {
                return FAIL;
            }

            /* If second digit is also 5 then third digit should not be greater
             * than 5.
             */
            if( '5' == pIPAddressOctet[1] )
            {
                if( '5' < pIPAddressOctet[2] )
                {
                    return FAIL;
                }
            }
        }

        if( ( '0' > pIPAddressOctet[1] ) ||
            ( '9' < pIPAddressOctet[1] ) )
        {
            return FAIL;
        }

        if( ( '0' > pIPAddressOctet[2] ) ||
            ( '9' < pIPAddressOctet[2] ) )
        {
            return FAIL;
        }
    }

    if( 2 == len )
    {
        if( ( '0' > pIPAddressOctet[0] ) ||
            ( '9' < pIPAddressOctet[0] ) )
        {
            return FAIL;
        }

        if( ( '0' > pIPAddressOctet[1] ) ||
            ( '9' < pIPAddressOctet[1] ) )
        {
            return FAIL;
        }
    }

    if( 1 == len )
    {
        if( ( '0' > pIPAddressOctet[0] ) ||
            ( '9' < pIPAddressOctet[0] ) )
        {
            return FAIL;
        }
    }
    return SUCCESS;
}

/*
 * Name:     IPUlongToDot
 *
 * Purpose:  Convert the IP address passed by the user in the form of ULONG
 *           value to a DOT format and copy it into the user buffer.
 *
 * Params:   IPAddress  - IP Address to be converted in dot notation.
 *           pIPAddress - Buffer to store IP Address converted to dot notation.
 *
 * Return:   None
 *
 * Notes:    None
 */
static void IPUlongToDot( uint32_t IPAddress, char* pIPAddress )
{
    char  tempBuf[5];
    uint8_t  idx = 0;
    uint8_t  shift = 0;
    uint32_t tempIPAddress = 0;
    int   IPAddr = 0;

    for( idx = 4; idx > 0; idx-- )
    {
        shift = ( idx - 1 ) * 8;
        tempIPAddress = IPAddress >> shift;
        IPAddr = tempIPAddress & IPADDREESS_OCTET_MASK;
        sprintf( tempBuf, "%d", IPAddr );
        strcat( pIPAddress, tempBuf );
        tempIPAddress = 0;
        if( 1 >= idx )
        {
            continue;
        }
        strcat( pIPAddress, "." );
    }
}

static uint16_t u8toU16(uint8_t u8Array[2])
{
    return (u8Array[0] + u8Array[1]*256);
}

void PrintHex(uint8_t *msg ,uint16_t Len)
{
   int j =0;
   for (j = 0; j < Len; j++)
   {
       printf("%02x ", msg[j]);
       if ((j+1)%16 == 0)
       {
           printf("\n");
       }
   }
   printf("\n");
   return ;
}

/*
 * Name:     IPDotToUlong
 *
 * Purpose:  Convert the IP address passed by the user in dot notation to
 *           a ULONG value.
 *
 * Params:   pIPAddress - IP Address to be converted to ULONG value.
 *           pAddress   - ULONG pointer to store converted IP Address.
 *
 * Return:   SUCCESS   - In case valid IP Address is provided by the user
 *           FAIL      - In case invalid IP Address is provided by the user
 *
 * Notes:    None
 */
uint8_t IPDotToUlong( char* pIPAddress, uint32_t* pAddress )
{
    char *pCharacterOccurence = NULL;
    int   IPAddressOctet = 0;
    uint8_t  len = 0, noOfShift = 8;
    uint32_t IPAddress = 0;

    /* Being here means correct no. of dots are there in the IP  address. Jump
     * to first occurrence of the dot.
     */
    pCharacterOccurence = strtok( pIPAddress,"." );
    while( NULL != pCharacterOccurence )
    {
        len = strlen( pCharacterOccurence );
        if( ( 0 == len ) || ( 3 < len ) )
        {
            #ifdef DBG
            fprintf( stderr, "Incorrect octet length : %d\n",len );
            #endif
            *pAddress = 0;
            return FAIL;
        }

        /* Check if the valid digits have been entered in IP Address */
        if( FAIL == ValidateIPAddressOctet( pCharacterOccurence, len ) )
        {
            #ifdef DBG
            fprintf( stderr, "Incorrect characters in octet : %s\n",
                              pCharacterOccurence );
            #endif
            *pAddress = 0;
            return FAIL;
        }

        IPAddressOctet = atoi( pCharacterOccurence );
        if( 255 < IPAddressOctet )
        {
            #ifdef DBG
            fprintf( stderr, "Incorrect octet value : %d\n",IPAddressOctet );
            #endif
            *pAddress = 0;
            return FAIL;
        }
        len = 0;

        /* Store the octet */
        IPAddress = ( IPAddress << noOfShift ) | IPAddressOctet;

        #ifdef DBG
        fprintf( stderr, "IP Address Octet Value: %s, Integer: %d\n",
                          pCharacterOccurence, IPAddressOctet );
        fprintf( stderr, "IP Address : %lx\n", (unsigned long)IPAddress );
        #endif
        IPAddressOctet = 0;

        /* look for the next dot */
        pCharacterOccurence = strtok( NULL, "." );
    }

    *pAddress = IPAddress;
    #ifdef DBG
    fprintf( stderr, "Final IP Address : %lx\n",(unsigned long)*pAddress );
    #endif
    return SUCCESS;
}

/*
 * Name:     GetIPFromUser
 *
 * Purpose:  Prompt the user to enter the IP address and copy it in the passed
 *           buffer.
 *
 * Return:   SUCCESS   - In case valid IP Address is entered by the user
 *           ENTER_KEY - If enter key is pressed by the user
 *
 * Params:   pAddressString - Name of the address to be retrieved.
 *           pIPAddress     - Buffer to receive the address from user.
 *           pAddress       - Pointer to store received IP address after
 *                            conversion from dot notation to ULONG value.
 *
 * Notes:    None
 */
uint8_t GetIPFromUser( char *pAddressString, char *pIPAddress, uint32_t *pAddress )
{
    int  len = 0;
    uint8_t count = 0, returnCode = 0, IPAddressWrong = FALSE;
    char *pCharacterOccurence = NULL, *pEndOfLine = NULL;

    while(1)
    {
        /* Print the menu */
        fprintf( stderr, "\nPlease enter %s Address in xxx.xxx.xxx.xxx format,"\
                         " or press <Enter> to leave the field as blank:\n%s"\
                         "Address : ", pAddressString, pAddressString );

        /* Clear the buffer */
        memset( pIPAddress, 0, MAX_FIELD_SIZE );

        /* Receive the input from the user */
        fgets( pIPAddress, MAX_FIELD_SIZE, stdin );

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( pIPAddress, ENTER_KEY );
        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }

        #ifdef DBG
        fprintf( stderr, "Entered Address : %s\n", pIPAddress );
        #endif

        /* If only <ENTER> is pressed by the user, return to main menu */
        if( ENTER_KEY == pIPAddress[0] )
        {
            *pAddress = 0;
            return ENTER_KEY;
        }

        len = strlen( pIPAddress );
        pIPAddress[len - 1] = '\0';
        len -= 1;

        /* Validate the address entered by the user */
        /* Check the IP Address Length */
        if( IP_ADDRESS_LEN < len )
        {
            #ifdef DBG
            fprintf( stderr, "Incorrect Address Length : %d\n",len );
            #endif
            continue;
        }

        /* Check if there is nothing followed by a Dot in the IP address or
         * there are two adjacent dots.
         */
        pCharacterOccurence = strchr( pIPAddress,'.' );
        while ( NULL != pCharacterOccurence )
        {
            if( ( '.'  == pCharacterOccurence[1] ) ||
                ( '\0' == pCharacterOccurence[1] ) )
            {
                #ifdef DBG
                fprintf( stderr, "Two Adjacent dots or NULL after a dot:"\
                                  "Wrong IP Address\n" );
                #endif
                IPAddressWrong = TRUE;
                break;
            }
            count++;
            pCharacterOccurence = strchr( ( pCharacterOccurence + 1 ),'.' );
        }

        /* If there are more than three dots in the IP address */
        if( ( count != 3 ) || ( IPAddressWrong == TRUE ) )
        {
            #ifdef DBG
            fprintf( stderr, "Incorrect No. of dots in address : %d\n",count );
            #endif
            IPAddressWrong = FALSE;
            count = 0;
            continue;
        }

        count = 0;

        /* Convert the IP address from DOT notation to ULONG */
        returnCode = IPDotToUlong( pIPAddress, pAddress );

        /* If IP Address is not correct */
        if( SUCCESS != returnCode )
        {
            continue;
        }

        /* Valid IP Address has been retrieved */
        return SUCCESS;
    }
}

/*
 * Name:     GetStringFromUser
 *
 * Purpose:  Prompt the user to enter a string and store it in the received
 *           buffer.
 *
 * Params:   pFieldName - Name of the string to be retrieved.
 *           pBuffer    - Buffer to receive the string from user.
 *           bufLen     - Length of the buffer to receive the string from user.
 *
 * Return:   SUCCESS   - If the user enters a valid value
 *           ENTER_KEY - If the user presses the Enter key
 *
 * Notes:    None
 */
uint8_t GetStringFromUser( char* pFieldName, char* pBuffer, int bufLen )
{
    char *pEndOfLine = NULL;
    int  len = 0;

    while(1)
    {
        /* Print the menu */
        fprintf( stderr, "\nPlease enter %s (up to %d Characters),"\
                         " or press <Enter> to leave the field as blank:\n",
                         pFieldName, ( bufLen - 3 ) );

        /* Clear the buffer */
        memset( pBuffer, 0, bufLen );

        /* Receive the input from the user */
        fgets( pBuffer, bufLen, stdin );

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( pBuffer, ENTER_KEY );
        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }

        #ifdef DBG
        fprintf( stderr, "Entered Value : %s\n", pBuffer );
        #endif

        /* If only <ENTER> is pressed by the user, return to main menu */
        if( ENTER_KEY == pBuffer[0] )
        {
            return ENTER_KEY;
        }

        /* Remove the enter key read at the end of the buffer */
        len = strlen( pBuffer );
        pBuffer[len - 1] = '\0';
        len -= 1;

        /* If the user has entered string of more than 17 characters */
        if( ( bufLen - 3) < len )
        {
            continue;
        }
        return SUCCESS;
    }
}

/*
 * Name:     GetPDPType
 *
 * Purpose:  Prompt the user to enter the PDP Type.
 *
 * Params:   None
 *
 * Return:   PDP Type value provided by the user(between 0 - 3),
 *           or hex value of enter key if pressed by the user.
 *
 * Notes:    None
 */
int GetPDPType()
{
    char PDPType[OPTION_LEN];
    uint8_t PDPTypeValue = 0;

    while(1)
    {
        char *pEndOfLine = NULL;
        int len = 0;
        /* Print the menu */
        fprintf( stderr, "\nPlease enter PDP Type value( 0 - IPv4, 1 - PPP, "\
                         "2 - IPV6, 3 - IPV4V6 ),or press <Enter> to exit:\n"\
                         "Option : ");

        /* Receive the input from the user */
        fgets( PDPType, ( OPTION_LEN ), stdin );

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( PDPType, ENTER_KEY );
        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }

        #ifdef DBG
        fprintf( stderr, "PDP Type Value : %s\n", PDPType );
        #endif

        len = strlen( PDPType );

        /* If only <ENTER> is pressed by the user, return to main menu */
        if( ENTER_KEY == PDPType[0] )
            return ENTER_KEY;

        /* If user has entered an invalid input, prompt again */
        if( len < 1 || 2 < len )
        {
            continue;
        }

        /* Convert the authentication value provided by user into integer */
        PDPType[ len - 1 ] = '\0';
        PDPTypeValue = atoi( PDPType );

        /* Validate the Authentication value */
        if( 3 < PDPTypeValue )
        {
            continue;
        }
        return PDPTypeValue;
    }
}

/*
 * Name:     GetAuthenticationValue
 *
 * Purpose:  Prompt the user to enter the Authentication value.
 *
 * Params:   None
 *
 * Return:   Authentication value provided by the user(between 0 - 3),
 *           or hex value of enter key if pressed by the user.
 *
 * Notes:    None
 */
int GetAuthenticationValue()
{
    char authenticationValue[OPTION_LEN];
    char *pEndOfLine = NULL;
    uint8_t authValue = 0;
    int  len = 0;

    while(1)
    {
        /* Print the menu */
        fprintf( stderr, "\nPlease enter Authentication value( 0 - None, 1 -"\
                         "PAP, 2 - CHAP, 3 - PAP/CHAP ),or press <Enter> to "\
                         "leave the field as blank:\nOption : ");

        /* Receive the input from the user */
        fgets( authenticationValue, ( OPTION_LEN ), stdin );

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( authenticationValue, ENTER_KEY );
        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }

        #ifdef DBG
        fprintf( stderr, "Authentication Value : %s\n", authenticationValue );
        #endif

        len = strlen( authenticationValue );

        /* If only <ENTER> is pressed by the user, return to main menu */
        if( ENTER_KEY == authenticationValue[0] )
            return ENTER_KEY;

        /* If user has entered an invalid input, prompt again */
        if( len < 1 || 2 < len )
        {
            continue;
        }

        /* Convert the authentication value provided by user into integer */
        authenticationValue[ len - 1 ] = '\0';
        authValue = atoi( authenticationValue );

        /* Validate the Authentication value */
        if( 3 < authValue )
        {
            continue;
        }
        return authValue;
    }
}

/*
 * Name:     GetIPFamilyPreference
 *
 * Purpose:  Prompt the user to enter the IP Family preference
 *
 * Params:   None
 *
 * Return:   IPFamilyPreference
 *
 * Notes:    None
 */
int GetIPFamilyPreference()
{
    char selOption[OPTION_LEN];
    char *pEndOfLine = NULL;
    int len = 0;

    while(1)
    {
        /* Print the menu */
        fprintf( stderr,"\n1. IPV4 (default)\n" \
                        "2. IPV6\n" \
                        "3. IPV4V6\n" \
                        "Please select IP family preference for the call, or press <Enter> to exit: ");

        /* Receive the input from the user */
        fgets( selOption, ( OPTION_LEN ), stdin );

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( selOption, ENTER_KEY );
        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }

        #ifdef DBG
        fprintf( stderr, "Selected Option : %s\n", selOption );
        #endif

        len = strlen( selOption );

        /* If only <ENTER> is pressed by the user, return to main menu */
        if( ENTER_KEY == selOption[0] )
            return ENTER_KEY_PRESSED;

        /* If user has entered an invalid input, prompt again */
        if( 2 > len )
        {
            continue;
        }
        /* Convert the option added by user into integer */
        selOption[ len - 1 ] = '\0';
        int selection = atoi( selOption );

        switch (selection)
        {
            case 1:
                g_ipfamilypreference = IPv4_FAMILY_PREFERENCE;
                break;
            case 2:
                g_ipfamilypreference = IPv6_FAMILY_PREFERENCE;
                break;
            case 3:
                g_ipfamilypreference = IPv4v6_FAMILY_PREFERENCE;
                break;
            default:
                continue; /* reject everything else */
                break;
        }

        if ( (g_ipfamilypreference == IPv4_FAMILY_PREFERENCE) ||
             (IPv6_FAMILY_PREFERENCE == g_ipfamilypreference) ||
             (IPv4v6_FAMILY_PREFERENCE == g_ipfamilypreference) )
        {
            break;
        }
    }
    return g_ipfamilypreference;
}

/*
 * Name:     GetUserProfileId
 *
 * Purpose:  Prompt the user to enter the profile id whose information needs to
 *           be retrieved.
 *
 * Params:   None
 *
 * Return:   Profile index selected by the user(between 1 - 16),
 *           or hex value of enter key if pressed by the user else FALSE.
 *
 * Notes:    None
 */
int GetUserProfileId()
{
    char selOption[OPTION_LEN];
    char *pEndOfLine = NULL;
    uint8_t profileIdx = 0;
    int len = 0;

    while(1)
    {
        /* Print the menu */
        fprintf( stderr, "\nPlease provide a profile id(1-16), or press <Enter> to exit: ");

        /* Receive the input from the user */
        fgets( selOption, ( OPTION_LEN ), stdin );

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( selOption, ENTER_KEY );
        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }

        #ifdef DBG
        fprintf( stderr, "Selected Option : %s\n", selOption );
        #endif

        len = strlen( selOption );

        /* If only <ENTER> is pressed by the user, return to main menu */
        if( ENTER_KEY == selOption[0] )
            return ENTER_KEY_PRESSED;

        /* If user has entered an invalid input, prompt again */
        if(len < 1 || 3 < len )
        {
            continue;
        }
        else if( 3 == len )
        {
            /* If the user has provided a 2 digit profile id, validate the
             * second digit as atoi() will ignore it.
             */
            if( ( selOption[ len - 2 ] < '0' ) ||
                ( selOption[ len - 2 ] > '6' ) )
            {
                continue;
            }
        }

        /* Convert the option added by user into integer */
        selOption[ len - 1 ] = '\0';
        profileIdx = atoi( selOption );

        /* If an invalid profile id is provided by the user */
        if( ( MIN_PROFILES > profileIdx ) ||
            ( MAX_PROFILES < profileIdx ) )
        {
            continue;
        }
        else
        {
             break;
        }
    }
    return profileIdx;
}

/* Write to wds fd and wait for response or time out */
static int write_wdsfd_waitrsp(int wdsFdIndex, uint16_t reqLen, struct timespec times)
{
    int rtn;
    uint8_t bResp = 0;
    int lock_rc = -1;

    g_read_success[wdsFdIndex] = false;
    pthread_mutex_init(&(con_mutex[wdsFdIndex]), NULL);
    pthread_cond_init(&con_cond[wdsFdIndex], NULL);

    if(wds_fd[wdsFdIndex]<0)
    {
        fprintf(stderr, "%s: Invalid fd %d\n",__func__,wdsFdIndex);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    //Lock before send QMI message
    lock_rc = pthread_mutex_trylock(&(con_mutex[wdsFdIndex]));
    if (lock_rc)
    {
       fprintf(stderr, "%s: mutex locked fd %d\n",__func__,wdsFdIndex);
       return eLITE_CONNECT_APP_ERR_QMI;
    }
    #ifdef DBG
    fprintf(stdout, ">> Sending QMI \n");
    PrintHex(g_qmi_req[wdsFdIndex],reqLen);
    #endif
    rtn = write(wds_fd[wdsFdIndex], g_qmi_req[wdsFdIndex], reqLen);
    if (rtn != reqLen)
    {
        fprintf(stderr, "%s: write %d wrote %d\n",__func__,reqLen, rtn);
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    /* Wait for timeout or some response for this request */
    if( pthread_cond_timedwait(&con_cond[wdsFdIndex],&con_mutex[wdsFdIndex], &times) == ETIMEDOUT )
    {
        #ifdef DBG
        fprintf(stderr, "pthread_cond_timedwait IP%s \n",GET_IP_VERSION(wdsFdIndex));
        #endif
        usleep(100000);//wait thread complete
    }
    else
    {
       #ifdef DBG
       fprintf(stderr, "Data arrived IP%s \n",GET_IP_VERSION(wdsFdIndex));
       #endif
       if (g_read_success[wdsFdIndex])
           bResp = 1;
    }

    if(bResp == 0)
    {
       fprintf(stderr, "%s: failed,response not received or error\n",__func__);
       return eLITE_CONNECT_APP_ERR_QMI;
    }
    return eLITE_CONNECT_APP_OK;
}

static int set_mux_id(uint8_t muxID, int wdsFdIndex)
{
    pack_qmi_t req_ctx;
    unpack_qmi_t rsp_ctx;
    uint16_t reqLen;
    int rtn;
    struct timespec times;
    long long timeout = QMI_CMD_TIMEOUT_SEC;
    unpack_wds_SetMuxID_t output;

    VALIDATE_WDS_FD_INDEX(wdsFdIndex)
    SETUP_REQRSP_CTX_WDS_FD(wdsFdIndex, req_ctx, rsp_ctx)

    clock_gettime(CLOCK_REALTIME, &times);
    times.tv_sec += timeout;

    rtn = pack_wds_SetMuxID(&req_ctx, g_qmi_req[wdsFdIndex], &reqLen, &muxID);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr, "pack_wds_SetMuxID error!\n");
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    rtn = write_wdsfd_waitrsp(wdsFdIndex, reqLen, times);
    if (rtn != eLITE_CONNECT_APP_OK) {
        fprintf(stderr, "%s: write_wdsfd_waitrsp failure, ret %d\n",__func__,rtn );
        return rtn;
    }

    rtn = unpack_wds_SetMuxID(g_qmi_rsp[wdsFdIndex], g_rspLen[wdsFdIndex], &output);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr, "%s: unpack failed, ret %d\n",__func__,rtn);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    return eLITE_CONNECT_APP_OK;
}

int get_number_of_qmap_supported()
{
    unsigned int iNumber = 0;
    int result = -1;
    char szDevicePath[32]={0};
    int fd = -1;

    snprintf(szDevicePath,31,"/dev/qcqmi%d",g_modem_index);
    fd = open(szDevicePath, O_RDWR);
    if(fd>=0)
    {
       //Get Max Qmap supported
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
    #ifdef DBG
    fprintf(stderr,"iNumber:%d\n",iNumber);
    #endif
    return iNumber;
}

/******************************************************************************
* Option 8 : Delete a profile stored on the device
******************************************************************************/
static int get_model_id(unpack_dms_GetModelID_t *pModelId)
{
    pack_qmi_t req_ctx;
    unpack_qmi_t rsp_ctx;
    uint16_t rspLen;
    uint16_t reqLen;
    int rtn;

    if (!pModelId) {
        fprintf (stderr, "%s: invalid arg\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }    

    memset(&req_ctx, 0, sizeof(req_ctx));
    memset(&rsp_ctx, 0, sizeof(rsp_ctx));
    g_xid_o++;
    req_ctx.xid = g_xid_o;
    int dms = client_fd(eDMS);
    if(dms<0)
    {
        fprintf (stderr, "%s: dms fd<0\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    memset(g_qmi_rsp_o,0,QMI_MSG_MAX);
    memset(g_qmi_req_o,0,QMI_MSG_MAX);
    if(pack_dms_GetModelID(&req_ctx, g_qmi_req_o, &reqLen, NULL) != eQCWWAN_ERR_NONE)
    {
        printf("pack_dms_GetModelID error\n");
        close(dms);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    rtn = write(dms, g_qmi_req_o, reqLen);
    if (rtn!=reqLen)
    {
        printf("write %d wrote %d\n", reqLen, rtn);
        close(dms);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    rspLen = read(dms, g_qmi_rsp_o, QMI_MSG_MAX);
    if(rspLen > 0)
    {
        helper_get_resp_ctx(eDMS, g_qmi_rsp_o, rspLen, &rsp_ctx);
        if (rsp_ctx.xid == g_xid_o) {
            rtn = unpack_dms_GetModelID(g_qmi_rsp_o, rspLen, pModelId);
            if(rtn != eQCWWAN_ERR_NONE)
            {
                printf("%s: failed, ret %d\n",__func__,rtn);
                close(dms);
                return eLITE_CONNECT_APP_ERR_QMI;
            }
        }
        else {
           printf("%s: failed,tx id not matched\n",__func__);
           close(dms);
           return eLITE_CONNECT_APP_ERR_QMI;
        }
    }
    else {
       printf("%s: failed,response length error\n",__func__);
       close(dms);
       return eLITE_CONNECT_APP_ERR_QMI;
    }    
    close(dms);
    return eLITE_CONNECT_APP_OK;
}

static int get_home_network(unpack_nas_GetHomeNetwork_t *pHomeNw)
{
    pack_qmi_t req_ctx;
    unpack_qmi_t rsp_ctx;
    uint16_t rspLen;
    uint16_t reqLen;
    int rtn;

    if (!pHomeNw) {
        fprintf (stderr, "%s: invalid arg\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    memset(&req_ctx, 0, sizeof(req_ctx));
    memset(&rsp_ctx, 0, sizeof(rsp_ctx));
    g_xid_o++;
    req_ctx.xid = g_xid_o;
    int nas = client_fd(eNAS);
    if(nas<0)
    {
        fprintf (stderr, "%s: nas fd<0\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    memset(g_qmi_rsp_o,0,QMI_MSG_MAX);
    memset(g_qmi_req_o,0,QMI_MSG_MAX);
    if(pack_nas_GetHomeNetwork(&req_ctx, g_qmi_req_o, &reqLen) != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr,"pack_nas_GetHomeNetwork error\n");
        close(nas);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    rtn = write(nas, g_qmi_req_o, reqLen);
    if (rtn!=reqLen)
    {
        fprintf(stderr, "write %d wrote %d\n", reqLen, rtn);
        close(nas);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    rspLen = read(nas, g_qmi_rsp_o, QMI_MSG_MAX);
    if(rspLen > 0)
    {
        helper_get_resp_ctx(eNAS, g_qmi_rsp_o, rspLen, &rsp_ctx);
        if (rsp_ctx.xid == g_xid_o) {
            rtn = unpack_nas_GetHomeNetwork(g_qmi_rsp_o, rspLen, pHomeNw);
            if(rtn != eQCWWAN_ERR_NONE)
            {
                fprintf(stderr,"%s: failed, ret %d\n",__func__,rtn);
                close(nas);
                return eLITE_CONNECT_APP_ERR_QMI;
            }
        }
        else {
           fprintf(stderr,"%s: failed,tx id not matched\n",__func__);
           close(nas);
           return eLITE_CONNECT_APP_ERR_QMI;
        }
    }
    else {
       fprintf(stderr,"%s: failed,response length error\n",__func__);
       close(nas);
       return eLITE_CONNECT_APP_ERR_QMI;
    }    
    close(nas);
    return eLITE_CONNECT_APP_OK;
}

static int delete_profile(
    pack_wds_SLQSDeleteProfile_t *pProfileIn,
    unpack_wds_SLQSDeleteProfile_t *pProfileOut)
{
    pack_qmi_t req_ctx;
    unpack_qmi_t rsp_ctx;
    uint16_t reqLen;
    int rtn;
    struct timespec times;
    long long timeout = QMI_CMD_TIMEOUT_SEC;

    /* profile related operation on wds fd 0 */
    int wdsFdIndex = eLITE_WDS_FD_ARR_INDEX_0;

    if ((!pProfileIn) || (!pProfileOut)) {
        fprintf (stderr, "%s: invalid arg\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    SETUP_REQRSP_CTX_WDS_FD(wdsFdIndex, req_ctx, rsp_ctx)

    clock_gettime(CLOCK_REALTIME, &times);
    times.tv_sec += timeout;

    if(pack_wds_SLQSDeleteProfile(&req_ctx, g_qmi_req[wdsFdIndex], &reqLen, pProfileIn) != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr, "pack_wds_SLQSDeleteProfile error!\n");
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    rtn = write_wdsfd_waitrsp(wdsFdIndex, reqLen, times);
    if (rtn != eLITE_CONNECT_APP_OK) {
        fprintf(stderr, "%s: write_wdsfd_waitrsp failure, ret %d\n",__func__,rtn );
        return rtn;
    }

    rtn = unpack_wds_SLQSDeleteProfile(g_qmi_rsp[wdsFdIndex], g_rspLen[wdsFdIndex], pProfileOut);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr, "%s: unpack failed, ret %d\n",__func__,rtn);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    return eLITE_CONNECT_APP_OK;
}

/*
 * Name:     delete_profile_from_device
 *
 * Purpose:  Delete the profile from the device for the profile id provided by
 *           the user.
 *
 * Params:   None
 *
 * Return:   None.
 *
 * Notes:    None
 */
void delete_profile_from_device()
{
    int                            rtn = 0;
    uint8_t                        profileType = PROFILE_TYPE_UMTS;
    uint8_t                        profileId;
    pack_wds_SLQSDeleteProfile_t   deleteProfile;
    unpack_wds_SLQSDeleteProfile_t   deleteProfileOut;

    while(1)
    {
        memset(&deleteProfile,0, sizeof(deleteProfile));
        memset(&deleteProfileOut,0, sizeof(deleteProfileOut));

        deleteProfileOut.extendedErrorCode = (uint16_t)-1;

        /* Display all the profiles stored on the device */
        display_all_profiles();

        /* If no profile exist on the device, return */
        if( 0 == indexInfo.totalProfilesOnDevice )
        {
            fprintf( stderr, "No Profile exist on the device for deletion "\
                             "or check device connectivity\n\n");
            return;
        }
            
        /* Receive the user input */
        profileId = GetUserProfileId();

        /* If only <ENTER> is pressed by the user, return to main menu */
        if( ENTER_KEY_PRESSED == profileId )
            return;

        deleteProfile.profileIndex = profileId;
        deleteProfile.profileType = profileType;

        /* Delete the profile from the device */
        rtn = delete_profile(&deleteProfile, &deleteProfileOut); 

        if (rtn != eLITE_CONNECT_APP_OK)
        {
            fprintf( stderr, "Profile deletion Failed\n"\
                             "Failure cause - %d\n", rtn );
            continue;
        }

        fprintf( stderr, "Profile for index %d deleted successfully\n",
                          profileId );
    }
}

/******************************************************************************
* Option 7 : Modify the settings of an existing profile stored on the device
******************************************************************************/

static int change_profile_setting(
    pack_wds_SLQSModifyProfile_t *pProfileIn,
    unpack_wds_SLQSModifyProfile_t *pProfileOut)
{
    pack_qmi_t req_ctx;
    unpack_qmi_t rsp_ctx;
    uint16_t reqLen;
    int rtn;
    struct timespec times;
    long long timeout = QMI_CMD_TIMEOUT_SEC;

    /* profile related operation on wds fd 0 */
    int wdsFdIndex = eLITE_WDS_FD_ARR_INDEX_0;

    if ((!pProfileIn) || (!pProfileOut)) {
        fprintf (stderr, "%s: invalid arg\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    SETUP_REQRSP_CTX_WDS_FD(wdsFdIndex, req_ctx, rsp_ctx)

    clock_gettime(CLOCK_REALTIME, &times);
    times.tv_sec += timeout;

    if(pack_wds_SLQSModifyProfile(&req_ctx, g_qmi_req[wdsFdIndex], &reqLen, pProfileIn) != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr,"pack_wds_SLQSModifyProfile error!\n");
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    rtn = write_wdsfd_waitrsp(wdsFdIndex, reqLen, times);
    if (rtn != eLITE_CONNECT_APP_OK) {
        fprintf(stderr, "%s: write_wdsfd_waitrsp failure, ret %d\n",__func__,rtn );
        return rtn;
    }

    rtn = unpack_wds_SLQSModifyProfile(g_qmi_rsp[wdsFdIndex], g_rspLen[wdsFdIndex], pProfileOut);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr, "%s: unpack failed, ret %d\n",__func__,rtn);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    return eLITE_CONNECT_APP_OK;
}

/*
 * Name:     modify_profile_settings
 *
 * Purpose:  Modify the Profile settings of the profile id selected by the user
 *           with the values entered by the user.
 *
 * Params:   None
 *
 * Return:   None
 *
 * Notes:    None
 */
void modify_profile_settings()
{
    pack_wds_SLQSModifyProfile_t modifyProfile;
    unpack_wds_SLQSModifyProfile_t modifyProfileOut;
    int rtn;
    struct profileInformation profileInfo;
    uint8_t                   profileId = 0, returnCode = 0, idx = 0;
    uint8_t                   profileIdMatch = FALSE;
    char                      IPAddress[MAX_FIELD_SIZE];
    uint16_t                  ProfileSize,APNSize, UserSize, PwdSize;
    uint16_t                  extendedErrorCode;

    profileInfo.profileType = PROFILE_TYPE_UMTS;

    while( 1 )
    {
        memset(&modifyProfile,0, sizeof(modifyProfile));
        memset(&modifyProfileOut,0, sizeof(modifyProfileOut));
        extendedErrorCode = (uint16_t)-1;

        modifyProfileOut.pExtErrorCode = &extendedErrorCode;

        /* Display all the profiles stored on the device */
        display_all_profiles();

        if( 0 == indexInfo.totalProfilesOnDevice )
        {
            fprintf( stderr, "No Profile exist on device for modification"\
                             "or check device connectivity\n\n");
            return;
        }

       while(1)
       {
            /* Prompt the user to enter the profile id whose values need to be
             * modified.
             */
            profileId = GetUserProfileId();

            /* If only <ENTER> is pressed by the user, return to main menu */
            if( ENTER_KEY_PRESSED == profileId )
            {
                return;
            }

            /* If the user has enter an invalid profile id */
            for( idx = 0; idx < indexInfo.totalProfilesOnDevice; idx++ )
            {
                if( profileId == indexInfo.profileIndex[idx] )
                {
                     profileIdMatch = TRUE;
                     break;
                }
            }

            if( profileIdMatch == TRUE )
            {
                break;
            }
        }

        modifyProfile.pProfileId = &profileId;
        modifyProfile.pProfileType = &profileInfo.profileType;
        //modifyProfile.curProfile.SlqsProfile3GPP.pPDPtype = &profileInfo.PDPType;

         /* Get PDP Type */
        profileInfo.PDPType = GetPDPType();
        if( ENTER_KEY != profileInfo.PDPType )
        {
            modifyProfile.curProfile.SlqsProfile3GPP.pPDPtype = &profileInfo.PDPType;
        }

        /* Prompt the user to enter the profile parameter values */
        /* Get the IP Address */
        returnCode = GetIPFromUser( "IP", IPAddress, &profileInfo.IPAddress );
        if( ENTER_KEY != returnCode )
        {
            modifyProfile.curProfile.SlqsProfile3GPP.pIPv4AddrPref = &profileInfo.IPAddress; 
        }

        /* Get the Primary DNS Address */
        returnCode = GetIPFromUser( "PrimaryDNS Address", IPAddress,
                                             &profileInfo.primaryDNS );
        if( ENTER_KEY != returnCode )
        {
            modifyProfile.curProfile.SlqsProfile3GPP.pPriDNSIPv4AddPref = &profileInfo.primaryDNS;
        }
        returnCode = 0;

        /* Get the Secondary DNS Address */
        returnCode = GetIPFromUser( "SecondaryDNS Address", IPAddress,
                                               &profileInfo.secondaryDNS );
        if( ENTER_KEY != returnCode )
        {
            modifyProfile.curProfile.SlqsProfile3GPP.pSecDNSIPv4AddPref = &profileInfo.secondaryDNS;
        }
        returnCode = 0;

        /* Get Authentication From the user */
        profileInfo.Authentication = GetAuthenticationValue();
        if( ENTER_KEY != profileInfo.Authentication )
        {
            modifyProfile.curProfile.SlqsProfile3GPP.pAuthenticationPref = &profileInfo.Authentication;
        }
        returnCode = 0;

        /* Get Profile Name from the user, Max size is 14 characters */
        returnCode = GetStringFromUser( "Profile Name", (char*)profileInfo.profileName,
                                                        MAX_PROFILE_NAME_SIZE );
        if( ENTER_KEY != returnCode )
        {
            modifyProfile.curProfile.SlqsProfile3GPP.pProfilename = profileInfo.profileName;
            ProfileSize = strlen((char*)profileInfo.profileName);
            modifyProfile.curProfile.SlqsProfile3GPP.pProfilenameSize = &ProfileSize;
        }
        returnCode = 0;

        /* Get APN Name from the user */
        returnCode = GetStringFromUser( "APN Name", (char*)profileInfo.APNName,
                                                    MAX_APN_SIZE );
        if( ENTER_KEY != returnCode )
        {
            modifyProfile.curProfile.SlqsProfile3GPP.pAPNName = profileInfo.APNName;
            APNSize = strlen((char*)profileInfo.APNName);
            modifyProfile.curProfile.SlqsProfile3GPP.pAPNnameSize = &APNSize;
        }
        returnCode = 0;

        /* Get User Name from the user */
        returnCode = GetStringFromUser( "User Name", (char*)profileInfo.userName,
                                                     MAX_USER_NAME_SIZE );
        if( ENTER_KEY != returnCode )
        {
            modifyProfile.curProfile.SlqsProfile3GPP.pUsername = profileInfo.userName;
            UserSize = strlen((char*)profileInfo.userName);
            modifyProfile.curProfile.SlqsProfile3GPP.pUsernameSize = &UserSize;
        }
        returnCode = 0;

        /* Get Password from the user */
        returnCode = GetStringFromUser( "Password", (char*)profileInfo.password,
                                                    MAX_FIELD_SIZE );
        if( ENTER_KEY != returnCode )
        {
            modifyProfile.curProfile.SlqsProfile3GPP.pPassword = profileInfo.password;
            PwdSize = strlen((char*)profileInfo.password);
            modifyProfile.curProfile.SlqsProfile3GPP.pPasswordSize = &PwdSize;
        }

        rtn = change_profile_setting(&modifyProfile, &modifyProfileOut); 

        if (rtn != eLITE_CONNECT_APP_OK)
        {
            fprintf( stderr, "Profile Modification Failed\n"\
                             "Failure cause - %d\n", rtn );
            continue;
        }
        fprintf( stderr, "Profile Settings updated successfully for Profile ID:"
                          " %d\n",profileId );
    }
}


/******************************************************************************
* Option 6 : Create a profile on the device
******************************************************************************/
static int create_new_profile(
    pack_wds_SLQSCreateProfile_t *pProfileIn,
    unpack_wds_SLQSCreateProfile_t *pProfileOut)
{
    pack_qmi_t req_ctx;
    unpack_qmi_t rsp_ctx;
    uint16_t reqLen;
    int rtn;
    struct timespec times;
    long long timeout = QMI_CMD_TIMEOUT_SEC;

    /* profile related operation on wds fd 0 */
    int wdsFdIndex = eLITE_WDS_FD_ARR_INDEX_0;

    if ((!pProfileIn) || (!pProfileOut)) {
        fprintf (stderr, "%s: invalid arg\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    SETUP_REQRSP_CTX_WDS_FD(wdsFdIndex, req_ctx, rsp_ctx)

    clock_gettime(CLOCK_REALTIME, &times);
    times.tv_sec += timeout;

    if(pack_wds_SLQSCreateProfile(&req_ctx, g_qmi_req[wdsFdIndex], &reqLen, pProfileIn) != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr,"pack_wds_SLQSCreateProfile error!\n");
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    rtn = write_wdsfd_waitrsp(wdsFdIndex, reqLen, times);
    if (rtn != eLITE_CONNECT_APP_OK) {
        fprintf(stderr, "%s: write_wdsfd_waitrsp failure, ret %d\n",__func__,rtn );
        return rtn;
    }

    rtn = unpack_wds_SLQSCreateProfile(g_qmi_rsp[wdsFdIndex], g_rspLen[wdsFdIndex], pProfileOut);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr, "%s: unpack failed, ret %d\n",__func__,rtn);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    return eLITE_CONNECT_APP_OK;
}

/*
 * Name:     create_profile
 *
 * Purpose:  Create the Profile with the values provided by the user.
 *
 * Params:   None
 *
 * Return:   None
 *
 * Notes:    None
 */
void create_profile()
{
    pack_wds_SLQSCreateProfile_t createProfile;
    unpack_wds_SLQSCreateProfile_t createProfileOut;
    PackCreateProfileOut         profileInfoOut;
    wds_profileInfo              curProfile;    
    struct profileInformation    profileInfo;
    int                          rtn;
    uint8_t                      profileId = 0, returnCode = 0;
    uint8_t                      PDPType;
    char                         IPAddress[MAX_FIELD_SIZE];
    uint16_t                     ProfileSize,APNSize, UserSize, PwdSize;

    profileInfo.profileType = PROFILE_TYPE_UMTS;

    while( 1 )
    {
        memset(&createProfile,0, sizeof(createProfile));
        memset(&curProfile,0, sizeof(curProfile));
        memset(&createProfileOut,0, sizeof(createProfileOut));
        memset(&profileInfoOut,0, sizeof(profileInfoOut));

        createProfileOut.pCreateProfileOut = &profileInfoOut;
        createProfileOut.pProfileID = &profileId;

        /* Display all the profiles stored on the device */
        display_all_profiles();

        if( 0 == indexInfo.totalProfilesOnDevice )
        {
            fprintf( stderr, "No Profile exist on device for modification"\
                             "or check device connectivity\n\n");
            return;
        }
        createProfile.pCurProfile = &curProfile;
        createProfile.pProfileType = &profileInfo.profileType;        

        /* Get PDP Type */
        PDPType = GetPDPType();
        if( ENTER_KEY == PDPType )
        {
            return;
        }

        createProfile.pCurProfile->SlqsProfile3GPP.pPDPtype = &PDPType; 

        /* Prompt the user to enter the profile parameter values */
        /* Get the IP Address */
        returnCode = GetIPFromUser( "IP", IPAddress, &profileInfo.IPAddress );
        if( ENTER_KEY != returnCode )
        {
            createProfile.pCurProfile->SlqsProfile3GPP.pIPv4AddrPref = &profileInfo.IPAddress; 
        }

        /* Get the Primary DNS Address */
        returnCode = GetIPFromUser( "PrimaryDNS Address", IPAddress,
                                             &profileInfo.primaryDNS );
        if( ENTER_KEY != returnCode )
        {
            createProfile.pCurProfile->SlqsProfile3GPP.pPriDNSIPv4AddPref = &profileInfo.primaryDNS;
        }
        returnCode = 0;

        /* Get the Secondary DNS Address */
        returnCode = GetIPFromUser( "SecondaryDNS Address", IPAddress,
                                               &profileInfo.secondaryDNS );
        if( ENTER_KEY != returnCode )
        {
            createProfile.pCurProfile->SlqsProfile3GPP.pSecDNSIPv4AddPref = &profileInfo.secondaryDNS;
        }
        returnCode = 0;

        /* Get Authentication From the user */
        profileInfo.Authentication = GetAuthenticationValue();
        if( ENTER_KEY != profileInfo.Authentication )
        {
            createProfile.pCurProfile->SlqsProfile3GPP.pAuthenticationPref = &profileInfo.Authentication;
        }
        returnCode = 0;

        /* Get Profile Name from the user, Max size is 14 characters */
        returnCode = GetStringFromUser( "Profile Name", (char*)profileInfo.profileName,
                                                        MAX_PROFILE_NAME_SIZE );
        if( ENTER_KEY != returnCode )
        {
            createProfile.pCurProfile->SlqsProfile3GPP.pProfilename = profileInfo.profileName;
            ProfileSize = strlen((char*)profileInfo.profileName);
            createProfile.pCurProfile->SlqsProfile3GPP.pProfilenameSize = &ProfileSize;
        }
        returnCode = 0;

        /* Get APN Name from the user */
        returnCode = GetStringFromUser( "APN Name", (char*)profileInfo.APNName,
                                                    MAX_APN_SIZE );
        if( ENTER_KEY != returnCode )
        {
            createProfile.pCurProfile->SlqsProfile3GPP.pAPNName = profileInfo.APNName;
            APNSize = strlen((char*)profileInfo.APNName);
            createProfile.pCurProfile->SlqsProfile3GPP.pAPNnameSize = &APNSize;
        }
        returnCode = 0;

        /* Get User Name from the user */
        returnCode = GetStringFromUser( "User Name", (char*)profileInfo.userName,
                                                     MAX_USER_NAME_SIZE );
        if( ENTER_KEY != returnCode )
        {
            createProfile.pCurProfile->SlqsProfile3GPP.pUsername = profileInfo.userName;
            UserSize = strlen((char*)profileInfo.userName);
            createProfile.pCurProfile->SlqsProfile3GPP.pUsernameSize = &UserSize;
        }
        returnCode = 0;

        /* Get Password from the user */
        returnCode = GetStringFromUser( "Password", (char*)profileInfo.password,
                                                    MAX_FIELD_SIZE );
        if( ENTER_KEY != returnCode )
        {
            createProfile.pCurProfile->SlqsProfile3GPP.pPassword = profileInfo.password;
            PwdSize = strlen((char*)profileInfo.password);
            createProfile.pCurProfile->SlqsProfile3GPP.pPasswordSize = &PwdSize;
        }

        rtn = create_new_profile(&createProfile, &createProfileOut); 

        if (rtn != eLITE_CONNECT_APP_OK)
        {
            fprintf( stderr, "Profile Modification Failed\n"\
                             "Failure cause - %d\n", rtn );
            continue;
        }
        fprintf( stderr, "Profile Settings updated successfully for Profile ID:"
                          " %d\n",profileId );
    }
}

/******************************************************************************
* Option 5 : Display the settings for a particular profile stored on the device
******************************************************************************/

static int get_profile_setting(
    uint8_t profileType,
    uint8_t profileId,
    unpack_wds_SLQSGetProfileSettings_t *pProfile)
{
    pack_qmi_t req_ctx;
    unpack_qmi_t rsp_ctx;
    uint16_t reqLen;
    int rtn;
    struct timespec times;
    pack_wds_SLQSGetProfileSettings_t reqarg;
    long long timeout = QMI_CMD_TIMEOUT_SEC;

    /* profile related operation on wds fd 0 */
    int wdsFdIndex = eLITE_WDS_FD_ARR_INDEX_0;

    if (!pProfile) {
        fprintf (stderr, "%s: invalid arg\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    SETUP_REQRSP_CTX_WDS_FD(wdsFdIndex, req_ctx, rsp_ctx)
    clock_gettime(CLOCK_REALTIME, &times);
    times.tv_sec += timeout;

    reqarg.ProfileId = profileId;
    reqarg.ProfileType = profileType;
    if(pack_wds_SLQSGetProfileSettings(&req_ctx, g_qmi_req[wdsFdIndex], &reqLen, &reqarg) != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr,"pack_wds_SLQSGetProfileSettings error!\n");
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    rtn = write_wdsfd_waitrsp(wdsFdIndex, reqLen, times);
    if (rtn != eLITE_CONNECT_APP_OK) {
        fprintf(stderr, "%s: write_wdsfd_waitrsp failure, ret %d\n",__func__,rtn );
        return rtn;
    }

    rtn = unpack_wds_SLQSGetProfileSettings(g_qmi_rsp[wdsFdIndex], g_rspLen[wdsFdIndex], pProfile);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        #ifdef DBG
        fprintf(stderr, "%s: unpack failed, ret %d\n",__func__,rtn);
        #endif
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    return eLITE_CONNECT_APP_OK;
}

/*
 * Name:     display_profile_info
 *
 * Purpose:  Display the profile information for the profile index provided by
 *           the user.
 *
 * Params:   None
 *
 * Return:   None.
 *
 * Notes:    None
 */
static void display_profile_info()
{
    int rtn;
    unpack_wds_SLQSGetProfileSettings_t profileSettingsRsp;
    UnPackGetProfileSettingOut profileSetOut;
    uint8_t profileType = PROFILE_TYPE_UMTS;
    uint8_t  profileId;
    uint8_t PDPType = (uint8_t) -1;
    uint32_t IPAddress = (uint32_t) -1;
    char  bufIPAddress[MAX_FIELD_SIZE];
    uint32_t primaryDNS = (uint32_t) -1;
    char  bufPrimaryDNS[MAX_FIELD_SIZE];
    uint32_t secondaryDNS = (uint32_t) -1;
    char  bufSecondaryDNS[MAX_FIELD_SIZE];
    uint8_t authentication= (uint8_t) -1;
    uint8_t  profileName[MAX_PROFILE_NAME_SIZE];
    uint16_t profileNameSize;
    uint8_t  APNName[MAX_APN_SIZE];
    uint16_t APNNameSize;
    uint8_t  Username[MAX_USER_NAME_SIZE];
    uint16_t UsernameSize;
    uint16_t  extendedErrorCode = (uint16_t)-1;
    int w3 = -3, w5 = -5, w8 = -8, w20 = -20;

    memset( &profileSettingsRsp, 0, sizeof(profileSettingsRsp));
    memset( &profileSetOut, 0, sizeof(UnPackGetProfileSettingOut));
    memset( profileName, 0, MAX_PROFILE_NAME_SIZE );
    memset( APNName, 0, MAX_APN_SIZE );
    memset( Username, 0, MAX_USER_NAME_SIZE );
    profileNameSize = sizeof(profileName);
    APNNameSize = sizeof(APNName);
    UsernameSize = sizeof(Username);
    profileSettingsRsp.pProfileSettings = &profileSetOut;
    profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pPDPtype = 
              &PDPType;
    profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pIPv4AddrPref =
              &IPAddress;
    profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pPriDNSIPv4AddPref = 
              &primaryDNS;
    profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pSecDNSIPv4AddPref = 
              &secondaryDNS;
    profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pAuthenticationPref =
              &authentication;
    profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pProfilename = 
              profileName;
    profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pProfilenameSize = 
              &profileNameSize;
    profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pAPNName = 
              APNName;
    profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pAPNnameSize = 
              &APNNameSize;
    profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pUsername = 
              Username;
    profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pUsernameSize = 
              &UsernameSize;
    profileSettingsRsp.pProfileSettings->pExtErrCode = &extendedErrorCode;

    while(1)
    {
        /* Receive the user input */
        profileId = GetUserProfileId();

        /* If only <ENTER> is pressed by the user, return to main menu */
        if( ENTER_KEY_PRESSED == profileId )
        {
            return;
        }
        rtn = get_profile_setting(profileType, profileId, &profileSettingsRsp);
        if (rtn != eLITE_CONNECT_APP_OK) {
            #ifdef DBG
            fprintf( stderr, "Profile retrieving Failed\n"\
                             "Failure cause - %d\nError Code - %d\n\n",
                             rtn, (int)extendedErrorCode );
            #endif
            extendedErrorCode = (uint16_t)-1;
            continue;
        }
        if (profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pProfilename == NULL)
            strcpy((char*)profileName,"Unknown");
        if (profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pAPNName == NULL)
            strcpy((char*)APNName,"Unknown");
        if (profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pUsername == NULL)
            strcpy((char*)Username,"Unknown");

        /* Reset the buffers */
        memset( bufIPAddress, 0, MAX_FIELD_SIZE );
        memset( bufPrimaryDNS, 0, MAX_FIELD_SIZE );
        memset( bufSecondaryDNS, 0, MAX_FIELD_SIZE );

        /* Convert ULONG to Dot notation for display */
        IPUlongToDot( IPAddress, bufIPAddress );
        IPUlongToDot( primaryDNS, bufPrimaryDNS );
        IPUlongToDot( secondaryDNS, bufSecondaryDNS );

        /* Display the header */
        fprintf( stderr, "%*s%*s%*s%*s%*s%*s%*s%*s%*s\n",
                         w3, "ID", w8, "PDPType", w20, "IPAddress",
                         w20, "PrimaryDNS", w20, "SecondaryDNS", w5, "Auth", w20, "ProfileName",
                         w20, "APNName", w20, "UserName" );

        /* Display the retrieved profile information */
        fprintf( stderr, "%*d%*u%*s%*s%*s%*u%*s%*s%*s\n",
                         w3, profileId, w8, PDPType, w20, bufIPAddress,
                         w20, bufPrimaryDNS, w20, bufSecondaryDNS, w5, authentication,
                         w20, profileName, w20, APNName, w20, Username );

    }
}



/*************************************************************************
* Option 4 : Display all the profiles stored on the device
*************************************************************************/
/*
 * Name:     display_all_profiles
 *
 * Purpose:  Display all the profiles stored on the device.
 *
 * Params:   None
 *
 * Return:   None
 *
 * Notes:    None
 */
static void display_all_profiles()
{
     int rtn;
    unpack_wds_SLQSGetProfileSettings_t profileSettingsRsp;
    UnPackGetProfileSettingOut profileSetOut;
    uint8_t profileType = PROFILE_TYPE_UMTS;
    uint8_t  profileId;
    uint8_t PDPType;
    uint32_t IPAddress;
    char  bufIPAddress[MAX_FIELD_SIZE];
    uint32_t primaryDNS;
    char  bufPrimaryDNS[MAX_FIELD_SIZE];
    uint32_t secondaryDNS;
    char  bufSecondaryDNS[MAX_FIELD_SIZE];
    uint8_t authentication;
    uint8_t  profileName[MAX_PROFILE_NAME_SIZE];
    uint16_t profileNameSize;
    uint8_t  APNName[MAX_APN_SIZE];
    uint16_t APNNameSize;
    uint8_t  Username[MAX_USER_NAME_SIZE];
    uint16_t UsernameSize;
    uint16_t  extendedErrorCode;
    int w3 = -3, w5 = -5, w8 = -8, w20 = -20;

    indexInfo.totalProfilesOnDevice = 0;

    profileNameSize = sizeof(profileName);
    APNNameSize = sizeof(APNName);
    UsernameSize = sizeof(Username);

    /* Display the header */
    fprintf( stderr, "\n%*s%*s%*s%*s%*s%*s%*s%*s%*s\n",
                     w3, "ID", w8, "PDPType", w20, "IPAddress",
                     w20, "PrimaryDNS", w20, "SecondaryDNS", w5, "Auth", w20, "ProfileName",
                     w20, "APNName", w20, "UserName" );

    /* Retrieve the information for all the profiles loaded on the device */
    for( profileId = MIN_PROFILES; profileId <= MAX_PROFILES; profileId++ )
    {
        /* Initialize the buffers */
        memset( &profileSettingsRsp, 0, sizeof(profileSettingsRsp));
        memset( &profileSetOut, 0, sizeof(UnPackGetProfileSettingOut));
        memset( profileName, 0, MAX_PROFILE_NAME_SIZE );
        memset( APNName, 0, MAX_APN_SIZE );
        memset( Username, 0, MAX_USER_NAME_SIZE );
        PDPType = (uint8_t) -1;
        IPAddress = (uint32_t) -1;
        primaryDNS = (uint32_t) -1;
        secondaryDNS = (uint32_t) -1;
        authentication= (uint8_t) -1;
        extendedErrorCode = (uint16_t)-1;

        profileSettingsRsp.pProfileSettings = &profileSetOut;
        profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pPDPtype = 
              &PDPType;
        profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pIPv4AddrPref =
              &IPAddress;
        profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pPriDNSIPv4AddPref = 
              &primaryDNS;
        profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pSecDNSIPv4AddPref = 
              &secondaryDNS;
        profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pAuthenticationPref =
              &authentication;
        profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pProfilename = 
              profileName;
        profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pProfilenameSize = 
              &profileNameSize;
        profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pAPNName = 
              APNName;
        profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pAPNnameSize = 
              &APNNameSize;
        profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pUsername = 
              Username;
        profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pUsernameSize = 
              &UsernameSize;
        profileSettingsRsp.pProfileSettings->pExtErrCode = &extendedErrorCode;

        rtn = get_profile_setting(profileType, profileId, &profileSettingsRsp);
        if (rtn != eLITE_CONNECT_APP_OK) {
            #ifdef DBG
            fprintf( stderr, "Profile retrieving Failed\n"\
                             "Failure cause - %d\nError Code - %d\n\n",
                             rtn, (int)extendedErrorCode );
            #endif
            extendedErrorCode = (uint16_t)-1;
            continue;
        }

        /* Store the profile indexes for successfully retrieved profiles */
        indexInfo.profileIndex[indexInfo.totalProfilesOnDevice] = profileId;
        indexInfo.totalProfilesOnDevice++;

        if (profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pProfilename == NULL)
            strcpy((char*)profileName,"Unknown");
        if (profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pAPNName == NULL)
            strcpy((char*)APNName,"Unknown");
        if (profileSettingsRsp.pProfileSettings->curProfile.SlqsProfile3GPP.pUsername == NULL)
            strcpy((char*)Username,"Unknown");

        /* Reset the buffers */
        memset( bufIPAddress, 0, MAX_FIELD_SIZE );
        memset( bufPrimaryDNS, 0, MAX_FIELD_SIZE );
        memset( bufSecondaryDNS, 0, MAX_FIELD_SIZE );

        /* Convert ULONG to Dot notation for display */
        IPUlongToDot( IPAddress, bufIPAddress );
        IPUlongToDot( primaryDNS, bufPrimaryDNS );
        IPUlongToDot( secondaryDNS, bufSecondaryDNS );

        /* Display the retrieved profile information */
        fprintf( stderr, "%*d%*u%*s%*s%*s%*u%*s%*s%*s\n",
                         w3, profileId, w8, PDPType, w20, bufIPAddress,
                         w20, bufPrimaryDNS, w20, bufSecondaryDNS, w5, authentication,
                         w20, profileName, w20, APNName, w20, Username );
    }
}

/*
 * Name:     display_all_profile
 *
 * Purpose:  Call display_all_profiles to display all the profiles stored on the
 *           device.
 *
 * Params:   None
 *
 * Return:   None
 *
 * Notes:    None
 */
void display_all_profile()
{
    display_all_profiles();
    if( 0 == indexInfo.totalProfilesOnDevice )
    {
        fprintf( stderr, "No Profile exist on the device"\
                         "or check device connectivity\n\n" );
    }
}


/*************************************************************************
* Option 3 : Stop the currently active Data Session
*************************************************************************/
static int stop_data_session(pack_wds_SLQSStopDataSession_t *pSession, int wdsFdIndex)
{
    pack_qmi_t req_ctx;
    unpack_qmi_t rsp_ctx;
    uint16_t reqLen;
    int rtn;
    struct timespec times;
    long long timeout = QMI_CMD_TIMEOUT_SEC;
    unpack_wds_SLQSStopDataSession_t resp;

    if (!pSession) {
        fprintf (stderr, "%s: invalid arg\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    memset(&resp,0,sizeof(unpack_wds_SLQSStopDataSession_t));

    VALIDATE_WDS_FD_INDEX(wdsFdIndex)
    SETUP_REQRSP_CTX_WDS_FD(wdsFdIndex, req_ctx, rsp_ctx)

    clock_gettime(CLOCK_REALTIME, &times);
    times.tv_sec += timeout;

    if(pack_wds_SLQSStopDataSession(&req_ctx, g_qmi_req[wdsFdIndex], &reqLen, pSession) != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr, "pack_wds_SLQSStartDataSession error!\n");
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    rtn = write_wdsfd_waitrsp(wdsFdIndex, reqLen, times);
    if (rtn != eLITE_CONNECT_APP_OK) {
        fprintf(stderr, "%s: write_wdsfd_waitrsp failure, ret %d\n",__func__,rtn );
        return rtn;
    }

    rtn = unpack_wds_SLQSStopDataSession(g_qmi_rsp[wdsFdIndex], g_rspLen[wdsFdIndex],&resp);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr, "%s: unpack failed, ret %d\n",__func__,rtn);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    return eLITE_CONNECT_APP_OK;
}

/*
 * Name:     stop_current_datasession
 *
 * Purpose:  Stop the ongoing data session
 *
 * Params:   None
 *
 * Return:   None
 *
 * Notes:    None
 */
void stop_current_datasession()
{
    pack_wds_SLQSStopDataSession_t stopSession;
    unpack_dms_GetModelID_t modelId;
    int rtn;

    memset(&stopSession,0, sizeof(stopSession));
    memset(&modelId, 0, sizeof(modelId));


    if ((g_sessionv4 == FALSE) && (g_sessionv6 == FALSE)) {
        fprintf( stderr, "no data session active currently!\n\n");
        return;
    }

    if (g_sessionv4)
    {
        stopSession.psid = &g_sessionIdv4;
        rtn = stop_data_session(&stopSession, eLITE_WDS_FD_ARR_INDEX_0);

        if (rtn != eLITE_CONNECT_APP_OK) {
            #ifdef DBG
            fprintf( stderr, "data session v4 not stopped\n\n");
            #endif
        }
        else
        {
            fprintf( stderr, "data session v4 stopped\n\n");
            g_sessionv4 = FALSE;
        }
        if (get_number_of_qmap_supported() >= 2)
        {
            if (g_ipfamilypreference == IPv4_FAMILY_PREFERENCE ||
                g_ipfamilypreference == IPv4v6_FAMILY_PREFERENCE)
            {
                struct QmuxIPTable ipTable;

                memset(&ipTable, 0, sizeof(ipTable));
                ipTable.muxID = 0x80;
                ipTable.ipAddress = 0x00;
                rtn = ioctl(wds_fd[eLITE_WDS_FD_ARR_INDEX_0], QMI_SET_QMAP_IP_TABLE, (void *)&ipTable);
                if(rtn < 0)
                {
                   printf("QMI_SET_QMAP_IP_TABLE error!\n");
                }
            }
        }
    }
    if (g_sessionv6)
    {
        stopSession.psid = &g_sessionIdv6;
        rtn = stop_data_session(&stopSession, eLITE_WDS_FD_ARR_INDEX_1);

        if (rtn != eLITE_CONNECT_APP_OK) {
            #ifdef DBG
            fprintf( stderr, "data session v6 not stopped\n\n");
            #endif
        }
        else
        {
            fprintf( stderr, "data session v6 stopped\n\n");
            g_sessionv6 = FALSE;
        }
        if (get_number_of_qmap_supported() >= 2)
        {
            if (g_ipfamilypreference == IPv6_FAMILY_PREFERENCE ||
                g_ipfamilypreference == IPv4v6_FAMILY_PREFERENCE)
            {
                QmuxIPv6Table ipTable;

                memset(&ipTable, 0, sizeof(ipTable));
                ipTable.muxID = 0x80;
                ipTable.prefix_len = 0;
                rtn = ioctl(wds_fd[eLITE_WDS_FD_ARR_INDEX_1], QMI_SET_QMAP_IPV6_TABLE, (void *)&ipTable);
                if(rtn < 0)
                {
                   printf("QMI_SET_QMAP_IPV6_TABLE error!\n");
                   return;
                }
            }
        }
    }
}


/*************************************************************************
 * Option 2 : Start LTE Data Session
 ************************************************************************/

static int start_data_session(
    pack_wds_SLQSStartDataSession_t *pSession,
    unpack_wds_SLQSStartDataSession_t *pSessionOut,
    int wdsFdIndex)
{
    pack_qmi_t req_ctx;
    unpack_qmi_t rsp_ctx;
    uint16_t reqLen;
    int rtn;
    struct timespec times;
    long long timeout = QMI_CMD_TIMEOUT_SEC;

    if ((!pSession) || (!pSessionOut))
    {
        fprintf (stderr, "%s: invalid arg\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    VALIDATE_WDS_FD_INDEX(wdsFdIndex)
    SETUP_REQRSP_CTX_WDS_FD(wdsFdIndex, req_ctx, rsp_ctx)

    clock_gettime(CLOCK_REALTIME, &times);
    times.tv_sec += timeout;

    if(pack_wds_SLQSStartDataSession(&req_ctx, g_qmi_req[wdsFdIndex], &reqLen, pSession) != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr,"pack_wds_SLQSStartDataSession error!\n");
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    rtn = write_wdsfd_waitrsp(wdsFdIndex, reqLen, times);
    if (rtn != eLITE_CONNECT_APP_OK) {
        fprintf(stderr, "%s: write_wdsfd_waitrsp failure, ret %d\n",__func__,rtn );
        return rtn;
    }

    rtn = unpack_wds_SLQSStartDataSession(g_qmi_rsp[wdsFdIndex], g_rspLen[wdsFdIndex], pSessionOut);
    if(rtn == eQCWWAN_ERR_NONE)
    {
        if(wdsFdIndex==eLITE_WDS_FD_ARR_INDEX_0)
        {
            g_sessionIdv4 = *(pSessionOut->psid);
        }
        else if(wdsFdIndex==eLITE_WDS_FD_ARR_INDEX_1)
        {
            g_sessionIdv6 = *(pSessionOut->psid);
        }
    }
    else if (rtn == eQCWWAN_ERR_QMI_NO_EFFECT)
    {
        #ifdef DBG
        fprintf(stderr, "%s: already started, ret %d\n",__func__,rtn);
        #endif
    }
    else
    {
        fprintf(stderr, "%s: unpack failed, ret %d\n",__func__,rtn);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    return eLITE_CONNECT_APP_OK;
}

static int set_IP_family(
    pack_wds_SLQSSetIPFamilyPreference_t *pIPfamily,
    unpack_wds_SLQSSetIPFamilyPreference_t *pIPfamilyOut,
    int wdsFdIndex
    )
{
    pack_qmi_t req_ctx;
    unpack_qmi_t rsp_ctx;
    uint16_t reqLen;
    int rtn;
    struct timespec times;
    long long timeout = QMI_CMD_TIMEOUT_SEC;

    if ((!pIPfamily) || (!pIPfamilyOut))
    {
        fprintf (stderr, "%s: invalid arg\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    VALIDATE_WDS_FD_INDEX(wdsFdIndex)
    SETUP_REQRSP_CTX_WDS_FD(wdsFdIndex, req_ctx, rsp_ctx)

    clock_gettime(CLOCK_REALTIME, &times);
    times.tv_sec += timeout;

    if(pack_wds_SLQSSetIPFamilyPreference(&req_ctx, g_qmi_req[wdsFdIndex], &reqLen, pIPfamily) != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr,"pack_wds_SLQSSetIPFamilyPreference error!\n");
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    rtn = write_wdsfd_waitrsp(wdsFdIndex, reqLen, times);
    if (rtn != eLITE_CONNECT_APP_OK) {
        fprintf(stderr, "%s: write_wdsfd_waitrsp failure, ret %d\n",__func__,rtn );
        return rtn;
    }

    rtn = unpack_wds_SLQSSetIPFamilyPreference(g_qmi_rsp[wdsFdIndex], g_rspLen[wdsFdIndex], pIPfamilyOut);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr, "%s: unpack failed, ret %d\n",__func__,rtn);
        return eLITE_CONNECT_APP_ERR_QMI;
    }
    return eLITE_CONNECT_APP_OK;
}

static void bind_IPFamilies()
{
   pack_wds_SLQSSetIPFamilyPreference_t setIPfamily;
    unpack_wds_SLQSSetIPFamilyPreference_t setIPfamilyOut;
    int rtn;
    int i =0 ;
    for (i=0;i<2;i++)
    {
       memset(&setIPfamily,0, sizeof(setIPfamily));
       memset(&setIPfamilyOut,0, sizeof(setIPfamilyOut));

       if (get_number_of_qmap_supported() >= 2)
       {
           if (set_mux_id(0x80, i ) != eLITE_CONNECT_APP_OK)
           {
               fprintf(stderr, "set mux id failure!\n");
           }
       }

       setIPfamily.IPFamilyPreference = i==eLITE_WDS_FD_ARR_INDEX_1 ? IPv6_FAMILY_PREFERENCE : IPv4_FAMILY_PREFERENCE;
       rtn = set_IP_family(&setIPfamily, &setIPfamilyOut, i);
       if (rtn != eLITE_CONNECT_APP_OK)
       {
           #ifdef DBG
           fprintf( stderr, "setIPFamily preference failed\n"\
                    "Failure cause - %d\n", rtn);
           #endif
       }
    }
}

static void start_common_datasessionv4(
    pack_wds_SLQSStartDataSession_t *pSession,
    unpack_wds_SLQSStartDataSession_t *pSessionOut)
{
    int rtn;

    rtn = start_data_session(pSession, pSessionOut, eLITE_WDS_FD_ARR_INDEX_0);

    if (rtn != eLITE_CONNECT_APP_OK)
    {
        g_sessionv4 = FALSE;
        #ifdef DBG
        fprintf( stderr, "failed to start v4 data session\n"\
                 "Failure cause - %d\nError Code - %u\n\n",
                 rtn, *pSessionOut->pFailureReason );
        #endif
    }
    else
    {
        fprintf( stderr, "data session v4 started successfully\n");
        g_sessionv4 = TRUE;
        update_user_display( eCALL_STATUS, "CONNECTED" );
    }
}

static void start_common_datasessionv6(
    pack_wds_SLQSStartDataSession_t *pSession,
    unpack_wds_SLQSStartDataSession_t *pSessionOut)
{
    int rtn;

    rtn = start_data_session(pSession, pSessionOut, eLITE_WDS_FD_ARR_INDEX_1);

    if (rtn != eLITE_CONNECT_APP_OK)
    {
        g_sessionv6 = FALSE;
        #ifdef DBG
        fprintf( stderr, "failed to start v6 data session\n"\
                 "Failure cause - %d\nError Code - %u\n\n",
                 rtn, *pSessionOut->pFailureReason );
        #endif
    }
    else
    {
        fprintf( stderr, "data session v6 started successfully\n");
        g_sessionv6 = TRUE;
        update_user_display( eCALL_STATUS, "CONNECTED" );
    }
}


/*
 * Name:     StartLteCdmaDataSession
 *
 * Purpose:  Starts a LTE or CDMA Data Session
 *
 * Params:   isCdma - TRUE for CDMA connection
 *
 * Return:   None
 *
 * Notes:    None
 */
void start_lte_cdma_datasession(uint8_t isCdma)
{
    pack_wds_SLQSStartDataSession_t startSession;
    unpack_wds_SLQSStartDataSession_t startSessionOut;
    unpack_dms_GetModelID_t modelId;
    uint8_t                       technology = (isCdma) ? TECHNOLOGY_3GPP2: TECHNOLOGY_3GPP;
    uint32_t                      profileId3gpp;
    uint8_t                       profileIdMatch = FALSE;
    uint32_t                      failReason = 0;
    uint8_t                       idx = 0;
    uint32_t                      sessionIdv4 = 0;
    uint32_t                      sessionIdv6 = 0;

    /* If connected device is GOBI, return after displaying an error message
     * as LTE data call is not supported on GOBI devices.
     */
    memset(&startSession,0, sizeof(startSession));
    memset(&startSessionOut,0, sizeof(startSessionOut));

    /* If connected device is GOBI, return after displaying an error message
     * as this data call is not supported on GOBI devices.
     */
    get_model_id(&modelId);

    if (strstr(modelId.modelid, "MC83"))
    {
        fprintf( stderr, "LTE/CDMA Data call is not supported on this device!!!\n" );
        return;
    }

    if ( (0 == strcmp("SL9090", modelId.modelid) ||\
          0 == strcmp("MC9090", modelId.modelid)) && (1 == isCdma))
    {

        /* Fill the information for required data session, for SL9090, it only supports mono PDN,
           hence, do not consider mutiple PDN in this case */
        startSession.pTech = &technology;
        startSessionOut.psid = &sessionIdv4;
        startSessionOut.pFailureReason = &failReason;

        fprintf( stderr, "start data session for SL/MC9090\n");
        start_common_datasessionv4(&startSession, &startSessionOut);
    }
    else
    {
        /* handle data connection for NON-SL9090 modules */
        /* Display all the profiles stored on the device */
        display_all_profiles();

        if( 0 == indexInfo.totalProfilesOnDevice )
        {
            fprintf( stderr, "No Profile exist on the device for Data session\n"
                         "or check device connectivity\n\n");
            return;
        }
        while(1)
        {
            /* Get the profile id using which the data session needs to be started */
            profileId3gpp  = GetUserProfileId();

           /* If only <ENTER> is pressed by the user, return to main menu */
           if( ENTER_KEY_PRESSED == profileId3gpp )
               return;

            /* If the user has enter an invalid profile id */
            for( idx = 0; idx < indexInfo.totalProfilesOnDevice; idx++ )
            {
                if( profileId3gpp == indexInfo.profileIndex[idx] )
                {
                    profileIdMatch = TRUE;
                    break;
                }
            }

            if( profileIdMatch == TRUE )
            {
                break;
            }
        }   

        g_ipfamilypreference = GetIPFamilyPreference();
        if( ENTER_KEY_PRESSED == g_ipfamilypreference )
            g_ipfamilypreference = IPv4_FAMILY_PREFERENCE;

        if (isCdma)
        {
            fprintf( stderr, "Start CDMA data session for Profile id: %u\n",profileId3gpp);
            profileId3gpp += CDMA_PROFILE_OFFSET;
            startSession.pprofileid3gpp2 = &profileId3gpp;
        }
        else
        {
            fprintf( stderr, "Start LTE data session for Profile id: %u\n",profileId3gpp);
            startSession.pprofileid3gpp = &profileId3gpp;
        }

        startSessionOut.pFailureReason = &failReason;

        if ((g_ipfamilypreference == IPv4_FAMILY_PREFERENCE) ||
           (g_ipfamilypreference == IPv4v6_FAMILY_PREFERENCE))
        {

            startSessionOut.psid = &sessionIdv4;
            start_common_datasessionv4(&startSession, &startSessionOut);
         }

        if ((g_ipfamilypreference == IPv6_FAMILY_PREFERENCE) ||
           (g_ipfamilypreference == IPv4v6_FAMILY_PREFERENCE))
        {
            failReason = 0;
            startSessionOut.psid = &sessionIdv6;
            start_common_datasessionv6(&startSession, &startSessionOut);
        }
    }
}


/*************************************************************************
 * Option 1 : Start UMTS Data Session
 ************************************************************************/
/*
 * Name:     start_UMTS_data_session
 *
 * Purpose:  Starts a UMTS Data Session
 *
 * Params:   None
 *
 * Return:   None
 *
 * Notes:    None
 */
void start_UMTS_datasession()
{
    pack_wds_SLQSStartDataSession_t startSession;
    unpack_wds_SLQSStartDataSession_t startSessionOut;
    uint32_t                       profileId3gpp;
    uint8_t                        profileIdMatch = FALSE;
    uint8_t                        idx = 0;
    uint32_t                       failReason = 0;
    uint32_t                       sessionIdv4 = 0;
    uint32_t                       sessionIdv6 = 0;

    /* Display all the profiles stored on the device */
    display_all_profiles();

    if( 0 == indexInfo.totalProfilesOnDevice )
    {
        fprintf( stderr, "No Profile exist on the device for Data session\n"
                         "or check device connectivity\n\n");
        return;
    }

    memset(&startSession,0, sizeof(startSession));
    memset(&startSessionOut,0, sizeof(startSessionOut));

    while(1)
    {
        /* Get the profile id using which the data session needs to be started */
        profileId3gpp  = GetUserProfileId();

        /* If only <ENTER> is pressed by the user, return to main menu */
        if( ENTER_KEY_PRESSED == profileId3gpp )
            return;

        /* If the user has enter an invalid profile id */
        for( idx = 0; idx < indexInfo.totalProfilesOnDevice; idx++ )
        {
            if( profileId3gpp == indexInfo.profileIndex[idx] )
            {
                profileIdMatch = TRUE;
                break;
            }
        }

        if( profileIdMatch == TRUE )
        {
            break;
        }
    }

    g_ipfamilypreference = GetIPFamilyPreference();
    if( ENTER_KEY_PRESSED == g_ipfamilypreference )
        g_ipfamilypreference = IPv4_FAMILY_PREFERENCE;

    startSession.pprofileid3gpp = &profileId3gpp;
    startSessionOut.pFailureReason = &failReason;
    fprintf( stderr, "Start UMTS data session for Profile id: %u\n",profileId3gpp);

    if ((g_ipfamilypreference == IPv4_FAMILY_PREFERENCE) ||
       (g_ipfamilypreference == IPv4v6_FAMILY_PREFERENCE))
    {
        startSessionOut.psid = &sessionIdv4;
        start_common_datasessionv4(&startSession, &startSessionOut);

    }

    if ((g_ipfamilypreference == IPv6_FAMILY_PREFERENCE) ||
       (g_ipfamilypreference == IPv4v6_FAMILY_PREFERENCE))
    {
        failReason = 0;
        startSessionOut.psid = &sessionIdv6;
        start_common_datasessionv6(&startSession, &startSessionOut);
    }
}


/*
 * Name:     start_cdma_datasession_RUIM
 *
 * Purpose:  Starts a CDMA Data Session for RUIM Firmware
 *
 * Params:   None
 *
 * Return:   None
 *
 * Notes:    None
 */
void start_cdma_datasession_RUIM()
{
    pack_wds_SLQSStartDataSession_t startSession;
    unpack_wds_SLQSStartDataSession_t startSessionOut;
    unpack_dms_GetModelID_t modelId;
    uint8_t                        technology = TECHNOLOGY_3GPP2;  
    uint32_t                       failReason = 0;
    uint32_t                       sessionIdv4 = 0;
    uint32_t                       sessionIdv6 = 0;

    memset(&startSession,0, sizeof(startSession));
    memset(&startSessionOut,0, sizeof(startSessionOut));

    /* If connected device is GOBI, return after displaying an error message
     * as this data call is not supported on GOBI devices.
     */
    get_model_id(&modelId);

    if (strstr(modelId.modelid, "MC83"))
    {
        fprintf( stderr, "This CDMA Data call is not supported on this device!!!\n" );
        return;
    }

    g_ipfamilypreference = GetIPFamilyPreference();
    if( ENTER_KEY_PRESSED == g_ipfamilypreference )
        g_ipfamilypreference = IPv4_FAMILY_PREFERENCE;

    startSession.pTech = &technology;
    startSessionOut.pFailureReason = &failReason;
    fprintf( stderr, "Start CDMA/RUIM data session\n");

    if ((g_ipfamilypreference == IPv4_FAMILY_PREFERENCE) ||
       (g_ipfamilypreference == IPv4v6_FAMILY_PREFERENCE))
    {
        startSessionOut.psid = &sessionIdv4;
        start_common_datasessionv4(&startSession, &startSessionOut);
    }

    if ((g_ipfamilypreference == IPv6_FAMILY_PREFERENCE) ||
       (g_ipfamilypreference == IPv4v6_FAMILY_PREFERENCE))
    {
        startSessionOut.psid = &sessionIdv6;
        start_common_datasessionv6(&startSession, &startSessionOut);
    }
}

/*
 * Name:     display_device_info
 *
 * Purpose:  Display the information about the connected device
 *
 * Params:   None
 *
 * Return:   None
 *
 * Notes:    None
 */
static void display_device_info()
{
    int rtn;
    unpack_dms_GetModelID_t modelId;
    unpack_nas_GetHomeNetwork_t homeNw;
    char  *pNAString = "UNAVAILABLE";
    
    memset( &modelId, 0, sizeof(unpack_dms_GetModelID_t));

    /* Get the Device Model ID */
    rtn = get_model_id(&modelId);
    if( rtn != eLITE_CONNECT_APP_OK)
    {
        #ifdef DBG
        fprintf( stderr, "Failed to get model ID\n" );
        #endif
        update_user_display( eMODEL_ID, pNAString );
    }
    else
    {
        /* Update the model ID field of the user window */
        update_user_display( eMODEL_ID, modelId.modelid );
    }

    memset( &homeNw, 0, sizeof(unpack_nas_GetHomeNetwork));

    /* Get the Home Network */
    rtn = get_home_network(&homeNw);
    if( rtn != eLITE_CONNECT_APP_OK)
    {
        #ifdef DBG
        fprintf( stderr, "Failed to get home network\n" );
        #endif
        update_user_display( eHOME_NETWORK, pNAString );
    }
    else
    {
        /* Update the model ID field of the user window */
        update_user_display( eHOME_NETWORK, homeNw.name );
    }
}

void *wds_read_thread(void* ptr)
{
    unpack_qmi_t rsp_ctx;
    uint8_t rsp[QMI_MSG_MAX];
    uint16_t rspLen = 0;
    int rtn = 0;
    int fdIndex;
    int rc = -1;
    if (!ptr)
    {
        fprintf(stderr, "wds_read_thread input arg NULL\n");
        return NULL;
    }
    fdIndex = *((int*)ptr);

    if ( (fdIndex != eLITE_WDS_FD_ARR_INDEX_0) &&
         (fdIndex != eLITE_WDS_FD_ARR_INDEX_1) )
    {
        fprintf(stderr, "wds_read_thread IP%s input arg invalid\n",GET_IP_VERSION(fdIndex));
        return NULL;
    }
    while(1)
    {
        if (wds_fd[fdIndex] < 0)
            break;
        rtn = read(wds_fd[fdIndex], rsp, QMI_MSG_MAX);
        if (rtn > 0)
        {
            rspLen = (uint16_t)rtn;
            helper_get_resp_ctx(eWDS, rsp, rspLen, &rsp_ctx);
            #ifdef DBG
            printf("<< receiving QMI WDS for wds_read_thread%s\n",GET_IP_VERSION(fdIndex));
            PrintHex(rsp,rspLen);

            if (rsp_ctx.type == eIND)
                printf("WDS IND for wds_read_thread%s: ",GET_IP_VERSION(fdIndex));
            else if (rsp_ctx.type == eRSP)
                printf("WDS RSP for wds_read_thread%s: ",GET_IP_VERSION(fdIndex));
            printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
            #endif
            switch(rsp_ctx.msgid)
            {
                case eQMI_WDS_START_NET:
                case eQMI_WDS_STOP_NET:
                case eQMI_WDS_SET_IP_FAMILY:
                case eQMI_WDS_GET_PROFILE:
                case eQMI_WDS_CREATE_PROFILE:
                case eQMI_WDS_MODIFY_PROFILE:
                case eQMI_WDS_DELETE_PROFILE:
                case eQMI_WDS_GET_SETTINGS:
                case eQMI_WDS_SET_QMUX_ID:
                    /* if this response is request by the waiting main thread */
                    if (rsp_ctx.xid == g_xid[fdIndex])
                    {

                        /* 
                         || g_read_success is used to inform main thread that valid 
                         || response is received and ignore duplicate response as well
                         */
                        if (!g_read_success[fdIndex])
                        {
                            g_read_success[fdIndex] = true;
                            memcpy (g_qmi_rsp[fdIndex],rsp, rspLen);
                            g_rspLen[fdIndex] = rspLen;
                        }
                        else
                        {
                           fprintf(stderr, "!g_read_success IP%s \n",GET_IP_VERSION(fdIndex));
                           break;
                        }

                        #ifdef DBG
                        fprintf(stderr, "pthread_cond_signal IP%s \n",GET_IP_VERSION(fdIndex));
                        #endif
                        rc = pthread_cond_signal(&con_cond[fdIndex]);
                        if(rc)
                        {
                           fprintf(stderr, "pthread_cond_signal IP%s \n",GET_IP_VERSION(fdIndex));
                           break;
                        }
                        rc = pthread_mutex_trylock(&(con_mutex[fdIndex]));
                        if (!rc)//Prevent Dead lock
                        {
                           rc = pthread_mutex_unlock(&(con_mutex[fdIndex]));
                           if(rc)
                           {
                              #ifdef DBG
                              fprintf(stderr, "pthread_mutex_unlock IP%s \n",GET_IP_VERSION(fdIndex));
                              #endif
                              break;
                           }
                        }
                        else
                        {
                           #ifdef DBG
                           fprintf(stderr, "%s: mutex already unlocked %s\n",__func__,GET_IP_VERSION(fdIndex));
                           #endif
                        }
                    }
                    break;
                // TODO: Handle the indications here
                default:
                    break;
            }
        }
        else
        {
           /* break the thread */
           #ifdef DBG
           fprintf(stderr,"\nwds read thread IP%s error\n",GET_IP_VERSION(fdIndex));
           #endif
           break;
        }
    }
    return NULL;
}

static int get_runtime_setting(
    unpack_wds_SLQSGetRuntimeSettings_t *pRuntime, int wdsFdIndex)
{
    pack_qmi_t req_ctx;
    unpack_qmi_t rsp_ctx;
    uint16_t reqLen;
    int rtn;
    struct timespec times;
    uint32_t reqSettings = 0xFFFF;
    pack_wds_SLQSGetRuntimeSettings_t reqarg;
    long long timeout = QMI_CMD_TIMEOUT_SEC;

    if (!pRuntime)
    {
        fprintf (stderr, "%s: invalid arg\n", __func__);
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    VALIDATE_WDS_FD_INDEX(wdsFdIndex)
    SETUP_REQRSP_CTX_WDS_FD(wdsFdIndex, req_ctx, rsp_ctx)

    clock_gettime(CLOCK_REALTIME, &times);
    times.tv_sec += timeout;

    reqarg.pReqSettings = &reqSettings;
    if(pack_wds_SLQSGetRuntimeSettings(&req_ctx, g_qmi_req[wdsFdIndex], &reqLen, &reqarg) != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr,"pack_wds_SLQSGetRuntimeSettings error!\n");
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    rtn = write_wdsfd_waitrsp(wdsFdIndex, reqLen, times);
    if (rtn != eLITE_CONNECT_APP_OK) {
        fprintf(stderr, "%s: write_wdsfd_waitrsp failure, ret %d\n",__func__,rtn );
        return rtn;
    }

    rtn = unpack_wds_SLQSGetRuntimeSettings(g_qmi_rsp[wdsFdIndex], g_rspLen[wdsFdIndex], pRuntime);
    if(rtn != eQCWWAN_ERR_NONE)
    {
        fprintf(stderr, "%s: unpack failed, ret %d\n",__func__,rtn);
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    if (get_number_of_qmap_supported() >= 2)
    {
        if (wdsFdIndex == eLITE_WDS_FD_ARR_INDEX_0)
        {
            struct QmuxIPTable ipTable;

            ipTable.muxID = 0x80;
            ipTable.ipAddress = pRuntime->IPv4;
            rtn = ioctl(wds_fd[wdsFdIndex], QMI_SET_QMAP_IP_TABLE, (void *)&ipTable);
        }

        else if (wdsFdIndex == eLITE_WDS_FD_ARR_INDEX_1)
        {
            QmuxIPv6Table ipTable;
            int idx=0;

            ipTable.muxID = 0x80;
            ipTable.prefix_len = pRuntime->IPV6AddrInfo.IPV6PrefixLen;
            for(idx=0;idx<8;idx++)
            {
                uint8_t tipv6addr[2]={0};
                memcpy(&tipv6addr,&pRuntime->IPV6AddrInfo.IPAddressV6[idx],sizeof(uint16_t));
                if(u8toU16(tipv6addr)==pRuntime->IPV6AddrInfo.IPAddressV6[idx])
                {
                    ipTable.ipv6addr[idx*2] = tipv6addr[1];
                    ipTable.ipv6addr[idx*2+1] = tipv6addr[0];
                }
                else
                {
                    ipTable.ipv6addr[idx*2] = tipv6addr[0];
                    ipTable.ipv6addr[idx*2+1] = tipv6addr[1];
                }
            }
            rtn = ioctl(wds_fd[wdsFdIndex], QMI_SET_QMAP_IPV6_TABLE, (void *)&ipTable);
        }
    }

    if(rtn < 0)
    {
        fprintf(stderr, "set qmap ip table error!\n");
        return eLITE_CONNECT_APP_ERR_QMI;
    }

    return eLITE_CONNECT_APP_OK;
}

static void print_runtime_info(unpack_wds_SLQSGetRuntimeSettings_t runtime)
{
    struct in_addr ip_addr;
    struct in_addr gw_addr;
    struct in_addr subnet;
    int idx=0;
    int8_t count;
    char  bufPrimaryDNS[MAX_FIELD_SIZE];
    char  bufSecondaryDNS[MAX_FIELD_SIZE];

    printf("runtime setting :\n");
    swi_uint256_print_mask (runtime.ParamPresenceMask);
    ip_addr.s_addr = htonl(runtime.IPv4);
    gw_addr.s_addr = htonl(runtime.GWAddressV4);
    subnet.s_addr = htonl(runtime.SubnetMaskV4);
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 30))
    {
        printf("\tip address: %s\n", inet_ntoa(ip_addr));
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 32))
    {
        printf("\tgateway addr %s\n", inet_ntoa(gw_addr));
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 33))
    {
        printf("\tsubnet mask %s\n", inet_ntoa(subnet));
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 16))
    {
        printf("\tprofile name %s\n",runtime.ProfileName);
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 17))
    {
        printf("\tpdp type %d\n",runtime.PDPType);
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 21))
    {
        memset( bufPrimaryDNS, 0, MAX_FIELD_SIZE );
        IPUlongToDot(runtime.PrimaryDNSV4, bufPrimaryDNS);
        printf("\tPrimaryDNSV4   : %s\n",bufPrimaryDNS);
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 22))
    {
        memset( bufSecondaryDNS, 0, MAX_FIELD_SIZE );
        IPUlongToDot(runtime.SecondaryDNSV4, bufSecondaryDNS);
        printf("\tSecondaryDNSV4 : %s\n",bufSecondaryDNS);
    }

    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 23))
    {
        printf("UMTS Granted QoS Parameters\n");
        printf("\ttrafficClass       : %d\n", runtime.UMTSGrantedQoS.trafficClass);
        printf("\tmaxUplinkBitrate   : %x\n",runtime.UMTSGrantedQoS.maxUplinkBitrate);
        printf("\tmaxDownlinkBitrate : %x\n",runtime.UMTSGrantedQoS.maxDownlinkBitrate);
        printf("\tgrntUplinkBitrate  : %x\n",runtime.UMTSGrantedQoS.grntUplinkBitrate);
        printf("\tgrntDownlinkBitrate: %x\n",runtime.UMTSGrantedQoS.grntDownlinkBitrate);
        printf("\tqosDeliveryOrder   : %d\n",runtime.UMTSGrantedQoS.qosDeliveryOrder);
        printf("\tmaxSDUSize         : %x\n",runtime.UMTSGrantedQoS.maxSDUSize);
        printf("\tsduErrorRatio      : %d\n",runtime.UMTSGrantedQoS.sduErrorRatio);
        printf("\tresBerRatio        : %d\n",runtime.UMTSGrantedQoS.resBerRatio);
        printf("\tdeliveryErrSDU     : %d\n",runtime.UMTSGrantedQoS.deliveryErrSDU);
        printf("\ttransferDelay      : %x\n",runtime.UMTSGrantedQoS.transferDelay);
        printf("\ttrafficPriority    : %x\n",runtime.UMTSGrantedQoS.trafficPriority);
    }

    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 25))
    {
        printf("GPRS Granted QoS Parameters\n");
        printf("\tprecedenceClass     : %x\n",runtime.GPRSGrantedQoS.precedenceClass);
        printf("\tdelayClass          : %x\n",runtime.GPRSGrantedQoS.delayClass);
        printf("\treliabilityClass    : %x\n",runtime.GPRSGrantedQoS.reliabilityClass);
        printf("\tpeakThroughputClass : %x\n",runtime.GPRSGrantedQoS.peakThroughputClass);
        printf("\tmeanThroughputClass : %x\n",runtime.GPRSGrantedQoS.meanThroughputClass);
    }

    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 27))
    {
        printf("Username       : %s\n",runtime.Username);
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 29))
    {
        printf("Authentication : %x\n",runtime.Authentication);
    }

    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 20))
    {
        printf("\tapn name %s\n",runtime.APNName);
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 17))
    {
        printf("\tProfile ID:\n\t\tIndex:%d, ProfileType:%d\n",runtime.ProfileID.profileIndex,runtime.ProfileID.profileType);
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 43))
    {
        printf("\tIP Family Preference : %d\n",runtime.IPFamilyPreference);
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 44))
    {
        printf("\tIMCNflag       : %d\n",runtime.IMCNflag);
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 45))
    {
        printf("\tTechnology     : %d\n",runtime.Technology);
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 41))
    {
        printf("\tMTU : %d\n",runtime.Mtu);
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 34))
    {
        printf("PCSCFAddrPCO   : %d\n",runtime.PCSCFAddrPCO);
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 35))
    {
        printf("IPV4 Server AddressList \n");
        printf("\tPCSCF Server AddressList Count %d\n",runtime.ServerAddrList.numInstances);
        for ( idx = 0; idx < runtime.ServerAddrList.numInstances; idx++ )
            printf("\tPCSCF Server Address[%d] %x\n",
                        idx,runtime.ServerAddrList.pscsfIPv4Addr[idx] );
    }

    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 36))
    {
        printf("PCSCFFQDNAddressList \n");
        printf("\tPCSCFFQDNAddressList Count %d\n",runtime.PCSCFFQDNAddrList.numInstances);
        for ( idx = 0;
              idx < runtime.PCSCFFQDNAddrList.numInstances ;
              idx++ )
        {
            printf("\tPCSCFFQDNAddressLength[%d]  %d\n",
                        idx,runtime.PCSCFFQDNAddrList.pcsfFQDNAddress[idx].fqdnLen );
            printf("\tPCSCFFQDNAddress[%d]        %s\n",
                        idx,runtime.PCSCFFQDNAddrList.pcsfFQDNAddress[idx].fqdnAddr );
        }
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 42))
    {
        printf("DomainNameList \n");
        printf("\tDomainNameList Count %d\n",runtime.DomainList.numInstances);

        for ( idx = 0; idx < runtime.DomainList.numInstances; idx++ )
        {
            printf("\tDomainLen[%d]   %d\n",
                        idx, runtime.DomainList.domain[idx].domainLen);
            printf("\tDomainName[%d]  %s\n",
                        idx,runtime.DomainList.domain[idx].domainName);
        }
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 39))
    {
        printf("\tPrimary DNS V6   : ");
        for (idx = 0; idx < 8; idx++)
            printf("%hx ", runtime.PrimaryDNSV6[idx] );
        printf("\n");
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 40))
    {
        printf("\tSecondary DNS V6 : ");
        for (idx = 0; idx < 8; idx++)
            printf("%hx ", runtime.SecondaryDNSV6[idx] );
        printf("\n");
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 37))
    {
        printf("\tIPAddressV6 Information\n");
        printf("\t\tIPAddressV6 Address   :");
        for (idx = 0; idx < 8; idx++)
            printf("%hx ",
                         runtime.IPV6AddrInfo.IPAddressV6[idx] );
        printf( "\n");
        printf("\t\tIPAddressV6 Length    : %d\n",
                    runtime.IPV6AddrInfo.IPV6PrefixLen );
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 38))
    {
        printf("\tIPV6 Gateway Address Information\n");
        printf("\t\tIPV6 Gateway Address  :");
        for (idx = 0; idx < 8; idx++)
            printf("%hx ",
                        runtime.IPV6GWAddrInfo.gwAddressV6[idx] );
        printf( "\n");
        printf("\t\tIPV6 Gateway Address Length    : %d\n",
                    runtime.IPV6GWAddrInfo.gwV6PrefixLen );
        printf("\n");
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 37))
    {
        printf("\tIPAddressV6 Address    ");
        for (count = 0; count < 8; count++)
            printf("%hx:",
                 runtime.IPV6AddrInfo.IPAddressV6[count] );
        printf("\b \b\n");
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 38))
    {
        printf("\tIPV6 Gateway Address   ");
        for (count = 0; count < 8; count++)
            printf("%hx:",
                    runtime.IPV6GWAddrInfo.gwAddressV6[count] );
        printf("\b \b\n");
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 39))
    {
        printf("\tIPV6 Primary DNS       ");
        for (count = 0; count < 8; count++)
            printf("%hx:",
                    runtime.PrimaryDNSV6[count] );
        printf("\b \b\n");
    }
    if(swi_uint256_get_bit (runtime.ParamPresenceMask, 40))
    {
        printf("\tIPV6 Secondary DNS     ");
        for (count = 0; count < 8; count++)
            printf("%hx:",
                    runtime.SecondaryDNSV6[count] );
        printf("\b \b\n");
    }
}

static void display_runtime_info(void)
{
    unpack_wds_SLQSGetRuntimeSettings_t runtime;

    memset(&runtime, 0, sizeof(runtime));
    if ((g_sessionv4 == FALSE) && (g_sessionv6 == FALSE))
    {
        fprintf(stderr, "No data session\n");
        return;
    }

    if (g_sessionv4)
    {
        if (get_runtime_setting(&runtime, eLITE_WDS_FD_ARR_INDEX_0) == eLITE_CONNECT_APP_ERR_QMI)
        {
            fprintf(stderr, "get runtime setting v4 failure!\n");
        }
        fprintf(stderr, "\nget runtime setting v4 :: \n");
        print_runtime_info(runtime);
    }

    if (g_sessionv6)
    {
        if (get_runtime_setting(&runtime, eLITE_WDS_FD_ARR_INDEX_1) == eLITE_CONNECT_APP_ERR_QMI)
        {
            fprintf(stderr, "\nget runtime setting v6 failure!\n");
            return;
        }
        fprintf(stderr, "get runtime setting v6 :: \n");
        print_runtime_info(runtime);
    }
}


/*
 * Name:     main
 *
 * Purpose:  Entry point of the application
 *
 * Params:   None
 *
 * Return:   EXIT_SUCCESS, EXIT_FAILURE on unexpected error
 *
 * Notes:    None
 */
int main( int argc, const char *argv[])
{
    char selOption[OPTION_LEN];
    char *pEndOfLine = NULL;
    unsigned int len = 0;
    uint32_t userOption;
    int ret;  
    pthread_attr_t wds_attr;
    pthread_t wds_tid[2] = {0,0};
    int wdsFdIndex0;
    int wdsFdIndex1;
    int iNum;

    UNUSEDPARAM(argc);
    if ( argc >= 2)
    {
        if (argv[1])
        {
            g_modem_index = atoi(argv[1]);
        }
    }

    /* Initialize the output log file */
    initialize_display();

    /* Being here means, device is connected, update the required field */
    update_user_display( eDEVICE_STATE, "DEVICE CONNECTED" );

    /* Display the information about the connected device */
    display_device_info();

    /* open wds descriptor for IPv4 */
    wds_fd[eLITE_WDS_FD_ARR_INDEX_0] = client_fd(eWDS);

    if ((wds_fd[eLITE_WDS_FD_ARR_INDEX_0]) < 0)
    {
        fprintf( stderr, "%s: WDS svc could not open for IPv4!\n", __func__ );
        exit(EXIT_FAILURE);
    }

    /* open wds descriptor for IPv6 */
    wds_fd[eLITE_WDS_FD_ARR_INDEX_1] = client_fd(eWDS);
    if ((wds_fd[eLITE_WDS_FD_ARR_INDEX_1]) < 0)
    {
        fprintf( stderr, "%s: WDS svc could not open for IPv6!\n", __func__ );
        close(wds_fd[eLITE_WDS_FD_ARR_INDEX_0]);
        wds_fd[eLITE_WDS_FD_ARR_INDEX_0] = -1;
        exit(EXIT_FAILURE);
    }

    wdsFdIndex0 = eLITE_WDS_FD_ARR_INDEX_0;
    wdsFdIndex1 = eLITE_WDS_FD_ARR_INDEX_1;
    pthread_attr_init(&wds_attr);

    /* create wds read thread */
    if ((pthread_create(&wds_tid[eLITE_WDS_FD_ARR_INDEX_0], &wds_attr, wds_read_thread, &wdsFdIndex0)) < 0) {
        fprintf( stderr, "%s: WDS read thread v4 not created!\n", __func__ );
        close(wds_fd[eLITE_WDS_FD_ARR_INDEX_0]);
        wds_fd[eLITE_WDS_FD_ARR_INDEX_0] = -1;

        close(wds_fd[eLITE_WDS_FD_ARR_INDEX_0]);
        wds_fd[eLITE_WDS_FD_ARR_INDEX_0] = -1;
        exit(EXIT_FAILURE);
    }

    pthread_attr_destroy(&wds_attr);
    usleep(1000);

    /* create wds read thread */
    if ((pthread_create(&wds_tid[eLITE_WDS_FD_ARR_INDEX_1], &wds_attr, wds_read_thread, &wdsFdIndex1)) < 0) {
        fprintf( stderr, "%s: WDS read thread v6 not created!\n", __func__ );
        close(wds_fd[eLITE_WDS_FD_ARR_INDEX_0]);
        wds_fd[eLITE_WDS_FD_ARR_INDEX_0] = -1;

        close(wds_fd[eLITE_WDS_FD_ARR_INDEX_1]);
        wds_fd[eLITE_WDS_FD_ARR_INDEX_1] = -1;
        exit(EXIT_FAILURE);
    }

    pthread_attr_destroy(&wds_attr);
    usleep(1000);

    iNum = get_number_of_qmap_supported();
    fprintf(stderr, "QMAP iNum %d\n",iNum);

    bind_IPFamilies();
    while(1)
    {
        /* Print the menu */
        fprintf( stderr, "\nPlease select one of the following options or press <Enter> to exit:\n"\
                         "1.  Start UMTS Data Session\n"\
                         "2.  Start LTE Data Session\n"\
                         "3.  Start CDMA Data Session\n"\
                         "4.  Start RUIM data session\n"
                         "5.  Stop the currently active Data Session\n"\
                         "6.  Display all the profiles stored on the device\n"\
                         "7.  Display the settings for a particular profile stored on the device\n"\
                         "8.  Create a Profile on the device\n"\
                         "9.  Modify the settings of an existing profile stored on the device\n"\
                         "10. Delete a profile stored on the device\n"\
                         "11. Get runtime setting of data session\n"\
                         "Option : ");

        /* Receive the input from the user */
        fgets( selOption, ( OPTION_LEN ), stdin );

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( selOption, ENTER_KEY );
        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }

        #ifdef DBG
        fprintf( stderr, "Selected Option : %s\n", selOption );
        #endif

        len = strlen( selOption );

        /* If only <ENTER> is pressed by the user quit Application */
        if( ENTER_KEY == selOption[0] )
        {
            break;
        }

        /* check descriptor is still valid or not */
        ret = fcntl(wds_fd[eLITE_WDS_FD_ARR_INDEX_0], F_GETFD);
        if (ret < 0)
        {
            fprintf( stderr, "Device seems disconnected, exiting application!\n");
            break;
        }

        /* check descriptor is still valid or not */
        ret = fcntl(wds_fd[eLITE_WDS_FD_ARR_INDEX_1], F_GETFD);
        if (ret < 0)
        {
            fprintf( stderr, "Device seems disconnected, exiting application!\n");
            break;
        }

        /* Convert the option added by user into integer */
        selOption[ len - 1 ] = '\0';
        userOption = atoi( selOption );

        /* Process user input */
        switch( userOption )
        {
            case eSTART_UMTS_DATA_SESSION:
                start_UMTS_datasession();
                break;

            case eSTART_LTE_DATA_SESSION:
                start_lte_cdma_datasession(0);
                break;

            case eSTART_CDMA_DATA_SESSION:
                start_lte_cdma_datasession(1);
                break;

            case eSTART_CDMA_RUIM_DATASESSION:
                start_cdma_datasession_RUIM();
                break;

            case eSTOP_DATA_SESSION:
                stop_current_datasession();
                break;

            case eDISPLAY_ALL_PROFILES:
                display_all_profile();
                break;

            case eDISPLAY_SINGLE_PROFILE:
                display_profile_info();
                break;

            case eCREATE_PROFILE:
                create_profile();
                break;

            case eMODIFY_PROFILE_SETTINGS:
                modify_profile_settings();
                break;

            case eDELETE_PROFILE:
                delete_profile_from_device();
                break;

            case eGET_RUNTIME_SETTINGS:
                display_runtime_info();
                break;

            default:
                break;
        }
    }

    close(wds_fd[eLITE_WDS_FD_ARR_INDEX_0]);
    wds_fd[eLITE_WDS_FD_ARR_INDEX_0] = -1;

    close(wds_fd[eLITE_WDS_FD_ARR_INDEX_1]);
    wds_fd[eLITE_WDS_FD_ARR_INDEX_1] = -1;

    /* Wait for wds read thread termination */
    pthread_join(wds_tid[eLITE_WDS_FD_ARR_INDEX_0], NULL);
    pthread_join(wds_tid[eLITE_WDS_FD_ARR_INDEX_1], NULL);
    return 0;
}
