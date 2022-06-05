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
flags = -g -Wall -std=c++2a

all: server client

server:
	g++ $(flags) $(objects) src/serverMain.cpp -o build/server.o

client:
	g++ $(flags) $(objects) src/clientMain.cpp -o build/client.o

rs:
	g++ $(flags) $(objects) src/serverMain.cpp -o build/server.o
	./build/server.o

rc:
	g++ $(flags) $(objects) src/clientMain.cpp -o build/client.o
	./build/client.o

.cpp.o: $*.cpp
	g++ $*.cpp -c $(flags)

clean:
	rm *.o -f
	rm zip -fr
	rm *.zip -f
	rm client
	rm server

FORCE:

zip: FORCE
	rm code.zip -f
	mkdir zip -p
	rm zip/* -f
	cp src zip
	cp makefile zip 
	cp README.md zip
	zip code.zip zip/* -j	

rebuild:
	make clean
	make all
