BIN = openbikecomputer-core
DESTDIR ?= /usr/bin

SRC = src/main.c \
      $(SYSROOT)/usr/include/lvgl/lv_drivers/wayland/protocols/wayland-xdg-shell-client-protocol.c

INCLUDE = -Isrc \
          -I$(SYSROOT)/usr/include \
          -I$(SYSROOT)/usr/include/lvgl \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers/wayland \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers/wayland/protocols

CC ?= gcc
CCLD ?= gcc

CFLAGS += $(INCLUDE)
LDFLAGS +=
LIBS += -L$(SYSROOT)/usr/lib/ -llvgl -llv_drivers -lwayland-client -lxkbcommon -lwayland-cursor

OBJS = $(patsubst %.c, %.o, $(SRC))

all: $(BIN)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CCLD) $(LDFLAGS) -o $@  $(OBJS) $(LIBS)

install:
	install -D $(BIN) $(DESTDIR)/$(BIN)

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean install
