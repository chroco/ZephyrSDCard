#include "zsdcard.h"

int main(void)
{
	SDCard sdcard = SDCard();

	sdcard.testWrite();

//*
	while (1) 
	{

		k_sleep(K_MSEC(1000));
	}
//*/

	return 0;
}

