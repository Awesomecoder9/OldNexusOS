ASM=nasm
CC=gcc
SRC_DIR=src
BUILD_DIR=build

# FLOPPY DISK

floppy_image: $(BUILD_DIR)/main.img 
$(BUILD_DIR)/main.img: bootloader kernel
	dd if=/dev/zero of=$(BUILD_DIR)/main.img count=2880
	mkfs.fat -F 12 -n "NEXUS" $(BUILD_DIR)/main.img
	dd if=$(BUILD_DIR)/bootloader.bin of=$(BUILD_DIR)/main.img conv=notrunc
	mcopy -i $(BUILD_DIR)/main.img $(BUILD_DIR)/kernel.bin "::kernel.bin"


# BOOTLOADER

bootloader: $(BUILD_DIR)/bootloader.bin
$(BUILD_DIR)/bootloader.bin:
	$(ASM) $(SRC_DIR)/bootloader/old.asm -f bin -o $(BUILD_DIR)/bootloader.bin


# KERNEL

kernel: $(BUILD_DIR)/kernel.bin
$(BUILD_DIR)/kernel.bin:
	$(ASM) $(SRC_DIR)/kernel/main.asm -f bin -o $(BUILD_DIR)/kernel.bin

clean:
	rm -rf $(BUILD_DIR)/*