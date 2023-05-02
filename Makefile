#получение исполняемого файла путем компоновки объектных (пока только 1 объектный). 
compile: objectfiles
	g++ -o ./build/thermalimager ./build/thermalimager.o -L./lib -lInfraredTempSDK -lhyvstream
#  -lIRNetClient -ljpeg -lcurl -ljson_linux-gcc-5.4.0_libmt 
	
# получение объектного модуля из исходного кода. Получается ELF-файл типа "перемещаемый". Выполнить его пока нельзя, далее нужно его передать компоновщику 

objectfiles: thermalimager.cpp
	g++ -g -c thermalimager.cpp
	
	mv thermalimager.o build/thermalimager.o


clean: 
	rm -f build/*.o
	rm -f build/thermalimager


