killall wpa_supplicant
#ifconfig eth1 192.168.250.242 up
#sleep 1
#iwconfig wlan0 power off
#rfkill unblock all
killall -9 wpa_supplicant
killall -9 wfa_ca
killall -9 wfa_dut
sleep 1
#./dut/wfa_dut eth0 8000 &

serial_agent 'wfa_dut dut_test_setup wlan0 8000'

export WFA_ENV_AGENT_IPADDR=10.90.49.79
export WFA_ENV_AGENT_PORT=8000
sleep 1
./ca/wfa_ca enx1027f5513d57 9000 &
#sleep 1
#./wpa3_suiteb_WFA_DUT_donatello_kranthi/wpa_supplicant -Dnl80211 -c cert.conf -i wlan0 -d -K -f supp_log_suiteb_test.txt -B
#wpa_supplicant -Dnl80211 -c cert.conf -i wlan0 -d -K -f log_calder.txt -B
#cd /home/user/work/stqa/certification/WPA3_SuiteB/wpa_supplicant-2.9/wpa_supplicant
#wpa_supplicant -Dnl80211 -c wpa3.conf -i wlan0 -d -K -f supp_log_11ax__test.txt -B

sleep 1
ps aux|grep wfa
ps aux|grep wpa
