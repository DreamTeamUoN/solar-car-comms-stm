#ifndef _GPS_H_
#define _GPS_H_

#include "main.h"

#define GPS_USART    huart3
#define GPS_DEBUG    0

typedef struct
{
  uint8_t rxBuffer[512];
  uint16_t rxIndex;
  uint8_t rxTmp;
  uint32_t LastTime;

  char GNRMC[100];

} GPS_t;

extern GPS_t GPS;

void GPS_Init(void);
void GPS_Callback(void);
void GPS_Process(void);

#endif
