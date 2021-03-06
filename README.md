# Glover Speedrun Practice Hack


## Easy Install:

To easily install a non-dev build use the web-builder which can be found here:
https://glover.run

## Note for Porject64 Users:

When using Project64 please use GlideN64 Video Plugin or ANgrylion.
They give the best results. The only big issue on PJ64 is that the
memory monitor cursor does not render on Glide, but if you're running on emulator
you likely will not need it anyway.


## How to build from source?

I recommend using the Windows Subsystem for Linux if you are on Windows.
A lot of tools used are related to Linux so this is the easiest way of building it for now and I primarily developed this on Linux.
### How to install WSL:

https://docs.microsoft.com/en-us/windows/wsl/install-win10
I tested this with Debian GNU/Linux WSL.

### Initial Build

After WSL is set up run the following commands:
```bash
sudo apt update
sudo apt install gcc g++ make python3 git coreutils build-essential wget texinfo libgmp-dev libmpfr-dev libmpc-dev
git clone https://github.com/unlink2/glover_patch
cd glover_patch
chmod +x configure.sh
./configure
./build_gcc.sh
```

Note that configure.sh will ask for your root password twice.

After gcc is built make sure to add it to your PATH like this:

export PATH=$PATH:/usr/local/mips-elf/bin

To open the current folder in the Windows Filebrowser type:

explorer.exe .

Now you can browse the glover patch folder that contains almost everything you need to patch the rom.
Navigate to glover\rom and copy a NTSC rom of glover into it. The filename must be "glover_base_ntsc.z64".
Once done go back to WSL and run the last command:

make

Now inside of the rom folder you will find a file called glover_patched.z64. This is the final patched rom.

