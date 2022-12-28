/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** @file
 * @brief WiFi shell sample main function
 */

#include <stdio.h>
#include <zephyr/sys/printk.h>
#include <nrfx_clock.h>
#include <zephyr/device.h>
#include <zephyr/net/net_config.h>

void main(void)
{
#ifdef CLOCK_FEATURE_HFCLK_DIVIDE_PRESENT
	/* For now hardcode to 128MHz */
	nrfx_clock_divider_set(NRF_CLOCK_DOMAIN_HFCLK,
			       NRF_CLOCK_HFCLK_DIV_1);
#endif
	printk("Starting %s with CPU frequency: %d MHz\n", CONFIG_BOARD, SystemCoreClock/MHZ(1));

/* Test bench for serial agent */
#if 0
	for(int i = 0; i < 10; i++) {
		size_t buf_size = 500 + i * 100;

		printf("Trying buf size: %d\n", buf_size);

		char *buf = malloc(buf_size);
		if (!buf) {
			printf("Failed to alloc: %d\n", buf_size);
			continue;
		}
		memset(buf, 0XAA + i, buf_size);
		printf("START\n");
		for (int j = 0; j < buf_size; j++)
			printf("%02X", buf[j]);
		printf("END\n");
		free(buf);
	}
	exit(0);
#endif
}
