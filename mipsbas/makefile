CC=gcc

INCLUDEDIR=./src/include
SRCDIR=./src
ODIR=./obj
ODIRLOC=./obj/local
BINDIR=./bin

LIBS=
CFLAGS=-Wall -nostdlib
TEST_MAIN = test
MODULES = utility lispvalue

.DEFAULT_GOAL := test

DEPS=$(patsubst %,$(INCLUDEDIR)/%.h,$(MODULES))
TEST_OBJ=$(patsubst %,$(ODIRLOC)/%.o,$(MODULES))
TEST_OBJ+=$(patsubst %,$(ODIRLOC)/%.o,$(TEST_MAIN))

# main


# test

$(ODIRLOC)/%.o: $(SRCDIR)/%.c $(DEPS) | init
	$(CC) -c -o $@ $< -Wall -g

build_test: $(TEST_OBJ)
	$(CC) -o $(BINDIR)/${TEST_MAIN} $^ $(LIBS) -l cmocka

test: build_test
	$(BINDIR)/$(TEST_MAIN)

.PHONY: clean
clean:
	@echo Cleaning stuff. This make file officially is doing better than you irl.
	rm -f $(ODIR)/*.o
	rm -f $(ODIRLOC)/*.o
	rm -f $(BINDIR)/*


.PHONY: setup
init:
	mkdir -p $(ODIR)
	mkdir -p $(BINDIR)
	mkdir -p $(ODIRLOC)

