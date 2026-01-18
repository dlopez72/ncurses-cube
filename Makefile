CC ?= clang
CFLAGS = -Wall -Wextra -O3
LIBS = -lm -lncurses
TARGET = ncurses-cube
PREFIX = /usr/local

all: $(TARGET)

$(TARGET): src/main.c
	$(CC) $(CFLAGS) src/main.c -o $(TARGET) $(LIBS)

install: $(TARGET)
	install -D $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all install uninstall clean
