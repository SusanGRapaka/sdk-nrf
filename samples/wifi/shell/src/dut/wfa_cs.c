/****************************************************************************
Copyright (c) 2016 Wi-Fi Alliance.  All Rights Reserved

Permission to use, copy, modify, and/or distribute this software for any purpose with or
without fee is hereby granted, provided that the above copyright notice and this permission
notice appear in all copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

******************************************************************************/

/*
 *   File: wfa_cs.c -- configuration and setup
 *   This file contains all implementation for the dut setup and control
 *   functions, such as network interfaces, ip address and wireless specific
 *   setup with its supplicant.
 *
 *   The current implementation is to show how these functions
 *   should be defined in order to support the Agent Control/Test Manager
 *   control commands. To simplify the current work and avoid any GPL licenses,
 *   the functions mostly invoke shell commands by calling linux system call,
 *   system("<commands>").
 *
 *   It depends on the differnt device and platform, vendors can choice their
 *   own ways to interact its systems, supplicants and process these commands
 *   such as using the native APIs.
 *
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <zephyr/posix/sys/socket.h>
#include <icmpv4.h>
#include <arpa/inet.h>
#include <zephyr/types.h>
#include <zephyr/net/socket.h>
#include <poll.h>

#include "wfa_portall.h"
#include "wfa_debug.h"
#include "wfa_ver.h"
#include "wfa_main.h"
#include "wfa_types.h"
#include "wfa_ca.h"
#include "wfa_tlv.h"
#include "wfa_sock.h"
#include "wfa_tg.h"
#include "wfa_cmds.h"
#include "wfa_rsp.h"
#include "wfa_utils.h"
#ifdef WFA_WMM_PS_EXT
#include "wfa_wmmps.h"
#endif

#include <src/utils/common.h>
#include <wpa_supplicant/config.h>
#include <wpa_supplicant/wpa_supplicant_i.h>
#define CERTIFICATES_PATH    "/etc/wpa_supplicant"

/* Some device may only support UDP ECHO, activate this line */
//#define WFA_PING_UDP_ECHO_ONLY 1

#define WFA_ENABLED 1
int stmp;
extern int count_seq;
extern struct wpa_global *global;
extern unsigned short wfa_defined_debug;
int wfaExecuteCLI(char *CLI);

/* Since the two definitions are used all over the CA function */
char gCmdStr[WFA_CMD_STR_SZ];
dutCmdResponse_t gGenericResp;
//int wfaTGSetPrio(int sockfd, int tgClass);
void create_apts_msg(int msg, unsigned int txbuf[],int id);

int sret = 0;

extern char chan_buf1[32];
extern char chan_buf2[32];
extern char e2eResults[];

FILE *e2efp = NULL;
int chk_ret_status()
{
    char *ret = getenv(WFA_RET_ENV);

    if(*ret == '1')
        return WFA_SUCCESS;
    else
        return WFA_FAILURE;
}

/*
 * agtCmdProcGetVersion(): response "ca_get_version" command to controller
 *  input:  cmd --- not used
 *          valLen -- not used
 *  output: parms -- a buffer to store the version info response.
 */
int agtCmdProcGetVersion(int len, BYTE *parms, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *getverResp = &gGenericResp;

    DPRINT_INFO(WFA_OUT, "entering agtCmdProcGetVersion ...\n");

    getverResp->status = STATUS_COMPLETE;
    wSTRNCPY(getverResp->cmdru.version, WFA_SYSTEM_VER, WFA_VERNAM_LEN);

    wfaEncodeTLV(WFA_GET_VERSION_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)getverResp, respBuf);

    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);

    return WFA_SUCCESS;
}

/*
 * wfaStaAssociate():
 *    The function is to force the station wireless I/F to re/associate
 *    with the AP.
 */
int wfaStaAssociate(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCommand_t *assoc = (dutCommand_t *)caCmdBuf;
    caStaAssociate_t *setassoc = &assoc->cmdsu.assoc;
    char *ifname = assoc->intf;
    dutCmdResponse_t *staAssocResp = &gGenericResp;
	int ret;
    DPRINT_INFO(WFA_OUT, "entering wfaStaAssociate ...\n");
    /*
     * if bssid appears, station should associate with the specific
     * BSSID AP at its initial association.
     * If it is different to the current associating AP, it will be forced to
     * roam the new AP
     */
    if(assoc->cmdsu.assoc.bssid[0] != '\0')
    {
        /* if (the first association) */
        /* just do initial association to the BSSID */


        /* else (station already associate to an AP) */
        /* Do forced roaming */

    }
    else
    {
        /* use 'ifconfig' command to bring down the interface (linux specific) */
     //	sprintf(gCmdStr, "ifconfig %s down", ifname);
      //	sret = system(gCmdStr);

        /* use 'ifconfig' command to bring up the interface (linux specific) */
      	//sprintf(gCmdStr, "ifconfig %s up", ifname);
        //sret = system(gCmdStr);

        /*
         *  use 'wpa_cli' command to force a 802.11 re/associate
         *  (wpa_supplicant specific)
         */
//sprintf(gCmdStr, "wpa_cli -i%s select_network 0", ifname);
  //     sret = system(gCmdStr);
    }
   //sprintf(gCmdStr, "wpa_cli select_network 0", ifname);
//		printf("\n %s \n", gCmdStr);
  // sret = system(gCmdStr);
   //sleep(2);

    /*
     * Then report back to control PC for completion.
     * This does not have failed/error status. The result only tells
     * a completion.
     */
    int k;
    k=strlen(setassoc->bssid);
    printf("\n%d\n",k);
    if(k!='\0')
    {
	sprintf(gCmdStr, "wpa_cli bssid 0 '\"%s\"'", setassoc->bssid);
	ret = shell_execute_cmd(NULL, gCmdStr);
    	printf("\n %s \n", gCmdStr);
    }
    ret = shell_execute_cmd(NULL, "wpa_cli select_network 0");
    staAssocResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_ASSOCIATE_RESP_TLV, 4, (BYTE *)staAssocResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * wfaStaReAssociate():
 *    The function is to force the station wireless I/F to re/associate
 *    with the AP.
 */
int wfaStaReAssociate(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCommand_t *assoc = (dutCommand_t *)caCmdBuf;
    char *ifname = assoc->intf;
    dutCmdResponse_t *staAssocResp = &gGenericResp;
	 int ret;
    DPRINT_INFO(WFA_OUT, "entering wfaStaAssociate ...\n");
    /*
     * if bssid appears, station should associate with the specific
     * BSSID AP at its initial association.
     * If it is different to the current associating AP, it will be forced to
     * roam the new AP
     */

    if(assoc->cmdsu.assoc.bssid[0] != '\0')
    {
        /* if (the first association) */
        /* just do initial association to the BSSID */


        /* else (station already associate to an AP) */
        /* Do forced roaming */

    }
    else
    {
        /* use 'ifconfig' command to bring down the interface (linux specific) */
        //sprintf(gCmdStr, "ifconfig %s down", ifname);
        //sret = system(gCmdStr);

        /* use 'ifconfig' command to bring up the interface (linux specific) */
        //sprintf(gCmdStr, "ifconfig %s up", ifname);

        /*
         *  use 'wpa_cli' command to force a 802.11 re/associate
         *  (wpa_supplicant specific)
         */
        //sprintf(gCmdStr, "wpa_cli -i%s reassociate", ifname);
        //sret = system(gCmdStr);
    }

	ret = shell_execute_cmd(NULL, "wpa_cli reassociate");
    /*
     * Then report back to control PC for completion.
     * This does not have failed/error status. The result only tells
     * a completion.
     */
    staAssocResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_ASSOCIATE_RESP_TLV, 4, (BYTE *)staAssocResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * wfaStaIsConnected():
 *    The function is to check whether the station's wireless I/F has
 *    already connected to an AP.
 */
int wfaStaIsConnected(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCommand_t *connStat = (dutCommand_t *)caCmdBuf;
    dutCmdResponse_t *staConnectResp = &gGenericResp;
    char *ifname = connStat->intf;
    FILE *tmpfile = NULL;
    char result[32];
    struct wpa_supplicant *wpa_s;
	int ret;
    DPRINT_INFO(WFA_OUT, "Entering isConnected ...\n");

#ifdef WFA_NEW_CLI_FORMAT
    sprintf(gCmdStr, "wfa_chkconnect %s\n", ifname);
    sret = system(gCmdStr);

    if(chk_ret_status() == WFA_SUCCESS)
        staConnectResp->cmdru.connected = 1;
    else
        staConnectResp->cmdru.connected = 0;
#else
    /*
     * use 'wpa_cli' command to check the interface status
     * none, scanning or complete (wpa_supplicant specific)
     */
	ret = shell_execute_cmd(NULL, "wpa_cli status");
	wpa_s = wpa_supplicant_get_iface(global, ifname);
	if (!wpa_s) {
		printf("Unable to find the interface: %s, quitting", ifname);
		return -1;
	}
	ret = os_snprintf(result, 9,"%s",wpa_supplicant_state_txt(wpa_s->wpa_state));
    /*
     * the status is saved in a file.  Open the file and check it.
     */
    if(strncmp(result, "COMPLETE", 9) == 0)
        staConnectResp->cmdru.connected = 1;
    else
        staConnectResp->cmdru.connected = 0;
#endif

    /*
     * Report back the status: Complete or Failed.
     */
    staConnectResp->status = STATUS_COMPLETE;

    wfaEncodeTLV(WFA_STA_IS_CONNECTED_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)staConnectResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);

    return WFA_SUCCESS;
}

/*
 * wfaStaGetIpConfig():
 * This function is to retriev the ip info including
 *     1. dhcp enable
 *     2. ip address
 *     3. mask
 *     4. primary-dns
 *     5. secondary-dns
 *
 *     The current implementation is to use a script to find these information
 *     and store them in a file.
 */
int wfaStaGetIpConfig(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    int slen, ret, i = 0,count = 0;
    dutCommand_t *getIpConf = (dutCommand_t *)caCmdBuf;
    dutCmdResponse_t *ipconfigResp = &gGenericResp;
    char *ifname = getIpConf->intf;
    caStaGetIpConfigResp_t *ifinfo = &ipconfigResp->cmdru.getIfconfig;
	char tmp[30];
	char string_ip[30];
	struct wpa_supplicant *wpa_s;

    DPRINT_INFO(WFA_OUT, "Entering GetIpConfig...\n");
		printf("interface %s\n", ifname);

	struct net_if *iface;
	struct net_if_ipv4 *ipv4;

	iface = net_if_get_by_index(0);
	printf("\nInterface %p \n", iface);
	ipv4 = iface->config.ip.ipv4;
	printf("IPv4 unicast addresses (max %d):\n", NET_IF_MAX_IPV4_ADDR);
	/*
        for (i = 0; ipv4 && i < NET_IF_MAX_IPV4_ADDR; i++) {
                unicast = &ipv4->unicast[i];

                if (!unicast->is_used) {
                        continue;
                }

	printf("IPv4 unicast addresses %s:\n", &unicast->address.in_addr);

        }*/
	wpa_s = wpa_supplicant_get_iface(global, ifname);
        if (!wpa_s) {
                printf("Unable to find the interface: %s, quitting", ifname);
                return -1;
        }
	if (wpa_s->l2 && l2_packet_get_ip_addr(wpa_s->l2, tmp, sizeof(tmp)) >= 0) {
              ret = os_snprintf(string_ip,sizeof(string_ip), "%s", tmp);
		printf("IP ADDRESS :%s\n", string_ip);
	}
            if(string_ip != NULL)
            {
                wSTRNCPY(ifinfo->ipaddr, string_ip,15);

                ifinfo->ipaddr[15]='\0';
            }
            else
                wSTRNCPY(ifinfo->ipaddr, "none", 15);
        
    strcpy(ifinfo->dns[0], "0");
    strcpy(ifinfo->dns[1], "0");



#if 0
    FILE *tmpfd;
    char string[256];
    char *str;

    /*
     * check a script file (the current implementation specific)
     */
    ret = access("/usr/local/sbin/getipconfig.sh", F_OK);
    if(ret == -1)
    {
        ipconfigResp->status = STATUS_ERROR;
        wfaEncodeTLV(WFA_STA_GET_IP_CONFIG_RESP_TLV, 4, (BYTE *)ipconfigResp, respBuf);
        *respLen = WFA_TLV_HDR_LEN + 4;

        DPRINT_ERR(WFA_ERR, "file not exist\n");
        return WFA_FAILURE;

    }

    strcpy(ifinfo->dns[0], "0");
    strcpy(ifinfo->dns[1], "0");

    /*
     * Run the script file "getipconfig.sh" to check the ip status
     * (current implementation  specific).
     * note: "getipconfig.sh" is only defined for the current implementation
     */
    sprintf(gCmdStr, "getipconfig.sh /tmp/ipconfig.txt %s\n", ifname);

    sret = system(gCmdStr);

    /* open the output result and scan/retrieve the info */
    tmpfd = fopen("/tmp/ipconfig.txt", "r+");

    if(tmpfd == NULL)
    {
        ipconfigResp->status = STATUS_ERROR;
        wfaEncodeTLV(WFA_STA_GET_IP_CONFIG_RESP_TLV, 4, (BYTE *)ipconfigResp, respBuf);
        *respLen = WFA_TLV_HDR_LEN + 4;

        DPRINT_ERR(WFA_ERR, "file open failed\n");
        return WFA_FAILURE;
    }

    for(;;)
    {
        if(fgets(string, 256, tmpfd) == NULL)
            break;

        /* check dhcp enabled */
        if(strncmp(string, "dhcpcli", 7) ==0)
        {
            str = strtok(string, "=");
            str = strtok(NULL, "=");
            if(str != NULL)
                ifinfo->isDhcp = 1;
            else
                ifinfo->isDhcp = 0;
        }

        /* find out the ip address */
        if(strncmp(string, "ipaddr", 6) == 0)
        {
            str = strtok(string, "=");
            str = strtok(NULL, " ");
            if(str != NULL)
            {
                wSTRNCPY(ifinfo->ipaddr, str, 15);

                ifinfo->ipaddr[15]='\0';
            }
            else
                wSTRNCPY(ifinfo->ipaddr, "none", 15);
        }

        /* check the mask */
        if(strncmp(string, "mask", 4) == 0)
        {
            char ttstr[16];
            char *ttp = ttstr;

            str = strtok_r(string, "=", &ttp);
            if(*ttp != '\0')
            {
                strcpy(ifinfo->mask, ttp);
                slen = strlen(ifinfo->mask);
                ifinfo->mask[slen-1] = '\0';
            }
            else
                strcpy(ifinfo->mask, "none");
        }

        /* find out the dns server ip address */
        if(strncmp(string, "nameserv", 8) == 0)
        {
            char ttstr[16];
            char *ttp = ttstr;

            str = strtok_r(string, " ", &ttp);
            if(str != NULL && i < 2)
            {
                strcpy(ifinfo->dns[i], ttp);
                slen = strlen(ifinfo->dns[i]);
                ifinfo->dns[i][slen-1] = '\0';
            }
            else
                strcpy(ifinfo->dns[i], "none");

            i++;
        }
    }

    /*
     * Report back the results
     */
    ipconfigResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_GET_IP_CONFIG_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)ipconfigResp, respBuf);

    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);

#if 0
    DPRINT_INFO(WFA_OUT, "%i %i %s %s %s %s %i\n", ipconfigResp->status,
                ifinfo->isDhcp, ifinfo->ipaddr, ifinfo->mask,
                ifinfo->dns[0], ifinfo->dns[1], *respLen);
#endif

    fclose(tmpfd);
#endif
    ipconfigResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_GET_IP_CONFIG_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)ipconfigResp, respBuf);

    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
    return WFA_SUCCESS;
}

/*
 * wfaStaSetIpConfig():
 *   The function is to set the ip configuration to a wireless I/F.
 *   1. IP address
 *   2. Mac address
 *   3. default gateway
 *   4. dns nameserver (pri and sec).
 */
int wfaStaSetIpConfig(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCommand_t *setIpConf = (dutCommand_t *)caCmdBuf;
    caStaSetIpConfig_t *ipconfig = &setIpConf->cmdsu.ipconfig;
    dutCmdResponse_t *staSetIpResp = &gGenericResp;

    DPRINT_INFO(WFA_OUT, "entering wfaStaSetIpConfig ...\n");

    /*
     * Use command 'ifconfig' to configure the interface ip address, mask.
     * (Linux specific).
     *
    *sprintf(gCmdStr, "/sbin/ifconfig %s %s netmask %s > /dev/null 2>&1 ", ipconfig->intf, ipconfig->ipaddr, ipconfig->mask);
     * sret = system(gCmdStr);
     * 
     * use command 'route add' to set set gatewway (linux specific) */
    /* if(ipconfig->defGateway[0] != '\0')
    {
        sprintf(gCmdStr, "/sbin/route add default gw %s > /dev/null 2>&1", ipconfig->defGateway);
        sret = system(gCmdStr);
    }
 */
    /* set dns (linux specific) */
    /* sprintf(gCmdStr, "cp /etc/resolv.conf /tmp/resolv.conf.bk");
    sret = system(gCmdStr);
    sprintf(gCmdStr, "echo nameserv %s > /etc/resolv.conf", ipconfig->pri_dns);
    sret = system(gCmdStr);
    sprintf(gCmdStr, "echo nameserv %s >> /etc/resolv.conf", ipconfig->sec_dns);
     sret = system(gCmdStr);
	*/
    /*
     * report status
     */
    staSetIpResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_IP_CONFIG_RESP_TLV, 4, (BYTE *)staSetIpResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * wfaStaVerifyIpConnection():
 * The function is to verify if the station has IP connection with an AP by
 * send ICMP/pings to the AP.
 */
int wfaStaVerifyIpConnection(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCommand_t *verip = (dutCommand_t *)caCmdBuf;
    dutCmdResponse_t *verifyIpResp = &gGenericResp;

#ifndef WFA_PING_UDP_ECHO_ONLY
    char strout[64], *pcnt;
    FILE *tmpfile;

    DPRINT_INFO(WFA_OUT, "Entering wfaStaVerifyIpConnection ...\n");

    /* set timeout value in case not set */
    if(verip->cmdsu.verifyIp.timeout <= 0)
    {
        verip->cmdsu.verifyIp.timeout = 10;
    }

    /* execute the ping command  and pipe the result to a tmp file */
#if 1
    sprintf(gCmdStr, "ping %s -c 3 -W %u | grep loss | cut -f3 -d, 1>& /tmp/pingout.txt", verip->cmdsu.verifyIp.dipaddr, verip->cmdsu.verifyIp.timeout);
    sret = system(gCmdStr);

    /* scan/check the output */
    tmpfile = fopen("/tmp/pingout.txt", "r+");
    if(tmpfile == NULL)
    {
        verifyIpResp->status = STATUS_ERROR;
        wfaEncodeTLV(WFA_STA_VERIFY_IP_CONNECTION_RESP_TLV, 4, (BYTE *)verifyIpResp, respBuf);
        *respLen = WFA_TLV_HDR_LEN + 4;

        DPRINT_ERR(WFA_ERR, "file open failed\n");
        return WFA_FAILURE;
    }
#endif
    verifyIpResp->status = STATUS_COMPLETE;
    if(fscanf(tmpfile, "%s", strout) == EOF)
        verifyIpResp->cmdru.connected = 0;
    else
    {
        pcnt = strtok(strout, "%");

        /* if the loss rate is 100%, not able to connect */
        if(atoi(pcnt) == 100)
            verifyIpResp->cmdru.connected = 0;
        else
            verifyIpResp->cmdru.connected = 1;
    }

    fclose(tmpfile);
#else
    int btSockfd;
    struct pollfd fds[2];
    int timeout = 2000;
    char anyBuf[64];
    struct sockaddr_in toAddr;
    int done = 1, cnt = 0, ret, nbytes;

    verifyIpResp->status = STATUS_COMPLETE;
    verifyIpResp->cmdru.connected = 0;

    btSockfd = wfaCreateUDPSock("127.0.0.1", WFA_UDP_ECHO_PORT);

    if(btSockfd == -1)
    {
        verifyIpResp->status = STATUS_ERROR;
        wfaEncodeTLV(WFA_STA_VERIFY_IP_CONNECTION_RESP_TLV, 4, (BYTE *)verifyIpResp, respBuf);
        *respLen = WFA_TLV_HDR_LEN + 4;
        return WFA_FAILURE;;
    }

    toAddr.sin_family = AF_INET;
    toAddr.sin_addr.s_addr = inet_addr(verip->cmdsu.verifyIp.dipaddr);
    toAddr.sin_port = htons(WFA_UDP_ECHO_PORT);

    while(done)
    {
        wfaTrafficSendTo(btSockfd, (char *)anyBuf, 64, (struct sockaddr *)&toAddr);
        cnt++;

        fds[0].fd = btSockfd;
        fds[0].events = POLLIN | POLLOUT;

        ret = poll(fds, 1, timeout);
        switch(ret)
        {
        case 0:
            /* it is time out, count a packet lost*/
            break;
        case -1:
        /* it is an error */
        default:
        {
            switch(fds[0].revents)
            {
            case POLLIN:
            case POLLPRI:
            case POLLOUT:
                nbytes = wfaTrafficRecv(btSockfd, (char *)anyBuf, (struct sockaddr *)&toAddr);
                if(nbytes != 0)
                    verifyIpResp->cmdru.connected = 1;
                done = 0;
                break;
            default:
                /* errors but not care */
                ;
            }
        }
        }
        if(cnt == 3)
        {
            done = 0;
        }
    }

#endif

    wfaEncodeTLV(WFA_STA_VERIFY_IP_CONNECTION_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)verifyIpResp, respBuf);

    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);

    return WFA_SUCCESS;
}

/*
 * wfaStaGetMacAddress()
 *    This function is to retrieve the MAC address of a wireless I/F.
 */
int wfaStaGetMacAddress(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
	
    dutCommand_t *getMac = (dutCommand_t *)caCmdBuf;
    dutCmdResponse_t *getmacResp = &gGenericResp;
    char *ifname = getMac->intf;
	int mac_addr_len ;
	int idx = 1, ret;
	static char mac_buf[sizeof("%02x:%02x:%02x:%02x:%02x:%02x")];
    DPRINT_INFO(WFA_OUT, "Entering wfaStaGetMacAddress ...\n");
                
	struct wpa_supplicant *wpa_s;

	wpa_s = wpa_supplicant_get_iface(global, ifname);
	if (!wpa_s) {
		printf("Unable to find the interface: %s, quitting", ifname);
		return -1;
	}
	ret = os_snprintf(mac_buf,sizeof(mac_buf), "" MACSTR "\n",MAC2STR(wpa_s->own_addr));
		printf("***************MAC BUF SUPP = %s\n",mac_buf);

    		printf("%s:MAC ADDRESS mac buf = %s size = %d\n",__func__,mac_buf,sizeof(mac_buf));
    		printf("%s:MAC ADDRESS = %s\n",__func__,getmacResp->cmdru.mac);


	strcpy(getmacResp->cmdru.mac, mac_buf);
        getmacResp->status = STATUS_COMPLETE;



    wfaEncodeTLV(WFA_STA_GET_MAC_ADDRESS_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)getmacResp, respBuf);

    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);

    return WFA_SUCCESS;
}

/*
 * wfaStaGetStats():
 * The function is to retrieve the statistics of the I/F's layer 2 txFrames,
 * rxFrames, txMulticast, rxMulticast, fcsErrors/crc, and txRetries.
 * Currently there is not definition how to use these info.
 */
int wfaStaGetStats(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *statsResp = &gGenericResp;

    /* this is never used, you can skip this call */

    statsResp->status = STATUS_ERROR;
    wfaEncodeTLV(WFA_STA_GET_STATS_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)statsResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);


    return WFA_SUCCESS;
}

/*
 * wfaSetEncryption():
 *   The function is to set the wireless interface with WEP or none.
 *
 *   Since WEP is optional test, current function is only used for
 *   resetting the Security to NONE/Plaintext (OPEN). To test WEP,
 *   this function should be replaced by the next one (wfaSetEncryption1())
 *
 *   Input parameters:
 *     1. I/F
 *     2. ssid
 *     3. encpType - wep or none
 *     Optional:
 *     4. key1
 *     5. key2
 *     6. key3
 *     7. key4
 *     8. activeKey Index
 */

int wfaSetEncryption1(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetEncryption_t *setEncryp = (caStaSetEncryption_t *)caCmdBuf;
    dutCmdResponse_t *setEncrypResp = &gGenericResp;
	int ret;
    /*
     * disable the network first
     */
	ret = shell_execute_cmd(NULL, "wpa_cli add_network 0");
	ret = shell_execute_cmd(NULL, "wpa_cli add disable_network 0");

    /*
     * set SSID
     */
	sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", setEncryp->ssid);
	ret = shell_execute_cmd(NULL, gCmdStr);

    /*
     * Tell the supplicant for infrastructure mode (1)
     */
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 mode 0");

    /*
     * set Key management to NONE (NO WPA) for plaintext or WEP
     */
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt NONE");

     //IMG
	ret = shell_execute_cmd(NULL, "wpa_cli sta_autoconnect 1");
	ret = shell_execute_cmd(NULL, "wpa_cli enable_network 0");


    setEncrypResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_ENCRYPTION_RESP_TLV, 4, (BYTE *)setEncrypResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 *  Since WEP is optional, this function could be used to replace
 *  wfaSetEncryption() if necessary.
 */
int wfaSetEncryption(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetEncryption_t *setEncryp = (caStaSetEncryption_t *)caCmdBuf;
    dutCmdResponse_t *setEncrypResp = &gGenericResp;
    int i,ret;
	
    /*
     * disable the network first
     */

	ret = shell_execute_cmd(NULL, "wpa_cli add_network 0");
	ret = shell_execute_cmd(NULL, "wpa_cli disable_network 0");

    /*
     * set SSID
     */
	sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", setEncryp->ssid);
	ret = shell_execute_cmd(NULL, gCmdStr);


    /*
     * Tell the supplicant for infrastructure mode (1)
     */
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 mode 0");

    /*
     * set Key management to NONE (NO WPA) for plaintext or WEP
     */
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt NONE");

    /* set keys */
    if(setEncryp->encpType == 1)
    {
        for(i=0; i<4; i++)
        {
            if(setEncryp->keys[i][0] != '\0')
            {
		sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", setEncryp->ssid);
		ret = shell_execute_cmd(NULL, gCmdStr);
            }
        }

        /* set active key */
        i = setEncryp->activeKeyIdx;
        if(setEncryp->keys[i][0] != '\0')
        {
		sprintf(gCmdStr, "wpa_cli set_network 0 wep_tx_keyid %i",i, setEncryp->activeKeyIdx);
		ret = shell_execute_cmd(NULL, gCmdStr);
        }
    }
    else /* clearly remove the keys -- reported by p.schwann */
    {

        for(i = 0; i < 4; i++)
        {
            	sprintf(gCmdStr, "wpa_cli set_network 0 wep_key %i ", i);
		ret = shell_execute_cmd(NULL, gCmdStr);
        }
    }

     //IMG
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 scan_ssid 1 ");
	ret = shell_execute_cmd(NULL, "wpa_cli sta_autoconnect 1");
	ret = shell_execute_cmd(NULL, "wpa_cli select_network 0");



    setEncrypResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_ENCRYPTION_RESP_TLV, 4, (BYTE *)setEncrypResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

int wfaStaSetSecurity(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
	dutCommand_t *setSecurity = (dutCommand_t *)caCmdBuf;
	caStaSetSecurity_t *setsec = &setSecurity->cmdsu.setsec;
	dutCmdResponse_t infoResp;
	char *ifname = setSecurity->intf;
	if(ifname[0] == '\0')
	{
		ifname = "wlan0";
		
	}
	printf("\n Entry wfaStaSetSecurity...\n ");

	int ret;

ret = shell_execute_cmd(NULL, "wpa_cli remove_network 0");
ret = shell_execute_cmd(NULL, "wpa_cli add_network 0");


	sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", setsec->ssid);
	ret = shell_execute_cmd(NULL, gCmdStr);

	printf("\n Interface = %s \n",setSecurity->intf );
	printf("\n keyMgmType = %s \n",setsec->keyMgmtType );
	printf("\n certType = %s \n",setsec->certType );
	printf("\n ssid = %s \n",setsec->ssid );
	printf("\n keyMgmtType = %s \n",setsec->keyMgmtType );
	printf("\n encpType = %s \n",setsec->encpType );
	printf("\n ecGroupID = %s \n",setsec->ecGroupID );
	printf("\n type = %d \n",setsec->type );
	printf("\n SEC_TYPE_PSKSAE = %d \n",SEC_TYPE_PSKSAE);
	printf("\n pmf = %d \n",setsec->pmf );
if(setsec->type == SEC_TYPE_PSKSAE)
                {
                        printf("\n IMG DEBUG >>>>>>> IN SEC_TYPE_PSKSAE");
			ret = shell_execute_cmd(NULL, "wpa_cli SAE_PWE 2");
			ret = shell_execute_cmd(NULL, "wpa_cli disable_network 0");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 pairwise CCMP");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 group CCMP");


			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-PSK SAE");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 1");
			sprintf(gCmdStr, "wpa_cli set_network 0 sae_password '\"%s\"'", setsec->secu.passphrase);
			ret = shell_execute_cmd(NULL, gCmdStr);
			sprintf(gCmdStr, "wpa_cli set_network 0 psk '\"%s\"'",  setsec->secu.passphrase);
			ret = shell_execute_cmd(NULL, gCmdStr);
                        sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-PSK SAE", ifname);
			ret = shell_execute_cmd(NULL, gCmdStr);
			ret = shell_execute_cmd(NULL, "wpa_cli enable_network 0");
                   }
	
		if(setsec->type == SEC_TYPE_SAE)
		{
			printf("\n IMG DEBUG >>>>>>> IN SEC_TYPE_SAE");
			ret = shell_execute_cmd(NULL, "wpa_cli SAE_PWE 2");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0  pairwise CCMP");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 group CCMP");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt SAE");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 2");
			sprintf(gCmdStr, "wpa_cli set_network 0 sae_password '\"%s\"'", setsec->secu.passphrase);
			ret = shell_execute_cmd(NULL, gCmdStr);
			ret = shell_execute_cmd(NULL, "wpa_cli enable_network 0");

		}
		else if(setsec->type == SEC_TYPE_PSKSAE)
		{
			printf("\n IMG DEBUG >>>>>>> IN SEC_TYPE_PSKSAE");
			
			ret = shell_execute_cmd(NULL, "wpa_cli SAE_PWE 2");
			ret = shell_execute_cmd(NULL, "wpa_cli disable_network 0");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0  pairwise CCMP");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 group CCMP");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-PSK SAE");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 1");
			sprintf(gCmdStr, "wpa_cli set_network 0 sae_password '\"%s\"'", setsec->secu.passphrase);
			ret = shell_execute_cmd(NULL, gCmdStr);
			sprintf(gCmdStr, "wpa_cli set_network 0 psk '\"%s\"'", setsec->secu.passphrase);
			ret = shell_execute_cmd(NULL, gCmdStr);
			ret = shell_execute_cmd(NULL, "wpa_cli enable_network 0");
	   	}
		else if(setsec->type == SEC_TYPE_PSK)
		{
			printf("\n IMG DEBUG >>>>>>> IN SEC_TYPE_PSK");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-PSK WPA-PSK-SHA256");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 auth_alg OPEN");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 group CCMP");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 pairwise CCMP");
			sprintf(gCmdStr, "wpa_cli set_network 0 ieee80211w %d", setsec->pmf);
			ret = shell_execute_cmd(NULL, gCmdStr);
			sprintf(gCmdStr, "wpa_cli set pmf %d", setsec->pmf);
			ret = shell_execute_cmd(NULL, gCmdStr);
			ret = shell_execute_cmd(NULL, "wpa_cli sta_autoconnect 1");
			sprintf(gCmdStr, "wpa_cli set_network 0 psk '\"%s\"'", setsec->secu.passphrase);
			ret = shell_execute_cmd(NULL, gCmdStr);

                }
		else if(setsec->type == SEC_TYPE_EAPTLS)
		{
			printf("\n IMG DEBUG >>>>>>> IN SEC_TYPE_EAPTLS");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-EAP");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 1");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 eap TLS");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 identity '\"user@example.com\"'");
			sprintf(gCmdStr, "wpa_cli set_network 0 ca_cert '\"/etc/wpa_supplicant/%s\"'", setsec->trustedRootCA);
			ret = shell_execute_cmd(NULL, gCmdStr);
			sprintf(gCmdStr, "wpa_cli set_network 0 client_cert '\"/etc/wpa_supplicant/%s\"'", setsec->clientCertificate);
			ret = shell_execute_cmd(NULL, gCmdStr);
			sprintf(gCmdStr, "wpa_cli set_network 0 private_key '\"/etc/wpa_supplicant/%s\"'", setsec->clientCertificate);
			ret = shell_execute_cmd(NULL, gCmdStr);
		}
		else if(setsec->type == 0)
                {
                        printf("\n IMG DEBUG >>>>>>> IN SEC_TYPE_OPEN");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt NONE");
                }

		if(setsec->ecGroupID[0] != '\0')
		{
			sprintf(gCmdStr, "wpa_cli SET sae_groups %s", setsec->ecGroupID);
			ret = shell_execute_cmd(NULL, gCmdStr);
			printf("\n %s \n", gCmdStr);
		}
	
	if(strcasecmp(setsec->keyMgmtType, "OWE") == 0)	
	{
		printf("\n IMG DEBUG >>>>>>> IN OWE");
		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt OWE");
		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 group CCMP");
		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 pairwise CCMP");
		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 proto RSN");
		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 2");
	}
	else if(strcasecmp(setsec->keyMgmtType, "SuiteB") == 0)	
	{
		printf("\n IMG DEBUG >>>>>>> IN SuiteB");
		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-EAP-SUITE-B-192");
		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 pairwise GCMP-256");
		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 group GCMP-256");
		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 eap TLS");
		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 identity '\"user@example.com\"'");
		sprintf(gCmdStr, "wpa_cli set_network 0 ca_cert '\"/etc/wpa_supplicant/%s\"'", setsec->trustedRootCA);
		ret = shell_execute_cmd(NULL, gCmdStr);
		sprintf(gCmdStr, "wpa_cli set_network 0 client_cert '\"/etc/wpa_supplicant/%s\"'", setsec->clientCertificate);
		ret = shell_execute_cmd(NULL, gCmdStr);
		sprintf(gCmdStr, "wpa_cli set_network 0 private_key '\"/etc/wpa_supplicant/%s\"'", setsec->clientCertificate);
		ret = shell_execute_cmd(NULL, gCmdStr);
		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 group_mgmt BIP-GMAC-256");
		if(strcasecmp(setsec->certType, "ecc") == 0)
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 openssl_ciphers '\"ECDHE-ECDSA-AES256-GCM-SHA384\"'");
		else if(strcasecmp(setsec->certType, "rsa") == 0)
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 openssl_ciphers '\"ECDHE-RSA-AES256-GCM-SHA384:DHE-RSA-AES256-GCM-SHA384\"'");

		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 2");
	}

	else
	{
		 if(setsec->type == 0)
                {
                        printf("\n IMG DEBUG >>>>>>> IN SEC_TYPE_OPEN");
			ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 1");
                }
		
	}
                printf("\n IMG DEBUG >>>>>>> IN STA_AUTO_CONNECT");
		ret = shell_execute_cmd(NULL, "wpa_cli sta_autoconnect 1 ");

		ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 scan_ssid 1 ");

		ret = shell_execute_cmd(NULL, "wpa_cli select_network 0");
	sleep(2);
	infoResp.status = STATUS_COMPLETE;
	wfaEncodeTLV(WFA_STA_SET_SECURITY_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
	*respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

	return WFA_SUCCESS;
}

/*
 * wfaStaSetEapTLS():
 *   This is to set
 *   1. ssid
 *   2. encrypType - tkip or aes-ccmp
 *   3. keyManagementType - wpa or wpa2
 *   4. trustedRootCA
 *   5. clientCertificate
 */
int wfaStaSetEapTLS(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetEapTLS_t *setTLS = (caStaSetEapTLS_t *)caCmdBuf;
    char *ifname = setTLS->intf;
    dutCmdResponse_t *setEapTlsResp = &gGenericResp;
    int ret;
    DPRINT_INFO(WFA_OUT, "Entering wfaStaSetEapTLS ...\n");
    DPRINT_INFO(WFA_OUT, " <><><><><><><><><><> IMG DEBUG <><><><><><><><><><>\n");

    /*
     * need to store the trustedROOTCA and clientCertificate into a file first.
     */
#ifdef WFA_NEW_CLI_FORMAT
	sprintf(gCmdStr, "wfa_set_eaptls -i '\"%s\"' '\"%s\"' '\"%s\"' '\"%s\"'",ifname, setTLS->ssid, setTLS->trustedRootCA, setTLS->clientCertificate);
	ret = shell_execute_cmd(NULL, gCmdStr);
#else

	ret = shell_execute_cmd(NULL, "wpa_cli remove_network 0");
	ret = shell_execute_cmd(NULL, "wpa_cli add_network 0");
	/*ret = shell_execute_cmd(NULL, "wpa_cli disable_network 0");*/
    /* ssid */
    sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", ifname, setTLS->ssid);
    ret = shell_execute_cmd(NULL, gCmdStr);

    /* key management */
    if(strcasecmp(setTLS->keyMgmtType, "wpa2-sha256") == 0)
    {
    }
    else if(strcasecmp(setTLS->keyMgmtType, "wpa2-eap") == 0)
    {
    }
    else if(strcasecmp(setTLS->keyMgmtType, "wpa2-ft") == 0)
    {

    }
    else if(strcasecmp(setTLS->keyMgmtType, "wpa") == 0)
    {
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-EAP");
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 proto WPA");
    }
    else if(strcasecmp(setTLS->keyMgmtType, "wpa2") == 0)
    {
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-EAP");
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 pairwise CCMP");
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 group CCMP");
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 proto WPA2");
 
        // to take all and device to pick any one supported.
    }
    else
    {
        // ??

	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-EAP WPA-EAP-SHA256");
    }

    /* if PMF enable */
    if(setTLS->pmf == WFA_ENABLED || setTLS->pmf == WFA_OPTIONAL)
    {

	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 1");
    }
    else if(setTLS->pmf == WFA_REQUIRED)
    {
 	 ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 2");
    }
    else if(setTLS->pmf == WFA_F_REQUIRED)
    {

 	 ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 2");
   } 
    else if(setTLS->pmf == WFA_F_DISABLED)
    {

    }
    else
    {
        /* Disable PMF */

/*    sprintf(gCmdStr, "wpa_cli set_network 0 iee80211w 0", setTLS->intf);
		printf("\n %s \n", gCmdStr);
   sret = system(gCmdStr);
 */    /* protocol WPA */

     } 
    
    //sprintf(gCmdStr, "wpa_cli set_network 0 proto WPA", ifname);
    //sret = system(gCmdStr);

 	 ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 eap TLS");
 	 ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 identity '\"wifiuser\"'");

 	 ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 password '\"test%11\"'");


 	 ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ca_cert '\"/usr/test/cas.pem\"'");

   /* sprintf(gCmdStr, "wpa_cli set_network 0 private_key '\"/etc/wpa_supplicant/wifiuser.pem\"'", ifname);//IMG EDITED */
 	 ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 private_key '\"/usr/test/wifiuser.pem\"'");

 	 ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 private_key_passwd '\"wifi\"'");

   /* sprintf(gCmdStr, "wpa_cli set_network 0 client_cert '\"/etc/wpa_supplicant/wifiuser.pem\"'", ifname);//IMG EDITED */
 	 ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 client_cert '\"/usr/test/wifiuser.pem\"'");
   
   //IMG
 	 ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 scan_ssid 1 ");

    DPRINT_INFO(WFA_OUT, "Entering sta_autoconnect ...\n");
 	 ret = shell_execute_cmd(NULL, "wpa_cli sta_autoconnect 1");
 	 ret = shell_execute_cmd(NULL, "wpa_cli select_network 0");

   sleep(2);
#endif

    setEapTlsResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_EAPTLS_RESP_TLV, 4, (BYTE *)setEapTlsResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * The function is to set
 *   1. ssid
 *   2. passPhrase
 *   3. keyMangementType - wpa/wpa2
 *   4. encrypType - tkip or aes-ccmp
 */
int wfaStaSetPSK(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    /*Incompleted function*/
    dutCmdResponse_t *setPskResp = &gGenericResp;

	int ret;
#ifndef WFA_PC_CONSOLE
    caStaSetPSK_t *setPSK = (caStaSetPSK_t *)caCmdBuf;
#ifdef WFA_NEW_CLI_FORMAT
    sprintf(gCmdStr, "wfa_set_psk '\"%s\"' '\"%s\"' '\"%s\"'", setPSK->intf, setPSK->ssid, setPSK->passphrase);
    ret = shell_execute_cmd(NULL, gCmdStr);
#else

ret = shell_execute_cmd(NULL, "wpa_cli remove_network 0");
ret = shell_execute_cmd(NULL, "wpa_cli add_network 0");
    


	sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", setPSK->ssid);
	ret = shell_execute_cmd(NULL, gCmdStr);

  if(strcasecmp(setPSK->keyMgmtType, "wpa2-sha256") == 0)
     {
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-PSK WPA-PSK-SHA256");
     }
    else if(strcasecmp(setPSK->keyMgmtType, "wpa2") == 0)
    {
     // take all and device to pick it supported.
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-PSK WPA-PSK-SHA256");
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0  proto WPA2");
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 pairwise CCMP");
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0  group CCMP TKIP");
    }
    else if(strcasecmp(setPSK->keyMgmtType, "wpa2-psk") == 0)
    {
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-PSK WPA-PSK-SHA256");
    }
    else if(strcasecmp(setPSK->keyMgmtType, "wpa2-ft") == 0)
    {

    }
    else if (strcasecmp(setPSK->keyMgmtType, "wpa2-wpa-psk") == 0)
    {

    }
    else
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 key_mgmt WPA-PSK WPA-PSK-SHA256");

	sprintf(gCmdStr, "wpa_cli set_network 0 psk '\"%s\"'", setPSK->passphrase);
	ret = shell_execute_cmd(NULL, gCmdStr);

	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 scan_ssid 1");
 
	ret = shell_execute_cmd(NULL, "wpa_cli sta_autoconnect 1 ");
	ret = shell_execute_cmd(NULL, "wpa_cli enable_network 0");


    /* if PMF enable */
    if(setPSK->pmf == WFA_ENABLED || setPSK->pmf == WFA_OPTIONAL)
    {
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 1");
    }
    else if(setPSK->pmf == WFA_REQUIRED)
    {
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 2");
    }
    else if(setPSK->pmf == WFA_F_REQUIRED)
    {

	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 2");
   } 
    else if(setPSK->pmf == WFA_F_DISABLED)
    {

    }
    else
    {
        /* Disable PMF */
	ret = shell_execute_cmd(NULL, "wpa_cli set_network 0 ieee80211w 1");
    }
	ret = shell_execute_cmd(NULL, "wpa_cli select_network 0");
    sleep(2);
#endif

#endif

    setPskResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_PSK_RESP_TLV, 4, (BYTE *)setPskResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * wfaStaGetInfo():
 * Get vendor specific information in name/value pair by a wireless I/F.
 */
int wfaStaGetInfo(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    dutCommand_t *getInfo = (dutCommand_t *)caCmdBuf;

    /*
     * Normally this is called to retrieve the vendor information
     * from a interface, no implement yet
     */
    sprintf(infoResp.cmdru.info, "interface,%s,vendor,XXX,cardtype,802.11a/b/g", getInfo->intf);

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_GET_INFO_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}

/*
 * wfaStaSetEapTTLS():
 *   This is to set
 *   1. ssid
 *   2. username
 *   3. passwd
 *   4. encrypType - tkip or aes-ccmp
 *   5. keyManagementType - wpa or wpa2
 *   6. trustedRootCA
 */
int wfaStaSetEapTTLS(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetEapTTLS_t *setTTLS = (caStaSetEapTTLS_t *)caCmdBuf;
    char *ifname = setTTLS->intf;
    dutCmdResponse_t *setEapTtlsResp = &gGenericResp;

#ifdef WFA_NEW_CLI_FORMAT
    sprintf(gCmdStr, "wfa_set_eapttls %s %s %s %s %s", ifname, setTTLS->ssid, setTTLS->username, setTTLS->passwd, setTTLS->trustedRootCA);
    sret = system(gCmdStr);
#else

   sprintf(gCmdStr, "wpa_cli add_network 0 ", ifname); 
   sret = system(gCmdStr);
    sprintf(gCmdStr, "wpa_cli disable_network 0", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", ifname, setTTLS->ssid);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 identity '\"%s\"'", ifname, setTTLS->username);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 password '\"%s\"'", ifname, setTTLS->passwd);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-EAP", ifname);
    sret = system(gCmdStr);

    /* This may not need to set. if it is not set, default to take all */
//   sprintf(cmdStr, "wpa_cli set_network 0 pairwise '\"%s\"", ifname, setTTLS->encrptype);
    if(strcasecmp(setTTLS->keyMgmtType, "wpa2-sha256") == 0)
    {
    }
    else if(strcasecmp(setTTLS->keyMgmtType, "wpa2-eap") == 0)
    {
    }
    else if(strcasecmp(setTTLS->keyMgmtType, "wpa2-ft") == 0)
    {

    }
    else if(strcasecmp(setTTLS->keyMgmtType, "wpa") == 0)
    {

      sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-EAP", ifname);//IMG EDITED
      sret = system(gCmdStr);//IMG EDITED
      sprintf(gCmdStr, "wpa_cli set_network 0 proto WPA", ifname);//IMG EDITED
      sret = system(gCmdStr);//IMG EDITED
    }
    else if(strcasecmp(setTTLS->keyMgmtType, "wpa2") == 0)
    {
        // to take all and device to pick one it supported
      sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-EAP", ifname);//IMG EDITED
      sret = system(gCmdStr);//IMG EDITED
      sprintf(gCmdStr, "wpa_cli set_network 0 proto WPA2", ifname);//IMG EDITED
      sret = system(gCmdStr);//IMG EDITED
    }
    else
    {
        // ??
    }
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 eap TTLS", ifname);
    sret = system(gCmdStr);

   sprintf(gCmdStr, "wpa_cli set_network 0 ca_cert '\"/etc/wpa_supplicant/cas.pem\"'", ifname);
   sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 phase2 '\"auth=MSCHAPV2\"'", ifname);
    sret = system(gCmdStr);

     //IMG
 sprintf(gCmdStr, "wpa_cli sta_autoconnect 0 ", ifname);
    sret = system(gCmdStr);
    sprintf(gCmdStr, "wpa_cli enable_network 0", ifname);
    sret = system(gCmdStr);
#endif

    setEapTtlsResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_EAPTTLS_RESP_TLV, 4, (BYTE *)setEapTtlsResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * wfaStaSetEapSIM():
 *   This is to set
 *   1. ssid
 *   2. user name
 *   3. passwd
 *   4. encrypType - tkip or aes-ccmp
 *   5. keyMangementType - wpa or wpa2
 */
int wfaStaSetEapSIM(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetEapSIM_t *setSIM = (caStaSetEapSIM_t *)caCmdBuf;
    char *ifname = setSIM->intf;
    dutCmdResponse_t *setEapSimResp = &gGenericResp;

#ifdef WFA_NEW_CLI_FORMAT
    sprintf(gCmdStr, "wfa_set_eapsim %s %s %s %s", ifname, setSIM->ssid, setSIM->username, setSIM->encrptype);
    sret = system(gCmdStr);
#else

   sprintf(gCmdStr, "wpa_cli add_network 0 ", ifname); 
   sret = system(gCmdStr);
    sprintf(gCmdStr, "wpa_cli disable_network 0", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", ifname, setSIM->ssid);
    sret = system(gCmdStr);


    sprintf(gCmdStr, "wpa_cli set_network 0 identity '\"%s\"'", ifname, setSIM->username);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 pairwise '\"%s\"'", ifname, setSIM->encrptype);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 eap SIM", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 proto WPA", ifname);
    sret = system(gCmdStr);

     //IMG
    sprintf(gCmdStr, "wpa_cli sta_autoconnect 0 ", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli enable_network 0", ifname);
    sret = system(gCmdStr);

    if(strcasecmp(setSIM->keyMgmtType, "wpa2-sha256") == 0)
    {
        sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-SHA256", ifname);
    }
    else if(strcasecmp(setSIM->keyMgmtType, "wpa2-eap") == 0)
    {
        sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-EAP", ifname);
    }
    else if(strcasecmp(setSIM->keyMgmtType, "wpa2-ft") == 0)
    {
        sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-FT", ifname);
    }
    else if(strcasecmp(setSIM->keyMgmtType, "wpa") == 0)
    {
        sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-EAP", ifname);
    }
    else if(strcasecmp(setSIM->keyMgmtType, "wpa2") == 0)
    {
        // take all and device to pick one which is supported.
    }
    else
    {
        // ??
    }
    sret = system(gCmdStr);

#endif

    setEapSimResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_EAPSIM_RESP_TLV, 4, (BYTE *)setEapSimResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * wfaStaSetPEAP()
 *   This is to set
 *   1. ssid
 *   2. user name
 *   3. passwd
 *   4. encryType - tkip or aes-ccmp
 *   5. keyMgmtType - wpa or wpa2
 *   6. trustedRootCA
 *   7. innerEAP
 *   8. peapVersion
 */
int wfaStaSetPEAP(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetEapPEAP_t *setPEAP = (caStaSetEapPEAP_t *)caCmdBuf;
    char *ifname = setPEAP->intf;
    dutCmdResponse_t *setPeapResp = &gGenericResp;

#ifdef WFA_NEW_CLI_FORMAT
    sprintf(gCmdStr, "wfa_set_peap %s %s %s %s %s %s %i %s", ifname, setPEAP->ssid, setPEAP->username,
            setPEAP->passwd, setPEAP->trustedRootCA,
            setPEAP->encrptype, setPEAP->peapVersion,
            setPEAP->innerEAP);
    sret = system(gCmdStr);
#else

   sprintf(gCmdStr, "wpa_cli add_network 0 ", ifname); 
   sret = system(gCmdStr);
    sprintf(gCmdStr, "wpa_cli disable_network 0", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", ifname, setPEAP->ssid);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 eap PEAP", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 anonymous_identity '\"anonymous\"' ", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 identity '\"%s\"'", ifname, setPEAP->username);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 password '\"%s\"'", ifname, setPEAP->passwd);
    sret = system(gCmdStr);

   sprintf(gCmdStr, "wpa_cli set_network 0 ca_cert '\"/etc/wpa_supplicant/cas.pem\"'", ifname);
   sret = system(gCmdStr);

   /* if this not set, default to set support all */
   //sprintf(gCmdStr, "wpa_cli set_network 0 pairwise '\"%s\"'", ifname, setPEAP->encrptype);
   //sret = system(gCmdStr);

    if(strcasecmp(setPEAP->keyMgmtType, "wpa2-sha256") == 0)
    {
        sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-SHA256", ifname);
    }
    else if(strcasecmp(setPEAP->keyMgmtType, "wpa2-eap") == 0)
    {
        sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-EAP", ifname);
    }
    else if(strcasecmp(setPEAP->keyMgmtType, "wpa2-ft") == 0)
    {
        sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-FT", ifname);
    }
    else if(strcasecmp(setPEAP->keyMgmtType, "wpa") == 0)
    {
        sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-EAP", ifname);
    }
    else if(strcasecmp(setPEAP->keyMgmtType, "wpa2") == 0)
    {
        // take all and device to pick one which is supported.
    }
    else
    {
        // ??
    }
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 phase1 '\"peaplabel=%i\"'", ifname, setPEAP->peapVersion);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 phase2 '\"auth=%s\"'", ifname, setPEAP->innerEAP);
    sret = system(gCmdStr);

     //IMG
    sprintf(gCmdStr, "wpa_cli sta_autoconnect 0 ", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli enable_network 0", ifname);
    sret = system(gCmdStr);
#endif

    setPeapResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_PEAP_RESP_TLV, 4, (BYTE *)setPeapResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * wfaStaSetUAPSD()
 *    This is to set
 *    1. acBE
 *    2. acBK
 *    3. acVI
 *    4. acVO
 */
int wfaStaSetUAPSD(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *setUAPSDResp = &gGenericResp;
#if 0 /* used for only one specific device, need to update to reflect yours */
    caStaSetUAPSD_t *setUAPSD = (caStaSetUAPSD_t *)caCmdBuf;
    char *ifname = setUAPSD->intf;
    char tmpStr[10];
    char line[100];
    char *pathl="/etc/Wireless/RT61STA";
    BYTE acBE=1;
    BYTE acBK=1;
    BYTE acVO=1;
    BYTE acVI=1;
    BYTE APSDCapable;
    FILE *pipe;

    /*
     * A series of setting need to be done before doing WMM-PS
     * Additional steps of configuration may be needed.
     */

    /*
     * bring down the interface
     */
    sprintf(gCmdStr, "ifconfig %s down",ifname);
    sret = system(gCmdStr);
    /*
     * Unload the Driver
     */
    sprintf(gCmdStr, "rmmod rt61");
    sret = system(gCmdStr);
#ifndef WFA_WMM_AC
    if(setUAPSD->acBE != 1)
        acBE=setUAPSD->acBE = 0;
    if(setUAPSD->acBK != 1)
        acBK=setUAPSD->acBK = 0;
    if(setUAPSD->acVO != 1)
        acVO=setUAPSD->acVO = 0;
    if(setUAPSD->acVI != 1)
        acVI=setUAPSD->acVI = 0;
#else
    acBE=setUAPSD->acBE;
    acBK=setUAPSD->acBK;
    acVO=setUAPSD->acVO;
    acVI=setUAPSD->acVI;
#endif

    APSDCapable = acBE||acBK||acVO||acVI;
    /*
     * set other AC parameters
     */

    sprintf(tmpStr,"%d;%d;%d;%d",setUAPSD->acBE,setUAPSD->acBK,setUAPSD->acVI,setUAPSD->acVO);
    sprintf(gCmdStr, "sed -e \"s/APSDCapable=.*/APSDCapable=%d/g\" -e \"s/APSDAC=.*/APSDAC=%s/g\" %s/rt61sta.dat >/tmp/wfa_tmp",APSDCapable,tmpStr,pathl);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "mv /tmp/wfa_tmp %s/rt61sta.dat",pathl);
    sret = system(gCmdStr);
    pipe = popen("uname -r", "r");
    /* Read into line the output of uname*/
    fscanf(pipe,"%s",line);
    pclose(pipe);

    /*
     * load the Driver
     */
    sprintf(gCmdStr, "insmod /lib/modules/%s/extra/rt61.ko",line);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "ifconfig %s up",ifname);
    sret = system(gCmdStr);
#endif

    setUAPSDResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_UAPSD_RESP_TLV, 4, (BYTE *)setUAPSDResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;
    return WFA_SUCCESS;
}

int wfaDeviceGetInfo(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCommand_t *dutCmd = (dutCommand_t *)caCmdBuf;
    caDevInfo_t *devInfo = &dutCmd->cmdsu.dev;
    dutCmdResponse_t *infoResp = &gGenericResp;
    /*a vendor can fill in the proper info or anything non-disclosure */
    caDeviceGetInfoResp_t dinfo = {"WFA Lab", "DemoUnit", WFA_SYSTEM_VER};

    DPRINT_INFO(WFA_OUT, "Entering wfaDeviceGetInfo ...\n");

    if(devInfo->fw == 0)
        memcpy(&infoResp->cmdru.devInfo, &dinfo, sizeof(caDeviceGetInfoResp_t));
    else
    {
        // Call internal API to pull the version ID */
        memcpy(infoResp->cmdru.devInfo.firmware, "NOVERSION", sizeof("NOVERSION"));
    }

    infoResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_DEVICE_GET_INFO_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);

    return WFA_SUCCESS;

}

/*
 * This funciton is to retrieve a list of interfaces and return
 * the list back to Agent control.
 * ********************************************************************
 * Note: We intend to make this WLAN interface name as a hardcode name.
 * Therefore, for a particular device, you should know and change the name
 * for that device while doing porting. The MACRO "WFA_STAUT_IF" is defined in
 * the file "inc/wfa_ca.h". If the device OS is not linux-like, this most
 * likely is hardcoded just for CAPI command responses.
 * *******************************************************************
 *
 */
int wfaDeviceListIF(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *infoResp = &gGenericResp;
    dutCommand_t *ifList = (dutCommand_t *)caCmdBuf;
    caDeviceListIFResp_t *ifListResp = &infoResp->cmdru.ifList;

    DPRINT_INFO(WFA_OUT, "!!!!!!!!Entering wfaDeviceListIF ...\n");
    switch(ifList->cmdsu.iftype)
    {
    case IF_80211:
    DPRINT_INFO(WFA_OUT, "Entering Switch IF_80211 ...\n");
        infoResp->status = STATUS_COMPLETE;
        ifListResp->iftype = IF_80211;
        //strcpy(ifListResp->ifs[0], WFA_STAUT_IF);
        strcpy(ifListResp->ifs[0], "wlan0");
        strcpy(ifListResp->ifs[1], "NULL");
        strcpy(ifListResp->ifs[2], "NULL");
        break;
    case IF_ETH:
    DPRINT_INFO(WFA_OUT, "Entering Switch IF_ETH ...\n");
        infoResp->status = STATUS_COMPLETE;
        ifListResp->iftype = IF_ETH;
        strcpy(ifListResp->ifs[0], "eth0");
        strcpy(ifListResp->ifs[1], "NULL");
        strcpy(ifListResp->ifs[2], "NULL");
        break;
    default:
    {
    DPRINT_INFO(WFA_OUT, "Entering Switch DEFAULT...\n");
        infoResp->status = STATUS_ERROR;
        wfaEncodeTLV(WFA_DEVICE_LIST_IF_RESP_TLV, 4, (BYTE *)infoResp, respBuf);
        *respLen = WFA_TLV_HDR_LEN + 4;

        return WFA_SUCCESS;
    }
    }

    wfaEncodeTLV(WFA_DEVICE_LIST_IF_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);

    return WFA_SUCCESS;
}

int wfaStaDebugSet(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *debugResp = &gGenericResp;
    dutCommand_t *debugSet = (dutCommand_t *)caCmdBuf;

    DPRINT_INFO(WFA_OUT, "Entering wfaStaDebugSet ...\n");

    if(debugSet->cmdsu.dbg.state == 1) /* enable */
        wfa_defined_debug |= debugSet->cmdsu.dbg.level;
    else
        wfa_defined_debug = (~debugSet->cmdsu.dbg.level & wfa_defined_debug);

    debugResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_GET_INFO_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)debugResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);


    return WFA_SUCCESS;
}


/*
 *   wfaStaGetBSSID():
 *     This function is to retrieve BSSID of a specific wireless I/F.
 */
int wfaStaGetBSSID(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    char string_bssid[100];
    char *str;
    FILE *tmpfd;
    dutCmdResponse_t *bssidResp = &gGenericResp;
    dutCommand_t *connStat = (dutCommand_t *)caCmdBuf;
    char *ifname = connStat->intf;
    struct wpa_supplicant *wpa_s;

    DPRINT_INFO(WFA_OUT, "Entering wfaStaGetBSSID ...\n");
    /* retrieve the BSSID */
	int ret;	
	ret = shell_execute_cmd(NULL, "wpa_cli status");


	wpa_s = wpa_supplicant_get_iface(global, ifname);
	if (!wpa_s) {
		printf("Unable to find the interface: %s, quitting", ifname);
		return -1;
	}
		ret = os_snprintf(string_bssid,64, "" MACSTR "\n",MAC2STR(wpa_s->bssid));
		//ret = os_snprintf(string_bssid,64,"%s",MAC2STR(wpa_s->bssid));
		//os_memcpy(string_bssid, wpa_s->bssid, ETH_ALEN);
		printf("...string BSSID = %s",string_bssid);

                strcpy(bssidResp->cmdru.bssid, string_bssid);
                bssidResp->status = STATUS_COMPLETE;
		printf("string BSSID = %s bssidresp=  %s",string_bssid,bssidResp->cmdru.bssid);

    wfaEncodeTLV(WFA_STA_GET_BSSID_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)bssidResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);

    return WFA_SUCCESS;
}

/*
 * wfaStaSetIBSS()
 *    This is to set
 *    1. ssid
 *    2. channel
 *    3. encrypType - none or wep
 *    optional
 *    4. key1
 *    5. key2
 *    6. key3
 *    7. key4
 *    8. activeIndex - 1, 2, 3, or 4
 */
int wfaStaSetIBSS(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetIBSS_t *setIBSS = (caStaSetIBSS_t *)caCmdBuf;
    dutCmdResponse_t *setIbssResp = &gGenericResp;
    int i;

    /*
     * disable the network first
     */
    sprintf(gCmdStr, "wpa_cli disable_network 0", setIBSS->intf);
    sret = system(gCmdStr);

    /*
     * set SSID
     */
    sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", setIBSS->intf, setIBSS->ssid);
    sret = system(gCmdStr);

    /*
     * Set channel for IBSS
     */
    sprintf(gCmdStr, "iwconfig %s channel %i", setIBSS->intf, setIBSS->channel);
    sret = system(gCmdStr);

    /*
     * Tell the supplicant for IBSS mode (1)
     */
    sprintf(gCmdStr, "wpa_cli set_network 0 mode 1", setIBSS->intf);
    sret = system(gCmdStr);

    /*
     * set Key management to NONE (NO WPA) for plaintext or WEP
     */
    sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt NONE", setIBSS->intf);
    sret = system(gCmdStr);

    if(setIBSS->encpType == 1)
    {
        for(i=0; i<4; i++)
        {
            if(strlen(setIBSS->keys[i]) ==5 || strlen(setIBSS->keys[i]) == 13)
            {
                sprintf(gCmdStr, "wpa_cli set_network 0 wep_key%i \"%s\"",
                        setIBSS->intf, i, setIBSS->keys[i]);
                sret = system(gCmdStr);
            }
        }

        i = setIBSS->activeKeyIdx;
        if(strlen(setIBSS->keys[i]) ==5 || strlen(setIBSS->keys[i]) == 13)
        {
            sprintf(gCmdStr, "wpa_cli set_network 0 wep_tx_keyidx %i",
                    setIBSS->intf, setIBSS->activeKeyIdx);
            sret = system(gCmdStr);
        }
    }

     //IMG
    sprintf(gCmdStr, "wpa_cli sta_autoconnect 0 ", setIBSS->intf);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli enable_network 0", setIBSS->intf);
    sret = system(gCmdStr);

    setIbssResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_IBSS_RESP_TLV, 4, (BYTE *)setIbssResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 *  wfaSetMode():
 *  The function is to set the wireless interface with a given mode (possible
 *  adhoc)
 *  Input parameters:
 *    1. I/F
 *    2. ssid
 *    3. mode adhoc or managed
 *    4. encType
 *    5. channel
 *    6. key(s)
 *    7. active  key
 */
int wfaStaSetMode(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetMode_t *setmode = (caStaSetMode_t *)caCmdBuf;
    dutCmdResponse_t *SetModeResp = &gGenericResp;
    int i;

    /*
     * bring down the interface
     */
    sprintf(gCmdStr, "ifconfig %s down",setmode->intf);
    sret = system(gCmdStr);

    /*
     * distroy the interface
     */
    sprintf(gCmdStr, "wlanconfig %s destroy",setmode->intf);
    sret = system(gCmdStr);


    /*
     * re-create the interface with the given mode
     */
    if(setmode->mode == 1)
        sprintf(gCmdStr, "wlanconfig %s create wlandev wifi0 wlanmode adhoc",setmode->intf);
    else
        sprintf(gCmdStr, "wlanconfig %s create wlandev wifi0 wlanmode managed",setmode->intf);

    sret = system(gCmdStr);
    if(setmode->encpType == ENCRYPT_WEP)
    {
        int j = setmode->activeKeyIdx;
        for(i=0; i<4; i++)
        {
            if(setmode->keys[i][0] != '\0')
            {
                sprintf(gCmdStr, "iwconfig  %s key  s:%s",
                        setmode->intf, setmode->keys[i]);
                sret = system(gCmdStr);
            }
            /* set active key */
            if(setmode->keys[j][0] != '\0')
                sprintf(gCmdStr, "iwconfig  %s key  s:%s",
                        setmode->intf, setmode->keys[j]);
            sret = system(gCmdStr);
        }

    }
    /*
     * Set channel for IBSS
     */
    if(setmode->channel)
    {
        sprintf(gCmdStr, "iwconfig %s channel %i", setmode->intf, setmode->channel);
        sret = system(gCmdStr);
    }


    /*
     * set SSID
     */
    sprintf(gCmdStr, "iwconfig %s essid %s", setmode->intf, setmode->ssid);
    sret = system(gCmdStr);

    /*
     * bring up the interface
     */
    sprintf(gCmdStr, "ifconfig %s up",setmode->intf);
    sret = system(gCmdStr);

    SetModeResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_MODE_RESP_TLV, 4, (BYTE *)SetModeResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

int wfaStaSetPwrSave(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetPwrSave_t *setps = (caStaSetPwrSave_t *)caCmdBuf;
    dutCmdResponse_t *SetPSResp = &gGenericResp;

    sprintf(gCmdStr, "iwconfig %s power %s", setps->intf, setps->mode);
    sret = system(gCmdStr);


    SetPSResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_PWRSAVE_RESP_TLV, 4, (BYTE *)SetPSResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

int wfaStaUpload(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaUpload_t *upload = &((dutCommand_t *)caCmdBuf)->cmdsu.upload;
    dutCmdResponse_t *upLoadResp = &gGenericResp;
    caStaUploadResp_t *upld = &upLoadResp->cmdru.uld;

    if(upload->type == WFA_UPLOAD_VHSO_RPT)
    {
        int rbytes;
        /*
         * if asked for the first packet, always to open the file
         */
        if(upload->next == 1)
        {
            if(e2efp != NULL)
            {
                fclose(e2efp);
                e2efp = NULL;
            }

            e2efp = fopen(e2eResults, "r");
        }

        if(e2efp == NULL)
        {
            upLoadResp->status = STATUS_ERROR;
            wfaEncodeTLV(WFA_STA_UPLOAD_RESP_TLV, 4, (BYTE *)upLoadResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + 4;
            return WFA_FAILURE;
        }

        rbytes = fread(upld->bytes, 1, 256, e2efp);

        if(rbytes < 256)
        {
            /*
             * this means no more bytes after this read
             */
            upld->seqnum = 0;
            fclose(e2efp);
            e2efp=NULL;
        }
        else
        {
            upld->seqnum = upload->next;
        }

        upld->nbytes = rbytes;

        upLoadResp->status = STATUS_COMPLETE;
        wfaEncodeTLV(WFA_STA_UPLOAD_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)upLoadResp, respBuf);
        *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
    }
    else
    {
        upLoadResp->status = STATUS_ERROR;
        wfaEncodeTLV(WFA_STA_UPLOAD_RESP_TLV, 4, (BYTE *)upLoadResp, respBuf);
        *respLen = WFA_TLV_HDR_LEN + 4;
    }

    return WFA_SUCCESS;
}
/*
 * wfaStaSetWMM()
 *  TO be ported on a specific plaform for the DUT
 *  This is to set the WMM related parameters at the DUT.
 *  Currently the function is used for GROUPS WMM-AC and WMM general configuration for setting RTS Threshhold, Fragmentation threshold and wmm (ON/OFF)
 *  It is expected that this function will set all the WMM related parametrs for a particular GROUP .
 */
int wfaStaSetWMM(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
#ifdef WFA_WMM_AC
    caStaSetWMM_t *setwmm = (caStaSetWMM_t *)caCmdBuf;
    char *ifname = setwmm->intf;
    dutCmdResponse_t *setwmmResp = &gGenericResp;

    switch(setwmm->group)
    {
    case GROUP_WMMAC:
        if (setwmm->send_trig)
        {
            int Sockfd;
            struct sockaddr_in psToAddr;
            unsigned int TxMsg[512];

            Sockfd = wfaCreateUDPSock(setwmm->dipaddr, 12346);
            memset(&psToAddr, 0, sizeof(psToAddr));
            psToAddr.sin_family = AF_INET;
            psToAddr.sin_addr.s_addr = inet_addr(setwmm->dipaddr);
            psToAddr.sin_port = htons(12346);


            switch (setwmm->trig_ac)
            {
            case WMMAC_AC_VO:
                wfaTGSetPrio(Sockfd, 7);
                create_apts_msg(APTS_CK_VO, TxMsg, 0);
                printf("\r\nSending AC_VO trigger packet\n");
                break;

            case WMMAC_AC_VI:
                wfaTGSetPrio(Sockfd, 5);
                create_apts_msg(APTS_CK_VI, TxMsg, 0);
                printf("\r\nSending AC_VI trigger packet\n");
                break;

            case WMMAC_AC_BK:
                wfaTGSetPrio(Sockfd, 2);
                create_apts_msg(APTS_CK_BK, TxMsg, 0);
                printf("\r\nSending AC_BK trigger packet\n");
                break;

            default:
            case WMMAC_AC_BE:
                wfaTGSetPrio(Sockfd, 0);
                create_apts_msg(APTS_CK_BE, TxMsg, 0);
                printf("\r\nSending AC_BE trigger packet\n");
                break;
            }

            sendto(Sockfd, TxMsg, 256, 0, (struct sockaddr *)&psToAddr,
                   sizeof(struct sockaddr));
            close(Sockfd);
            usleep(1000000);
        }
        else if (setwmm->action == WMMAC_ADDTS)
        {
            printf("ADDTS AC PARAMS: dialog id: %d, TID: %d, "
                   "DIRECTION: %d, PSB: %d, UP: %d, INFOACK: %d BURST SIZE DEF: %d"
                   "Fixed %d, MSDU Size: %d, Max MSDU Size %d, "
                   "MIN SERVICE INTERVAL: %d, MAX SERVICE INTERVAL: %d, "
                   "INACTIVITY: %d, SUSPENSION %d, SERVICE START TIME: %d, "
                   "MIN DATARATE: %d, MEAN DATA RATE: %d, PEAK DATA RATE: %d, "
                   "BURSTSIZE or MSDU Aggreg: %d, DELAY BOUND: %d, PHYRATE: %d, SPLUSBW: %f, "
                   "MEDIUM TIME: %d, ACCESSCAT: %d\n",
                   setwmm->actions.addts.dialog_token,
                   setwmm->actions.addts.tspec.tsinfo.TID,
                   setwmm->actions.addts.tspec.tsinfo.direction,
                   setwmm->actions.addts.tspec.tsinfo.PSB,
                   setwmm->actions.addts.tspec.tsinfo.UP,
                   setwmm->actions.addts.tspec.tsinfo.infoAck,
                   setwmm->actions.addts.tspec.tsinfo.bstSzDef,
                   setwmm->actions.addts.tspec.Fixed,
                   setwmm->actions.addts.tspec.size,
                   setwmm->actions.addts.tspec.maxsize,
                   setwmm->actions.addts.tspec.min_srvc,
                   setwmm->actions.addts.tspec.max_srvc,
                   setwmm->actions.addts.tspec.inactivity,
                   setwmm->actions.addts.tspec.suspension,
                   setwmm->actions.addts.tspec.srvc_strt_tim,
                   setwmm->actions.addts.tspec.mindatarate,
                   setwmm->actions.addts.tspec.meandatarate,
                   setwmm->actions.addts.tspec.peakdatarate,
                   setwmm->actions.addts.tspec.burstsize,
                   setwmm->actions.addts.tspec.delaybound,
                   setwmm->actions.addts.tspec.PHYrate,
                   setwmm->actions.addts.tspec.sba,
                   setwmm->actions.addts.tspec.medium_time,
                   setwmm->actions.addts.accesscat);

            //tspec should be set here.

            sret = system(gCmdStr);
        }
        else if (setwmm->action == WMMAC_DELTS)
        {
            // send del tspec
        }

        setwmmResp->status = STATUS_COMPLETE;
        break;

    case GROUP_WMMCONF:
        sprintf(gCmdStr, "iwconfig %s rts %d",
                ifname,setwmm->actions.config.rts_thr);

        sret = system(gCmdStr);
        sprintf(gCmdStr, "iwconfig %s frag %d",
                ifname,setwmm->actions.config.frag_thr);

        sret = system(gCmdStr);
        sprintf(gCmdStr, "iwpriv %s wmmcfg %d",
                ifname, setwmm->actions.config.wmm);

        sret = system(gCmdStr);
        setwmmResp->status = STATUS_COMPLETE;
        break;

    default:
        DPRINT_ERR(WFA_ERR, "The group %d is not supported\n",setwmm->group);
        setwmmResp->status = STATUS_ERROR;
        break;

    }

    wfaEncodeTLV(WFA_STA_SET_WMM_RESP_TLV, 4, (BYTE *)setwmmResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;
#endif

    return WFA_SUCCESS;
}

int wfaStaSendNeigReq(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *sendNeigReqResp = &gGenericResp;

    /*
     *  run your device to send NEIGREQ
     */

    sendNeigReqResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SEND_NEIGREQ_RESP_TLV, 4, (BYTE *)sendNeigReqResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

int wfaStaSetEapFAST(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetEapFAST_t *setFAST= (caStaSetEapFAST_t *)caCmdBuf;
    char *ifname = setFAST->intf;
    dutCmdResponse_t *setEapFastResp = &gGenericResp;

#ifdef WFA_NEW_CLI_FORMAT
    sprintf(gCmdStr, "wfa_set_eapfast %s %s %s %s %s %s", ifname, setFAST->ssid, setFAST->username,
            setFAST->passwd, setFAST->pacFileName,
            setFAST->innerEAP);
    sret = system(gCmdStr);
#else

    sprintf(gCmdStr, "wpa_cli add_network 0", ifname);
    sret = system(gCmdStr);
    sprintf(gCmdStr, "wpa_cli disable_network 0", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", ifname, setFAST->ssid);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 identity '\"%s\"'", ifname, setFAST->username);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 password '\"%s\"'", ifname, setFAST->passwd);
    sret = system(gCmdStr);

    if(strcasecmp(setFAST->keyMgmtType, "wpa2-sha256") == 0)
    {
    }
    else if(strcasecmp(setFAST->keyMgmtType, "wpa2-eap") == 0)
    {
    }
    else if(strcasecmp(setFAST->keyMgmtType, "wpa2-ft") == 0)
    {

    }
    else if(strcasecmp(setFAST->keyMgmtType, "wpa") == 0)
    {
        sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-EAP", ifname);
    }
    else if(strcasecmp(setFAST->keyMgmtType, "wpa2") == 0)
    {
        // take all and device to pick one which is supported.
    }
    else
    {
        // ??
    }
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 eap FAST", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 pac_file '\"%s/%s\"'", ifname, CERTIFICATES_PATH,     setFAST->pacFileName);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 anonymous_identity '\"anonymous\"'", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 phase1 '\"fast_provisioning=1\"'", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 phase2 '\"auth=%s\"'", ifname,setFAST->innerEAP);
    sret = system(gCmdStr);

    //IMG
    sprintf(gCmdStr, "wpa_cli sta_autoconnect 0 ", ifname);
    sret = system(gCmdStr);



    sprintf(gCmdStr, "wpa_cli enable_network 0", ifname);
    sret = system(gCmdStr);
#endif

    setEapFastResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_EAPFAST_RESP_TLV, 4, (BYTE *)setEapFastResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

int wfaStaSetEapAKA(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetEapAKA_t *setAKA= (caStaSetEapAKA_t *)caCmdBuf;
    char *ifname = setAKA->intf;
    dutCmdResponse_t *setEapAkaResp = &gGenericResp;

#ifdef WFA_NEW_CLI_FORMAT
    sprintf(gCmdStr, "wfa_set_eapaka %s %s %s %s", ifname, setAKA->ssid, setAKA->username, setAKA->passwd);
    sret = system(gCmdStr);
#else

    sprintf(gCmdStr, "wpa_cli disable_network 0", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 ssid '\"%s\"'", ifname, setAKA->ssid);
    sret = system(gCmdStr);

    if(strcasecmp(setAKA->keyMgmtType, "wpa2-sha256") == 0)
    {
    }
    else if(strcasecmp(setAKA->keyMgmtType, "wpa2-eap") == 0)
    {
    }
    else if(strcasecmp(setAKA->keyMgmtType, "wpa2-ft") == 0)
    {

    }
    else if(strcasecmp(setAKA->keyMgmtType, "wpa") == 0)
    {
        sprintf(gCmdStr, "wpa_cli set_network 0 key_mgmt WPA-EAP", ifname);
    }
    else if(strcasecmp(setAKA->keyMgmtType, "wpa2") == 0)
    {
        // take all and device to pick one which is supported.
    }
    else
    {
        // ??
    }
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 proto WPA2", ifname);
    sret = system(gCmdStr);
    sprintf(gCmdStr, "wpa_cli set_network 0 pairwise CCMP", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 eap AKA", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 phase1 \"result_ind=1\"", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 identity '\"%s\"'", ifname, setAKA->username);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli set_network 0 password '\"%s\"'", ifname, setAKA->passwd);
    sret = system(gCmdStr);

     //IMG
    sprintf(gCmdStr, "wpa_cli sta_autoconnect 0 ", ifname);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "wpa_cli enable_network 0", ifname);
    sret = system(gCmdStr);
#endif

    setEapAkaResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_EAPAKA_RESP_TLV, 4, (BYTE *)setEapAkaResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

int wfaStaSetSystime(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaSetSystime_t *systime = (caStaSetSystime_t *)caCmdBuf;
    dutCmdResponse_t *setSystimeResp = &gGenericResp;

    DPRINT_INFO(WFA_OUT, "Entering wfaStaSetSystime ...\n");

    sprintf(gCmdStr, "date %d-%d-%d",systime->month,systime->date,systime->year);
    sret = system(gCmdStr);

    sprintf(gCmdStr, "time %d:%d:%d", systime->hours,systime->minutes,systime->seconds);
    sret = system(gCmdStr);

    setSystimeResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_SYSTIME_RESP_TLV, 4, (BYTE *)setSystimeResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

#ifdef WFA_STA_TB
int wfaStaPresetParams(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    int i,ret;
    caStaPresetParameters_t *preset = (caStaPresetParameters_t *)caCmdBuf;
    char *ifname = preset->intf;
	ret = shell_execute_cmd(NULL, "wpa_cli set mbo_cell_capa 1");
    DPRINT_INFO(WFA_OUT, "Inside wfaStaPresetParameters function ...\n");
    if((preset->Ch_Op_Class)!=0)
   {
     printf("\n Operating class= %d \n",preset->Ch_Op_Class);
     printf("\n Channel Pref Number=%d \n",preset->Ch_Pref_Num);
     printf("\n Channel Pref= %d \n",preset->Ch_Pref);
     printf("\n Reason code=%d \n",preset->Ch_Reason_Code);
	sprintf(gCmdStr, "wpa_cli -i wlan0 set non_pref_chan=%d:%d:%d:%d", preset->Ch_Op_Class,preset->Ch_Pref_Num,preset->Ch_Pref,preset->Ch_Reason_Code);
	ret = shell_execute_cmd(NULL, gCmdStr);
   }
    // Implement the function and its sub commands
    infoResp.status = STATUS_COMPLETE;

    wfaEncodeTLV(WFA_STA_PRESET_PARAMETERS_RESP_TLV, 4, (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
/* 	dutCmdResponse_t *PresetParamsResp = &gGenericResp;
    caStaPresetParameters_t *presetParams = (caStaPresetParameters_t *)caCmdBuf;
    BYTE presetDone = 1;
    int st = 0;
   char cmdStr[128];
   char string[256];
   FILE *tmpfd = NULL;
   long val;
   char *endptr;

    DPRINT_INFO(WFA_OUT, "Inside wfaStaPresetParameters function ...\n");
    wfaEncodeTLV(WFA_STA_PRESET_PARAMETERS_RESP_TLV, 4, (BYTE *)PresetParamsResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
 */  
 
}

int wfaStaSet11n(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *v11nParamsResp = &gGenericResp;

    v11nParamsResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, 4, (BYTE *)v11nParamsResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;
    return WFA_SUCCESS;
}
int wfaStaSetWireless(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *staWirelessResp = &gGenericResp;

    staWirelessResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_WIRELESS_RESP_TLV, 4, (BYTE *)staWirelessResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;
    return WFA_SUCCESS;
}

int wfaStaSendADDBA(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *staSendADDBAResp = &gGenericResp;
    staSendADDBAResp->status = STATUS_COMPLETE;

    wfaEncodeTLV(WFA_STA_SET_SEND_ADDBA_RESP_TLV, 4, (BYTE *)staSendADDBAResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;
    return WFA_SUCCESS;
}

int wfaStaSetRIFS(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *staSetRIFSResp = &gGenericResp;

    wfaEncodeTLV(WFA_STA_SET_RIFS_TEST_RESP_TLV, 4, (BYTE *)staSetRIFSResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

int wfaStaSendCoExistMGMT(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *staSendMGMTResp = &gGenericResp;

    wfaEncodeTLV(WFA_STA_SEND_COEXIST_MGMT_RESP_TLV, 4, (BYTE *)staSendMGMTResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;

}

int wfaStaResetDefault(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    caStaResetDefault_t *reset = (caStaResetDefault_t *)caCmdBuf;
    dutCmdResponse_t *ResetResp = &gGenericResp;


    // need to make your own command available for this, here is only an example
//    sprintf(gCmdStr, "myresetdefault %s program %s", reset->intf, reset->prog);
   // sret = system("killall -9 wpa_supplicant");
//    sprintf(gCmdStr,"wpa_supplicant -Dnl80211 -c /etc/no_cfg.conf -i %s -d -K -f log_CALDER_SAE.txt &",reset->intf);
  //  sret=system(gCmdStr);
    //sprintf(gCmdStr, "wpa_cli disable_network 0", reset->intf);
    //sret = system(gCmdStr);
    //printf("\n %s \n",gCmdStr);
    sprintf(gCmdStr, " 'wpa_cli disconnect'", reset->intf);
    sret = shell_execute_cmd(NULL, gCmdStr);
    printf("\n %s \n",gCmdStr);
    sprintf(gCmdStr, " 'wpa_cli set mbo_cell_capa 1'", reset->intf);
    sret = shell_execute_cmd(NULL, gCmdStr);
    printf("\n %s \n",gCmdStr);


    ResetResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_RESET_DEFAULT_RESP_TLV, 4, (BYTE *)ResetResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

#else

int wfaStaTestBedCmd(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t *staCmdResp = &gGenericResp;

    wfaEncodeTLV(WFA_STA_DISCONNECT_RESP_TLV, 4, (BYTE *)staCmdResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}
#endif

/*
 * This is used to send a frame or action frame
 */
int wfaStaDevSendFrame(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCommand_t *cmd = (dutCommand_t *)caCmdBuf;
    /* uncomment it if needed */
    // char *ifname = cmd->intf;
    dutCmdResponse_t *devSendResp = &gGenericResp;
    caStaDevSendFrame_t *sf = &cmd->cmdsu.sf;

    DPRINT_INFO(WFA_OUT, "Inside wfaStaDevSendFrame function ...\n");
    /* processing the frame */
       // sprintf(gCmdStr, "wpa_cli wnm_bss_query 1",cmd->intf);
        //sret = system(gCmdStr);
        //printf("\n %s \n", gCmdStr);

    switch(sf->program)
    {
    case PROG_TYPE_PMF:
    {
        pmfFrame_t *pmf = &sf->frameType.pmf;
        switch(pmf->eFrameName)
        {
        case PMF_TYPE_DISASSOC:
        {
            /* use the protected to set what type of key to send */

        }
        break;
        case PMF_TYPE_DEAUTH:
        {

        }
        break;
        case PMF_TYPE_SAQUERY:
        {

        }
        break;
        case PMF_TYPE_AUTH:
        {
        }
        break;
        case PMF_TYPE_ASSOCREQ:
        {
        }
        break;
        case PMF_TYPE_REASSOCREQ:
        {
        }
        break;
        }
    }
    break;
    case PROG_TYPE_TDLS:
    {
        tdlsFrame_t *tdls = &sf->frameType.tdls;
        switch(tdls->eFrameName)
        {
        case TDLS_TYPE_DISCOVERY:
            /* use the peer mac address to send the frame */
            break;
        case TDLS_TYPE_SETUP:
            break;
        case TDLS_TYPE_TEARDOWN:
            break;
        case TDLS_TYPE_CHANNELSWITCH:
            break;
        case TDLS_TYPE_NULLFRAME:
            break;
        }
    }
    break;
    case PROG_TYPE_VENT:
    {
        ventFrame_t *vent = &sf->frameType.vent;
        switch(vent->type)
        {
        case VENT_TYPE_NEIGREQ:
            break;
        case VENT_TYPE_TRANSMGMT:
            break;
        }
    }
    break;
    case PROG_TYPE_WFD:
    {
        wfdFrame_t *wfd = &sf->frameType.wfd;
        switch(wfd->eframe)
        {
        case WFD_FRAME_PRBREQ:
        {
            /* send probe req */
        }
        break;

        case WFD_FRAME_PRBREQ_TDLS_REQ:
        {
            /* send tunneled tdls probe req  */
        }
        break;

        case WFD_FRAME_11V_TIMING_MSR_REQ:
        {
            /* send 11v timing mearurement request */
        }
        break;

        case WFD_FRAME_RTSP:
        {
            /* send WFD RTSP messages*/
            // fetch the type of RTSP message and send it.
            switch(wfd->eRtspMsgType)
            {
            case WFD_RTSP_PAUSE:
                break;
            case WFD_RTSP_PLAY:
                //send RTSP PLAY
                break;
            case WFD_RTSP_TEARDOWN:
                //send RTSP TEARDOWN
                break;
            case WFD_RTSP_TRIG_PAUSE:
                //send RTSP TRIGGER PAUSE
                break;
            case WFD_RTSP_TRIG_PLAY:
                //send RTSP TRIGGER PLAY
                break;
            case WFD_RTSP_TRIG_TEARDOWN:
                //send RTSP TRIGGER TEARDOWN
                break;
            case WFD_RTSP_SET_PARAMETER:
                //send RTSP SET PARAMETER
                if (wfd->eSetParams == WFD_CAP_UIBC_KEYBOARD)
                {
                    //send RTSP SET PARAMETER message for UIBC keyboard
                }
                if (wfd->eSetParams == WFD_CAP_UIBC_MOUSE)
                {
                    //send RTSP SET PARAMETER message for UIBC Mouse
                }
                else if (wfd->eSetParams == WFD_CAP_RE_NEGO)
                {
                    //send RTSP SET PARAMETER message Capability re-negotiation
                }
                else if (wfd->eSetParams == WFD_STANDBY)
                {
                    //send RTSP SET PARAMETER message for standby
                }
                else if (wfd->eSetParams == WFD_UIBC_SETTINGS_ENABLE)
                {
                    //send RTSP SET PARAMETER message for UIBC settings enable
                }
                else if (wfd->eSetParams == WFD_UIBC_SETTINGS_DISABLE)
                {
                    //send RTSP SET PARAMETER message for UIBC settings disable
                }
                else if (wfd->eSetParams == WFD_ROUTE_AUDIO)
                {
                    //send RTSP SET PARAMETER message for route audio
                }
                else if (wfd->eSetParams == WFD_3D_VIDEOPARAM)
                {
                    //send RTSP SET PARAMETER message for 3D video parameters
                }
                else if (wfd->eSetParams == WFD_2D_VIDEOPARAM)
                {
                    //send RTSP SET PARAMETER message for 2D video parameters
                }
                break;
            }
        }
        break;
        }
    }
    break;
    /* not need to support HS2 release 1, due to very short time period  */
    case PROG_TYPE_HS2_R2:
    {
        /* type of frames */
        hs2Frame_t *hs2 = &sf->frameType.hs2_r2;
        switch(hs2->eframe)
        {
        case HS2_FRAME_ANQPQuery:
        {

        }
        break;
        case HS2_FRAME_DLSRequest:
        {

        }
        break;
        case HS2_FRAME_GARPReq:
        {

        }
        break;
        case HS2_FRAME_GARPRes:
        {
        }
        break;
        case HS2_FRAME_NeighAdv:
        {
        }
        case HS2_FRAME_ARPProbe:
        {
        }
        case HS2_FRAME_ARPAnnounce:
        {

        }
        break;
        case HS2_FRAME_NeighSolicitReq:
        {

        }
        break;
        case HS2_FRAME_ARPReply:
        {

        }
        break;
        }

        }/*  PROG_TYPE_HS2-R2  */
    case PROG_TYPE_GEN:
    {
        /* General frames */
    }


    }
    devSendResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_DEV_SEND_FRAME_RESP_TLV, 4, (BYTE *)devSendResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * This is used to set a temporary MAC address of an interface
 */
int wfaStaSetMacAddr(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    // Uncomment it if needed
    //dutCommand_t *cmd = (dutCommand_t *)caCmdBuf;
    // char *ifname = cmd->intf;
    dutCmdResponse_t *staCmdResp = &gGenericResp;
    // Uncomment it if needed
    //char *macaddr = &cmd->cmdsu.macaddr[0];

    wfaEncodeTLV(WFA_STA_SET_MAC_ADDRESS_RESP_TLV, 4, (BYTE *)staCmdResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}


int wfaStaDisconnect(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCommand_t *disc = (dutCommand_t *)caCmdBuf;
    char *intf = disc->intf;
    dutCmdResponse_t *staDiscResp = &gGenericResp;

    sprintf(gCmdStr, "wpa_cli -i%s disconnect", intf);
	printf("\n %s \n", gCmdStr);		
    sret = system(gCmdStr);

    staDiscResp->status = STATUS_COMPLETE;

    wfaEncodeTLV(WFA_STA_DISCONNECT_RESP_TLV, 4, (BYTE *)staDiscResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/* Execute CLI, read the status from Environment variable */
int wfaExecuteCLI(char *CLI)
{
    char *retstr;

    sret = system(CLI);

    retstr = getenv("WFA_CLI_STATUS");
    printf("cli status %s\n", retstr);
    return atoi(retstr);
}

/* Supporting Functions */
void wfaSendPing(tgPingStart_t *staPing, int duration, int streamid)
{
    int totalpkts, tos=-1;
    char cmdStr[256];
//    char *addr = staPing->dipaddr;
    char addr[40];
    char bflag[] = "-b";
    char *tmpstr;
    int inum=0;
	
    totalpkts = (int)(staPing->duration * staPing->frameRate);
    strcpy(addr,staPing->dipaddr);
	int ret;
	stmp = duration;
    	printf("Printing PING OUTPUT\n");
	sprintf(gCmdStr, "net ping  -c %d %s", duration,addr);
	ret = shell_execute_cmd(NULL, gCmdStr);
    	printf("Printing PING OUTPUT DONE\n");
}

int wfaStopPing(dutCmdResponse_t *stpResp, int streamid)
{
    char strout[256];
    FILE *tmpfile = NULL;
    char cmdStr[128];
  printf("\nIn func %s :: stream id=%d\n", __func__,streamid);

            stpResp->cmdru.pingStp.sendCnt = stmp-1;
            stpResp->cmdru.pingStp.repliedCnt = count_seq;
    printf("\nCount of the seq_num from NET SHELL is  %d",count_seq);
    printf("wfaStopPing send count %i\n", stpResp->cmdru.pingStp.sendCnt);
    printf("wfaStopPing replied count %i\n", stpResp->cmdru.pingStp.repliedCnt);
    return WFA_SUCCESS;
}

/*
 * wfaStaGetP2pDevAddress():
 */
int wfaStaGetP2pDevAddress(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* dutCommand_t *getInfo = (dutCommand_t *)caCmdBuf; */

    printf("\n Entry wfaStaGetP2pDevAddress... ");

    // Fetch the device ID and store into infoResp->cmdru.devid
    //strcpy(infoResp->cmdru.devid, str);
    strcpy(&infoResp.cmdru.devid[0], "ABCDEFGH");

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_GET_DEV_ADDRESS_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}



/*
 * wfaStaSetP2p():
 */
int wfaStaSetP2p(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* caStaSetP2p_t *getStaSetP2p = (caStaSetP2p_t *)caCmdBuf; uncomment and use it*/

    printf("\n Entry wfaStaSetP2p... ");

    // Implement the function and this does not return any thing back.

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_SETP2P_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}
/*
 * wfaStaP2pConnect():
 */
int wfaStaP2pConnect(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* caStaP2pConnect_t *getStaP2pConnect = (caStaP2pConnect_t *)caCmdBuf; uncomment and use it */

    printf("\n Entry wfaStaP2pConnect... ");

    // Implement the function and does not return anything.


    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_CONNECT_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}

/*
 * wfaStaStartAutoGo():
 */
int wfaStaStartAutoGo(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    //caStaStartAutoGo_t *getStaStartAutoGo = (caStaStartAutoGo_t *)caCmdBuf;

    printf("\n Entry wfaStaStartAutoGo... ");

    // Fetch the group ID and store into 	infoResp->cmdru.grpid
    strcpy(&infoResp.cmdru.grpid[0], "ABCDEFGH");

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_START_AUTO_GO_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}




/*
 * wfaStaP2pStartGrpFormation():
 */
int wfaStaP2pStartGrpFormation(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;

    printf("\n Entry wfaStaP2pStartGrpFormation... ");

    strcpy(infoResp.cmdru.grpFormInfo.result, "CLIENT");
    strcpy(infoResp.cmdru.grpFormInfo.grpId, "AA:BB:CC:DD:EE:FF_DIRECT-SSID");


    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_START_GRP_FORMATION_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}


/*
 * wfaStaP2pDissolve():
 */
int wfaStaP2pDissolve(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;

    printf("\n Entry wfaStaP2pDissolve... ");

    // Implement the function and this does not return any thing back.

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_DISSOLVE_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}

/*
 * wfaStaSendP2pInvReq():
 */
int wfaStaSendP2pInvReq(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* caStaSendP2pInvReq_t *getStaP2pInvReq= (caStaSendP2pInvReq_t *)caCmdBuf; */

    printf("\n Entry wfaStaSendP2pInvReq... ");

    // Implement the function and this does not return any thing back.

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_SEND_INV_REQ_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}


/*
 * wfaStaAcceptP2pInvReq():
 */
int wfaStaAcceptP2pInvReq(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* uncomment and use it
     * caStaAcceptP2pInvReq_t *getStaP2pInvReq= (caStaAcceptP2pInvReq_t *)caCmdBuf;
     */

    printf("\n Entry wfaStaAcceptP2pInvReq... ");

    // Implement the function and this does not return any thing back.

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_ACCEPT_INV_REQ_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}


/*
 * wfaStaSendP2pProvDisReq():
 */
int wfaStaSendP2pProvDisReq(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* uncomment and use it
     * caStaSendP2pProvDisReq_t *getStaP2pProvDisReq= (caStaSendP2pProvDisReq_t *)caCmdBuf;
     */

    printf("\n Entry wfaStaSendP2pProvDisReq... ");

    // Implement the function and this does not return any thing back.

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_SEND_PROV_DIS_REQ_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}

/*
 * wfaStaSetWpsPbc():
 */
int wfaStaSetWpsPbc(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* uncomment and use it
     * caStaSetWpsPbc_t *getStaSetWpsPbc= (caStaSetWpsPbc_t *)caCmdBuf;
     */

    printf("\n Entry wfaStaSetWpsPbc... ");

    // Implement the function and this does not return any thing back.

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_WPS_SETWPS_PBC_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}

/*
 * wfaStaWpsReadPin():
 */
int wfaStaWpsReadPin(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* uncomment and use it
     * caStaWpsReadPin_t *getStaWpsReadPin= (caStaWpsReadPin_t *)caCmdBuf;
     */

    printf("\n Entry wfaStaWpsReadPin... ");

    // Fetch the device PIN and put in 	infoResp->cmdru.wpsPin
    //strcpy(infoResp->cmdru.wpsPin, "12345678");
    strcpy(&infoResp.cmdru.wpsPin[0], "1234456");


    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_WPS_READ_PIN_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}



/*
 * wfaStaWpsReadLabel():
 */
int wfaStaWpsReadLabel(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;

    printf("\n Entry wfaStaWpsReadLabel... ");

    // Fetch the device Label and put in	infoResp->cmdru.wpsPin
    //strcpy(infoResp->cmdru.wpsPin, "12345678");
    strcpy(&infoResp.cmdru.wpsPin[0], "1234456");


    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_WPS_READ_PIN_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}


/*
 * wfaStaWpsEnterPin():
 */
int wfaStaWpsEnterPin(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* uncomment and use it
     * caStaWpsEnterPin_t *getStaWpsEnterPin= (caStaWpsEnterPin_t *)caCmdBuf;
     */

    printf("\n Entry wfaStaWpsEnterPin... ");

    // Implement the function and this does not return any thing back.


    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_WPS_ENTER_PIN_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}


/*
 * wfaStaGetPsk():
 */
int wfaStaGetPsk(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* caStaGetPsk_t *getStaGetPsk= (caStaGetPsk_t *)caCmdBuf; uncomment and use it */

    printf("\n Entry wfaStaGetPsk... ");


    // Fetch the device PP and SSID  and put in 	infoResp->cmdru.pskInfo
    strcpy(&infoResp.cmdru.pskInfo.passPhrase[0], "1234456");
    strcpy(&infoResp.cmdru.pskInfo.ssid[0], "WIFI_DIRECT");


    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_GET_PSK_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}

/*
 * wfaStaP2pReset():
 */
int wfaStaP2pReset(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* dutCommand_t *getStaP2pReset= (dutCommand_t *)caCmdBuf; */

    printf("\n Entry wfaStaP2pReset... ");
    // Implement the function and this does not return any thing back.

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_RESET_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}



/*
 * wfaStaGetP2pIpConfig():
 */
int wfaStaGetP2pIpConfig(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* caStaGetP2pIpConfig_t *staGetP2pIpConfig= (caStaGetP2pIpConfig_t *)caCmdBuf; */

    caStaGetIpConfigResp_t *ifinfo = &(infoResp.cmdru.getIfconfig);

    printf("\n Entry wfaStaGetP2pIpConfig... ");

    ifinfo->isDhcp =0;
    strcpy(&(ifinfo->ipaddr[0]), "192.165.100.111");
    strcpy(&(ifinfo->mask[0]), "255.255.255.0");
    strcpy(&(ifinfo->dns[0][0]), "192.165.100.1");
    strcpy(&(ifinfo->mac[0]), "ba:ba:ba:ba:ba:ba");

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_GET_IP_CONFIG_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}




/*
 * wfaStaSendServiceDiscoveryReq():
 */
int wfaStaSendServiceDiscoveryReq(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;

    printf("\n Entry wfaStaSendServiceDiscoveryReq... ");
    // Implement the function and this does not return any thing back.


    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_SEND_SERVICE_DISCOVERY_REQ_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}



/*
 * wfaStaSendP2pPresenceReq():
 */
int wfaStaSendP2pPresenceReq(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_SEND_PRESENCE_REQ_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}

/*
 * wfaStaSetSleepReq():
 */
int wfaStaSetSleepReq(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* caStaSetSleep_t *staSetSleepReq= (caStaSetSleep_t *)caCmdBuf; */

    printf("\n Entry wfaStaSetSleepReq... ");
    // Implement the function and this does not return any thing back.


    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_SET_SLEEP_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN +4;

    return WFA_SUCCESS;
}

/*
 * wfaStaSetOpportunisticPsReq():
 */
int wfaStaSetOpportunisticPsReq(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;

    printf("\n Entry wfaStaSetOpportunisticPsReq... ");
    // Implement the function and this does not return any thing back.


    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_SET_OPPORTUNISTIC_PS_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}
#ifndef WFA_STA_TB
/*
 * wfaStaPresetParams():
 */

int wfaStaPresetParams(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;

    DPRINT_INFO(WFA_OUT, "Inside wfaStaPresetParameters function ...\n");

    // Implement the function and its sub commands
    infoResp.status = STATUS_COMPLETE;

    wfaEncodeTLV(WFA_STA_PRESET_PARAMETERS_RESP_TLV, 4, (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}
int wfaStaSet11n(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{

    dutCmdResponse_t infoResp;
    dutCmdResponse_t *v11nParamsResp = &infoResp;

#ifdef WFA_11N_SUPPORT_ONLY

    caSta11n_t * v11nParams = (caSta11n_t *)caCmdBuf;

    int st =0; // SUCCESS

    DPRINT_INFO(WFA_OUT, "Inside wfaStaSet11n function....\n");

    if(v11nParams->addba_reject != 0xFF && v11nParams->addba_reject < 2)
    {
        // implement the funciton
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_addba_reject failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

    if(v11nParams->ampdu != 0xFF && v11nParams->ampdu < 2)
    {
        // implement the funciton

        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_ampdu failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

    if(v11nParams->amsdu != 0xFF && v11nParams->amsdu < 2)
    {
        // implement the funciton
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_amsdu failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

    if(v11nParams->greenfield != 0xFF && v11nParams->greenfield < 2)
    {
        // implement the funciton
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "_set_greenfield failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

    if(v11nParams->mcs32!= 0xFF && v11nParams->mcs32 < 2 && v11nParams->mcs_fixedrate[0] != '\0')
    {
        // implement the funciton
        //st = wfaExecuteCLI(gCmdStr);
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_mcs failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }
    else if (v11nParams->mcs32!= 0xFF && v11nParams->mcs32 < 2 && v11nParams->mcs_fixedrate[0] == '\0')
    {
        // implement the funciton
        //st = wfaExecuteCLI(gCmdStr);
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_mcs32 failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }
    else if (v11nParams->mcs32 == 0xFF && v11nParams->mcs_fixedrate[0] != '\0')
    {
        // implement the funciton
        //st = wfaExecuteCLI(gCmdStr);
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_mcs32 failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

    if(v11nParams->rifs_test != 0xFF && v11nParams->rifs_test < 2)
    {
        // implement the funciton
        //st = wfaExecuteCLI(gCmdStr);
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_rifs_test failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

    if(v11nParams->sgi20 != 0xFF && v11nParams->sgi20 < 2)
    {
        // implement the funciton
        //st = wfaExecuteCLI(gCmdStr);
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_sgi20 failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

    if(v11nParams->smps != 0xFFFF)
    {
        if(v11nParams->smps == 0)
        {
            // implement the funciton
            //st = wfaExecuteCLI(gCmdStr);
        }
        else if(v11nParams->smps == 1)
        {
            // implement the funciton
            //st = wfaExecuteCLI(gCmdStr);
            ;
        }
        else if(v11nParams->smps == 2)
        {
            // implement the funciton
            //st = wfaExecuteCLI(gCmdStr);
            ;
        }
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_smps failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

    if(v11nParams->stbc_rx != 0xFFFF)
    {
        // implement the funciton
        //st = wfaExecuteCLI(gCmdStr);
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_stbc_rx failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

    if(v11nParams->width[0] != '\0')
    {
        // implement the funciton
        //st = wfaExecuteCLI(gCmdStr);
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_11n_channel_width failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

    if(v11nParams->_40_intolerant != 0xFF && v11nParams->_40_intolerant < 2)
    {
        // implement the funciton
        //st = wfaExecuteCLI(gCmdStr);
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_40_intolerant failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

    if(v11nParams->txsp_stream != 0 && v11nParams->txsp_stream <4)
    {
        // implement the funciton
        //st = wfaExecuteCLI(gCmdStr);
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_txsp_stream failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }

    }

    if(v11nParams->rxsp_stream != 0 && v11nParams->rxsp_stream < 4)
    {
        // implement the funciton
        //st = wfaExecuteCLI(gCmdStr);
        if(st != 0)
        {
            v11nParamsResp->status = STATUS_ERROR;
            strcpy(v11nParamsResp->cmdru.info, "set_rxsp_stream failed");
            wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, sizeof(dutCmdResponse_t), (BYTE *)v11nParamsResp, respBuf);
            *respLen = WFA_TLV_HDR_LEN + sizeof(dutCmdResponse_t);
            return FALSE;
        }
    }

#endif

    v11nParamsResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_11N_RESP_TLV, 4, (BYTE *)v11nParamsResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;
    return WFA_SUCCESS;
}
#endif
/*
 * wfaStaAddArpTableEntry():
 */
int wfaStaAddArpTableEntry(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* caStaAddARPTableEntry_t *staAddARPTableEntry= (caStaAddARPTableEntry_t *)caCmdBuf; uncomment and use it */

    printf("\n Entry wfastaAddARPTableEntry... ");
    // Implement the function and this does not return any thing back.

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_ADD_ARP_TABLE_ENTRY_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}

/*
 * wfaStaBlockICMPResponse():
 */
int wfaStaBlockICMPResponse(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    /* caStaBlockICMPResponse_t *staAddARPTableEntry= (caStaBlockICMPResponse_t *)caCmdBuf; uncomment and use it */

    printf("\n Entry wfaStaBlockICMPResponse... ");
    // Implement the function and this does not return any thing back.

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_P2P_BLOCK_ICMP_RESPONSE_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}

/*
 * wfaStaSetRadio():
 */

int wfaStaSetRadio(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCommand_t *setRadio = (dutCommand_t *)caCmdBuf;
    dutCmdResponse_t *staCmdResp = &gGenericResp;
    caStaSetRadio_t *sr = &setRadio->cmdsu.sr;

    if(sr->mode == WFA_OFF)
    {
        // turn radio off
    }
    else
    {
        // always turn the radio on
    }

    staCmdResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_RADIO_RESP_TLV, 4, (BYTE *)staCmdResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * wfaStaSetRFeature():
 */

int wfaStaSetRFeature(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCommand_t *dutCmd = (dutCommand_t *)caCmdBuf;
    caStaRFeat_t *rfeat = &dutCmd->cmdsu.rfeat;
    dutCmdResponse_t *caResp = &gGenericResp;
    char *ifname = dutCmd->intf;

    if(strcasecmp(rfeat->prog, "tdls") == 0)
    {

    }

    if(strcasecmp(rfeat->prog, "mbo") == 0)
    {
        printf("\n------------INSIDE MBO--------\n");
        printf("\n------------rfeat->cellulardatacap =%d--------\n", rfeat->cellulardatacap);
       sprintf(gCmdStr, "wpa_cli set mbo_cell_capa %d", ifname, rfeat->cellulardatacap);
       sret = shell_execute_cmd(NULL, gCmdStr);
       printf("\n %s \n ",gCmdStr);
       sleep(5);
       if (chan_buf2 != NULL)
       {
               sprintf(gCmdStr, "wpa_cli set non_pref_chan %s %s",
                       chan_buf1, chan_buf2);
	       sret = shell_execute_cmd(NULL, gCmdStr);
	}
      /* sprintf(gCmdStr, "wpa_cli set non_pref_chan %d:%d:%s:%d",
		rfeat->ch_op_class, rfeat->ch_pref_num, rfeat->ch_pref,
		rfeat->ch_reason_code);//AJAY
//      sprintf(gCmdStr, "wpa_cli set non_pref_chan 115:48:0:0", ifname);
        sret = shell_execute_cmd(NULL, gCmdStr);
//      sprintf(gCmdStr, "wpa_cli set non_pref_chan 115:44:1:1", ifname);
//      sret = system(gCmdStr);*/
      printf("\n %s \n", gCmdStr);

    }

    caResp->status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_SET_RFEATURE_RESP_TLV, 4, (BYTE *)caResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + 4;

    return WFA_SUCCESS;
}

/*
 * wfaStaStartWfdConnection():
 */
int wfaStaStartWfdConnection(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    //caStaStartWfdConn_t *staStartWfdConn= (caStaStartWfdConn_t *)caCmdBuf; //uncomment and use it

    printf("\n Entry wfaStaStartWfdConnection... ");


    // Fetch the GrpId and WFD session and return
    strcpy(&infoResp.cmdru.wfdConnInfo.wfdSessionId[0], "1234567890");
    strcpy(&infoResp.cmdru.wfdConnInfo.p2pGrpId[0], "WIFI_DISPLAY");
    strcpy(&infoResp.cmdru.wfdConnInfo.result[0], "GO");

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_START_WFD_CONNECTION_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}
/*
 * wfaStaCliCommand():
 */

int wfaStaCliCommand(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    char cmdName[32];
    char *pcmdStr=NULL, *str;
    int  st = 1;
    char CmdStr[WFA_CMD_STR_SZ];
    FILE *wfaCliFd;
    char wfaCliBuff[64];
    char retstr[256];
    int CmdReturnFlag =0;
    char tmp[256];
    FILE * sh_pipe;
    caStaCliCmdResp_t infoResp;

    printf("\nEntry wfaStaCliCommand; command Received: %s\n",caCmdBuf);
#if 0
    memcpy(cmdName, strtok_r((char *)caCmdBuf, ",", (char **)&pcmdStr), 32);
    sprintf(CmdStr, "%s",cmdName);

    for(;;)
    {
        // construct CLI standard cmd string
        str = strtok_r(NULL, ",", &pcmdStr);
        if(str == NULL || str[0] == '\0')
            break;
        else
        {
            sprintf(CmdStr, "%s /%s",CmdStr,str);
            str = strtok_r(NULL, ",", &pcmdStr);
            sprintf(CmdStr, "%s %s",CmdStr,str);
        }
    }
    // check the return process
    wfaCliFd=fopen("/etc/WfaEndpoint/wfa_cli.txt","r");
    if(wfaCliFd!= NULL)
    {
        while(fgets(wfaCliBuff, 64, wfaCliFd) != NULL)
        {
            //printf("\nLine read from CLI file : %s",wfaCliBuff);
            if(ferror(wfaCliFd))
                break;

            str=strtok(wfaCliBuff,"-");
            if(strcmp(str,cmdName) == 0)
            {
                str=strtok(NULL,",");
                if (str != NULL)
                {
                    if(strcmp(str,"TRUE") == 0)
                        CmdReturnFlag =1;
                }
                else
                    printf("ERR wfa_cli.txt, inside line format not end with , or missing TRUE/FALSE\n");
                break;
            }
        }
        fclose(wfaCliFd);
    }
    else
    {
        printf("/etc/WfaEndpoint/wfa_cli.txt is not exist\n");
        goto cleanup;
    }

    //printf("\n Command Return Flag : %d",CmdReturnFlag);
    memset(&retstr[0],'\0',255);
    memset(&tmp[0],'\0',255);
    sprintf(gCmdStr, "%s",  CmdStr);
    printf("\nCLI Command -- %s\n", gCmdStr);

    sh_pipe = popen(gCmdStr,"r");
    if(!sh_pipe)
    {
        printf ("Error in opening pipe\n");
        goto cleanup;
    }

    sleep(5);
    //tmp_val=getdelim(&retstr,255,"\n",sh_pipe);
    if (fgets(&retstr[0], 255, sh_pipe) == NULL)
    {
        printf("Getting NULL string in popen return\n");
        goto cleanup;
    }
    else
        printf("popen return str=%s\n",retstr);

    sleep(2);
    if(pclose(sh_pipe) == -1)
    {
        printf("Error in closing shell cmd pipe\n");
        goto cleanup;
    }
    sleep(2);

    // find status first in output
    str = strtok_r((char *)retstr, "-", (char **)&pcmdStr);
    if (str != NULL)
    {
        memset(tmp, 0, 10);
        memcpy(tmp, str,  2);
        printf("cli status=%s\n",tmp);
        if(strlen(tmp) > 0)
            st = atoi(tmp);
        else printf("Missing status code\n");
    }
    else
    {
        printf("wfaStaCliCommand no return code found\n");
    }
#endif
    infoResp.resFlag=CmdReturnFlag;
    st = 1;

cleanup:

    switch(st)
    {
    case 0:
        infoResp.status = STATUS_COMPLETE;
        if (CmdReturnFlag)
        {
            if((pcmdStr != NULL) && (strlen(pcmdStr) > 0) )
            {
                memset(&(infoResp.result[0]),'\0',WFA_CLI_CMD_RESP_LEN-1);
                strncpy(&infoResp.result[0], pcmdStr ,(strlen(pcmdStr) < WFA_CLI_CMD_RESP_LEN ) ? strlen(pcmdStr) : (WFA_CLI_CMD_RESP_LEN-2) );
                printf("Return CLI result string to CA=%s\n", &(infoResp.result[0]));
            }
            else
            {
                strcpy(&infoResp.result[0], "No return string found\n");
            }
        }
        break;
    case 1:
        infoResp.status = STATUS_ERROR;
        break;
    case 2:
        infoResp.status = STATUS_INVALID;
        break;
    }

    wfaEncodeTLV(WFA_STA_CLI_CMD_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    printf("Exit from wfaStaCliCommand\n");
    return TRUE;

}
/*
 * wfaStaConnectGoStartWfd():
 */

int wfaStaConnectGoStartWfd(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
//  caStaConnectGoStartWfd_t *staConnecGoStartWfd= (caStaConnectGoStartWfd_t *)caCmdBuf; //uncomment and use it

    printf("\n Entry wfaStaConnectGoStartWfd... ");

    // connect the specified GO and then establish the wfd session

    // Fetch WFD session and return
    strcpy(&infoResp.cmdru.wfdConnInfo.wfdSessionId[0], "1234567890");

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_CONNECT_GO_START_WFD_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}

/*
 * wfaStaGenerateEvent():
 */

int wfaStaGenerateEvent(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    caStaGenEvent_t *staGenerateEvent= (caStaGenEvent_t *)caCmdBuf; //uncomment and use it
    caWfdStaGenEvent_t *wfdGenEvent;

    printf("\n Entry wfaStaGenerateEvent... ");


    // Geneate the specified action and return with complete/error.
    if(staGenerateEvent->program == PROG_TYPE_WFD)
    {
        wfdGenEvent = &staGenerateEvent->wfdEvent;
        if(wfdGenEvent ->type == eUibcGen)
        {
        }
        else if(wfdGenEvent ->type == eUibcHid)
        {
        }
        else if(wfdGenEvent ->type == eFrameSkip)
        {

        }
        else if(wfdGenEvent ->type == eI2cRead)
        {
        }
        else if(wfdGenEvent ->type == eI2cWrite)
        {
        }
        else if(wfdGenEvent ->type == eInputContent)
        {
        }
        else if(wfdGenEvent ->type == eIdrReq)
        {
        }
    }

    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_GENERATE_EVENT_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}




/*
 * wfaStaReinvokeWfdSession():
 */

int wfaStaReinvokeWfdSession(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
//  caStaReinvokeWfdSession_t *staReinvokeSession= (caStaReinvokeWfdSession_t *)caCmdBuf; //uncomment and use it

    printf("\n Entry wfaStaReinvokeWfdSession... ");

    // Reinvoke the WFD session by accepting the p2p invitation   or sending p2p invitation


    infoResp.status = STATUS_COMPLETE;
    wfaEncodeTLV(WFA_STA_REINVOKE_WFD_SESSION_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
    *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

    return WFA_SUCCESS;
}


int wfaStaGetParameter(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{
    dutCmdResponse_t infoResp;
    caStaGetParameter_t *staGetParam= (caStaGetParameter_t *)caCmdBuf; //uncomment and use it


    caStaGetParameterResp_t *paramList = &infoResp.cmdru.getParamValue;

    printf("\n Entry wfaStaGetParameter... ");
    printf("\n Entry wfaStaGetParameter... ");
    printf("\n staGetParam... progra value = %d \n",staGetParam->program);
    printf("\n staGetParam... param value = %d \n",staGetParam->getParamValue);
     // Check the program type TODO 
    if(staGetParam->program == 0)
      {
        if(staGetParam->getParamValue == 0 )
          {
             // Get the measured RSSI
             //wpa_cli signal_poll|grep AVG_RSSI|cut -f2 -d=
             sprintf(gCmdStr, "wpa_cli -i wlan0 signal_poll|grep AVG_RSSI|cut -f2 -d=");
             printf("\n %s \n", gCmdStr);
             sret = system(gCmdStr);
             strcpy((char *)&paramList->masterPref, "-38");
           }
       }


    // Check the program type
    if(staGetParam->program == PROG_TYPE_WFD)
    {
        if(staGetParam->getParamValue == eDiscoveredDevList )
        {
            // Get the discovered devices, make space seperated list and return, check list is not bigger than 128 bytes.
            paramList->getParamType = eDiscoveredDevList;
            strcpy((char *)&paramList->devList, "11:22:33:44:55:66 22:33:44:55:66:77 33:44:55:66:77:88");
        }
    }

	if(staGetParam->program == PROG_TYPE_WFDS)
	{

		if(staGetParam->getParamValue == eDiscoveredDevList )
		{
			// Get the discovered devices, make space seperated list and return, check list is not bigger than 128 bytes.
			paramList->getParamType = eDiscoveredDevList;
			strcpy((char *)&paramList->devList, "11:22:33:44:55:66 22:33:44:55:66:77 33:44:55:66:77:88");
			
		}
		if(staGetParam->getParamValue == eOpenPorts)
		{
			// Run the port checker tool 
			// Get all the open ports and make space seperated list and return, check list is not bigger than 128 bytes.
			paramList->getParamType = eOpenPorts;
			strcpy((char *)&paramList->devList, "22 139 445 68 9700");
			
		}
		
	}
	if(staGetParam->program == PROG_TYPE_NAN)
   	{
      if(staGetParam->getParamValue == eMasterPref )
      {
          // Get the master preference of the device and return the value
          paramList->getParamType = eMasterPref;
          strcpy((char *)&paramList->masterPref, "0xff");
      }
    }

	infoResp.status = STATUS_COMPLETE;
	wfaEncodeTLV(WFA_STA_GET_PARAMETER_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);	
	*respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);
	
   return WFA_SUCCESS;
}


int wfaStaNfcAction(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{

	dutCmdResponse_t infoResp;
	caStaNfcAction_t *getStaNfcAction = (caStaNfcAction_t *)caCmdBuf;  //uncomment and use it
	
	 printf("\n Entry wfaStaNfcAction... ");

	if(getStaNfcAction->nfcOperation == eNfcHandOver)
	{
		printf("\n NfcAction - HandOver... ");
	
	}
	else if(getStaNfcAction->nfcOperation == eNfcReadTag)
	{
		printf("\n NfcAction - Read Tag... ");

	}
	else if(getStaNfcAction->nfcOperation == eNfcWriteSelect)
	{
		printf("\n NfcAction - Write Select... ");
	
	}
	else if(getStaNfcAction->nfcOperation == eNfcWriteConfig)
	{
		printf("\n NfcAction - Write Config... ");
	
	}
	else if(getStaNfcAction->nfcOperation == eNfcWritePasswd)
	{
		printf("\n NfcAction - Write Password... ");
	
	}
	else if(getStaNfcAction->nfcOperation == eNfcWpsHandOver)
	{
		printf("\n NfcAction - WPS Handover... ");
	
	}
	
	 // Fetch the device mode and put in	 infoResp->cmdru.p2presult 
	 //strcpy(infoResp->cmdru.p2presult, "GO");
	
	 // Fetch the device grp id and put in	 infoResp->cmdru.grpid 
	 //strcpy(infoResp->cmdru.grpid, "AA:BB:CC:DD:EE:FF_DIRECT-SSID");
	 
	 strcpy(infoResp.cmdru.staNfcAction.result, "CLIENT");
	 strcpy(infoResp.cmdru.staNfcAction.grpId, "AA:BB:CC:DD:EE:FF_DIRECT-SSID");
	 infoResp.cmdru.staNfcAction.peerRole = 1;
	
	


	infoResp.status = STATUS_COMPLETE;
	wfaEncodeTLV(WFA_STA_NFC_ACTION_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf); 
	*respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);
	
   return WFA_SUCCESS;
}

int wfaStaExecAction(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{

	dutCmdResponse_t infoResp;
	caStaExecAction_t *staExecAction = (caStaExecAction_t *)caCmdBuf;  //comment if not used
	
	 printf("\n Entry wfaStaExecAction... \n");

	if(staExecAction->prog == PROG_TYPE_NAN)
	{
		// Perform necessary configurations and actions
		// return the MAC address conditionally as per CAPI specification
	}
	
	infoResp.status = STATUS_COMPLETE;
	wfaEncodeTLV(WFA_STA_EXEC_ACTION_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf); 
	*respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);
	
   return WFA_SUCCESS;
}


int wfaStaScan(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{

        dutCmdResponse_t infoResp;
        caStaScan_t *staScan = (caStaScan_t *)caCmdBuf;  //comment if not used
	char *ifname = staScan->intf;
        printf("\n Entry wfaStaScan ...\n ");

	// SCAN command

	sprintf(gCmdStr, "wifi scan");
        sret = system(gCmdStr);
        printf("\n %s \n", gCmdStr);
        sleep(2);

        infoResp.status = STATUS_COMPLETE;
        wfaEncodeTLV(WFA_STA_SCAN_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf);
        *respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);

   return WFA_SUCCESS;
}

int wfaStaInvokeCommand(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{

	dutCmdResponse_t infoResp;
	caStaInvokeCmd_t *staInvokeCmd = (caStaInvokeCmd_t *)caCmdBuf;  //uncomment and use it
	
	 printf("\n Entry wfaStaInvokeCommand...\n ");


	 // based on the command type , invoke API or complete the required procedures
	 // return the  defined parameters based on the command that is received ( example response below)

	if(staInvokeCmd->cmdType == ePrimitiveCmdType && staInvokeCmd->InvokeCmds.primtiveType.PrimType == eCmdPrimTypeAdvt )
	{
		 infoResp.cmdru.staInvokeCmd.invokeCmdRspType = eCmdPrimTypeAdvt;
		 infoResp.cmdru.staInvokeCmd.invokeCmdResp.advRsp.numServInfo = 1;
		 strcpy(infoResp.cmdru.staInvokeCmd.invokeCmdResp.advRsp.servAdvInfo[0].servName,"org.wi-fi.wfds.send.rx");
		 infoResp.cmdru.staInvokeCmd.invokeCmdResp.advRsp.servAdvInfo[0].advtID = 0x0000f;
		 strcpy(infoResp.cmdru.staInvokeCmd.invokeCmdResp.advRsp.servAdvInfo[0].serviceMac,"ab:cd:ef:gh:ij:kl");
	}
	else if (staInvokeCmd->cmdType == ePrimitiveCmdType && staInvokeCmd->InvokeCmds.primtiveType.PrimType == eCmdPrimTypeSeek)
	{
		infoResp.cmdru.staInvokeCmd.invokeCmdRspType = eCmdPrimTypeSeek;
		infoResp.cmdru.staInvokeCmd.invokeCmdResp.seekRsp.searchID = 0x000ff;	
	}
	else if (staInvokeCmd->cmdType == ePrimitiveCmdType && staInvokeCmd->InvokeCmds.primtiveType.PrimType == eCmdPrimTypeConnSession)
	{
		infoResp.cmdru.staInvokeCmd.invokeCmdRspType = eCmdPrimTypeConnSession;
		infoResp.cmdru.staInvokeCmd.invokeCmdResp.connSessResp.sessionID = 0x000ff;  
		strcpy(infoResp.cmdru.staInvokeCmd.invokeCmdResp.connSessResp.result,"GO");
		strcpy(infoResp.cmdru.staInvokeCmd.invokeCmdResp.connSessResp.grpId,"DIRECT-AB WFADUT");
	
	}	
	infoResp.status = STATUS_COMPLETE;
	wfaEncodeTLV(WFA_STA_INVOKE_CMD_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf); 
	*respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);
	
   return WFA_SUCCESS;
}


int wfaStaManageService(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{

	dutCmdResponse_t infoResp;
	//caStaMngServ_t *staMngServ = (caStaMngServ_t *)caCmdBuf;  //uncomment and use it
	
	 printf("\n Entry wfaStaManageService... ");

	// based on the manage service type , invoke API's or complete the required procedures
	// return the  defined parameters based on the command that is received ( example response below)
	strcpy(infoResp.cmdru.staManageServ.result, "CLIENT");
	strcpy(infoResp.cmdru.staManageServ.grpId, "AA:BB:CC:DD:EE:FF_DIRECT-SSID");
    infoResp.cmdru.staManageServ.sessionID = 0x000ff;

	infoResp.status = STATUS_COMPLETE;
	wfaEncodeTLV(WFA_STA_MANAGE_SERVICE_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf); 
	*respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);
	
   return WFA_SUCCESS;
}


	
int wfaStaGetEvents(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{

	dutCmdResponse_t infoResp;
	caStaGetEvents_t *staGetEvents = (caStaGetEvents_t *)caCmdBuf;  //uncomment and use it
	
	 printf("\n Entry wfaStaGetEvents... ");
	 
	 if(staGetEvents->program == PROG_TYPE_NAN)
	{ 
		// Get all the events from the Log file or stored events
		// return the  received/recorded event details - eventName, remoteInstanceID, localInstanceID, mac
	}

	// Get all the event from the Log file or stored events
	// return the  received/recorded events as space seperated list   ( example response below)
	strcpy(infoResp.cmdru.staGetEvents.result, "SearchResult SearchTerminated AdvertiseStatus SessionRequest ConnectStatus SessionStatus PortStatus");
	
	infoResp.status = STATUS_COMPLETE;
	wfaEncodeTLV(WFA_STA_GET_EVENTS_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf); 
	*respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);
	
   return WFA_SUCCESS;
}

int wfaStaGetEventDetails(int len, BYTE *caCmdBuf, int *respLen, BYTE *respBuf)
{

	dutCmdResponse_t infoResp;
	caStaGetEventDetails_t *getStaGetEventDetails = (caStaMngServ_t *)caCmdBuf;  //uncomment and use it
	
	 printf("\n Entry wfaStaGetEventDetails... ");


	 // based on the Requested Event type
	 // return the latest corresponding evnet detailed parameters  ( example response below)

	if(getStaGetEventDetails->eventId== eSearchResult )
	{
		// fetch from log file or event history for the search result event and return the parameters
		infoResp.cmdru.staGetEventDetails.eventID= eSearchResult;

		infoResp.cmdru.staGetEventDetails.getEventDetails.searchResult.searchID = 0x00abcd;
		strcpy(infoResp.cmdru.staGetEventDetails.getEventDetails.searchResult.serviceMac,"ab:cd:ef:gh:ij:kl");
		infoResp.cmdru.staGetEventDetails.getEventDetails.searchResult.advID = 0x00dcba;
		strcpy(infoResp.cmdru.staGetEventDetails.getEventDetails.searchResult.serviceName,"org.wi-fi.wfds.send.rx");

		infoResp.cmdru.staGetEventDetails.getEventDetails.searchResult.serviceStatus = eServiceAvilable;
	}
	else if (getStaGetEventDetails->eventId == eSearchTerminated)
	{		// fetch from log file or event history for the search terminated event and return the parameters
		infoResp.cmdru.staGetEventDetails.eventID= eSearchTerminated;	
		infoResp.cmdru.staGetEventDetails.getEventDetails.searchTerminated.searchID = 0x00abcd;
	}
	else if (getStaGetEventDetails->eventId == eAdvertiseStatus)
	{// fetch from log file or event history for the Advertise Status event and return the parameters
		infoResp.cmdru.staGetEventDetails.eventID= eAdvertiseStatus;	
		infoResp.cmdru.staGetEventDetails.getEventDetails.advStatus.advID = 0x00dcba;

		infoResp.cmdru.staGetEventDetails.getEventDetails.advStatus.status = eAdvertised;	
	}	
	else if (getStaGetEventDetails->eventId == eSessionRequest)
	{// fetch from log file or event history for the session request event and return the parameters
		infoResp.cmdru.staGetEventDetails.eventID= eSessionRequest;	
		infoResp.cmdru.staGetEventDetails.getEventDetails.sessionReq.advID = 0x00dcba;
		strcpy(infoResp.cmdru.staGetEventDetails.getEventDetails.sessionReq.sessionMac,"ab:cd:ef:gh:ij:kl");
		infoResp.cmdru.staGetEventDetails.getEventDetails.sessionReq.sessionID = 0x00baba;	
	}	
	else if (getStaGetEventDetails->eventId ==eSessionStatus )
	{// fetch from log file or event history for the session status event and return the parameters
		infoResp.cmdru.staGetEventDetails.eventID= eSessionStatus;	
		infoResp.cmdru.staGetEventDetails.getEventDetails.sessionStatus.sessionID = 0x00baba;	
		strcpy(infoResp.cmdru.staGetEventDetails.getEventDetails.sessionStatus.sessionMac,"ab:cd:ef:gh:ij:kl");
		infoResp.cmdru.staGetEventDetails.getEventDetails.sessionStatus.state = eSessionStateOpen;	
	}	
	else if (getStaGetEventDetails->eventId == eConnectStatus)
	{
		infoResp.cmdru.staGetEventDetails.eventID= eConnectStatus;	
		infoResp.cmdru.staGetEventDetails.getEventDetails.connStatus.sessionID = 0x00baba;	
		strcpy(infoResp.cmdru.staGetEventDetails.getEventDetails.connStatus.sessionMac,"ab:cd:ef:gh:ij:kl");
		infoResp.cmdru.staGetEventDetails.getEventDetails.connStatus.status = eGroupFormationComplete;	
	
	}	
	else if (getStaGetEventDetails->eventId == ePortStatus)
	{
		infoResp.cmdru.staGetEventDetails.eventID= ePortStatus;	
		infoResp.cmdru.staGetEventDetails.getEventDetails.portStatus.sessionID = 0x00baba;	
		strcpy(infoResp.cmdru.staGetEventDetails.getEventDetails.portStatus.sessionMac,"ab:cd:ef:gh:ij:kl");
		infoResp.cmdru.staGetEventDetails.getEventDetails.portStatus.port = 1009;
		infoResp.cmdru.staGetEventDetails.getEventDetails.portStatus.status = eLocalPortAllowed;	
	}	



	infoResp.status = STATUS_COMPLETE;
	wfaEncodeTLV(WFA_STA_GET_EVENT_DETAILS_RESP_TLV, sizeof(infoResp), (BYTE *)&infoResp, respBuf); 
	*respLen = WFA_TLV_HDR_LEN + sizeof(infoResp);
	
   return WFA_SUCCESS;
}

	


