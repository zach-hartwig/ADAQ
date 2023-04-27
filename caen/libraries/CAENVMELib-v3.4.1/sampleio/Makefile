TARGET = IODemo
CFLAGS = -O2 -g
LDFLAGS = -lm -lCAENVME

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = Pulser_Demo.o Scaler_Demo.o Sys_Func.o main_console.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGET)
