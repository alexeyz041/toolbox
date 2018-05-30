
#include <stdio.h>
#include <unistd.h>
#include "pi_2_dht_read.h"


int main( void )
{
float humidity = 0;
float temperature = 0;

    for(;;) {
	if(pi_2_dht_read(DHT11,4,&humidity,&temperature) == DHT_SUCCESS) {
	    printf("h = %f, t = %f\n",humidity,temperature);
	}
	usleep( 1000*1000 ); 
    }
    return 0;
}
