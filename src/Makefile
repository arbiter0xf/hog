include ../vars.mk

CC := cc
LD := ld

CFLAGS ?= -O2 -g -Wall -Wextra -pipe
LDFLAGS ?=

override CFILES := $(shell find ./ -type f -name '*.c')
override ASMFILES := $(shell find ./ -type f -name '*.asm')
override OBJ := $(CFILES:.c=.o) $(ASMFILES:.asm=.o)
override HEADER_DEPS := $(CFILES:.c=.d)

override LINKER_SCRIPT_PATH := linker.ld

override INTERNALCFLAGS :=   \
	-I../include         \
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
	-T$(LINKER_SCRIPT_PATH) \
	-nostdlib              \
	-zmax-page-size=0x1000 \
	-static

.PHONY: all clean deploy kernel iso
all: $(KERNEL_ELF)

clean:
	rm -rf $(KERNEL_ELF) $(OBJ) $(HEADER_DEPS)

../include/stivale2.h:
	curl https://raw.githubusercontent.com/stivale/stivale/master/stivale2.h -o $@

$(KERNEL_ELF): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) $(INTERNALLDFLAGS) -o $@

-include $(HEADER_DEPS)
%.o: %.c ../include/stivale2.h
	$(CC) $(CFLAGS) $(INTERNALCFLAGS) -c $< -o $@

%.o: %.asm
	nasm -f elf64 $<
