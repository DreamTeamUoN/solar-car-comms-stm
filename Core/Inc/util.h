#ifndef INC_UTIL_H_
#define INC_UTIL_H_

typedef enum
{
  OUT_USB,
  OUT_XBee,
  OUT_GPS,
  OUT_CAN
} OutputDevice;

HAL_StatusTypeDef io_printf(OutputDevice out, const char *format, ...);
HAL_StatusTypeDef CAN_Transmit(uint8_t *buffer, size_t len);
HAL_StatusTypeDef encodeData(char *const buffer, size_t size, const int32_t speed,
    const int32_t state_of_charge, const char* gnmrc);
int32_t decodeSpeed(const char *const json);

#endif /* INC_UTIL_H_ */
