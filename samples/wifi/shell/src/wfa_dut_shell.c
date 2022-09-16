
/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/* @file
 * @brief WFA-DUT shell sample
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <zephyr/zephyr.h>
#include <zephyr/shell/shell.h>
#include <zephyr/init.h>

static int cmd_wfa_dut_test(const struct shell *shell,
			  size_t argc,
			  const char *argv[])
{
	printf("arg0: %s\n", argv[0]);
	return dut_main(argc,
			argv);
}

static int cmd_wfa_dut(const struct shell *shell,
			  size_t argc,
			  const char *argv[])
{
	printf("arg0: %s\n", argv[0]);
	unsigned char *respBuf = os_zalloc(64 * sizeof(unsigned char));
	int bufLen = 0;
	int streamid = 1;
	unsigned char streamId = streamid; 
	int status = -1;
	char * cmdBuf = argv[1];
	status = commandHandle(cmdBuf);
	return status;
}

static int cmd_wfa_dut_traffic_config(const struct shell *shell,
			  size_t argc,
			  const char *argv[])
{
	printf("arg0: %s\n", argv[0]);
	unsigned char *respBuf = os_zalloc(64 * sizeof(unsigned char));
	int bufLen = 0;
	int status = -1;
	char * cmdBuf = argv[1];
	status = commandHandle(cmdBuf);
	return status;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
	wfa_dut_cmds,
	SHELL_CMD(dut_test,
		  NULL,
		  "\"Start DUT\"",
		  cmd_wfa_dut_test),
	SHELL_CMD(dut_traffic_config,
		  NULL,
		  "\"Set Traffic Config params\"",
		  cmd_wfa_dut_traffic_config),
	SHELL_CMD(dut_send_start,
		  NULL,
		  "\"Send Traffic Start\"",
		  cmd_wfa_dut),
	SHELL_CMD(dut_receive_start,
		  NULL,
		  "\"Receive Traffic Start\"",
		  cmd_wfa_dut),
	SHELL_CMD(dut_receive_stop,
		  NULL,
		  "\"Receive Traffic Stop\"",
		  cmd_wfa_dut),
	SHELL_SUBCMD_SET_END
);

/* "wfa_dut" shell tool for Wi-Fi alliance certification test */
SHELL_CMD_REGISTER(wfa_dut,
		   &wfa_dut_cmds,
		   "wfa_dut_commands (only for internal use)",
		   NULL);
