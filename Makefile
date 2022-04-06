include vars.mk

override EMULATOR_ROOT := /mnt/f/my/tools/qemu/
override HOG_DEPENDENCIES := $(PROJECT_ROOT)/../hog_dependencies
override LIMINE_ROOT := $(HOG_DEPENDENCIES)/limine
override LIMINE_DEPLOY := $(LIMINE_ROOT)/bin/limine-s2deploy

override ISO_OUT := $(PROJECT_ROOT)/out/iso
override ISO_DEPS_LIMINE_CFG := $(PROJECT_ROOT)/src/limine.cfg
override ISO_DEPS_LIMINE_FILES :=			\
	$(LIMINE_ROOT)/bin/limine.sys			\
	$(LIMINE_ROOT)/bin/limine-cd.bin		\
	$(LIMINE_ROOT)/bin/limine-eltorito-efi.bin
override HOG_ISO := hog.iso
override HOG_ISO_PATH := $(ISO_OUT)/$(HOG_ISO)

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
		-o $(HOG_ISO_PATH)
	$(LIMINE_DEPLOY) $(HOG_ISO_PATH)

deploy: iso
	cp $(HOG_ISO_PATH) $(EMULATOR_ROOT)

kernel:
	make -C src all
