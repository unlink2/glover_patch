#!/bin/sh

# arch to build
export TARGET=mips-elf
# location after install
export PREFIX=/usr/local/$TARGET
# add PREFIX/bin to your path!

# download all the stuff
mkdir gcc_build
cd gcc_build
wget https://ftp.gnu.org/gnu/binutils/binutils-2.34.tar.bz2 -O binutils.tar.bz2
wget https://ftp.gnu.org/gnu/gcc/gcc-9.2.0/gcc-9.2.0.tar.gz -O gcc.tar.gz
wget https://ftp.gnu.org/gnu/gdb/gdb-8.3.tar.gz -O gdb.tar.gz
tar xjfv binutils.tar.bz2
tar xzfv gcc.tar.gz
tar xzfv gdb.tar.gz
# move to nicer names
mv binutils-2.34 binutils
mv gcc-9.2.0 gcc
mv gdb-8.3 gdb

mkdir gcc_build
mkdir binutils_build
mkdir gdb_build

# binutils
cd binutils_build
../binutils/configure --target=$TARGET --prefix=$PREFIX
make all
sudo make install
cd ..

# gcc
cd gcc_build
../gcc/configure --target=$TARGET --prefix=$PREFIX --with-gnu-as --with-gnu-ld
make all-gcc
sudo make install-gcc
cd ..

# gdb
cd gdb_build
../gdb/configure --target=$TARGET --prefix=$PREFIX --enable-sim-mips --enable-sim-stdio
make all
sudo make install
