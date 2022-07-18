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

flags = -g -Wall -std=c++1z -pthread -rdynamic

# lists all subdirectories inside src
srcdirs = src/netlib/ src/netlib/**/
objdirs = $(patsubst src/netlib/%, build/%, $(wildcard $(srcdirs)))

sources = $(foreach d, $(srcdirs), $(wildcard $(d)*.cpp))
objects = $(patsubst src/netlib/%.cpp, build/%.o, $(sources))

all: server client

server: $(objdirs) $(objects) build/serverMain.o
	g++ $(objects) build/serverMain.o -o build/server $(flags)

client: $(objdirs) $(objects) build/clientMain.o
	g++ $(objects) build/clientMain.o -o build/client $(flags)

build/%.o: src/netlib/%.cpp
	g++ -c $(flags) -o $@ $<

build/serverMain.o: src/serverMain.cpp
	g++ -c $(flags) -o $@ $<

build/clientMain.o: src/clientMain.cpp
	g++ -c $(flags) -o $@ $<

$(objdirs):
	mkdir -p $@	

rs: server
	./build/server

rc: client
	./build/client

gs: server
	gdb ./build/server -tui

gc: client
	gdb ./build/client -tui

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
