#
# Copyright (c) 2022 Nordic Semiconductor ASA
#
# SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(nrf_wifi_shell)

set(WIFI_NRF_DRV_BASE ${WEST_TOPDIR}/nrf/drivers/wifi/nrf700x)
if(CONFIG_WPA_SUPP)
set(WPA_SUPP_BASE ${WEST_TOPDIR}/modules/lib/hostap)
endif()

# TODO: MbedTLS Remove this workaround
target_include_directories(mbedcrypto_common
	INTERFACE
	${CMAKE_SOURCE_DIR}
	inc
)

#zephyr_library_compile_definitions(
	#	COMMAND_BYTE_STREAM
	#)

zephyr_library_compile_definitions(WFA_STA_TB)
target_sources(app PRIVATE
	src/wifi_main.c
	src/wfa_dut_shell.c
	src/dut/wfa_sock.c
	src/dut/wfa_tg.c
	src/dut/wfa_cs.c
	src/dut/wfa_ca_resp.c
	src/dut/wfa_tlv.c
	src/dut/wfa_typestr.c
	src/dut/wfa_cmdtbl.c
	src/dut/wfa_cmdproc.c
	src/dut/wfa_miscs.c
	src/dut/wfa_thr.c
	src/dut/wfa_wmmps.c
	src/dut/wfa_dut_init.c
	src/dut/wfa_dut.c
)
