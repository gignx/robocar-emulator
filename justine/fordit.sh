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

case $mit in
	all)
	all;
;;
	rcemu)
	rcemu;
;;
	rcwin)
	rcwin;
;;
	src)
	src;
;;
	allstart)
	all;
	./start.sh;
;;
	rcemustart)
	rcemu;
	./start.sh;
;;
	rcwinstart)
	rcwin;
	./start.sh;
;;
	srcstart)
	src;
	./start.sh;
;;
esac
