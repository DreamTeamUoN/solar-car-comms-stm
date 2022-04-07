#ifndef INC_UTIL_H_
#define INC_UTIL_H_

typedef enum {
    OUT_USB,
	OUT_XBee,
	OUT_GPS,
	OUT_CAN
} OutputDevice;

void io_printf(OutputDevice out, const char *format, ...);
void CAN_Transmit(uint8_t * buffer, size_t len);
void encodeSpeed(char *const buffer, size_t size, const int32_t speed,
    const int32_t state_of_charge, const double latitude,
    const double longitude);
int32_t decodeSpeed(const char * const json);

#endif /* INC_UTIL_H_ */
