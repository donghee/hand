#Set this to @ to keep the makefile quiet
SILENCE = @

.SUFFIXS : .c .o
CFLAGS = `pkg-config --libs --cflags libmodbus`
# CFLAGS = -I/usr/include/modbus  -lmodbus  

EXE = hand
OBJS = $(EXE).o arduino-serial.o
SRCS = $(OBJS:.o=.c)

all: $(EXE)

$(EXE): $(OBJS) 
	gcc $(OBJS) $(CFLAGS) -o $@

ctags:
	rm -f TAGS
	ctags -eR
