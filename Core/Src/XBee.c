#define USART1RxDMABuf_SIZE 100

#include "XBee.h"
#include "usart.h"
#include "string.h"

uint8_t uart1RxDMABuf[USART1RxDMABuf_SIZE];
uint8_t uart1MainBuf[USART1RxDMABuf_SIZE];

void XBee_Init(void)
{
  // Begin receiving UART through DMA
  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1RxDMABuf, USART1RxDMABuf_SIZE);
  // Disable Half Transmission interrupt, not used
  __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}

void XBee_Callback(uint16_t Size)
{
  memcpy(uart1MainBuf, uart1RxDMABuf, Size); // Use this to first copy data to buffer (Safer, to prevent new data from overwriting with DMA)

  int32_t targetSpeed, gpsSpeed;

  if (decodeInputData((char*) uart1MainBuf, &targetSpeed, &gpsSpeed) == HAL_OK)
  {
    updateTargetSpeed(targetSpeed);
    updateGPSSpeed(gpsSpeed);
  }

  io_printf(OUT_USB, "Received with Idle! %s\r\n", uart1MainBuf);

  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1RxDMABuf, USART1RxDMABuf_SIZE);
  __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}

void XBee_ErrorCallback(void)
{
  // TODO Handle HAL_UART_ERROR_FE Frame Error
  io_printf(OUT_USB, "Error %d on UART1\r\n", huart1.ErrorCode);
  __HAL_UART_CLEAR_FLAG(&huart1, UART_CLEAR_OREF);

  HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1RxDMABuf, USART1RxDMABuf_SIZE);
  __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}

void XBee_Process(void)
{

}
