/****************************************************************************
*
* Copyright (c) 2016 Wi-Fi Alliance
*
* Permission to use, copy, modify, and/or distribute this software for any
* purpose with or without fee is hereby granted, provided that the above
* copyright notice and this permission notice appear in all copies.
*
* THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
* WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
* SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
* RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
* NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE
* USE OR PERFORMANCE OF THIS SOFTWARE.
*
*****************************************************************************/


/*
 * File: wfa_ca.c
 *       This is the main program for Control Agent.
 *
 */
#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/select.h>

#include "wfa_debug.h"
#include "wfa_main.h"
#include "wfa_types.h"
#include "wfa_tlv.h"
#include "wfa_tg.h"
#include "wfa_cmds.h"
#include "wfa_miscs.h"
#include "wfa_sock.h"
#include "wfa_ca.h"
#include "wfa_agtctrl.h"
#include "wfa_main.h"

#define WFA_ENV_AGENT_IPADDR "WFA_ENV_AGENT_IPADDR"

extern int xcCmdProcGetVersion(unsigned char *parms);
extern dutCommandRespFuncPtr wfaCmdRespProcFuncTbl[];
extern typeNameStr_t nameStr[];
extern char gRespStr[];

int gSock = -1, tmsockfd, gCaSockfd = -1, xcSockfd, btSockfd;
int gtgSend, gtgRecv, gtgTransac;
char gnetIf[32] = "any";
tgStream_t    *theStreams;
long          itimeout = 0;

unsigned short wfa_defined_debug = WFA_DEBUG_ERR | WFA_DEBUG_WARNING | WFA_DEBUG_INFO;
unsigned short dfd_lvl = WFA_DEBUG_DEFAULT | WFA_DEBUG_ERR | WFA_DEBUG_INFO;

/*
 * the output format can be redefined for file output.
 */
stringconvert(unsigned char *hex_arr, unsigned int hex_arr_sz, unsigned char *str)
{
        int i = 0;
        int j = 0;
        unsigned char ch = 0;
        unsigned char val = 0;
        int len = 0;

        len = strlen(str);

        if (len / 2 > hex_arr_sz) {
                printf("ERROR: %s: String length (%d) greater than array size (%d)\n", __func__, len,
                       hex_arr_sz);
                return -1;
        }

        if (len % 2) {
                printf("ERROR: %s:String length = %d, is not the multiple of 2\n", __func__, len);
                return -1;
        }

        for (i = 0; i < len; i++) {
                /* Convert to lower case */
                ch = ((str[i] >= 'A' && str[i] <= 'Z') ? str[i] + 32 : str[i]);

                if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f')) {
                        printf("ERROR: %s: Invalid hex character in string %d\n", __func__, ch);
                        return -1;
                }

                if (ch >= '0' && ch <= '9') {
                        ch = ch - '0';
                } else {
                        ch = ch - 'a' + 10;
                }

                val += ch;

                if (!(i % 2)) {
                        val <<= 4;
                } else {
                        hex_arr[j] = val;
                        j++;
                        val = 0;
                }
        }

        return j;

}
int main(int argc, char *argv[])
{
    int nfds;
    struct sockaddr_in servAddr;
    unsigned short servPort, myport;
    char *servIP=NULL, *tstr=NULL;
    int bytesRcvd;
    fd_set sockSet;
    char cmdName[WFA_BUFF_32];
    int i, isFound = 0, nbytes, ret_status, slen;
    WORD tag;
    int tmsockfd, cmdLen = WFA_BUFF_1K;
    int maxfdn1;
    BYTE xcCmdBuf[WFA_BUFF_4K];
    BYTE caCmdBuf[WFA_BUFF_4K];
    BYTE pcmdBuf[WFA_BUFF_1K];
    char *pcmdStr = NULL;
    char respStr[WFA_BUFF_512];
    unsigned char dutBuf[WFA_BUFF_4K];
    //start of CLI handling variables
    char wfaCliBuff[128];
    FILE *wfaCliFd;
    FILE *wfadutFd;
    char * cliCmd,*tempCmdBuff;
    if(argc < 3)
        {
            DPRINT_ERR(WFA_ERR, "Usage: %s <control interface> <local control agent port>\n", argv[0]);
            exit(1);
        }

    myport = atoi(argv[2]);

    if(argc > 3)
        {
            if(argc < 5)
                {
                    DPRINT_ERR(WFA_ERR, "Usage: %s <control interface> <local control agent port> <DUT IP ADDRESS> <DUT PORT>\n", argv[0]);
                    exit(1);
                }
            servIP = argv[3];
            if(isIpV4Addr(argv[3])== WFA_FAILURE)
                return WFA_FAILURE;
            if(isNumber(argv[4])== WFA_FAILURE)
                return WFA_FAILURE;
            servPort = atoi(argv[4]);
            if(argc > 5)
                {
                    FILE *logfile;
                    int fd;
                    logfile = fopen(argv[5],"a");
                    if(logfile != NULL)
                        {
                            fd = fileno(logfile);
                            DPRINT_INFO(WFA_OUT,"redirecting the output to %s\n",argv[5]);
                            dup2(fd,1);
                            dup2(fd,2);
                        }
                    else
                        {
                            DPRINT_ERR(WFA_ERR, "Cant open the log file continuing without redirecting\n");
                        }
                }
        }
    else
        {
            if((tstr = getenv("WFA_ENV_AGENT_IPADDR")) == NULL)
                {
                    DPRINT_ERR(WFA_ERR, "Environment variable WFA_ENV_AGENT_IPADDR not set or specify DUT IP/PORT\n");
                    exit(1);
                }
            if(isIpV4Addr(tstr)== WFA_FAILURE)
                return WFA_FAILURE;
            servIP= tstr;
            if((tstr = getenv("WFA_ENV_AGENT_PORT")) == NULL)
                {
                    DPRINT_ERR(WFA_ERR, "Environment variable WFA_ENV_AGENT_PORT not set or specify DUT IP/PORT\n");
                    exit(1);
                }
            if(isNumber(tstr)== WFA_FAILURE)
                return WFA_FAILURE;
            servPort = atoi(tstr);
        }
	printf("In CA servPort is %d\n",servPort);
    tmsockfd = wfaCreateTCPServSock(myport);

    maxfdn1 = tmsockfd + 1;

    FD_ZERO(&sockSet);
    for(;;)
        {
	printf("In CA: line %d........start for loop!\n",__LINE__);
            FD_ZERO(&sockSet);
            FD_SET(tmsockfd, &sockSet);
            maxfdn1 = tmsockfd + 1;

            if(gCaSockfd != -1)
                {
                    FD_SET(gCaSockfd, &sockSet);
                    if(maxfdn1 < gCaSockfd)
                        maxfdn1 = gCaSockfd +1;
                }
            if(gSock != -1)
                {
                    FD_SET(gSock, &sockSet);
                    if(maxfdn1 < gSock)
                        maxfdn1 = gSock +1;
                }
            if((nfds = select(maxfdn1, &sockSet, NULL, NULL, NULL)) < 0)
                {
                    if(errno == EINTR)
                        continue;
                    else
                        DPRINT_WARNING(WFA_WNG, "select error %i", errno);
                }

            DPRINT_INFO(WFA_OUT, "new event \n");
            if(FD_ISSET(tmsockfd, &sockSet))
                {
	printf("In CA: line %d........!\n",__LINE__);
                    gCaSockfd = wfaAcceptTCPConn(tmsockfd);
                    DPRINT_INFO(WFA_OUT, "accept new connection\n");
                    continue;
                }

	printf("In CA: line %d........!\n",__LINE__);
            if(gCaSockfd > 0 && FD_ISSET(gCaSockfd, &sockSet))
                {
	printf("In CA: line %d........!\n",__LINE__);
                    memset(xcCmdBuf, 0, WFA_BUFF_4K);
                    memset(gRespStr, 0, WFA_BUFF_512);

                    nbytes = wfaCtrlRecv(gCaSockfd, xcCmdBuf);
                    if(nbytes <=0)
                        {
	printf("In CA: line %d........!\n",__LINE__);
                            shutdown(gCaSockfd, SHUT_WR);
                            close(gCaSockfd);
                            gCaSockfd = -1;
                            continue;
                        }

                    /*
                     * send back to command line or TM.
                     */
#if 1
	printf("In CA: SOCK 1 .................!\n",__LINE__);
                    memset(respStr, 0, WFA_BUFF_128);
                    sprintf(respStr, "status,RUNNING\r\n");
                    wfaCtrlSend(gCaSockfd, (BYTE *)respStr, strlen(respStr));
	printf("In CA: line %d........!\n",__LINE__);
#endif
                    DPRINT_INFO(WFA_OUT, "%s\n", respStr);
                    DPRINT_INFO(WFA_OUT, "message %s %i\n", xcCmdBuf, nbytes);
                    slen = (int )strlen((char *)xcCmdBuf);

                    DPRINT_INFO(WFA_OUT, "last %x last-1  %x last-2 %x last-3 %x\n", cmdName[slen], cmdName[slen-1], cmdName[slen-2], cmdName[slen-3]);

                    xcCmdBuf[slen-3] = '\0';

	printf("In CA: line %d........!\n",__LINE__);
                    tempCmdBuff=(char* )malloc(sizeof(xcCmdBuf));
                    memcpy(tempCmdBuff,xcCmdBuf,sizeof(xcCmdBuf));
			printf("IN CA tempBUf = %s\n",tempCmdBuff);
		    	int sret;
			unsigned char gCmdStr[WFA_CMD_STR_SZ];
      			sprintf(gCmdStr,"serial_agent 'wfa_dut dut_command \"%s\"'",tempCmdBuff);
    			sret = system(gCmdStr);
			wfadutFd=fopen("/tmp/tembbuff.txt","r");
                    	printf("\nCA :Reading file from tembbuff\n");
                    if(wfadutFd!= NULL)
                        {
				printf("In CA: line %d........!\n",__LINE__);
                            while(fgets(dutBuf, 4096, wfadutFd) != NULL)
                                {
                                    if(ferror(wfadutFd))
                    			printf("\nwfa dut FD fail\n");
                                        break;
                                }
                            fclose(wfadutFd);
			int ret1 = remove("/tmp/tembbuff.txt");
			if(ret1 == 0)
  				printf("File deleted successfully");
    				printf("%s", dutBuf);

                        }
                } /* done with gCaSockfd */
                    memset(caCmdBuf, 0, WFA_BUFF_4K);
		    memcpy(caCmdBuf, (BYTE *)&dutBuf, sizeof(caCmdBuf));
                    memcpy(cmdName, strtok_r((char *)tempCmdBuff, ",", (char **)&pcmdStr), 32);
                    memset(respStr, 0, WFA_BUFF_128);
                    memset(caCmdBuf, 0, WFA_BUFF_4K);
		    stringconvert(caCmdBuf, sizeof(caCmdBuf), dutBuf);
				
    			printf("%s", caCmdBuf);
                    memset(dutBuf, 0, WFA_BUFF_4K);
#if 0
                    for(i = 0; i < bytesRcvd; i++)
                        printf("%02x ", caCmdBuf[i]);
                    printf("\n");
#endif
                    tag = ((wfaTLV *)caCmdBuf)->tag;
                    memcpy(&ret_status, caCmdBuf+4, 4);

                    DPRINT_INFO(WFA_OUT, "tag %i \n", tag);
                    if(tag != 0 && wfaCmdRespProcFuncTbl[tag] != NULL)
                        {
                           wfaCmdRespProcFuncTbl[tag](caCmdBuf);
                        }
                    else
		    {
                        DPRINT_WARNING(WFA_WNG, "function not defined\n");
		    }
	printf("In CA: line %d........!end fror \n",__LINE__);

        } /* for */

	printf("In CA: line %d........! before close gsock\n",__LINE__);
//    close(gSock);
  //  exit(0);
}
