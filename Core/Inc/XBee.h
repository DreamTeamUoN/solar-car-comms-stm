#ifndef INC_XBEE_H_
#define INC_XBEE_H_

#define _XBEE_DEBUG 0

#include "main.h"

void XBee_Init(void);
void XBee_Callback(uint16_t Size);
void XBee_ErrorCallback(void);
void XBee_Process(void);

#endif /* INC_XBEE_H_ */
