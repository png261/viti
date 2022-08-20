# Includes the config.mk igurations
include config.mk

#
# Validating project variables defined in config.mk
#

ifndef PROJECT_NAME
$(error Missing PROJECT_NAME. Put variables at config.mk file)
endif
ifndef BINARY
$(error Missing BINARY. Put variables at config.mk file)
endif
ifndef PROJECT_PATH
$(error Missing PROJECT_PATH. Put variables at config.mk file)
endif


# Gets the Operating system name
OS := $(shell uname -s)

# Default shell
SHELL := bash

# Color prefix for Linux distributions
COLOR_PREFIX := e

ifeq ($(OS),Darwin)
	COLOR_PREFIX := 033
endif

# Color definition for print purpose
BROWN=\$(COLOR_PREFIX)[0;33m
BLUE=\$(COLOR_PREFIX)[1;34m
END_COLOR=\$(COLOR_PREFIX)[0m

# Source code directory structure
BINDIR := bin
SRCDIR := src
LOGDIR := log
LIBDIR := lib
TESTDIR := test

# Source code file extension
SRCEXT := c


# Defines the C Compiler
CC := gcc

# Defines the language standards for GCC
STD := -std=c11 # See man gcc for more options

# Protection for stack-smashing attack
STACK := -fstack-protector-all -Wstack-protector

# Specifies to GCC the required warnings
WARNS := -Wall -Wextra -pedantic # -pedantic warns on language standards

# Flags for compiling 
CFLAGS := -O3 $(STD) $(STACK) $(WARNS)

# Debug options
DEBUG := -g3 -DDEBUG=1

# Dependency libraries
LIBS := -lm -lncurses 

# %.o file names
NAMES := $(notdir $(basename $(wildcard $(SRCDIR)/*.$(SRCEXT))))
OBJECTS :=$(patsubst %,$(LIBDIR)/%.o,$(NAMES))

#
# COMPILATION RULES
#

default: all

# Rule for link and generate the binary file
all: $(OBJECTS)
	@echo -en "$(BROWN)LD $(END_COLOR)";
	$(CC) -o $(BINDIR)/$(BINARY) $+ $(DEBUG) $(CFLAGS) $(LIBS)
	@echo -en "\n--\nBinary file placed at" \
			  "$(BROWN)$(BINDIR)/$(BINARY)$(END_COLOR)\n";


# Rule for object binaries compilation
$(LIBDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo -en "$(BROWN)CC $(END_COLOR)";
	$(CC) -c $^ -o $@ $(DEBUG) $(CFLAGS) $(LIBS)

# Run
FILES = $(filter-out $@,$(MAKECMDGOALS))
run:
	$(BINDIR)/$(BINARY) $(FILES)

# Clean
clean:
	@rm -rvf $(BINDIR)/* $(LIBDIR)/* $(LOGDIR)/*;

# Install
install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f $(BINDIR)/$(BINARY) ${DESTDIR}${PREFIX}/bin/$(PROJECT_NAME)
	chmod 755 ${DESTDIR}${PREFIX}/bin/$(PROJECT_NAME)
	@echo -en "$(BROWN)-- YAH! VITI HAS BEEN INSTALLED! --$(END_COLOR)\n"

# Uninstall
uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/$(PROJECT_NAME)
	@echo -en "$(BROWN)-- UNINSTALLED VITI! --$(END_COLOR)\n"

# Help
help:
	@echo "VITI TEXTEDITOR"
	@echo
	@echo "Target rules:"
	@echo "    all          - Compiles and generates binary file"
	@echo "    install      - Compiles and install binary file"
	@echo "    uninstall    - uninstall binary file"
	@echo "    run          - run binary file"
	@echo "    clean        - Clean the project by removing binaries"
	@echo "    help         - Prints a help message with target rules"
