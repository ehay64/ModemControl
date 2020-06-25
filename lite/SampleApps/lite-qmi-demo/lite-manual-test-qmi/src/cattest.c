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
#include "cat.h"

extern void FlushStdinStream();
void doexecute_lite_cat_test(testitem_t *pCatTestParams);
extern unsigned g_xid;
int cat_fd = -1;
extern testitem_t totestcat[];
extern unsigned int catarraylen;

void display_cat_test_list()
{
    unsigned int i;
    char testid[MAX_TEST_ID_INPUT_SIZE] = {0};
    char test[MAX_TEST_ID_INPUT_SIZE + 1] = {0};

    printf("\n\t==================================");
    printf("\tCard Application Toolkit (cat) APIs");
    printf("\t====================================\n\n");
    printf("Test id API name\n");
    printf("======= ========\n");

    for (i = 0; i < catarraylen; i++)
    {
        strcpy(test,"t");
        sprintf(testid, "%u", i);
        strcat(test,testid);
        printf ("%4s\t%s \n", test, &totestcat[i].unpack_func_name[7]);
    }
}
void run_cat_tests()
{
    char testid[MAX_TEST_ID_INPUT_SIZE];
    char *pEndOfLine = NULL;
    unsigned int len;
    unsigned int i;
    unsigned int id;
    int bInvalid;
    
    display_cat_test_list();

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
        
        if (id >= catarraylen) {
            printf("\nTest case id is not valid \n");
            continue;
        }

        /* Invoke the selected API function */
        printf ("Invoking test: %s\n", &totestcat[id].unpack_func_name[7]);
        doexecute_lite_cat_test(&totestcat[id]);            
    }
}

void *cat_test_read_thread(void* ptr)
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
        if (cat_fd < 0)
            break;
        rtn = read(cat_fd, rsp, QMI_MSG_MAX);
        if (rtn > 0)
        {
            rspLen = (uint16_t)rtn;
            helper_get_resp_ctx(eCAT, rsp, rspLen, &rsp_ctx);
            #ifdef QMI_DBG      
            int j;
            printf("<< receiving QMI cat \n");
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
                printf("cat IND: ");
            else if (rsp_ctx.type == eRSP)
                printf("cat RSP: ");
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
           fprintf(stderr,"cat read thread error\n");
           #endif
           break;
        }
    }
    return NULL;
}

void doexecute_lite_cat_test(testitem_t *pCatTestParams)
{
    pthread_attr_t cat_attr;
    pthread_t cat_tid = 0;
    unpack_qmi_t rsp_ctx;
    int rtn;
    pack_qmi_t req_ctx;
    uint8_t rsp[QMI_MSG_MAX];
    uint8_t req[QMI_MSG_MAX];
    uint16_t reqLen;

    /* open cat descriptor */
    cat_fd = client_fd(eCAT);

    if (cat_fd < 0) {
        fprintf( stderr, "%s: cat svc could not open!\n", __func__ );
        return;
    }

    pthread_attr_init(&cat_attr);

    /* create cat read thread */
    if ((pthread_create(&cat_tid, &cat_attr, cat_test_read_thread, (void*)pCatTestParams)) < 0) {
        fprintf( stderr, "%s: cat read thread not created!\n", __func__ );
        close(cat_fd);
        cat_fd = -1;
        pthread_attr_destroy(&cat_attr);
        return;
    }

    pthread_attr_destroy(&cat_attr);

    memset(req,0,QMI_MSG_MAX);
    memset(rsp,0,QMI_MSG_MAX);
    memset(&req_ctx, 0, sizeof(req_ctx));
    memset(&rsp_ctx, 0, sizeof(rsp_ctx));
    g_xid++;
    req_ctx.xid = g_xid;  

    if (pCatTestParams->pack) { 
        rtn = run_pack_item(pCatTestParams->pack)(&req_ctx, req, &reqLen,pCatTestParams->pack_ptr);
        if(rtn !=eQCWWAN_ERR_NONE)
        {
            close(cat_fd);
            cat_fd = -1;
            fprintf (stderr, "packing function %s returned error %d\n", 
                      pCatTestParams->pack_func_name,
                      rtn);
            goto finish;
        }
    } else {
        close(cat_fd);
        cat_fd = -1;
        #ifdef TEST_DBG
        fprintf (stderr, "%s: no pack function to test\n", __func__);
        #endif
        goto finish;
    }
    rtn = write(cat_fd, req, reqLen);
    if (rtn!=reqLen)
    {
       close(cat_fd);
       cat_fd = -1;
       #ifdef TEST_DBG
       fprintf(stderr,"error, write %d wrote %d\n", reqLen, rtn);
       #endif
    }

finish:
    /* Wait for cat read thread termination */
    pthread_join(cat_tid, NULL);
    if (cat_fd > 0) {
        close(cat_fd);
        cat_fd = -1;
    }
    return;
}




