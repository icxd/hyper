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
    dw 5            ; Type = Framebuffer information
    dw 1            ; Flags = required
    dd 20           ; Size = 20
    dd 1280         ; Width = 1280
    dd 720          ; Height = 720
    dd 32           ; Depth = 32

align 8
    dw 4            ; Type = ELF symbols
    dw 0            ; Flags = required
    dd 8            ; Size = 8
    dd 0x100000     ; Number of entries
    dd 0x100000     ; Size of each entry

align 8
    dw 0            ; Type = End tag
    dw 0            ; Flags = required
    dd 8            ; Size = 8
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

    ; Set up stack
    mov rsp, stack_end
    mov rbp, rsp

    ;            rdi             rsi
    ; void kmain(uintptr_t magic, uintptr_t addr);
    mov rdi, 0x36d76289
    mov rsi, header_start

    ; Call kmain
    call kmain

.loop:
    hlt
    jmp .loop

section .bss
align 8
stack:
    resb 0x1000
stack_end: