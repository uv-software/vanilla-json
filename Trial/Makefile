#
#	Trial Program
#	vanilla-json
#	Bart Simpson didn't do it
#
current_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown OS')
current_OS := $(patsubst CYGWIN%,Cygwin,$(current_OS))
current_OS := $(patsubst MINGW%,MinGW,$(current_OS))
current_OS := $(patsubst MSYS%,MinGW,$(current_OS))


ifeq ($(current_OS),$(filter $(current_OS),Linux Darwin))
TARGET  = vanilla_test
else
TARGET  = vanilla_test.exe
endif

INSTALL = ~/bin

HOME_DIR = ..
MAIN_DIR = .

SOURCE_DIR = $(HOME_DIR)/Sources

OBJECTS = $(OUTDIR)/main.o $(OUTDIR)/vanilla.o

DEFINES = 

HEADERS = -I$(SOURCE_DIR)

CFLAGS += -std=c90 -ansi -O0 -g -Wall -Wextra -Wno-parentheses \
	-fno-strict-aliasing \
	-pedantic \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -std=c++98 -O0 -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LDFLAGS += 

LIBRARIES = 

ifeq ($(current_OS),Darwin)  # macOS

ifeq ($(BINARY),UNIVERSAL)
CFLAGS += -arch arm64 -arch x86_64
CXXFLAGS += -arch arm64 -arch x86_64
LDFLAGS += -arch arm64 -arch x86_64
endif

CHECKER  = warning,information
IGNORE   = 
ifeq ($(HUNTER),BUGS)
CHECKER += --bug-hunting
endif

CXX = clang++
CC = clang
LD = clang++

else  # Linux, Cygwin

CXX = g++
CC = gcc
LD = gcc

endif

RM = rm -f
CP = cp -f

OUTDIR = .objects


.PHONY: info outdir


all: info outdir $(TARGET)

info:
	@echo $(CC)" on "$(current_OS)
	@echo "target: "$(TARGET)
	@echo "install: "$(INSTALL)

outdir:
	@mkdir -p $(OUTDIR)

check:
ifeq ($(current_OS),Darwin)  # macOS - libSLCAN.dylib
	cppcheck --enable=$(CHECKER) --suppressions-list=$(HOME_DIR)/suppress.txt \
	-D__APPLE__ $(DEFINES) $(HEADERS) $(IGNORE) $(SOURCE_DIR)
else
	echo "Please use Cppcheck GUI"
endif

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

pristine:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d

install:
	$(CP) $(TARGET) $(INSTALL)

test: info outdir $(TARGET)
	./$(TARGET) ./vanilla_test.files/test14.json


$(OUTDIR)/main.o: $(MAIN_DIR)/main.c $(SOURCE_DIR)/vanilla.h
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/vanilla.o: $(SOURCE_DIR)/vanilla.c $(SOURCE_DIR)/vanilla.h
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
