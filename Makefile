FILES =./build/kernel.o ./build/io/io.asm.o ./build/memory/memory.o ./build/memory/memory.asm.o ./build/terminal/terminal.o ./build/string/string.o ./build/idt/idt.asm.o ./build/idt/idt.o ./build/keyboardHandler/keyboardHandler.o ./build/gdt/gdt.o ./build/gdt/gdt.asm.o ./build/kernel.asm.o ./build/disk/disk.o ./build/nano/nano.o ./build/math/math.o ./build/raycaster/raycaster.o ./build/mouse/mouse.o ./build/exec/exec.o ./build/multitasking/multitasking.asm.o ./build/multitasking/multitasking.o ./build/window_management/window_management.o
INCLUDES = -I./src
FLAGS = -g -ffreestanding -falign-jumps -falign-functions -falign-labels -falign-loops -fstrength-reduce -fomit-frame-pointer -finline-functions -Wno-unused-function -fno-builtin -Werror -Wno-unused-label -Wno-cpp -Wno-unused-parameter -nostdlib -nostartfiles -nodefaultlibs -Wall -O0 -Iinc


all: ./bin/boot.o ./bin/kernel.bin

	cp bin/myos.bin isodir/boot/myos.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o myos.iso isodir
	dd if=/dev/zero bs=1048576 count=32 >> myos.iso

./bin/boot.o: ./src/boot.s
	nasm -f elf -g ./src/boot.s -o ./build/boot.o


./bin/kernel.bin: $(FILES)
	i686-elf-ld -g -relocatable $(FILES) -o ./build/kernelfull.o
	i686-elf-gcc -T linker.ld -o bin/myos.bin -ffreestanding -O2 -nostdlib build/boot.o build/kernelfull.o


./build/kernel.o: ./src/kernel.c
	i686-elf-gcc $(INCLUDES) $(FLAGS) -std=gnu99 -c ./src/kernel.c -o ./build/kernel.o

./build/idt/idt.asm.o: ./src/idt/idt.asm
	nasm -f elf -g ./src/idt/idt.asm -o ./build/idt/idt.asm.o 

./build/idt/idt.o: ./src/idt/idt.c
	i686-elf-gcc $(INCLUDES) -I./src/idt $(FLAGS) -std=gnu99 -c ./src/idt/idt.c -o ./build/idt/idt.o

./build/memory/memory.o: ./src/memory/memory.c
	i686-elf-gcc $(INCLUDES) -I./src/memory $(FLAGS) -std=gnu99 -c ./src/memory/memory.c -o ./build/memory/memory.o

./build/io/io.asm.o: ./src/io/io.asm
	nasm -f elf -g ./src/io/io.asm -o ./build/io/io.asm.o 

./build/memory/memory.asm.o: ./src/memory/memory.asm
	nasm -f elf -g ./src/memory/memory.asm -o ./build/memory/memory.asm.o 

./build/terminal/terminal.o: ./src/terminal/terminal.c
		i686-elf-gcc $(INCLUDES) -I./src/terminal $(FLAGS) -std=gnu99 -c ./src/terminal/terminal.c -o ./build/terminal/terminal.o

./build/string/string.o: ./src/string/string.c
		i686-elf-gcc $(INCLUDES) -I./src/string $(FLAGS) -std=gnu99 -c ./src/string/string.c -o ./build/string/string.o

./build/keyboardHandler/keyboardHandler.o: ./src/keyboardHandler/keyboardHandler.c
		i686-elf-gcc $(INCLUDES) -I./src/keyboardHandler $(FLAGS) -std=gnu99 -c ./src/keyboardHandler/keyboardHandler.c -o ./build/keyboardHandler/keyboardHandler.o

./build/gdt/gdt.o: ./src/gdt/gdt.c
		i686-elf-gcc $(INCLUDES) -I./src/gdt $(FLAGS) -std=gnu99 -c ./src/gdt/gdt.c -o ./build/gdt/gdt.o

./build/gdt/gdt.asm.o: ./src/gdt/gdt.asm
	nasm -f elf -g ./src/gdt/gdt.asm -o ./build/gdt/gdt.asm.o 

./build/kernel.asm.o: ./src/kernel.asm
	nasm -f elf -g ./src/kernel.asm -o ./build/kernel.asm.o 

./build/disk/disk.o: ./src/disk/disk.c
	i686-elf-gcc $(INCLUDES) -I./src/disk $(FLAGS) -std=gnu99 -c ./src/disk/disk.c -o ./build/disk/disk.o

./build/nano/nano.o: ./src/nano/nano.c
	i686-elf-gcc $(INCLUDES) -I./src/nano $(FLAGS) -std=gnu99 -c ./src/nano/nano.c -o ./build/nano/nano.o

./build/math/math.o: ./src/math/math.c
	i686-elf-gcc $(INCLUDES) -I./src/math $(FLAGS) -std=gnu99 -c ./src/math/math.c -o ./build/math/math.o

./build/raycaster/raycaster.o: ./src/raycaster/raycaster.c
	i686-elf-gcc $(INCLUDES) -I./src/raycaster $(FLAGS) -std=gnu99 -c ./src/raycaster/raycaster.c -o ./build/raycaster/raycaster.o

./build/mouse/mouse.o: ./src/mouse/mouse.c
	i686-elf-gcc $(INCLUDES) -I./src/mouse $(FLAGS) -std=gnu99 -c ./src/mouse/mouse.c -o ./build/mouse/mouse.o

./build/exec/exec.o: ./src/exec/exec.c
	i686-elf-gcc $(INCLUDES) -I./src/exec $(FLAGS) -std=gnu99 -c ./src/exec/exec.c -o ./build/exec/exec.o

./build/multitasking/multitasking.asm.o: ./src/multitasking/multitasking.asm
	nasm -f elf -g ./src/multitasking/multitasking.asm -o ./build/multitasking/multitasking.asm.o 

./build/multitasking/multitasking.o: ./src/multitasking/multitasking.c
	i686-elf-gcc $(INCLUDES) -I./src/multitasking $(FLAGS) -std=gnu99 -c ./src/multitasking/multitasking.c -o ./build/multitasking/multitasking.o

./build/window_management/window_management.o: ./src/window_management/window_management.c
	i686-elf-gcc $(INCLUDES) -I./src/window_management $(FLAGS) -std=gnu99 -c ./src/window_management/window_management.c -o ./build/window_management/window_management.o

run:
	qemu-system-i386 myos.iso

clear:
	rm $(FILES)