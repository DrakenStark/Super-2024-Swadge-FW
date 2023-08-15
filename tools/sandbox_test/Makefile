all : run

UNAME := $(shell uname)

BASE_FOLDER ?= .

ifeq ($(UNAME), Linux)
CFLAGS:=-g -O0
LDFLAGS:=-ludev
CC:=gcc
else
CFLAGS:=-Os -s
CC:=gcc
LDFLAGS:=C:/windows/system32/setupapi.dll
endif

SYSELF:=$(BASE_FOLDER)/../../build/swadge2024.elf

build : 
	mkdir -p build

sandbox.o : $(BASE_FOLDER)/buildhelp sandbox.c $(SYSELF) build
	$(BASE_FOLDER)/buildhelp $(BASE_FOLDER) $(SYSELF)
	xtensa-esp32s2-elf-objdump -s build/sandbox.o > build/debug_sandbox_s.txt
	xtensa-esp32s2-elf-objdump -t build/sandbox.o > build/debug_sandbox_t.txt
	xtensa-esp32s2-elf-objdump -S build/sandbox.o > build/debug_sandbox_S.txt

run : $(BASE_FOLDER)/sandbox_upload sandbox.o
	$(BASE_FOLDER)/sandbox_upload

$(BASE_FOLDER)/buildhelp : $(BASE_FOLDER)/buildhelp.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BASE_FOLDER)/sandbox_upload : $(BASE_FOLDER)/sandbox_upload.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BASE_FOLDER)/sandbox_interactive : $(BASE_FOLDER)/sandbox_interactive.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

monitor : $(BASE_FOLDER)/sandbox_interactive
	$(BASE_FOLDER)/sandbox_interactive

interactive : $(BASE_FOLDER)/sandbox_interactive build
	$(BASE_FOLDER)/sandbox_interactive sandbox.c sandbox.S $(SYSELF)

clean :
	rm -rf *.o *~ $(BASE_FOLDER)/buildhelp build/debug_sandbox_s.txt build/debug_sandbox_t.txt build/debug_sandbox_S.txt build/sandbox_inst.bin build/sandbox_data.bin build/buildhelp build/sandbox.o $(BASE_FOLDER)/sandbox_upload build/sandbox.lds build/provided.lds build/sandbox_symbols.txt build/system_symbols.txt $(BASE_FOLDER)/sandbox_interactive $(BASE_FOLDER)/buildhelp.exe $(BASE_FOLDER)/sandbox_upload.exe

