# Robocar Emulator

### Foreword

*The file `README_ORIGINAL.md` contains all information related to the Robocar World Championship and the research platform it uses. This file focuses on making things work to participate in the Championship.*

### How to kickstart this stuff?

You should start with checking whether you have all the dependencies required, please refer to the `README_ORIGINAL.md` file.

From now on, we assume that you were able to `make` the contents of `rcemu` and build the `.jar` from `rcwin`.

#### Smartcity
After acquiring the right `.osm` file, the first thing you start should be the `smartcity`, somehow like this:

```
 rcemu$ src/smartcity --osm=../berlin.osm --city=Berlin --shm=BerlinSharedMemory --node2gps=../berlin-lmap.txt
```

After the `--osm` you must the `.osm` file to run the simulation on. Make sure you specify the same `shm` option from now on to all applications.

#### Traffic
We have our playground, let's get some toys! Or something that can handle our toys, that's what `traffic` is:

```
rcemu$ src/traffic --shm=BerlinSharedMemory
```

**IMPORTANT: The default port is 10007. You can change it using the --port option!**

#### Car Window
At the moment we are *blind*, so let's just visualise what's happening:

```
rcwin$ java -jar target/site/justine-rcwin-0.0.16-jar-with-dependencies.jar ../berlin-lmap.txt
```

As you've probably noted, the input file given to the `Car Window` must be the same as the output file of the `smartcity`.

Also, don't forget to build the `jar` file this way:

```
rcwin$ mvn clean compile package site assembly:assembly
```

#### Samplemyshmclient
And here it comes! The fun part, adding cop cars and the gangsters:

```
rcemu$ src/samplemyshmclient --shm=BerlinSharedMemory --team=BerlinPolice
```

The default port is 10007.

Gangsters:

```
$ (sleep 1; echo "<init 0 50 g>"; sleep 1)|telnet localhost 10007
```

*NOTE: You may replace 50 with the number of gangsters you want to add to the simulation.*

### Some program options if you're in trouble
There are some other options for all the applications listed above, so here they are:

#### Traffic
```
--version     -> produce version message
--help -h     -> produce help message
--full_log -f -> enable logging of routes (much larger log file)
--verbose -v  -> verbose mode
--shm -s      -> shared memory segment name
--port -p     -> the TCP port that the traffic server is listening on to allow agents to communicate with the traffic simulation, the default value is 10007.
--cars -c     -> number of the random cars, the default value is 100.
--delay -d    -> sleep duration between calculations, the default value is 200
--minutes -m  -> how long does the traffic simulation run for? (-1 means infinite), default is 10
--catchdist   -> the catch distance of cop cars, default is 15.5
--traffictype -> NORMAL|ANTS|ANTS_RND|ANTS_RERND|ANTS_MRERND (default is NORMAL)
```
#### Samplemyshmclient
```
--version     -> produce version message
--help -h     -> produce help message
--verbose -v  -> verbose mode
--shm -s      -> shared memory segment name
--port -p     -> the TCP port that the traffic server is listening on to allow agents to communicate with the traffic simulation, default is 10007
--team -t     -> team name, default is Police
--cops -c     -> the number of cop cars, default is 10
```

#### Car Window
```
--playlog -> the log file to replay
```

If you'd like to replay a log file, there's no need to start the other processes, just the Car Window with the `--playlog=x` option where `x` denotes a valid log file. If you've been running the simulation with full logging enabled, then the routes of the cop cars can be examined during the replay.


`NEW` 
#### Scripts
There are some scripts, what you can use, if you want to do things more faster.
The scripts:
```
./start.sh				-> this script will automatically start the stuff
./kill.sh				-> this script, will run some killall commands, which kills the smartcity and the traffic processes
./compile.sh [argument]*		-> this script, will compile the codes, but there is an argument list, that you need to know
					-> this script, will play a funny sound, when finishes his works
 	
*IMPORTANT!: one argument must be given to the script, but only one, otherwise the script will do nothing

The argument list, for the compile.sh:
->	[all]				compiles the rcemu and the rcwin folder's codes
->	[rcemu]				compiles only the rcemu folder's codes	
->	[rcwin]				compiles only the rcwin folder's codes
->	[rcemu/src]			compiles only the rcemu/src folder's codes
->	[all&start]			compiles the rcemu and the rcwin folder's codes, than starts the stuff
->	[rcemu&start]			compiles only the rcemu folder's codes, than starts the stuff
->	[rcwin&start]			compiles only the rcwin folder's codes, than starts the stuff
->	[rcemu/src&start]		compiles only the rcemu/src folder's codes, than starts the stuff

**IMPORTANT!: the scripts are located in the robocar-emulator/justine directory
```


