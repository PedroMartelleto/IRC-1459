#=======================================================
# Makefile com comandos de compilação, execução e utilidades relacionadas ao programa
# make all: comando principal de compilação, que cria o executável
# make *.o: cria um dos arquivos objeto a partir da fonte
# make clean: remove todos os arquivos objeto do diretório atual
# make rebuild: como make all, mas recompila todos os arquivos objeto no diretório
# make zip: cria o arquivo zip para a submissão
# make rc: compila e roda o cliente.
# make rs: compila e roda o servidor.
#=======================================================

objects = src/netlib/**/*.cpp src/netlib/*.cpp
flags = -g -Wall -std=c++1z -pthread

all: server client

server:
	mkdir -p build
	g++ -g -rdynamic $(flags) $(objects) src/serverMain.cpp -o build/server.o

client:
	mkdir -p build
	g++ -g -rdynamic $(flags) $(objects) src/clientMain.cpp -o build/client.o

rs:
	mkdir -p build
	g++ -g -rdynamic $(flags) $(objects) src/serverMain.cpp -o build/server.o
	./build/server.o

rc:
	mkdir -p build
	g++ -g -rdynamic $(flags) $(objects) src/clientMain.cpp -o build/client.o
	./build/client.o

.cpp.o: $*.cpp
	g++ -g -rdynamic $*.cpp -c $(flags)

clean:
	rm -f ./build/*.o
	rm -fr zip 
	rm -f *.zip

FORCE:

zip: FORCE
	rm code.zip -f
	mkdir zip -p
	rm zip/* -f
	cp -r src zip
	cp makefile zip 
	cp README.md zip
	cd zip; zip -r code.zip *
	mv zip/code.zip .

rebuild:
	make clean
	make all
