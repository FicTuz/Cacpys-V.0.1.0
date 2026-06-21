//it doesn't use mlibio, because if mlibio dies, it won't be possible to display it on the screen.
#include "KERPANOS.h"

#define VGA_MEMORY          ((uint16_t*)0xB8000)
#define VGA_WIDTH           80
#define VGA_HEIGHT          25

#define COLOR_BLACK         0x0
#define COLOR_RED           0x4
#define COLOR_YELLOW        0xE
#define COLOR_WHITE         0xF

#define MAKE_ATTR(fg, bg)   (((bg) << 4) | (fg))

static int cursor_row = 0;
static int cursor_col = 0;

static void clear_screen(void) {
    uint16_t blank = (uint16_t)(' ' | (MAKE_ATTR(COLOR_WHITE, COLOR_BLACK) << 8));
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEMORY[i] = blank;
    }
    cursor_row = 0;
    cursor_col = 0;
}

static void putchar_attr(char c, uint8_t color) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
        if (cursor_row >= VGA_HEIGHT) {
            cursor_row = VGA_HEIGHT - 1;
        }
        return;
    }
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
        if (cursor_row >= VGA_HEIGHT) {
            cursor_row = VGA_HEIGHT - 1;
        }
    }
    uint16_t attr = (uint16_t)((color << 8) | c);
    VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_col] = attr;
    cursor_col++;
}

static void print_string_attr(const char* str, uint8_t color) {
    while (*str) {
        putchar_attr(*str, color);
        str++;
    }
}

static void print_hex(uint32_t value) {
    putchar_attr('0', COLOR_RED);
    putchar_attr('x', COLOR_RED);

    const char hex_digits[] = "0123456789ABCDEF";
    char buffer[9];
    for (int i = 7; i >= 0; i--) {
        buffer[i] = hex_digits[value & 0xF];
        value >>= 4;
    }
    buffer[8] = '\0';
    for (int i = 0; i < 8; i++) {
        putchar_attr(buffer[i], COLOR_RED);
    }
}

void NORETURN kpanic(uint32_t error_code, const char* description) {
    clear_screen();

    print_string_attr("[KERPANOS]\n", COLOR_YELLOW);
    print_string_attr("kernel Panic!\n\n", COLOR_RED);
    print_string_attr("Error code: ", COLOR_RED);
    print_hex(error_code);

    if (description != NULL && description[0] != '\0') {
        print_string_attr(" - ", COLOR_RED);
        print_string_attr(description, COLOR_RED);
    }

    putchar_attr('\n', COLOR_RED);
    print_string_attr("[try restarting the OS, if it doesn't help, reinstall the OS.]\n",
                      COLOR_YELLOW);

    for (;;) {
#if defined(__i386__) || defined(__x86_64__)
        __asm__ volatile("hlt");
#else
        for (volatile int i = 0; i < 100000; i++);
#endif
    }
}
