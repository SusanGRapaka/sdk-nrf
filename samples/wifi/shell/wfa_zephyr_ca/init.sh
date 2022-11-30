#!/bin/bash -x

killall -9 wfa_ca
killall -9 wfa_dut

ifconfig wlan0 192.165.100.166 up
./dut/wfa_dut enx1027f551734c 8000 &
export WFA_ENV_AGENT_IPADDR=10.90.48.182
export WFA_ENV_AGENT_PORT=8000
sleep 1
./ca/wfa_ca enx1027f551734c 9000 &
sleep 1
ps aux|grep wfa
ps aux|grep wpa

