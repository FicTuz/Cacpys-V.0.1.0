//FicTuz
//I don't add input, because it's too hard for my first kernel
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;
#define NULL 0

#define VGA_ADDR   0xB8000 
#define WIDTH      80
#define HEIGHT     25
#define COLOR      0x0F 

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
