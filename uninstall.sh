#!/bin/sh
# delete
rm -rf /opt/infosec-heartbeat
rm -rf /etc/ha.d
rm -rf /etc/ld.so.conf.d/hb_runtimes.conf
ldconfig > /dev/null
# rm /usr/local/lib/libprotobuf*
