global long_mode_start
extern kmain

section .multiboot_header
align 8
header_start:
    dd 0xE85250D6                   ; Magic number = Multiboot2
    dd 0                            ; Architecture = Protected mode i386
    dd header_end - header_start    ; Header Length
    ; Checksum
    dd 0x100000000 - (0xE85250D6 + 0 + (header_end - header_start))

align 8
    dw 5
    dw 1
    dd 20
    dd 1280
    dd 720
    dd 32

align 8     ; end tag
    dw 0    ; type
    dw 0    ; flags
    dd 8    ; size
header_end:

section .text
bits 64
long_mode_start:
    ; Load null into all data segment registers
    mov ax, 0
    ; mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ;            rdi             rsi
    ; void kmain(uintptr_t magic, uintptr_t addr);
    mov rdi, 0x36d76289
    mov rsi, header_start


    ; Call kmain
    call kmain

    hlt

section .bss
bss_end: