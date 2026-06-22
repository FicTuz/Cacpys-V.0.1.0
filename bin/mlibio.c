//FicTuz — I decided anyway, and now I've fully added input :)
//it seemed to me that I had too much free time, so I added all these functions :)
#include "sheaders.h"
#include "mlibio.h"

#define VGA_ADDR   0xB8000 
#define WIDTH      80
#define HEIGHT     25
#define COLOR      0x0F 

#define MAX_INPUT_LEN 256
#define HEAP_SIZE     65536

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

void putchar(char c) {
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

char getchar(void) {
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

void puts(const char* s) {
    print(s);
    putchar('\n');
}

static char* int_to_str(int value, char* buffer, int base) {
    char* buf = buffer;
    int sign = 0;
    unsigned int num;
    if (base == 10 && value < 0) {
        sign = 1;
        num = -value;
    } else {
        num = (unsigned int)value;
    }
    do {
        int digit = num % base;
        *buf++ = (digit < 10) ? '0' + digit : 'A' + digit - 10;
        num /= base;
    } while (num);
    if (sign) *buf++ = '-';
    *buf = '\0';
    char* start = buffer;
    char* end = buf - 1;
    while (start < end) {
        char tmp = *start;
        *start++ = *end;
        *end-- = tmp;
    }
    return buffer;
}

int printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int count = 0;
    char num_buf[32];

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == 'c') {
                char c = (char)va_arg(args, int);
                putchar(c);
                count++;
            } else if (*fmt == 's') {
                const char* s = va_arg(args, const char*);
                if (s == NULL) {
                    print("(null)");
                    count += 6;
                } else {
                    while (*s) { putchar(*s++); count++; }
                }
            } else if (*fmt == 'd' || *fmt == 'i') {
                int val = va_arg(args, int);
                int_to_str(val, num_buf, 10);
                char* p = num_buf;
                while (*p) { putchar(*p++); count++; }
            } else if (*fmt == 'x' || *fmt == 'X') {
                unsigned int val = va_arg(args, unsigned int);
                int_to_str((int)val, num_buf, 16);
                char* p = num_buf;
                while (*p) { putchar(*p++); count++; }
            } else if (*fmt == 'u') {
                unsigned int val = va_arg(args, unsigned int);
                int_to_str((int)val, num_buf, 10);
                char* p = num_buf;
                while (*p) { putchar(*p++); count++; }
            } else if (*fmt == 'p') {
                uintptr_t val = va_arg(args, uintptr_t);
                putchar('0'); putchar('x'); count += 2;
                int_to_str((int)val, num_buf, 16);
                char* p = num_buf;
                while (*p) { putchar(*p++); count++; }
            } else if (*fmt == '%') {
                putchar('%'); count++;
            } else {
                putchar('%'); putchar(*fmt); count += 2;
            }
        } else {
            putchar(*fmt);
            count++;
        }
        fmt++;
    }
    va_end(args);
    return count;
}

size_t strlen(const char* s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n-- && *s1 && (*s1 == *s2)) { s1++; s2++; }
    if (n == (size_t)-1) return 0;
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* d = dest;
    while (n-- && (*d++ = *src++));
    while (n-- > 0) *d++ = '\0';
    return dest;
}

char* strcat(char* dest, const char* src) {
    char* d = dest;
    while (*d) d++;
    while ((*d++ = *src++));
    return dest;
}

char* strchr(const char* s, int c) {
    while (*s) {
        if (*s == c) return (char*)s;
        s++;
    }
    return NULL;
}

char* strstr(const char* haystack, const char* needle) {
    if (!*needle) return (char*)haystack;
    for (; *haystack; haystack++) {
        const char* h = haystack;
        const char* n = needle;
        while (*h && *n && (*h == *n)) { h++; n++; }
        if (!*n) return (char*)haystack;
    }
    return NULL;
}

static char* strtok_state = NULL;
char* strtok(char* str, const char* delim) {
    char* token_start;
    if (str) {
        token_start = str;
    } else {
        if (!strtok_state) return NULL;
        token_start = strtok_state;
    }
    while (*token_start && strchr(delim, *token_start))
        token_start++;
    if (!*token_start) {
        strtok_state = NULL;
        return NULL;
    }
    char* token_end = token_start;
    while (*token_end && !strchr(delim, *token_end))
        token_end++;
    if (*token_end) {
        *token_end = '\0';
        strtok_state = token_end + 1;
    } else {
        strtok_state = NULL;
    }
    return token_start;
}

void* memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    while (n--) *d++ = *s++;
    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    unsigned char* d = dest;
    const unsigned char* s = src;
    if (d < s) {
        while (n--) *d++ = *s++;
    } else {
        d += n;
        s += n;
        while (n--) *--d = *--s;
    }
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* a = s1, *b = s2;
    while (n--) {
        if (*a != *b) return *a - *b;
        a++; b++;
    }
    return 0;
}

typedef struct block_header {
    size_t size;
    struct block_header* next;
    int free;
} block_header_t;

static uint8_t heap[HEAP_SIZE];
static block_header_t* free_list = NULL;

static void heap_init(void) {
    if (free_list == NULL) {
        free_list = (block_header_t*)heap;
        free_list->size = HEAP_SIZE - sizeof(block_header_t);
        free_list->next = NULL;
        free_list->free = 1;
    }
}

static void split_block(block_header_t* block, size_t needed) {
    size_t remaining = block->size - needed - sizeof(block_header_t);
    if (remaining > sizeof(block_header_t) + 8) {
        block_header_t* new_block = (block_header_t*)((uint8_t*)block + sizeof(block_header_t) + needed);
        new_block->size = remaining;
        new_block->free = 1;
        new_block->next = block->next;
        block->next = new_block;
        block->size = needed;
    }
}

static void merge_blocks(void) {
    block_header_t* cur = free_list;
    while (cur && cur->next) {
        if (cur->free && cur->next->free) {
            if ((uint8_t*)cur + sizeof(block_header_t) + cur->size == (uint8_t*)cur->next) {
                cur->size += sizeof(block_header_t) + cur->next->size;
                cur->next = cur->next->next;
                continue;
            }
        }
        cur = cur->next;
    }
}

void* malloc(size_t size) {
    if (size == 0) return NULL;
    heap_init();
    size = (size + 3) & ~3;
    block_header_t* cur = free_list;
    while (cur) {
        if (cur->free && cur->size >= size) {
            cur->free = 0;
            split_block(cur, size);
            return (void*)(cur + 1);
        }
        cur = cur->next;
    }
    return NULL;
}

void free(void* ptr) {
    if (ptr == NULL) return;
    block_header_t* block = (block_header_t*)ptr - 1;
    block->free = 1;
    merge_blocks();
}

void* calloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = malloc(total);
    if (ptr) memset(ptr, 0, total);
    return ptr;
}

void* realloc(void* ptr, size_t new_size) {
    if (ptr == NULL) return malloc(new_size);
    if (new_size == 0) { free(ptr); return NULL; }
    block_header_t* block = (block_header_t*)ptr - 1;
    size_t old_size = block->size;
    if (old_size >= new_size) return ptr;
    void* new_ptr = malloc(new_size);
    if (new_ptr) {
        memcpy(new_ptr, ptr, old_size < new_size ? old_size : new_size);
        free(ptr);
    }
    return new_ptr;
}