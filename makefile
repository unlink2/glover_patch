CC=mips-elf-gcc
LD=mips-elf-ld

INCLUDEDIR=./src/include
SRCDIR=./src
ODIR=./obj
BINDIR=./bin

LIBS=
CFLAGS=-Wall -nostdlib -nodefaultlibs -fno-builtin -EB -g -fno-pic -mabi=eabi\
	   -ffunction-sections -fdata-sections -march=vr4300 -mtune=vr4300 -mgp32 -mlong32
# optional flags
# CFLAGS+=-Os -flto -ffat-lto-objects  \
#			-mno-extern-sdata -mgpopt -mfix4300 -mbranch-likely \
# 			-mno-check-zero-division -mxgot
MAIN = main
TEST_MAIN = test
MODULES = utility

.DEFAULT_GOAL := glover_patch

DEPS=$(patsubst %,$(INCLUDEDIR)/%.h,$(MODULES))
OBJ=$(patsubst %,$(ODIR)/%.o,$(MODULES))
OBJ+=$(patsubst %,$(ODIR)/%.o,$(MAIN))
TEST_OBJ=$(patsubst %,$(ODIR)/%.o,$(MODULES))
TEST_OBJ+=$(patsubst %,$(ODIR)/%.o,$(TEST_MAIN))

# main

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) | init
	$(CC) -c -o  $@ $< $(CFLAGS)

$(BINDIR)/code.bin: $(OBJ)
	$(LD) -T linker.ld $^ -o $(BINDIR)/code.elf
	mips-elf-objcopy -O binary --only-section=.text --only-section=.rodata $(BINDIR)/code.elf $@


glover_patch: $(BINDIR)/code.bin
	bass payload.s -sym ./bin/payload.sym
	python3 glovepatch.py ./rom/glover_base_ntsc.z64 ./bin/payload.bin ./bin/code.bin ./rom/glover_patched.z64

.PHONY: clean
clean:
	@echo Cleaning stuff. This make file officially is doing better than you irl.
	rm $(ODIR)/*.o
	rm $(BINDIR)/*


.PHONY: setup
init:
	mkdir -p $(ODIR)
	mkdir -p $(BINDIR)

