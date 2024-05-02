TARGET  = json

OBJECTS = $(OUTDIR)/main.o $(OUTDIR)/vanilla.o

DEFINES = 

HEADERS = 

CFLAGS += -O2 -Wall -Wno-parentheses \
	-fno-strict-aliasing \
	$(DEFINES) \
	$(HEADERS)

CXXFLAGS += -O2 -g -Wall -pthread \
	$(DEFINES) \
	$(HEADERS)

LDFLAGS  += 

ifeq ($(BINARY),UNIVERSAL)
CFLAGS += -arch arm64 -arch x86_64
CXXFLAGS += -arch arm64 -arch x86_64
LDFLAGS += -arch arm64 -arch x86_64
endif

LIBRARIES = -lpthread

CXX = clang++
CC = clang
LD = clang
RM = rm -f
CP = cp -f

OUTDIR = .objects

.PHONY: info outdir


all: info outdir $(TARGET)

info:
	@echo $(CXX)" on "$(current_OS)
	@echo "target: "$(TARGET)
	@echo "install: "$(INSTALL)

outdir:
	@mkdir -p $(OUTDIR)

clean:
	$(RM) $(TARGET) $(OUTDIR)/*.o $(OUTDIR)/*.d


$(OUTDIR)/main.o: main.c
	$(CC) $(CXXFLAGS) -MMD -MF $*.d -o $@ -c $<

$(OUTDIR)/vanilla.o: vanilla.c
	$(CC) $(CFLAGS) -MMD -MF $*.d -o $@ -c $<


$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBRARIES)
	@echo "\033[1mTarget '"$@"' successfully build\033[0m"
