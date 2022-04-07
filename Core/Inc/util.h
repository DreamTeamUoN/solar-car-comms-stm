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
void encodeSpeed (const int32_t speed, char * const buffer, int size);

#endif /* INC_UTIL_H_ */
