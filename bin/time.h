// time.h
#ifndef TIME_H
#define TIME_H

#include "sheaders.h"

void init_time(void);
uint64_t get_clock_ms(void);
void sleep_ms(uint32_t ms);
void sleep_s(uint32_t s);
uint64_t get_cpu_freq_khz(void);

#define getclk(var) ((var) = get_clock_ms())
#define sleep(ms)   sleep_ms(ms)
#define sleeps(s)   sleep_s(s)

#endif