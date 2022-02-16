#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "main.h"
#include "util.h"

void io_printf(OutputDevice out, const char *format, ...) {
	va_list arguments;

	/* Initalize arguments to store all values after format */
	va_start(args, format);

	// Allocate buffer size based on size of data
	char buffer[vsnprintf(NULL, 0, format, args) + 1]; //uint8_t
	vsnprintf(buffer, sizeof buffer, format, args);

	va_end(args);

	// Output to correct device
	switch (out) {
	case OUT_USB:
		HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
		break;
	case OUT_XBee:
		HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
		break;
	case OUT_GPS:
		break;
	}
}
