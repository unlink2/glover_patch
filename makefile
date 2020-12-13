CC=mips-elf-gcc
CCLOC = gcc
LD=mips-elf-ld

INCLUDEDIR=./src/include
LIBEXINCLUDEDIR=./libex/src
SRCDIR=./src
LIBEXSRCDIR=./libex/src
ODIR=./obj
ODIRLOC=./obj/local
BINDIR=./bin

LIBS=
CFLAGS=-Wall -nostdlib -nodefaultlibs -fno-builtin -EB -g -fno-pic -mabi=eabi -ffreestanding \
	   -ffunction-sections -fdata-sections -march=vr4300 -mtune=vr4300 -mgp32 -mlong32 -G 0 -fno-builtin-memcpy
# optional flags
# CFLAGS+=-Os -flto -ffat-lto-objects  \
#			-mno-extern-sdata -mgpopt -mfix4300 -mbranch-likely \
# 			-mno-check-zero-division -mxgot
MAIN = main
TEST_MAIN = test
MODULES = utility inputs logic render memory memwatch font8x8_basic debug rdp menu matrix keyboard playerinfo\
		  script
LIBEX_MODULES = utility token basicmalloc scanner error expr ast_printer parser interpreter object visitor stmt\
								   map enviorment callable arraylist

.DEFAULT_GOAL := glover_patch

DEPS=$(patsubst %,$(INCLUDEDIR)/%.h,$(MODULES))
DEPS+=$(patsubst %,$(LIBEXINCLUDEDIR)/%.h,$(LIBEX_MODULES))
OBJ=$(patsubst %,$(ODIR)/%.o,$(MODULES))
OBJ+=$(patsubst %,$(ODIR)/%.o,$(MAIN))
OBJ+=$(patsubst %,$(ODIR)/libex/%.o,$(LIBEX_MODULES))

TEST_OBJ=$(patsubst %,$(ODIRLOC)/%.o,$(MODULES))
TEST_OBJ+=$(patsubst %,$(ODIRLOC)/%.o,$(TEST_MAIN))
TEST_OBJ+=$(patsubst %,$(ODIRLOC)/libex/%.o,$(LIBEX_MODULES))
# main

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) | init
	$(CC) -c -o  $@ $< $(CFLAGS)

$(ODIR)/libex/%.o: $(LIBEXSRCDIR)/%.c $(DEPS) | init
	$(CC) -c -o  $@ $< $(CFLAGS)

$(BINDIR)/code.bin: $(OBJ)
	$(LD) -T linker.ld $^ -o $(BINDIR)/code.elf
	mips-elf-objcopy -O binary --only-section=.text --only-section=.rodata $(BINDIR)/code.elf $@
	mips-elf-objdump -S $(BINDIR)/code.elf > $(BINDIR)/code.sym

glover_patch: $(BINDIR)/code.bin
	bass payload.s -sym ./bin/payload.sym
	bass entry.s -sym ./bin/entry.sym
	python3 glovepatch.py ./rom/glover_base_ntsc.z64 ./bin/payload.bin ./bin/code.bin ./bin/entry.bin ./rom/glover_patched.z64

# test

$(ODIRLOC)/%.o: $(SRCDIR)/%.c $(DEPS) | init
	$(CCLOC) -c -o $@ $< -Wall -g


$(ODIRLOC)/libex/%.o: $(LIBEXSRCDIR)/%.c $(DEPS) | init
	$(CCLOC) -c -o $@ $< -Wall -g

build_test: $(TEST_OBJ)
	$(CCLOC) -o $(BINDIR)/${TEST_MAIN} $^ $(LIBS) -l cmocka

test: build_test
	$(BINDIR)/$(TEST_MAIN)

.PHONY: clean
clean:
	@echo Cleaning stuff. This make file officially is doing better than you irl.
	rm -f $(ODIR)/*.o
	rm -f $(ODIR)/libex/*.o
	rm -f $(ODIRLOC)/libex/*.o
	rm -f $(ODIRLOC)/*.o
	rm -f $(BINDIR)/*


.PHONY: setup
init:
	mkdir -p $(ODIR)
	mkdir -p $(ODIR)/libex
	mkdir -p $(BINDIR)
	mkdir -p $(ODIRLOC)
	mkdir -p $(ODIRLOC)/libex

