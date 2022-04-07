#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "main.h"
#include "util.h"
#include "cJSON.h"

extern FDCAN_HandleTypeDef hfdcan1;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void io_printf(OutputDevice out, const char *format, ...) {
	va_list args;

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
	case OUT_CAN:
		CAN_Transmit((uint8_t *)buffer, strlen(buffer));
		break;
	}
}

void CAN_Transmit(uint8_t * pTxData, size_t len) {
  io_printf(OUT_USB, "Transmitting CAN\r\n");
  FDCAN_TxHeaderTypeDef pTxHeader; //declare a specific header for message transmissions

  pTxHeader.IdType = FDCAN_STANDARD_ID; //set identifier to standard
  pTxHeader.Identifier = 0x256; //define a standard identifier, used for message identification by filters (switch this for the other microcontroller)
  pTxHeader.TxFrameType = FDCAN_DATA_FRAME; //set data type to data, rather than remote transmission request
  pTxHeader.DataLength = len << 16; // TODO
  pTxHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
  pTxHeader.BitRateSwitch = FDCAN_BRS_OFF;
  pTxHeader.FDFormat = FDCAN_CLASSIC_CAN;
  pTxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
  pTxHeader.MessageMarker = 0;

  if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &pTxHeader, pTxData) != HAL_OK) {
    Error_Handler();
  }

  while (HAL_FDCAN_IsTxBufferMessagePending(&hfdcan1, 0));
  io_printf(OUT_USB, "Successfully transmitted CAN\r\n");
}

void encodeSpeed (const int32_t speed, char * const buffer, int size){
  cJSON *speedObject = cJSON_CreateObject();
  if (speedObject == NULL)
  {
    goto end;
  }

  cJSON *value = cJSON_CreateNumber(speed);
  if (speedObject == NULL)
  {
    goto end;
  }

  cJSON_AddItemToObject(speedObject, "speed", value);

  if (!cJSON_PrintPreallocated(speedObject, buffer, size, (cJSON_bool)0))
  {
    fprintf(stderr, "Failed to print speed.\n");
  }

end:
  cJSON_Delete(speedObject);
}

int32_t decodeSpeed(const char * const json) {
  int32_t speed = 0;
  cJSON *speedObject_json = cJSON_Parse(json);
  const cJSON *value = cJSON_GetObjectItemCaseSensitive(speedObject_json, "target");
  speed = value->valueint;

  cJSON_Delete(speedObject_json);
  return speed;
}
