// Tunes

void set_PWM(double frequency) {
	static double current_frequency;
	
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; }
			
		else { TCCR3B |= 0x03; }
			
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
			
		else if (frequency > 31250) { OCR3A = 0x0000; }
			
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
			
		TCNT3 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR3A = (1 << COM3A0);
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	set_PWM(0);
}

void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

void playSound(char tune)
{
	if(tune == 0x01)
	{
		// C4
		set_PWM(261.63);
	}
	else if(tune == 0x02)
	{
		// D4
		set_PWM(293.66);
	}
	else if (tune == 0x04)
	{
		// E4
		set_PWM(329.63);
	}
	else if(tune == 0x08)
	{
		// F4
		set_PWM(349.23);
	}
	else
	{
		// No sound will play
		set_PWM(0);
	}
}