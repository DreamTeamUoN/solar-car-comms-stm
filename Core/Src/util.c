#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "main.h"
#include "usart.h"
#include "fdcan.h"
#include "cJSON.h"

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
  return CAN_Transmit_ID(pTxData, len, 0x244);
}

HAL_StatusTypeDef CAN_Transmit_ID(uint8_t *pTxData, size_t len, uint32_t StdId)
{
  FDCAN_TxHeaderTypeDef pTxHeader; //declare a specific header for message transmissions

  pTxHeader.IdType = FDCAN_STANDARD_ID; //set identifier to standard
  pTxHeader.Identifier = StdId;
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
    const char *string)
{
  cJSON *item = cJSON_CreateString(string);
  if (item == NULL)
  {
    return HAL_ERROR;
  }
  cJSON_AddItemToObject(parentObject, name, item);
  return HAL_OK;
}

HAL_StatusTypeDef encodeData(char *const buffer, size_t size, DataOut_t *data)
{
  HAL_StatusTypeDef status = HAL_ERROR;

// Create JSON Object
  cJSON *dataObject = cJSON_CreateObject();
  if (dataObject == NULL)
  {
    goto end;
  }

  if (data->speed_Valid)
  {
    // Add speed
    if (addNumberToJSON(dataObject, "speed", data->speed))
    {
      goto end;
    }
  }

  if (data->SoC_Valid)
  {
    // Add test
    if (addNumberToJSON(dataObject, "SoC", data->SoC))
    {
      goto end;
    }
  }

  if (data->GNMRC_Valid)
  {
    // Add GNMRC data
    if (addStringToJSON(dataObject, "GNRMC", data->GNRMC))
    {
      goto end;
    }
  }

  if (dataObject->child != NULL)
  {
    // Print to string
    if (!cJSON_PrintPreallocated(dataObject, buffer, size, (cJSON_bool) 0))
    {
      fprintf(stderr, "Failed to print speed.\r\n");
      goto end;
    }

    status = HAL_OK;
  }

// Delete object
  end: cJSON_Delete(dataObject);
  return status;
}

int32_t decodeTargetSpeed(const char *const json)
{
  int32_t target_speed = 0;
  cJSON *speedObject_json = cJSON_Parse(json);
  const cJSON *value = cJSON_GetObjectItemCaseSensitive(speedObject_json,
      "target_speed");
  target_speed = value->valueint;

  cJSON_Delete(speedObject_json);
  return target_speed;
}
