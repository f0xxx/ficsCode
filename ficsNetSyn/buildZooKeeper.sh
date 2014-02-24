#!/bin/sh
tar zxvf zookeeper-3.4.5.tar.gz
rm -R zookeeper
mv zookeeper-3.4.5 zookeeper
cd zookeeper/src/c
./configure
make
make install
cp /usr/local/lib/libzookeeper_mt.so /usr/local/lib/libs/
cp /usr/local/lib/libzookeeper_mt.so.2.0.0 /usr/local/lib/libs/