#получение исполняемого файла путем компоновки объектных (пока только 1 объектный). 
compile: objectfiles
	g++ -o ./build/thermalimager ./build/thermalimager.o -static

	
# получение объектного модуля из исходного кода. Получается ELF-файл типа "перемещаемый". Выполнить его пока нельзя, далее нужно его передать компоновщику 

objectfiles: thermalimager.cpp
	g++ -g -c thermalimager.cpp
	
	mv thermalimager.o build/thermalimager.o

	
clean: 
	rm -f build/*.o
	rm -f build/thermalimager


