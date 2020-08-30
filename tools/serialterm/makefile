CC=gcc
DBG=gdb
BIN=lasm

INCLUDEDIR=./src/include
SRCDIR=./src
ODIR=./obj
BINDIR=./bin

LIBS=-lusb -lftdi -leditline
CFLAGS=-Wall -g
MAIN = main
TEST_MAIN = test
MODULES =


DEPS=$(patsubst %,$(INCLUDEDIR)/%.h,$(MODULES))
OBJ=$(patsubst %,$(ODIR)/%.o,$(MODULES))
OBJ+=$(patsubst %,$(ODIR)/%.o,$(MAIN))
TEST_OBJ=$(patsubst %,$(ODIR)/%.o,$(MODULES))
TEST_OBJ+=$(patsubst %,$(ODIR)/%.o,$(TEST_MAIN))

# main

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) | init
	$(CC) -c -o $@ $< $(CFLAGS)

serialterm: $(OBJ)
	$(CC) -o $(BINDIR)/$@ $^ $(LIBS)


# test

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS) | init
	$(CC) -c -o $@ $< $(CFLAGS)

build_test: $(TEST_OBJ)
	$(CC) -o $(BINDIR)/${TEST_MAIN} $^ $(LIBS) -l cmocka

test: build_test
	$(BINDIR)/$(TEST_MAIN)

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
