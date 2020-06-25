#define __STDC_FORMAT_MACROS
#include <pthread.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <errno.h>
#include "packingdemo.h"
#include "wds.h"

extern void FlushStdinStream();
void doexecute_lite_wds_test(testitem_t *pWdsTestParams);
extern unsigned g_xid;
int wds_fd = -1;
extern testitem_t wdstotest[];
extern unsigned int wdsarraylen;

void display_wds_test_list()
{
    unsigned int i;
    char testid[MAX_TEST_ID_INPUT_SIZE] = {0};
    char test[MAX_TEST_ID_INPUT_SIZE + 1] = {0};

    printf("\n\t==================================");
    printf("\tWireless Data Service (wds) APIs");
    printf("\t====================================\n\n");
    printf("Test id API name\n");
    printf("======= ========\n");

    for (i = 0; i < wdsarraylen; i++)
    {
        strcpy(test,"t");
        sprintf(testid, "%u", i);
        strcat(test,testid);
        printf ("%4s\t%s \n", test, &wdstotest[i].unpack_func_name[7]);
    }
}
void run_wds_tests()
{
    char testid[MAX_TEST_ID_INPUT_SIZE];
    char *pEndOfLine = NULL;
    unsigned int len;
    unsigned int i;
    unsigned int id;
    int bInvalid;
    
    display_wds_test_list();

    while(1) {
        bInvalid = 0;
        id = 0;
        printf("\n\nEnter the test id and enter key to execute or just press enter key to quit: ");

        memset(testid, 0, MAX_TEST_ID_INPUT_SIZE);
        /* Receive the input from the user */
        fgets( testid, MAX_TEST_ID_INPUT_SIZE, stdin );

        /* If '/n' character is not read, there are more characters in input
         * stream. Clear the input stream.
         */
        pEndOfLine = strchr( testid, ENTER_KEY );
        if( NULL == pEndOfLine )
        {
            FlushStdinStream();
        }

        /* If only <ENTER> is pressed by the user, return to main menu */
        if( ENTER_KEY == testid[0] )
           break;

        len= strlen(testid);
        testid[len-1] = '\0';

        if ((testid[0] != 't') || (testid[1] == '\0')) {
             printf("\nTest case id is not valid \n");
             continue;
        }

        for (i = 1; i < strlen(testid); i++) {
            if ((testid[i] < 0x30) || (testid[i] > 0x39)) {
                bInvalid = 1;
                break;
            }
            id = (id*10) + (testid[i] - 0x30);            
        }
        if (bInvalid) {
            printf("\nTest case id is not valid \n");
            continue;
        }
        if (id >= wdsarraylen) {
            printf("\nTest case id is not valid \n");
            continue;
        }

        /* Invoke the selected API function */
        printf ("Invoking test: %s\n", &wdstotest[id].unpack_func_name[7]);
        doexecute_lite_wds_test(&wdstotest[id]);            
    }
}

void *wds_test_read_thread(void* ptr)
{
    testitem_t *pTestParams = (testitem_t *)ptr;
    unpack_qmi_t rsp_ctx;
    uint8_t rsp[QMI_MSG_MAX];
    uint16_t rspLen = 0;
    int rtn = 0;

    if (!pTestParams)
        return NULL;

    while(1)
    {
        if (wds_fd < 0)
            break;
        rtn = read(wds_fd, rsp, QMI_MSG_MAX);
        if (rtn > 0)
        {
            rspLen = (uint16_t)rtn;
            helper_get_resp_ctx(eWDS, rsp, rspLen, &rsp_ctx);
            #ifdef QMI_DBG      
            int j;
            printf("<< receiving QMI wds \n");
            for (j = 0; j < rspLen; j++)
            {
                printf("%02x ", rsp[j]);
                if ((j+1)%16 == 0)
                {
                    printf("\n");
                }
            }
            printf("\n");
            if (rsp_ctx.type == eIND)
                printf("wds IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("wds RSP: ");
            printf("msgid 0x%x, type:%x\n", rsp_ctx.msgid,rsp_ctx.type);
            #endif

            if (rsp_ctx.type == eRSP) {
                if (rsp_ctx.xid == g_xid) {
                    if (pTestParams->unpack) {
                        rtn  = run_unpack_item(pTestParams->unpack)(rsp, 
                                       rspLen, 
                                       pTestParams->unpack_ptr);
                                          
                        if(rtn !=eQCWWAN_ERR_NONE) {
                            fprintf (stderr, "unpack function %s returned error %d\n", 
                                pTestParams->unpack_func_name, rtn);
                            break;
                        }
                        if( pTestParams->dump)
                            pTestParams->dump(pTestParams->unpack_ptr);
                        else {
                            #ifdef TEST_DBG
                            fprintf (stderr, "no dump function\n");
                            #endif
                        }
                        break;
                    } else {
                        #ifdef TEST_DBG
                        fprintf (stderr, "no unpack function\n");
                        #endif
                        break;
                    }
                } else {
                    /* continue read */
                    #ifdef TEST_DBG
                    fprintf (stderr, "invalid response, tx id not matched\n");
                    #endif
                    continue;
                }                            
            } else {
                /* continue read */
                #ifdef TEST_DBG
                fprintf (stderr, "not a response, indication or something else\n");
                #endif
                continue;
            }
        }else
        {
           /* break the thread */
           #ifdef TEST_DBG
           fprintf(stderr,"wds read thread error\n");
           #endif
           break;
        }
    }
    return NULL;
}

void doexecute_lite_wds_test(testitem_t *pWdsTestParams)
{
    pthread_attr_t wds_attr;
    pthread_t wds_tid = 0;
    unpack_qmi_t rsp_ctx;
    int rtn;
    pack_qmi_t req_ctx;
    uint8_t rsp[QMI_MSG_MAX];
    uint8_t req[QMI_MSG_MAX];
    uint16_t reqLen;

    /* open wds descriptor */
    wds_fd = client_fd(eWDS);

    if (wds_fd < 0) {
        fprintf( stderr, "%s: wds svc could not open!\n", __func__ );
        return;
    }

    pthread_attr_init(&wds_attr);

    /* create wds read thread */
    if ((pthread_create(&wds_tid, &wds_attr, wds_test_read_thread, (void*)pWdsTestParams)) < 0) {
        fprintf( stderr, "%s: wds read thread not created!\n", __func__ );
        close(wds_fd);
        wds_fd = -1;
        pthread_attr_destroy(&wds_attr);
        return;
    }

    pthread_attr_destroy(&wds_attr);

    memset(req,0,QMI_MSG_MAX);
    memset(rsp,0,QMI_MSG_MAX);
    memset(&req_ctx, 0, sizeof(req_ctx));
    memset(&rsp_ctx, 0, sizeof(rsp_ctx));
    g_xid++;
    req_ctx.xid = g_xid;  

    if (pWdsTestParams->pack) { 
        rtn = run_pack_item(pWdsTestParams->pack)(&req_ctx, req, &reqLen,pWdsTestParams->pack_ptr);
        if(rtn !=eQCWWAN_ERR_NONE)
        {
            close(wds_fd);
            wds_fd = -1;
            fprintf (stderr, "packing function %s returned error %d\n", 
                      pWdsTestParams->pack_func_name,
                      rtn);
            goto finish;
        }
    } else {
        close(wds_fd);
        wds_fd = -1;
        #ifdef TEST_DBG
        fprintf (stderr, "%s: no pack function to test\n", __func__);
        #endif
        goto finish;
    }
    rtn = write(wds_fd, req, reqLen);
    if (rtn!=reqLen)
    {
       close(wds_fd);
       wds_fd = -1;
       #ifdef TEST_DBG
       fprintf(stderr,"error, write %d wrote %d\n", reqLen, rtn);
       #endif
    }

finish:
    /* Wait for wds read thread termination */
    pthread_join(wds_tid, NULL);
    if (wds_fd > 0) {
        close(wds_fd);
        wds_fd = -1;
    }
    return;
}




