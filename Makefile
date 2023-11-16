kernel_source_files := $(shell find src/impl/kernel -name *.cpp)
kernel_object_files := $(patsubst src/impl/kernel/%.cpp, build/kernel/%.o, $(kernel_source_files))

x86_64_cpp_source_files := $(shell find src/impl/x86_64 -name *.cpp)
x86_64_cpp_object_files := $(patsubst src/impl/x86_64/%.cpp, build/x86_64/%.o, $(x86_64_cpp_source_files))

libc_source_files := $(shell find src/libc -name *.cpp)
libc_object_files := $(patsubst src/libc/%.cpp, build/libc/%.o, $(libc_source_files))
libc_c_source_files := $(shell find src/libc -name *.c)
libc_c_object_files := $(patsubst src/libc/%.c, build/libc/%.o, $(libc_c_source_files))

x86_64_asm_source_files := $(shell find src/impl/x86_64 -name *.asm)
x86_64_asm_object_files := $(patsubst src/impl/x86_64/%.asm, build/x86_64/%.o, $(x86_64_asm_source_files))

x86_64_object_files := $(x86_64_cpp_object_files) $(x86_64_asm_object_files) $(libc_object_files) $(libc_c_object_files)

$(kernel_object_files): build/kernel/%.o : src/impl/kernel/%.cpp
	mkdir -p $(dir $@) && \
	g++ --std=c++23 -c -I src/intf -I src/libc -ffreestanding $(patsubst build/kernel/%.o, src/impl/kernel/%.cpp, $@) -o $@
	
$(x86_64_cpp_object_files): build/x86_64/%.o : src/impl/x86_64/%.cpp
	mkdir -p $(dir $@) && \
	g++ --std=c++23 -c -I src/intf -I src/libc -ffreestanding $(patsubst build/x86_64/%.o, src/impl/x86_64/%.cpp, $@) -o $@

$(libc_object_files): build/libc/%.o : src/libc/%.cpp
	mkdir -p $(dir $@) && \
	g++ --std=c++23 -c -I src/intf -ffreestanding $(patsubst build/libc/%.o, src/libc/%.cpp, $@) -o $@

$(libc_c_object_files): build/libc/%.o : src/libc/%.c
	mkdir -p $(dir $@) && \
	gcc -c -I src/intf -ffreestanding $(patsubst build/libc/%.o, src/libc/%.c, $@) -o $@
	
$(x86_64_asm_object_files): build/x86_64/%.o : src/impl/x86_64/%.asm
	mkdir -p $(dir $@) && \
	nasm -f elf64 $(patsubst build/x86_64/%.o, src/impl/x86_64/%.asm, $@) -o $@ 

.PHONY: build-x86_64 run debug clean

build-x86_64: $(kernel_object_files) $(x86_64_object_files)
	mkdir -p dist/x86_64 && .
	ld -n -o dist/x86_64/kernel.bin -T targets/x86_64/linker.ld $(kernel_object_files) $(x86_64_object_files) && \
	cp dist/x86_64/kernel.bin targets/x86_64/iso/boot/kernel.bin && \
	grub-mkrescue /usr/lib/grub/i386-pc -o dist/x86_64/kernel.iso targets/x86_64/iso

run: build-x86_64
# -no-shutdown -no-reboot
	qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -serial stdio

run-debug: build-x86_64
	mkdir -p logs && \
	qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -serial stdio -d int -no-reboot -no-shutdown -D logs/qemu.log && \
	objdump -d dist/x86_64/kernel.bin > logs/objdump.log

debug: build-x86_64
	qemu-system-x86_64 -cdrom dist/x86_64/kernel.iso -s &
	gdb -ex "target remote localhost:1234" -ex "symbol-file dist/x86_64/kernel.bin"

clean:
	rm -rf build dist