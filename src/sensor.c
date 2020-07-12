#include <stdio.h>
#include <bcm2835.h>
#include "sensor.h"

int sensor(void) {


	uint8_t index = 0, byte = 0, counter = 7, last_state = HIGH, data[5];
	int cnt = 0;

	/* Reset data holder*/
	data[0] = data[1] = data[2] = data[3] = data[4] = 0x00;

	/* Configure pin as output and set HIGH */
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(PIN, HIGH);
	bcm2835_delay(500);
	
	/* Set pin LOW for 18 ms to ensure that DHT detects signal*/
	bcm2835_gpio_write(PIN, LOW);
	bcm2835_delay(18);
	
	/* Configure pin as input and wait until DHT response */
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
	while(bcm2835_gpio_lev(PIN));
	
	/* Wait until DHT pulls up voltage */
	while(!bcm2835_gpio_lev(PIN));
	/* Wait until DHT prepares response */
	while(bcm2835_gpio_lev(PIN));
	/* Wait until DHT starts transmission */
	while(!bcm2835_gpio_lev(PIN));

	/* Loop over incoming bits from*/
	for (index = 0; index < 80; index++) {
		cnt = 0;

		/* Compares pin state between iterations, in first iteration state is HIGH*/
		while( bcm2835_gpio_lev(PIN) == last_state){
			cnt++;
			if (cnt == 1000) break;
		}

		last_state = bcm2835_gpio_lev(PIN);
		if (cnt == 1000) return TIMEOUT;

		/* if pin is high*/
		if ( index % 2 == 0) {

			/* if duration of HIGH state is long, bit is one */
			if (cnt > 200) data[byte] |= (1<< counter);
	
			/* Allocates incoming bits */
			if (counter == 0) { counter = 7; byte++; }
			else {counter--;}
		}
	}

	/* Checksum */
	if ( data[0]+data[1]+data[2]+data[3] == data[4] ) { 

		sprintf(temp_hum, "Temperature: %.2f, Humidity: %.2f", data[2]+(data[3]*0.01), data[0]+(data[1]*0.01));
		return SUCCESS;
	} 

	else { 

		return CHECKSUM;
		fprintf(stderr, "SENSOR CHECKSUM FAILED");
	}

}


