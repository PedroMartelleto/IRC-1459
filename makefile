#=======================================================
# Makefile com comandos de compilação, execução e utilidades relacionadas ao programa
# make all: comando principal de compilação, que cria o executável
# make *.o: cria um dos arquivos objeto a partir da fonte
# make clean: remove todos os arquivos objeto do diretório atual
# make rebuild: como make all, mas recompila todos os arquivos objeto no diretório
# make zip: cria o arquivo zip para a submissão
#=======================================================

objects = socket.o
flags = -g

all: server client

server: $(objects) server.o
	g++ $(flags) $(objects) server.o -o server

client: $(objects) client.o
	g++ $(flags) $(objects) client.o -o client

rs: $(objects) server.o
	make all
	./server

rc: $(objects) client.o
	make all
	./client

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
	cp *.cpp zip
	cp *.h zip
	cp makefile zip 
	zip code.zip zip/* -j
	

rebuild:
	make clean
	make all
