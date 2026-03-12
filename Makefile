ifeq ($(OS),Windows_NT)

	SRC       := src
	BIN       := bin
	CC        := clang
	ASSEMBLY  := fast_reader
	EXTENSION :=.exe

	DIR := $(subst /,\,${CURDIR})
	INCLUDES     := -I./external/raylib/include 
	LINKER_FLAGS := -lraylib -lm -L./external/raylib/lib/ -lraylib
	COMPILER_FLAGS := -Wall -Werror -Wextra -g -O0 -Wno-system-headers -Wno-unused-but-set-variable -Wno-unused-variable -Wno-varargs -Wno-unused-private-field -Wno-unused-parameter -Wno-unused-function -fsanitize=undefined -fsanitize-trap
	BUILD_PLATFORM := windows

	rwildcard=$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))

	SRC_FILES_C := $(call rwildcard,$(SRC)/,*.c)
	DIRECTORIES := $(subst $(DIR),,$(shell dir $(SRC) /S /AD /B | findstr /i $(SRC) )) # Get all directories under src.

else
	SRC       := src
	BIN       := bin
	CC        := clang
	ASSEMBLY  := fast_reader 
	EXTENSION := 

	INCLUDES     := -I./external/raylib/include 
	LINKER_FLAGS := -lraylib -lm -L./external/raylib/lib/ -lraylib
	COMPILER_FLAGS := -Wall -Werror -Wextra -g -O0 -Wno-system-headers -Wno-unused-but-set-variable -Wno-unused-variable -Wno-varargs -Wno-unused-private-field -Wno-unused-parameter -Wno-unused-function -fsanitize=undefined -fsanitize-trap

	SRC_FILES := $(shell find $(SRC) -type f -name '*.c')

endif 

all: scaffold compile

scaffold:
ifeq ($(OS),Windows_NT)
	-@setlocal enableextensions enabledelayedexpansion && mkdir $(BIN) 2>NUL || cd .
else
	@mkdir -p $(BIN)
endif

compile: 
	$(CC) $(SRC_FILES) $(COMPILER_FLAGS) -o $(BIN)/$(ASSEMBLY)$(EXTENSION) $(INCLUDES) $(LINKER_FLAGS)

