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

cp ./build/bin/* $binPath
cp ./build/lib/plugins/* $pluginsPath
cp ./heartbeat-framework/ha.cf $rootPath
cp ./heartbeat-framework/haresources $rootPath
cp install.sh $rootPath
cp uninstall.sh $rootPath
cp /usr/local/lib/libprotobuf.so.23.0.3 $runtimes
cp ./heartbeat-framework/policy* $policyPath

