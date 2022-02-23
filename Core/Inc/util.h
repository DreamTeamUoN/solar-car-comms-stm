#ifndef INC_UTIL_H_
#define INC_UTIL_H_

typedef enum {
    OUT_USB,
	OUT_XBee,
	OUT_GPS
} OutputDevice;

void io_printf(OutputDevice out, const char *format, ...);

#endif /* INC_UTIL_H_ */
