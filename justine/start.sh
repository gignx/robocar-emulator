rcemu/src/smartcity --osm=debrecen.osm --city=Debrecen --shm=justineSharedMemory --node2gps=../debrecen-lmap.txt&

#--osm=debrecen.osm  --node2gps=lmap.txt&
#--osm=../debrecen.osm --city=Debrecen --shm=justineSharedMemory --node2gps=../debrecen-lmap.txt
sleep 8
rcemu/src/traffic --port=10007 --shm=justineSharedMemory&
sleep 3
java -Xmx4096M -Xms4096M -jar rcwin/target/site/justine-rcwin-0.0.16-jar-with-dependencies.jar ../debrecen-lmap.txt
##-Xmx2048M -Xms2048M