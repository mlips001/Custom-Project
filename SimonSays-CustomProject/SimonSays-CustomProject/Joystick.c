// ADC for joystick


void ADC_init() {
	ADMUX = (1<<REFS0);
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint16_t ADC_read(uint8_t ch){
	ch &= 0b00000111;
	ADMUX = (ADMUX & 0xF8)|ch;
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	
	return (ADC);
}

unsigned short a, b, c, d = 0x00;

void Tick(){
	unsigned short AD_LR = ADC_read(0);
	unsigned short AD_UD = ADC_read(1);
	
	if (AD_LR < 100 ) {
		a = 0x01;
	}
	
	else if (AD_LR > 900) {
		b = 0x02;
	}
	
	else if (AD_UD < 100) {
		c = 0x03;
	}
	
	else if (AD_UD > 900) {
		d = 0x04;
	}
	
	else if (AD_LR > 540 && AD_UD > 530 ) {
		a = 0x00;
		b = 0x00;
		c = 0x00;
		d = 0x00;
	}
}
	