#!/bin/sh
rootPath="/root/heartbeat"
if [ ! -d "$rootPath" ]
then
	mkdir "$rootPath"
fi

binPath=$rootPath/"bin"
if [ ! -d "$binPath" ]
then
	mkdir "$binPath"
fi

pluginsPath=$rootPath/"plugins"

#echo $pluginsPath
if [ ! -d "$pluginsPath" ]
then
	mkdir "$pluginsPath"
fi

runtimes=$rootPath/"runtimes"

if [ ! -d "$runtimes" ]
then
	mkdir "$runtimes"
fi

policyPath=$rootPath/"policy"

if [ ! -d "$policyPath" ]
then
  mkdir "$policyPath"
fi

echo "copy files"
cp ./build/bin/* $binPath
cp ./build/lib/plugins/* $pluginsPath
cp ./heartbeat-framework/ha.cf $rootPath
cp ./heartbeat-framework/haresources $rootPath
cp install.sh $rootPath
cp uninstall.sh $rootPath
cp infosec-heartbeat.service $rootPath
cp /usr/local/lib/libprotobuf.so.23.0.3 $runtimes
# gcc 4.8.1
#cp /usr/local/lib64/libstdc++.so.6.0.18 $runtimes
# gcc 4.9.2
cp /usr/local/lib64/libstdc++.so.6.0.20 $runtimes
cp ./heartbeat-framework/resource-mgr/policy* $policyPath
cp ./heartbeat-framework/manual-switch/*.sh $rootPath
echo "copy complete!"

echo "start pack files to tar.gz"
cd /root/
rm infosec-heartbeat.tar.gz
tar -czvf infosec-heartbeat.tar.gz ./heartbeat/
echo "pack complete!"

rm heartbeat -rf
