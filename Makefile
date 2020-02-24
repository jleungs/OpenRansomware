MINGW=gcc
FLAGS=-static

all: ransomware genkeys

ransomware: src/ransomware.c src/util.c src/crypt.c
	$(MINGW) -o bin/$@.exe $^ $(FLAGS)

genkeys: src/genkeys.c src/util.c src/crypt.c
	$(MINGW) -o bin/$@.exe $^ $(FLAGS)
