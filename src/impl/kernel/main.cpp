#include <Base.hpp>
#include <Defer.hpp>
#include <Idt.hpp>
#include <Isr.hpp>
#include <Logging.hpp>
#include <Pic.hpp>
#include <Qemu.hpp>
#include <Vga.hpp>
#include <multiboot2.h>

#include <stdint.h>
#include <sys/syscall.h>
#include <unistd.h>

Hyper::Qemu qemu;
Hyper::Idt idt;

void rainbow(Hyper::Vga vga, char *s) {
    Hyper::Vga::Color colors[] = {
        Hyper::Vga::Color::LightRed,   Hyper::Vga::Color::Yellow,
        Hyper::Vga::Color::LightGreen, Hyper::Vga::Color::LightCyan,
        Hyper::Vga::Color::LightBlue,  Hyper::Vga::Color::Pink,
    };

    for (int i = 0; s[i] != '\0'; i++) {
        vga.setColor(colors[i % 6], Hyper::Vga::Color::Black);
        vga.putchar(s[i]);
    }

    vga.setColor(Hyper::Vga::Color::White, Hyper::Vga::Color::Black);
}

extern "C" void kmain(uintptr_t magic, uintptr_t addr) {
    qemu = Hyper::Qemu();
    Hyper::Isr::isrInstall(&idt);
    Hyper::Isr::irqInstall();

    Hyper::Vga vga = Hyper::Vga();
    vga.clear();

    // Hyper::PIC pic = Hyper::PIC();
    // pic.remap(0x20, 0x28); // remap PIC1 to 0x20 and PIC2 to 0x28
    // pic.unmask(0);         // unmask IRQ0

    // 10 / 0;
    // syscall(SYS_write, 1, "Hello, world!\n", 14);

    // Optional<int> a = Optional<int>(10);
    // int b = a.unwrap_or(0);
    // qemu.printf("a: %d\n", b);

    // Optional<int> a = Optional<int>();
    // a.unwrap();

    multiboot_tag *tag;
    uintptr_t size;

    if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
        PANIC("Invalid magic number: 0x%x\n", magic);
        return;
    }

    if (addr & 7) {
        PANIC("Unaligned mbi: 0x%x\n", addr);
        return;
    }

    size = addr;
    qemu.printf("Announced mbi size 0x%x\n", size);
    for (tag = (multiboot_tag *)(addr + 8); tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = (multiboot_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7))) {
        qemu.printf("Tag 0x%x, Size 0x%x\n", tag->type, tag->size);
        switch (tag->type) {
        case MULTIBOOT_TAG_TYPE_CMDLINE:
            qemu.printf("Command line = %s\n",
                        ((multiboot_tag_string *)tag)->string);
            break;
        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            qemu.printf("Boot loader name = %s\n",
                        ((multiboot_tag_string *)tag)->string);
            break;
        case MULTIBOOT_TAG_TYPE_MODULE:
            qemu.printf("Module at 0x%x-0x%x. Command line %s\n",
                        ((multiboot_tag_module *)tag)->mod_start,
                        ((multiboot_tag_module *)tag)->mod_end,
                        ((multiboot_tag_module *)tag)->cmdline);
            break;
        case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
            qemu.printf("mem_lower = %uKB, mem_upper = %uKB\n",
                        ((multiboot_tag_basic_meminfo *)tag)->mem_lower,
                        ((multiboot_tag_basic_meminfo *)tag)->mem_upper);
            break;
        case MULTIBOOT_TAG_TYPE_BOOTDEV:
            qemu.printf("Boot device 0x%x,%u,%u\n",
                        ((multiboot_tag_bootdev *)tag)->biosdev,
                        ((multiboot_tag_bootdev *)tag)->slice,
                        ((multiboot_tag_bootdev *)tag)->part);
            break;
        case MULTIBOOT_TAG_TYPE_MMAP: {
            multiboot_memory_map_t *mmap;

            qemu.printf("mmap\n");

            for (mmap = ((multiboot_tag_mmap *)tag)->entries;
                 (multiboot_uint8_t *)mmap <
                 (multiboot_uint8_t *)tag + tag->size;
                 mmap = (multiboot_memory_map_t *)((uintptr_t)mmap +
                                                   ((multiboot_tag_mmap *)tag)
                                                       ->entry_size))
                qemu.printf(" base_addr = 0x%x%x,"
                            " length = 0x%x%x, type = 0x%x\n",
                            (mmap->addr >> 32), (mmap->addr & 0xffffffff),
                            (mmap->len >> 32), (mmap->len & 0xffffffff),
                            mmap->type);
        } break;
        case MULTIBOOT_TAG_TYPE_FRAMEBUFFER: {
            uint32_t color;
            uintptr_t i;
            multiboot_tag_framebuffer *tagfb = (multiboot_tag_framebuffer *)tag;
            void *fb = (void *)(uintptr_t)tagfb->common.framebuffer_addr;

            switch (tagfb->common.framebuffer_type) {
            case MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED: {
                uintptr_t best_distance, distance;
                multiboot_color *palette;

                palette = tagfb->framebuffer_palette;

                color = 0;
                best_distance = 4 * 256 * 256;

                for (i = 0; i < tagfb->framebuffer_palette_num_colors; i++) {
                    distance =
                        (0xff - palette[i].blue) * (0xff - palette[i].blue) +
                        palette[i].red * palette[i].red +
                        palette[i].green * palette[i].green;
                    if (distance < best_distance) {
                        color = i;
                        best_distance = distance;
                    }
                }
            } break;

            case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
                color = ((1 << tagfb->framebuffer_blue_mask_size) - 1)
                        << tagfb->framebuffer_blue_field_position;
                break;

            case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
                color = '\\' | 0x0100;
                break;

            default:
                color = 0xffffffff;
                break;
            }

            for (i = 0; i < tagfb->common.framebuffer_width &&
                        i < tagfb->common.framebuffer_height;
                 i++) {
                switch (tagfb->common.framebuffer_bpp) {
                case 8: {
                    uint8_t *pixel =
                        (uint8_t *)fb + tagfb->common.framebuffer_pitch * i + i;
                    *pixel = color;
                } break;
                case 15:
                case 16: {
                    uint16_t *pixel = (uint16_t *)fb +
                                      tagfb->common.framebuffer_pitch * i +
                                      2 * i;
                    *pixel = color;
                } break;
                case 24: {
                    uint32_t *pixel = (uint32_t *)fb +
                                      tagfb->common.framebuffer_pitch * i +
                                      3 * i;
                    *pixel = (color & 0xffffff) | (*pixel & 0xff000000);
                } break;

                case 32: {
                    uint32_t *pixel = (uint32_t *)fb +
                                      tagfb->common.framebuffer_pitch * i +
                                      4 * i;
                    *pixel = color;
                } break;
                }
            }
            break;
        }
        }
    }
    tag = (multiboot_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7));
    qemu.printf("Total mbi size 0x%x\n", tag - addr);
}

// extern "C" void kmain(uint32_t magic, multiboot_header *header) {
//     Hyper::Idt idt{};

//     Hyper::Qemu qemu;
//     Hyper::Vga vga;

//     vga.clear();
//     if (magic != MULTIBOOT2_BOOTLOADER_MAGIC) {
//         vga.setColor(Hyper::Vga::Color::LightRed, Hyper::Vga::Color::Black);
//         vga.puts("PANIC! magic number is invalid.");
//         return;
//     }

//     vga.puts("\n");
//     vga.setColor(Hyper::Vga::Color::LightRed, Hyper::Vga::Color::Black);
//     vga.puts("   _____ __ __ _____ _____ _____ \n");
//     vga.setColor(Hyper::Vga::Color::Yellow, Hyper::Vga::Color::Black);
//     vga.puts("  |  |  |  |  |  _  |   __| __  |\n");
//     vga.setColor(Hyper::Vga::Color::LightGreen, Hyper::Vga::Color::Black);
//     vga.puts("  |     |_   _|   __|   __|    -|\n");
//     vga.setColor(Hyper::Vga::Color::LightCyan, Hyper::Vga::Color::Black);
//     vga.puts("  |__|__| |_| |__|  |_____|__|__|\n");
//     vga.setColor(Hyper::Vga::Color::LightBlue, Hyper::Vga::Color::Black);
//     vga.puts("                           \n");
//     rainbow(vga, "Welcome to the HyperOS Kernel!");

//     vga.puts("\n\n");
//     vga.setColor(Hyper::Vga::Color::White, Hyper::Vga::Color::Black);
//     vga.puts("Multiboot2 header:\n");
//     vga.printf("  magic: 0x%x\n", magic);
//     vga.printf("  arch: 0x%x\n", header->architecture);
//     vga.printf("  header_length: 0x%x\n", header->header_length);
//     vga.printf("  checksum: 0x%x\n", header->checksum);

//     vga.puts("\n");
// }