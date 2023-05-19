# АККУРАТНО ЗАНИМАЕМСЯ ЭТИМ MAKEFILE'ом


LFLAG = -Wl,--no-as-needed
INCLUDE = -I/home/hoop/MY_PROG_PROJECTS/AT61-ThermalImager/include
LIBS = -L/home/hoop/MY_PROG_PROJECTS/AT61-ThermalImager/lib -lInfraredTempSDK -lIRNetClient -lhyvstream -ljson_linux-gcc-5.4.0_libmt -lcurl -ltiff -llzma -ldl



#получение исполняемого файла путем компоновки объектных (пока только 1 объектный). 
compile: objectfiles
	g++ $(LFLAG) -o ./build/thermalimager  -Wall -DVS_TRANMIST -fPIC -pthread $(INCLUDE) ./build/thermalimager.o $(LIBS)


# получение объектного модуля из исходного кода. Получается ELF-файл типа "перемещаемый". Выполнить его пока нельзя, далее нужно его передать компоновщику 

objectfiles: thermalimager.cpp
	g++ -g -c -DSYS_LINUX thermalimager.cpp
	
	mv thermalimager.o build/thermalimager.o


clean: 
	rm -f build/*.o
	rm -f build/thermalimager


