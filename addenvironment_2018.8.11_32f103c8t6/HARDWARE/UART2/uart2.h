#ifndef __UART2_H
#define __UART2_H 		
#include "sys.h"  
void uart2init(uint32_t bound);
typedef struct
{
    int Lux;
    int Temp;
		float P;
    int Hum;
    int Alt;
} Bme;
extern Bme bme;

#endif




