// time.c
#include "time.h"

static uint64_t tsc_khz = 0;

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    __asm__ volatile ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

static inline uint16_t pit_read_counter(void) {
    outb(0x43, 0x00);
    uint8_t low  = inb(0x40);
    uint8_t high = inb(0x40);
    return (high << 8) | low;
}

static void calibrate_tsc(void) {
    const uint16_t divisor = 0xFFFF;
    outb(0x43, 0x30);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);

    uint64_t start = rdtsc();

    while (pit_read_counter() != 0) {
        __asm__ volatile ("pause");
    }

    uint64_t end = rdtsc();
    uint64_t diff = end - start;
    tsc_khz = (diff * 1193182) / divisor / 1000;

    if (tsc_khz == 0) {
        tsc_khz = 2400000;
    }
}

void init_time(void) {
    calibrate_tsc();
}

uint64_t get_clock_ms(void) {
    return rdtsc() / tsc_khz;
}

void sleep_ms(uint32_t ms) {
    uint64_t target = rdtsc() + (uint64_t)ms * tsc_khz;
    while (rdtsc() < target) {
        __asm__ volatile ("pause");
    }
}

void sleep_s(uint32_t s) {
    sleep_ms(s * 1000);
}

uint64_t get_cpu_freq_khz(void) {
    return tsc_khz;
}