FILES = ./build/main_jump.o ./build/main.o ./build/asm.o ./build/os.o
FLAGS = -m32 -masm=intel -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc

#strip -s main
#objdump -S main
# strip --strip-debug --discard-all main     mantem alguns symbols
#objdump -Sdx main >> dump.txt
all: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/mainJoin.o
	i686-elf-gcc -T linker.ld -o main -ffreestanding -O2 -nostdlib ./build/mainJoin.o
	strip --strip-debug --discard-all --keep-section-symbols main


./build/asm.o: ./asm.asm
	nasm -f elf32 -o ./build/asm.o asm.asm

./build/main_jump.o: ./main_jump.asm
	nasm -f elf32 -o ./build/main_jump.o main_jump.asm

./build/main.o: ./main.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./main.c -o ./build/main.o

./build/os.o: ./main.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./os.c -o ./build/os.o

clear:
	rm $(FILES)