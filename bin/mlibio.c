//FicTuz — I decided anyway, and now I've fully added input :)
#include "sheaders.h"

#define VGA_ADDR   0xB8000 
#define WIDTH      80
#define HEIGHT     25
#define COLOR      0x0F 

#define MAX_INPUT_LEN 256

static volatile uint16_t* const vga = (volatile uint16_t*)VGA_ADDR;
static int x = 0, y = 0;

void clear(void) {
    for (int i = 0; i < WIDTH * HEIGHT; i++)
        vga[i] = (uint16_t)(' ' | (COLOR << 8));
    x = 0;
    y = 0;
}

static void scroll(void) {
    for (int row = 1; row < HEIGHT; row++) {
        for (int col = 0; col < WIDTH; col++) {
            vga[(row - 1) * WIDTH + col] = vga[row * WIDTH + col];
        }
    }
    for (int col = 0; col < WIDTH; col++) {
        vga[(HEIGHT - 1) * WIDTH + col] = (uint16_t)(' ' | (COLOR << 8));
    }
    x = 0;
    y = HEIGHT - 1;
}

void print(const char* str) {
    if (str == NULL) return;
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        if (c == '\n') {
            x = 0;
            y++;
        } else {
            vga[y * WIDTH + x] = (uint16_t)(c | (COLOR << 8));
            x++;
        }
        if (x >= WIDTH) {
            x = 0;
            y++;
        }
        if (y >= HEIGHT) {
            scroll();
        }
    }
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static char scancode_to_ascii(uint8_t scancode) {
    static const char table[] = {
        0,   0,   '1','2','3','4','5','6','7','8','9','0','-','=','\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
        0,   'a','s','d','f','g','h','j','k','l',';','\'','`',
        0,   '\\','z','x','c','v','b','n','m',',','.','/', 0,
        '*', 0,   ' '
    };
    if (scancode < sizeof(table))
        return table[scancode];
    return 0;
}

static void putchar(char c) {
    if (c == '\n') {
        x = 0;
        y++;
    } else {
        vga[y * WIDTH + x] = (uint16_t)((unsigned char)c | (COLOR << 8));
        x++;
    }
    if (x >= WIDTH) {
        x = 0;
        y++;
    }
    if (y >= HEIGHT) {
        scroll();
    }
}

static char getchar(void) {
    uint8_t scancode;
    char ch;
    while (1) {
        if (inb(0x64) & 0x01) {
            scancode = inb(0x60);
            if (scancode < 0x80) {
                ch = scancode_to_ascii(scancode);
                if (ch != 0)
                    return ch;
            }
        }
    }
}

static void readline(char* buf, int max) {
    int pos = 0;
    while (1) {
        char c = getchar();
        if (c == '\n' || c == '\r') {
            putchar('\n');
            buf[pos] = '\0';
            return;
        } else if (c == '\b') {
            if (pos > 0) {
                x--;
                if (x < 0) {
                    x = WIDTH - 1;
                    y = (y > 0) ? y - 1 : 0;
                }
                vga[y * WIDTH + x] = (uint16_t)(' ' | (COLOR << 8));
                pos--;
            }
        } else {
            if (pos < max - 1) {
                buf[pos++] = c;
                putchar(c);
            }
        }
    }
}

void input(char* variable) {
    if (variable == NULL) return;
    readline(variable, MAX_INPUT_LEN);
}