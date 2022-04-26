#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "main.h"
#include "util.h"
#include "cJSON.h"

extern FDCAN_HandleTypeDef hfdcan1;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

HAL_StatusTypeDef io_printf(OutputDevice out, const char *format, ...)
{
  va_list args;

  /* Initalize arguments to store all values after format */
  va_start(args, format);

  // Allocate buffer size based on size of data
  char buffer[vsnprintf(NULL, 0, format, args) + 1]; //uint8_t
  vsnprintf(buffer, sizeof buffer, format, args);

  va_end(args);

  // Output to correct device
  switch (out)
  {
  case OUT_USB:
    return HAL_UART_Transmit(&huart2, (uint8_t*) buffer, strlen(buffer),
        HAL_MAX_DELAY);
  case OUT_XBee:
    return HAL_UART_Transmit(&huart1, (uint8_t*) buffer, strlen(buffer),
        HAL_MAX_DELAY);
  case OUT_GPS:
    break;
  case OUT_CAN:
    return CAN_Transmit((uint8_t*) buffer, strlen(buffer));
  }

  return HAL_ERROR;
}

HAL_StatusTypeDef CAN_Transmit(uint8_t *pTxData, size_t len)
{
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

  if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &pTxHeader, pTxData) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (HAL_FDCAN_IsTxBufferMessagePending(&hfdcan1, 0))
  {
    return HAL_BUSY;
  }

  return HAL_OK;
}

HAL_StatusTypeDef addNumberToJSON(cJSON *parentObject, const char *name,
    double value)
{
  cJSON *item = cJSON_CreateNumber(value);
  if (item == NULL)
  {
    return HAL_ERROR;
  }
  cJSON_AddItemToObject(parentObject, name, item);
  return HAL_OK;
}

HAL_StatusTypeDef addStringToJSON(cJSON *parentObject, const char *name,
    const char * string)
{
  cJSON *item = cJSON_CreateString(string);
  if (item == NULL)
  {
    return HAL_ERROR;
  }
  cJSON_AddItemToObject(parentObject, name, item);
  return HAL_OK;
}

HAL_StatusTypeDef encodeData(char *const buffer, size_t size, const int32_t speed,
    const int32_t state_of_charge, const char* gnrmc)
{
  HAL_StatusTypeDef status = HAL_ERROR;

  // Create JSON Object
  cJSON *dataObject = cJSON_CreateObject();
  if (dataObject == NULL)
  {
    goto end;
  }

  // Add speed
  if (addNumberToJSON(dataObject, "speed", speed))
  {
    goto end;
  }

  // Add test
  if (addNumberToJSON(dataObject, "SoC", state_of_charge))
  {
    goto end;
  }

  // Add GNMRC data
  if (addStringToJSON(dataObject, "GNRMC", gnrmc))
  {
    goto end;
  }

  // Print to string
  if (!cJSON_PrintPreallocated(dataObject, buffer, size, (cJSON_bool) 0))
  {
    fprintf(stderr, "Failed to print speed.\n");
    goto end;
  }

  status = HAL_OK;

  // Delete object
  end: cJSON_Delete(dataObject);
  return status;
}

int32_t decodeSpeed(const char *const json)
{
  int32_t speed = 0;
  cJSON *speedObject_json = cJSON_Parse(json);
  const cJSON *value = cJSON_GetObjectItemCaseSensitive(speedObject_json, "target");
  speed = value->valueint;

  cJSON_Delete(speedObject_json);
  return speed;
}
