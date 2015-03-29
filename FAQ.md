# Robocar World Championship Wiki #

## Installation on Ubuntu ##

### Question ###

**What packages are required to build Robocar City Emulator Platform on Ubuntu 14.04.1 LTS?**

#### Answer ####

```
sudo apt-get install git
sudo apt-get install autoconf 
sudo apt-get install libtool
sudo apt-get install protobuf-compiler libprotobuf-dev
sudo apt-get install g++
sudo apt-get install libboost-all-dev
sudo apt-get install flex
```

for libosmpbf: (see also https://github.com/scrosby/OSM-binary/blob/master/README)

```
nbatfai@matrica:~$ mkdir OSM-binary
nbatfai@matrica:~$ cd OSM-binary/
nbatfai@matrica:~/OSM-binary$ git clone https://github.com/scrosby/OSM-binary.git
nbatfai@matrica:~/OSM-binary$ cd OSM-binary/
nbatfai@matrica:~/OSM-binary/OSM-binary$ make -C src
nbatfai@matrica:~/OSM-binary/OSM-binary$ sudo make -C src install
```

for libosmium: (see also https://github.com/osmcode/libosmium/blob/master/README.md)

```
sudo apt-get install libexpat1-dev
sudo apt-get install zlib1g-dev
sudo apt-get install libbz2-dev
sudo apt-get install libsparsehash-dev
sudo apt-get install libgdal1-dev
sudo apt-get install libgeos++-dev
sudo apt-get install libproj-dev
sudo apt-get install doxygen graphviz xmlstarlet
sudo apt-get install cmake
```

```
nbatfai@matrica:~$ mkdir libosmium
nbatfai@matrica:~$ cd libosmium/
nbatfai@matrica:~/libosmium$ git clone https://github.com/osmcode/libosmium.git
nbatfai@matrica:~/libosmium$ cd libosmium/
nbatfai@matrica:~/libosmium/libosmium$ cmake .
nbatfai@matrica:~/libosmium/libosmium$ make
nbatfai@matrica:~/libosmium/libosmium$ sudo make install
```

### Question ###

The configure script complains about the following:

```
Can't exec "libtoolize": No such file or directory at /usr/share/autoconf/Autom4te/FileUtils.pm line 345, <GEN2> line 4.
autoreconf: failed to run libtoolize: No such file or directory
```

#### Answer ####

`sudo apt-get install libtool`

### Question ###

The configure script complains about the following:

```
checking for protobuf... no
configure: error: Package requirements (protobuf) were not met:

No package 'protobuf' found
```

#### Answer ####

`sudo apt-get install protobuf-compiler libprotobuf-dev`

### Question ###

The configure script complains about the following:

```
checking for Boost headers version >= 1.46.0... no
configure: error: cannot find Boost headers version >= 1.46.0
```

#### Answer ####

`sudo apt-get install libboost-all-dev`

### Question ###

Make exits with the following error:

```
./osmreader.hpp:35:35: fatal error: osmium/io/any_input.hpp: No such file or directory
 #include <osmium/io/any_input.hpp>
```

#### Answer ####

```
nbatfai@matrica:~$ mkdir libosmium
nbatfai@matrica:~$ cd libosmium/
nbatfai@matrica:~/libosmium$ git clone https://github.com/osmcode/libosmium.git
nbatfai@matrica:~/libosmium$ cd libosmium/
nbatfai@matrica:~/libosmium/libosmium$ cmake .
nbatfai@matrica:~/libosmium/libosmium$ make
nbatfai@matrica:~/libosmium/libosmium$ sudo make install
```

## Installation on Fedora ##

## Installation on Windows ##