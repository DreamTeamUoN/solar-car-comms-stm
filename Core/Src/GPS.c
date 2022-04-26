#include "GPS.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"

GPS_t GPS;

void GPS_Init(void)
{
  GPS.rxIndex = 0;
  HAL_UART_Receive_IT(&GPS_USART, &GPS.rxTmp, 1);
}

void GPS_CallBack(void)
{
  GPS.LastTime = HAL_GetTick();
  if (GPS.rxIndex < sizeof(GPS.rxBuffer) - 2)
  {
    GPS.rxBuffer[GPS.rxIndex] = GPS.rxTmp;
    GPS.rxIndex++;
  }
  HAL_UART_Receive_IT(&GPS_USART, &GPS.rxTmp, 1);
}

void GPS_Process(void)
{
  if ((HAL_GetTick() - GPS.LastTime > 50) && (GPS.rxIndex > 0))
  {
    char *str;
#if (_GPS_DEBUG==1)
		io_printf(OUT_USB, "GPS Debug\n");
		io_printf(OUT_USB, "%s\n",GPS.rxBuffer);
#endif
    // Store pointer to beginning of GNRMC
		str = strstr((char*) GPS.rxBuffer, "$GNRMC,");
		// IF GNRMC
    if (str != NULL)
    {
      *(strchr(str, '\r')) = '\0';
      strcpy(GPS.GNRMC, str);
    }
    memset(GPS.rxBuffer, 0, sizeof(GPS.rxBuffer));
    GPS.rxIndex = 0;
  }
  HAL_UART_Receive_IT(&GPS_USART, &GPS.rxTmp, 1);
}
