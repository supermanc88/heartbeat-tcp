#!/bin/sh
# delete
rm -rf /opt/infosec-heartbeat
rm -rf /etc/ha.d
rm -rf /etc/ld.so.conf.d/hb_runtimes.conf
ldconfig
# rm /usr/local/lib/libprotobuf*