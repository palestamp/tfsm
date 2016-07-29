CC=gcc
CFLAGS=-O2 -Wall -Wextra -Isrc -DNDEBUG $(OPTFLAGS)

SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

TFSM_DEF=$(wildcard tests/dcl_*.tfsm)
TFSM_DEF_HEADERS=$(patsubst %.tfsm,%.h,$(TFSM_DEF))

TARGET=build/libtfsm.a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

all: $(TARGET) $(SO_TARGET) check

dev: CFLAGS=-g -Wall -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

$(TARGET): $(CFLAGS) += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

build:
	@mkdir -p build
	@mkdir -p bin

enum_gen: build $(OBJECTS)
	$(CC) -o bin/enum_gen $(OBJECTS)

gen_headers: enum_gen
	./bin/enum_gen $(TFSM_DEF) R $(TFSM_DEF_HEADERS)

check: CFLAGS += $(TARGET)
check: gen_headers $(TESTS)
	sh ./tests/runtests.sh

clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	rm -f tests/*.diff
	rm -f tests/*.h
	rm -rf `find . -name "*.dSYM" -print`
	rm -f bin/enum_gen

