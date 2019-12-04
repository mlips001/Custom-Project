#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdint.h>
#include "Randomize.h"

uint8_t state[1];

// Inits the system. Seed must not be 0.
void Initialize(uint8_t seed)
{
	state[0] = seed;
}

// 
// Do not call this function directly.
// Call getRandomNumber or getRandomLed
uint8_t xorshift8(uint8_t state[static 1])
{
	uint8_t x = state[0];
	x ^= x << 7;
	x ^= x >> 5;
	x ^= x << 3;
	state[0] = x;
	return x;
}

// Returns a random number
uint8_t getRandomNumber()
{
	return xorshift8(state);
}

// Returns a random LED out of the 4
uint8_t getRandomLed()
{
	uint8_t rn = getRandomNumber();
	return (0x01 << (rn % 4));
}