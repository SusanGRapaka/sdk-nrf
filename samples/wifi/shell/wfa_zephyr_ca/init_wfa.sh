#ifconfig eno1 192.168.250.146 up
killall -9 wpa_supplicant
sleep 1
rfkill unblock all
iwconfig wlan0 power off
killall -9 wpa_supplicant
killall -9 wfa_ca
killall -9 wfa_dut
sleep 1
ifconfig wlan0 192.168.1.117 up
sleep 1
#./dut/wfa_dut enp0s31f6 8000 &
./dut/wfa_dut enxd0374588ab12 8000 &
export WFA_ENV_AGENT_IPADDR=192.168.250.116
export WFA_ENV_AGENT_PORT=8000
sleep 1
#./ca/wfa_ca enp0s31f6 9000 &
./ca/wfa_ca enxd0374588ab12 9000 &
sleep 1
wpa_supplicant -Dnl80211 -c /home/user/work/stqa/donatello/certification/cert.conf -i wlan0 -ddddK -f supplog.txt -B
sleep 1
route add -net 224.0.0.0 netmask 240.0.0.0 dev wlan0
ps aux|grep wfa
ps aux|grep wpa
