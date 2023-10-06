#!/bin/bash
cd /root/olympic
/etc/init.d/aftpd start
ifconfig wlan0 192.168.0.120
wpa_supplicant -iwlan0 -c/etc/wpa_supplicant/wpa_supplicant.conf -B
./connectionTest
