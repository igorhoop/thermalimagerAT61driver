# АККУРАТНО ЗАНИМАЕМСЯ ЭТИМ MAKEFILE'ом


LFLAG = -Wl,--no-as-needed
INCLUDE = -I/home/hoop/MY_PROG_PROJECTS/AT61F/development/include
LIBS = -L/home/hoop/MY_PROG_PROJECTS/AT61F/development/lib -lInfraredTempSDK -lIRNetClient -lhyvstream -ljson_linux-gcc-5.4.0_libmt -lcurl -ltiff -llzma -ldl -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lavcodec -lswscale



#получение исполняемого файла путем компоновки объектных (пока только 1 объектный). 
compile: objectfiles
	g++  $(LFLAG) -o ./build/at61f -Wall -DVS_TRANMIST -fPIC -pthread $(INCLUDE) ./build/main.o ./build/thematic.o ./build/callbacks.o ./build/basis.o ./build/thermalmap_window.o ./build/video_window.o ./build/Netabstraction.o $(LIBS)


# получение объектного модуля из исходного кода. Получается ELF-файл типа "перемещаемый". Выполнить его пока нельзя, далее нужно его передать компоновщику 

objectfiles: ./sources/main.cpp ./sources/thematic.cpp ./sources/callbacks.cpp ./sources/basis.cpp ./sources/Netabstraction.cpp ./sources/thermalmap_window.cpp ./sources/video_window.cpp
	g++ -std=c++17 -g -c -DSYS_LINUX ./sources/main.cpp
	g++ -g -c -DSYS_LINUX ./sources/thematic.cpp
	g++ -g -c -DSYS_LINUX ./sources/callbacks.cpp
	g++ -c ./sources/basis.cpp
	g++ -c ./sources/Netabstraction.cpp
	g++ -c ./sources/thermalmap_window.cpp
	g++ -c ./sources/video_window.cpp
	
	mv main.o build/main.o
	mv thematic.o build/thematic.o
	mv callbacks.o build/callbacks.o
	mv basis.o build/basis.o
	mv Netabstraction.o build/Netabstraction.o
	mv thermalmap_window.o build/thermalmap_window.o
	mv video_window.o build/video_window.o

clean: 
	rm -f build/*.o
	rm -f build/at61f


