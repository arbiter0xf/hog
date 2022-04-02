override KERNEL_ELF := hog.elf
override HOG_ISO := hog.iso

CC := cc
LD := ld

CFLAGS ?= -O2 -g -Wall -Wextra -pipe
LDFLAGS ?=

override CFILES := $(shell find ./ -type f -name '*.c')
override OBJ := $(CFILES:.c=.o)
override HEADER_DEPS := $(CFILES:.c=.d)

override LINKER_SCRIPT_ABS := src/linker.ld
override EMULATOR_ROOT := /mnt/f/my/tools/qemu/

override HOG_DEPENDENCIES := ../hog_dependencies
override LIMINE_ROOT := $(HOG_DEPENDENCIES)/limine
override LIMINE_DEPLOY := $(LIMINE_ROOT)/bin/limine-s2deploy
override ISO_OUT := out/iso
override ISO_DEPS_LIMINE_CFG := src/limine.cfg
override ISO_DEPS_LIMINE_FILES :=			\
	$(LIMINE_ROOT)/bin/limine.sys			\
	$(LIMINE_ROOT)/bin/limine-cd.bin		\
	$(LIMINE_ROOT)/bin/limine-eltorito-efi.bin
override HOG_ISO_ABS := $(ISO_OUT)/$(HOG_ISO)

override INTERNALCFLAGS :=   \
	-Iinclude            \
	-std=gnu11           \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic             \
	-mabi=sysv           \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-mcmodel=kernel      \
	-MMD

override INTERNALLDFLAGS :=    \
	-T$(LINKER_SCRIPT_ABS) \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static

.PHONY: all deploy kernel iso
all: iso

kernel: $(KERNEL_ELF)

deploy: iso
	cp $(HOG_ISO_ABS) $(EMULATOR_ROOT)

iso: kernel
	rm -rf $(ISO_OUT)
	mkdir -p $(ISO_OUT)
	cp $(KERNEL_ELF) $(ISO_DEPS_LIMINE_CFG) $(ISO_DEPS_LIMINE_FILES) $(ISO_OUT)/
	xorriso						\
		-as mkisofs				\
		-b limine-cd.bin			\
		-no-emul-boot				\
		-boot-load-size 4			\
		-boot-info-table			\
		--efi-boot limine-eltorito-efi.bin	\
		-efi-boot-part --efi-boot-image		\
		--protective-msdos-label		\
		$(ISO_OUT)				\
		-o $(HOG_ISO_ABS)
	$(LIMINE_DEPLOY) $(HOG_ISO_ABS)

include/stivale2.h:
	curl https://raw.githubusercontent.com/stivale/stivale/master/stivale2.h -o $@

$(KERNEL_ELF): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) $(INTERNALLDFLAGS) -o $@

-include $(HEADER_DEPS)
%.o: %.c include/stivale2.h
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@
