#!/bin/sh
packagePath="/root/heartbeat"
if [ ! -d "$packagePath" ]
then
	mkdir "$packagePath"
fi

pluginsPath=$packagePath/"plugins"

#echo $pluginsPath
if [ ! -d "$pluginsPath" ]
then
	mkdir "$pluginsPath"
fi

cp ./build/bin/* $packagePath
cp ./build/lib/plugins/* $pluginsPath
cp ./heartbeat-framework/ha.cf $packagePath
