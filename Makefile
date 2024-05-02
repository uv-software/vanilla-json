TARGET  = json

OBJECTS = $(OUTDIR)/main.o $(OUTDIR)/vanilla.o

DEFINES = 

HEADERS = 

CFLAGS += -std=c90 -ansi -pedantic -O0 -g -Wall -Wextra -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -O0 -g -Wall -Wextra -pthread \
	$(DEFINES) \
	$(HEADERS)

LDFLAGS  += 

LIBRARIES = -lpthread

CXX = g++
CC = gcc
LD = gcc
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

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d


$(OUTDIR)/main.o: main.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/vanilla.o: vanilla.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
