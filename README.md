# Simple os


## Run without building it
To run the os without building a cross compiler and the OS it self just do:
```
sudo apt install qemu-system
cd os
qemu-system-i386 myos.iso
```
OBS: if the OS breaks you may need to download the ISO again and run it again
## How to build it

To build the repo you need to create a cross-compiler, you can use https://wiki.osdev.org/GCC_Cross-Compiler or follow the instructions for linux.

First install:

- build-essential 
- bison
- flex
- libgmp3-dev 
- libmpc-dev 
- libmpfr-dev 
- texinfo 
- libisl-dev  (optional)
- qemu-system

Go to your home folder (just do cd command) on your terminal and prompt this
```
cd
mkdir src
```
On that folder download binutils and gcc from this websites
https://ftp.gnu.org/gnu/binutils/ and 
https://ftp.gnu.org/gnu/gcc/ i used or 2.42 binutils and gcc 13.2.0 gcc or just


```
cd src
curl 'https://ftp.gnu.org/gnu/gcc/gcc-13.2.0/gcc-13.2.0.tar.gz' --output gcc-13.2.0.tar.gz
curl 'https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.gz' --output binutils-2.42.tar.gz

tar -xzvf gcc-13.2.0.tar.gz
tar -xzvf binutils-2.42.tar.gz
```



Set some variables 

```
  export PREFIX="$HOME/opt/cross"
  export TARGET=i686-elf
  export PATH="$PREFIX/bin:$PATH"
```

and 

```
cd $HOME/src

mkdir build-binutils
cd build-binutils
../binutils-2.42/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install
```

+

```
cd $HOME/src
 
which -- $TARGET-as || echo $TARGET-as is not in the PATH
 
mkdir build-gcc
cd build-gcc
../gcc-13.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
```

to use the compiler do:

```
$HOME/opt/cross/bin/$TARGET-gcc --version
```
## Run it
After building the cross-compiler just do:
```
cd os
./build.sh
make run
```
It should open the os in qemu :D
