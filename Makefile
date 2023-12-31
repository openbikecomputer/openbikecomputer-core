BIN = openbikecomputer-core
ROOTDIR ?=
BINDIR ?= usr/bin
CONFDIR ?= etc/openbikecomputer
SHAREDIR ?= usr/share/openbikecomputer

SRC = src/main.c \
      src/log/log.c \
      src/ui/ui.c \
      src/ui/screen/main_screen.c \
      src/ui/screen/data_screen.c \
      src/ui/screen/navigation_screen.c \
      src/ui/screen/profile_screen.c \
      src/ui/screen/rider_screen.c \
      src/ui/screen/bike_screen.c \
      src/ui/screen/results_screen.c \
      src/ui/screen/routes_screen.c \
      src/ui/screen/settings_screen.c \
      src/ui/lvgl_helper.c \
      src/config/obc_config.c \
      src/config/bike_config.c \
      src/config/rider_config.c \
      src/config/system_config.c \
      src/config/user_config.c \
      src/config/libconfig_helper.c \
      src/data/data.c \
      src/data/data_manager.c \
      src/data/data_recorder.c \
      src/utils/simulator.c \
      src/utils/fifo.c

INCLUDE = -Isrc \
          -Isrc/log \
          -Isrc/ui \
          -Isrc/ui/screen \
          -Isrc/data \
          -Isrc/config \
          -Isrc/utils \
          -I$(SYSROOT)/usr/include \
          -I$(SYSROOT)/usr/include/lvgl \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers/wayland \
          -I$(SYSROOT)/usr/include/lvgl/lv_drivers/wayland/protocols

CC ?= gcc
CCLD ?= gcc

CFLAGS += $(INCLUDE) -D_DEFAULT_SOURCE -D_REENTRANT -Wall -Werror -pedantic -std=c99
LDFLAGS +=
LIBS += -L$(SYSROOT)/usr/lib/ -llvgl -llv_drivers -llv-wayland-protocol -lwayland-client -lxkbcommon -lwayland-cursor -lpthread -lconfig

OBJS = $(patsubst %.c, %.o, $(SRC))

all: $(BIN)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN): $(OBJS)
	$(CCLD) $(LDFLAGS) -o $@  $(OBJS) $(LIBS)

install:
	install -D $(BIN) $(ROOTDIR)/$(BINDIR)/$(BIN)
	install -d $(ROOTDIR)/$(CONFDIR)
	install config/*.conf $(ROOTDIR)/$(CONFDIR)/
	install -d $(ROOTDIR)/$(SHAREDIR)/simulation
	install simulation/*.csv $(ROOTDIR)/$(SHAREDIR)/simulation
	install -d $(ROOTDIR)/$(SHAREDIR)/images
	install images/* $(ROOTDIR)/$(SHAREDIR)/images

clean:
	rm -f $(OBJS) $(BIN)

.PHONY: all clean install
