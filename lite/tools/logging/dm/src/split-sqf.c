//utility split SQF filter by command tail (0x7e) then send to DM port
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <errno.h>

#define MAX_FILTER_SZ (10*1024*1024)
#define MAX_RESP_SZ (1024)
#define TAIL (0x7e)
#define HEAD (0x73)
#define MIN_LENGH_SQF_MASK (0x0C)
#define VER_LEN (3)
#define OPERATION_GET_LOG_MASK 3

//#define DEBUG_ENABLE

uint32_t bytesToU32(uint8_t *bytes)
{
    uint32_t result;
    int i =0;
    result = bytes[i] | (uint32_t)bytes[i+1] << 8
        | (uint32_t)bytes[i+2] << 16 | (uint32_t)bytes[i+3] << 24;
    return result;
}

void hexdumplogmask(int len,uint8_t *buff,int mode)
{
    int i =0;
    for(i = 0; i < len; i++)
    {
        printf("0x%02x ",buff[i]);
        if((mode==0) && (i == 7))
        {
            printf("\n\t");
        }
        else if((mode == 1) && (i == 11))
        {
            printf("\n\t");
        }
    }
    printf("\n");
}

int PrintLogRequestMaskResp(int len,uint8_t *buff)
{
    uint8_t CMD_CODE=0;
    uint32_t CMD_OPERTAQION=0xffffffff;
    uint32_t CMD_RESULT=0xffffffff;
    int iCMDIndex=0;

    CMD_CODE = buff[0];
    if(CMD_CODE == HEAD)
    {
        if(len < MIN_LENGH_SQF_MASK)
        {
            return 0 ;
        }
        CMD_OPERTAQION = bytesToU32(&buff[4]);
        CMD_RESULT = bytesToU32(&buff[8]);
        if(CMD_OPERTAQION == OPERATION_GET_LOG_MASK)
        {
#ifdef DEBUG_ENABLE
            printf("\n-------------Set Log Mask For Read-------------\n");
            hexdumplogmask(len,buff,1);
#endif
            if(CMD_RESULT != 0)
            {
                printf("\n Set Log Mask For Read failed, Result: %d---------\n",CMD_RESULT);

                /*Set Mask Failed, return with error */
                return 1;
            }
        }
        else
        {
#ifdef DEBUG_ENABLE
            printf("\n---------OP For Write %d, Result: %d------------\n",CMD_OPERTAQION,CMD_RESULT);
#endif
        }
    }
    return 0;
}

int PrintLogRequestMaskReq(int len,uint8_t *buff)
{
    uint8_t CMD_CODE=0;
    uint32_t CMD_OPERTAQION=0xffffffff;
    int iCMDIndex=0;
    CMD_CODE=buff[0];
    if(CMD_CODE == HEAD)
    {
        if(len < MIN_LENGH_SQF_MASK)
        {
            return 0 ;
        }

        CMD_OPERTAQION = bytesToU32(&buff[4]);
        if(CMD_OPERTAQION == OPERATION_GET_LOG_MASK)
        {
#ifdef DEBUG_ENABLE
            printf("\n-------------Set Log Mask For Write-------------\n");
            hexdumplogmask(len,buff,0);
#endif
        }
        else
        {
#ifdef DEBUG_ENABLE
            printf("\n-------------OP For Write %d-------------\n",CMD_OPERTAQION);
#endif
        }
    }
    return 0;
}

int parse_dump_buffer(int len,uint8_t *buff,int mode)
{
    int i = 0;
    int start = 0;
    int ret = 0;
    for(i = 0; i < len; i++)
    {
        if(mode == 3)
        {
#ifdef DEBUG_ENABLE
            printf("0x%02x ",buff[i]);
#endif
        }
        if(buff[i] == TAIL)
        {
            if(mode == 1)
            {
                ret = PrintLogRequestMaskResp(len-start, &buff[start]);
                if (ret != 0)
                    break;
            }
            else if(mode==0)
            {
                ret = PrintLogRequestMaskReq(len-start, &buff[start]);
                if (ret != 0)
                    break;
            }
            start = i++;
        }
    }
    return ret;
}

void set_tty_raw_mode(int fd)
{
    /* Reconfigure the serial port */
    struct termios newtio;
    memset(&newtio, 0, sizeof(newtio));
    newtio.c_cflag = CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;

    /* Select the read() termination parameters as follows:
     * Intercharacter timeout = 1/10th of a second.
     * Minimum number of characters read = 1
     *
     * This is to be interpreted as follows:
     * The intercharacter timer is not started until the
     * first character is received. Then after VTIME 1/10ths
     * of a second, if no additional characters have been
     * received, return what we've got so far. The read()
     * call will also return when a minimum of VMIN chars
     * have been received.
     */
    newtio.c_cc[VTIME]    = 1;  /* inter-character timer */
    newtio.c_cc[VMIN]     = 1; /* blocking read until n chars received */

    tcflush(fd, TCIFLUSH);

    if( tcsetattr(fd, TCSANOW, &newtio) < 0 )
    {
        printf("USB tcsetattr error: %d", errno);
    }
}

int
main(int argc, char** argv) {

    int olen = 0;
    int ilen;
    int ret = 0;
    uint8_t *start, *stop;
    int osize;
    int tty = open(argv[2], O_RDWR );
    if (tty == -1)
    {
        exit(1);
    }
    FILE* filter_fd = fopen(argv[1], "rb");
    if (filter_fd == NULL)
    {
        close(tty);
        exit(1);
    }

    uint8_t *buf = malloc(MAX_FILTER_SZ);
    if (buf == NULL)
    {
        fclose(filter_fd);
        close(tty);
        exit(1);
    }

    uint8_t *resp = malloc(MAX_RESP_SZ);
    if (resp == NULL)
    {
        free(buf);
        fclose(filter_fd);
        close(tty);
        exit(1);
    }

    osize = fread(buf, 1, MAX_FILTER_SZ, filter_fd);
    if(osize == 0)
    {
        free(buf);
        fclose(filter_fd);
        close(tty);
        exit(1);
    }

    set_tty_raw_mode(tty);

    start = buf + VER_LEN;
    stop = start;
    while( (stop - start) < osize) {
        if ( *stop != TAIL) {
            stop++;
        } else {
            olen = write(tty, start, stop-start+1);
            if (olen == 0)
                break;
            ret = parse_dump_buffer(olen,start,0);
            if (ret)
                break;
            ilen = read(tty, resp, MAX_RESP_SZ);
            ret = parse_dump_buffer(ilen,resp,1);
            if (ret)
                break;
            //printf("olen, ilen: %d, %d\n", olen, ilen);
            start = stop+1;
            stop = start;
        }
    }

    free(resp);
    free(buf);
    fclose(filter_fd);
    close(tty);
    if((olen == 0) || (ret != 0))
    {
        exit(1);
    }
    return 0;
}
