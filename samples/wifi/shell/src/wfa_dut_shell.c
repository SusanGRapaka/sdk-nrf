
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
#include "wfa_main.h"
#include "wfa_debug.h"



static int cmd_wfa_dut_test(const struct shell *shell,
			  size_t argc,
			  const char *argv[])
{
	printf("arg0: %s\n", argv[0]);
	return dut_main(argc,
			argv);
}

unsigned char cmdBuf[WFA_BUFF_512] = {0};

static int wfa_dut_execute(const struct shell *shell,
			  size_t argc,
			  const char *argv[])
{
	printf("arg0: %s\n", argv[0]);
	int status = -1;

#if COMMAND_BYTE_STREAM
	hex_str_to_val(cmdBuf, sizeof(cmdBuf), argv[1]);
#else
	status = cmd_to_hex(argv[1], cmdBuf);
#endif /* COMMAND_BYTE_STREAM */
	status = commandHandle(cmdBuf);
	return status;
}

SHELL_STATIC_SUBCMD_SET_CREATE(
	wfa_dut_cmds,
	SHELL_CMD(dut_test_setup,
		  NULL,
		  "\"Start DUT\"",
		  cmd_wfa_dut_test),
	SHELL_CMD(dut_command,
		  NULL,
		  "\"Sets Traffic params or runs traffic\"",
		  wfa_dut_execute),
	SHELL_SUBCMD_SET_END
);

/* "wfa_dut" shell tool for Wi-Fi alliance certification test */
SHELL_CMD_REGISTER(wfa_dut,
		   &wfa_dut_cmds,
		   "wfa_dut_commands (only for internal use)",
		   NULL);
