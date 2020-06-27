#/bin/sh
installRootPath="/opt/infosec-heartbeat"

if [ ! -d "$installRootPath" ]
then
	mkdir "$installRootPath"
fi

binPath=$installRootPath/"bin"

if [ ! -d "$binPath" ]
then
	mkdir "$binPath"
fi

pluginsPath=$installRootPath/"plugins"

if [ ! -d "$pluginsPath" ]
then
	mkdir "$pluginsPath"
fi

hadir="/etc/ha.d"

if [ ! -d "$hadir" ]
then
  mkdir "$hadir"
fi

policyPath=$hadir/"policy"

if [ ! -d "$policyPath" ]
then
  mkdir "$policyPath"
fi

# install to the directory
cp ./bin/* $binPath
cp ./plugins/* $pluginsPath
cp ha.cf $installRootPath
cp ha.cf $hadir
cp haresources $installRootPath
cp uninstall.sh $installRootPath
cp ./policy/* $policyPath


# dependent libraries
cp ./runtimes/* /usr/local/lib/
cd /usr/local/lib
ln -s libprotobuf.so.23.0.3 libprotobuf.so.23
ln -s libprotobuf.so.23.0.3 libprotobuf.so
ldconfig

