#ifndef INC_UTIL_H_
#define INC_UTIL_H_

#include "main.h"
#include "data_handling.h"

typedef enum
{
  OUT_USB,
  OUT_XBee,
  OUT_GPS,
  OUT_CAN
} OutputDevice;

HAL_StatusTypeDef io_printf(OutputDevice out, const char *format, ...);
HAL_StatusTypeDef CAN_Transmit(uint8_t *buffer, size_t len);
HAL_StatusTypeDef CAN_Transmit_ID(uint8_t * buffer, size_t len, uint32_t StdId);
HAL_StatusTypeDef encodeData(char *const buffer, size_t size, DataOut_t *data);
HAL_StatusTypeDef decodeInputData(const char *const json, int32_t *target_speed, int32_t *gps_speed);

#endif /* INC_UTIL_H_ */
