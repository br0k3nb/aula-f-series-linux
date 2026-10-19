CC ?= cc
CFLAGS ?= -O2 -std=c11 -Wall -Wextra -Wpedantic -Werror
CPPFLAGS += -Isrc -Ibuild
LDFLAGS ?= -static

.PHONY: all test clean package
all: build/aula-control

build:
	mkdir -p build

build/gui_embedded.h: app/gui.py tools/embed_gui.py | build
	python3 tools/embed_gui.py $< $@

build/permissions_embedded.h: tools/setup_permissions.py tools/embed_gui.py | build
	python3 tools/embed_gui.py $< $@ aula_permissions_source

NATIVE_SOURCES = src/main.c src/hid_linux.c src/protocol.c src/lighting_ext.c src/keymap.c src/keymap_cli.c src/device_cli.c
build/aula-control: $(NATIVE_SOURCES) src/hid_linux.h src/protocol.h src/lighting_ext.h src/keymap.h src/keymap_cli.h src/device_cli.h build/gui_embedded.h build/permissions_embedded.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(NATIVE_SOURCES) $(LDFLAGS) -o $@

test: all
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_hid_descriptor.c src/hid_linux.c -o build/test-hid
	./build/test-hid
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_protocol.c src/protocol.c -o build/test-protocol
	./build/test-protocol
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_lighting_ext.c src/lighting_ext.c -o build/test-lighting-ext
	./build/test-lighting-ext
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_keymap.c src/keymap.c -o build/test-keymap
	./build/test-keymap
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_keymap_cli.c src/keymap.c src/keymap_cli.c -o build/test-keymap-cli
	./build/test-keymap-cli
	$(CC) $(CPPFLAGS) $(CFLAGS) tests/test_device_cli.c src/keymap.c src/device_cli.c -o build/test-device-cli
	./build/test-device-cli
	python3 tests/test_cli.py
	python3 -m py_compile app/gui.py
	/usr/bin/python3 tests/test_preview_effects.py

package: test
	mkdir -p dist
	tar -czf dist/aula-control-linux-x86_64.tar.gz -C build aula-control -C .. README.md udev

clean:
	rm -rf build
