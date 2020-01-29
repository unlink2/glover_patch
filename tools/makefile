CC=gcc
DBG=gdb
BIN=lasm

INCLUDEDIR=./src/include
SRCDIR=./src
ODIR=./obj
BINDIR=./bin

LIBS= 
CFLAGS=-Wall -g
MAIN = decompress

MODULES =


DEPS=$(patsubst %,$(INCLUDEDIR)/%.h,$(MODULES))
OBJ=$(patsubst %,$(ODIR)/%.o,$(MODULES))
OBJ+=$(patsubst %,$(ODIR)/%.o,$(MAIN))
TEST_OBJ=$(patsubst %,$(ODIR)/%.o,$(MODULES))
TEST_OBJ+=$(patsubst %,$(ODIR)/%.o,$(TEST_MAIN))

# main

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) | init
	$(CC) -c -o $@ $< $(CFLAGS)

decompress: $(OBJ)
	$(CC) -o $(BINDIR)/$@ $^ $(LIBS)


.PHONY: clean
clean:
	@echo Cleaning stuff. This make file officially is doing better than you irl.
	rm $(ODIR)/*.o
	rm $(BINDIR)/*


.PHONY: setup
init:
	mkdir -p $(ODIR)
	mkdir -p $(BINDIR)

.PHONY: install
install:
	cp ${BINDIR}/${BIN} ${INSTALLDIR}/${BIN}
