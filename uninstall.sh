#!/bin/sh
# delete
rm -rf /opt/infosec-heartbeat > /dev/null 2>&1
rm -rf /etc/ha.d > /dev/null 2>&1
rm -rf /etc/ld.so.conf.d/hb_runtimes.conf > /dev/null 2>&1
rm -rf /etc/init.d/infosec-heartbeat.service > /dev/null 2>&1
ldconfig > /dev/null 2>&1
# rm /usr/local/lib/libprotobuf*
