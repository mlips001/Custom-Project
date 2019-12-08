
#ifndef Randomize_H_
#define Randomize_H_

// Re-init with a given seed
void Initialize(uint8_t  seed);

// Returns a random number between 0 and 255
uint8_t getRandomNumber();

// Returns a random LED pattern 
uint8_t getRandomLed();

#endif 