mit=$1;

all() {
	cd rcemu &&
	echo "-------------------------------------------------------";
	echo "In rcemu: autoreconf --install";
	echo "-------------------------------------------------------";
	autoreconf --install &&
	echo "-------------------------------------------------------";
	echo "In rcemu: ./configure";
	echo "-------------------------------------------------------";
	./configure &&
	echo "-------------------------------------------------------";
	echo "In rcemu: make";
	echo "-------------------------------------------------------";
	make &&

	cd .. &&

	cd rcwin &&
	echo "---------------------------------------------------------------";
	echo "In rcwin: mvn clean compile package site assembly:assembly";
	echo "---------------------------------------------------------------";
	mvn clean compile package site assembly:assembly &&
	cd .. &&
	
	echo "-------------------------------------------------------";
	echo "PROCESSES HAS BEEN FINISHED! SUCCES in: all";
	echo "-------------------------------------------------------";
}

rcemu(){
	cd rcemu &&
	echo "-------------------------------------------------------";
	echo "In rcemu: make";
	echo "-------------------------------------------------------";
	make &&
	cd .. &&
	echo "-------------------------------------------------------";
	echo "PROCESSES HAS BEEN FINISHED! SUCCES in: rcemu";
	echo "-------------------------------------------------------";
}

rcwin(){
 	cd rcwin &&
	echo "---------------------------------------------------------------";
	echo "In rcwin: mvn clean compile package site assembly:assembly";
	echo "---------------------------------------------------------------";
	mvn clean compile package site assembly:assembly &&
	cd .. &&
	echo "-------------------------------------------------------";
	echo "PROCESSES HAS BEEN FINISHED! SUCCES in: rcwin";
	echo "-------------------------------------------------------";
}

src(){
 	cd rcemu/src &&
	echo "-------------------------------------------------------";
	echo "In rcemu/src: make";
	echo "-------------------------------------------------------";
	make &&
	cd .. &&
	cd .. &&
	echo "-------------------------------------------------------";
	echo "PROCESSES HAS BEEN FINISHED! SUCCES in: rcemu/src";
	echo "-------------------------------------------------------";
}

sound(){

	aplay people074.wav

}

case $mit in
	all)
	all;
	sound;
;;
	rcemu)
	rcemu;
	sound;
;;
	rcwin)
	rcwin;
	sound;
;;
	rcemu/src)
	src;
	sound;
;;
	all\&start)
	all;
	./start.sh;
	sound;
;;
	rcemu\&start)
	rcemu;
	./start.sh;
	sound;
;;
	rcwin\&start)
	rcwin;
	./start.sh;
	sound;
;;
	rcemu/src\&start)
	src;
	./start.sh;
	sound;
;;
	*)
	echo "--------------------------------------------------------------------------------";
	echo "Incorrect use! Usage: ./compile.sh [argument]";
	echo "";
	echo "Argument list:";
	echo "->[all] compiles the rcemu and the rcwin folder's codes";
	echo "->[rcemu] compiles only the rcemu folder's codes";	
	echo "->[rcwin]	compiles only the rcwin folder's codes";
	echo "->[rcemu/src] compiles only the rcemu/src folder's codes";
	echo "->[all&start] compiles the rcemu and the rcwin folder's codes, than starts the stuff";
	echo "->[rcemu&start] compiles only the rcemu folder's codes, than starts the stuff";
	echo "->[rcwin&start] compiles only the rcwin folder's codes, than starts the stuff";
	echo "->[rcemu/src&start] compiles only the rcemu/src folder's codes, than starts the stuff";
	echo "--------------------------------------------------------------------------------";
;;
esac
