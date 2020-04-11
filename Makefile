CC := gcc
SRCD := src
BLDD := build
INCD := include
TESTD := tests
EXEC := smash

ALL_SRCF := $(shell find $(SRCD) -type f -name *.c)
ALL_OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(ALL_SRCF:.c=.o))
ALL_TESTS := $(shell find $(TESTD) -type f -name *.sh)
FUNC_FILES := $(filter-out build/main.o, $(ALL_OBJF))
INC := -I $(INCD)

CFLAGS := -g -Wall -Werror
LDFLAGS := -lreadline
STD := -std=gnu11

CFLAGS += $(STD)

.PHONY: clean all test

all: setup $(EXEC)

setup:
	mkdir -p build

$(EXEC): $(ALL_OBJF)
	$(CC) $^ -o $@ $(LDFLAGS)

$(BLDD)/%.o: $(SRCD)/%.c
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(BLDD) $(EXEC)

tests: $(EXEC) $(ALL_TESTS)
	for i in $(ALL_TESTS); do \
		/bin/bash $$i; done
