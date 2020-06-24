#!/bin/sh
installRootPath="/opt/infosec-heartbeat"

if [ ! -d "$installRootPath" ]
then
	mkdir "$installRootPath"
fi

binPath=$installRootPath/"bin"

if [ ! -d $binPath" ]
then
	mkdir "$binPath"
fi

pluginsPath=$installRootPath/"plugins"

if [ ! -d "$plugins" ]
then
	mkdir "$pluginsPath"
fi

# install to the directory
cp ./bin/* $binPath
cp ./plugins/* $pluginsPath
cp ha.cf $installRootPath
cp uninstall.sh $installRootPath


# dependent libraries
cp ./runtimes/* /usr/local/lib/
ln -s libprotobuf.so.23.0.3 libprotobuf.so.23
ln -s libprotobuf.so.23.0.3 libprotobuf.so
ldconfig

