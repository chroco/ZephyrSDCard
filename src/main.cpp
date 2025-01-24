#include "zsdcard.h"

int main(void)
{
	SDCard sdcard = SDCard();

	sdcard.doSDCardThings();

//*
	while (1) 
	{

		k_sleep(K_MSEC(1000));
	}
//*/

	return 0;
}

