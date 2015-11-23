rcemu/src/smartcity --osm=debrecen.osm  --node2gps=lmap.txt&
sleep 8
rcemu/src/traffic&
sleep 3
java -Xmx4096M -Xms4096M -jar rcwin/target/site/justine-rcwin-0.0.16-jar-with-dependencies.jar lmap.txt
