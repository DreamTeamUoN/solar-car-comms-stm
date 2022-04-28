/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    fdcan.c
  * @brief   This file provides code for the configuration
  *          of the FDCAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "fdcan.h"

/* USER CODE BEGIN 0 */
#include "util.h"
#include "can_messages.h"
/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;

/* FDCAN1 init function */
void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV4;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = ENABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 25;
  hfdcan1.Init.NominalSyncJumpWidth = 1;
  hfdcan1.Init.NominalTimeSeg1 = 13;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 25;
  hfdcan1.Init.DataSyncJumpWidth = 1;
  hfdcan1.Init.DataTimeSeg1 = 13;
  hfdcan1.Init.DataTimeSeg2 = 2;
  hfdcan1.Init.StdFiltersNbr = 3;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  FDCAN_FilterTypeDef sFilterConfig;

  /* Configure Rx filter */
  sFilterConfig.IdType = FDCAN_STANDARD_ID; // Using standard CAN IDs
  sFilterConfig.FilterIndex = 0;  // Initialise filter 0
  sFilterConfig.FilterType = FDCAN_FILTER_MASK; // Classic filter: FilterID1 = filter, FilterID2 = mask
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // Store in Rx FIFO 0 if filter matches
  sFilterConfig.FilterID1 = 0x244;  // ID Filter
  sFilterConfig.FilterID2 = 0x0000; // Mask

  if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /* Configure global filter:
   Filter all remote frames with STD and EXT ID
   Accept non matching frames with STD ID and EXT ID */
  if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_ACCEPT_IN_RX_FIFO0,
      FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Start the FDCAN module */
  if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable callback on new message in RX FIFO0 */
  if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0)
      != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END FDCAN1_Init 2 */

}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspInit 0 */

  /* USER CODE END FDCAN1_MspInit 0 */
  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PCLK1;

    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* FDCAN1 clock enable */
    __HAL_RCC_FDCAN_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PA11 [PA9]     ------> FDCAN1_RX
    PA12 [PA10]     ------> FDCAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_FDCAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* FDCAN1 interrupt Init */
    HAL_NVIC_SetPriority(TIM16_FDCAN_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM16_FDCAN_IT0_IRQn);
  /* USER CODE BEGIN FDCAN1_MspInit 1 */

  /* USER CODE END FDCAN1_MspInit 1 */
  }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

  /* USER CODE END FDCAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FDCAN_CLK_DISABLE();

    /**FDCAN1 GPIO Configuration
    PA11 [PA9]     ------> FDCAN1_RX
    PA12 [PA10]     ------> FDCAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM16_FDCAN_IT0_IRQn);
  /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

  /* USER CODE END FDCAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
  * @brief  Rx FIFO 0 callback.
  * @param  hfdcan: pointer to an FDCAN_HandleTypeDef structure that contains
  *         the configuration information for the specified FDCAN.
  * @param  RxFifo0ITs: indicates which Rx FIFO 0 interrupts are signalled.
  *         This parameter can be any combination of @arg FDCAN_Rx_Fifo0_Interrupts.
  * @retval None
  */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
  FDCAN_RxHeaderTypeDef pRxHeader;
  uint8_t receivedData[100];

  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
  {
    /* Retrieve Rx messages from RX FIFO0 */
    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &pRxHeader, receivedData)
        != HAL_OK)
    {
      Error_Handler();
    }
    io_printf(OUT_USB, "New Message | ID:0x%X | ", pRxHeader.Identifier);

    if (pRxHeader.Identifier == 0x118)
    {
      ECU_0x118 ecu_0x118;
      ecu_0x118.raw[0] = receivedData[0];
      ecu_0x118.raw[1] = receivedData[1];
      io_printf(OUT_USB,
          ecu_0x118.data.left_indicator_switch ?
              "Left indicator on\n" : "Left indicator off\r\n");

    }
    else if (pRxHeader.Identifier == 0x6F4)
    {
      BMS_SOC_0x6F4 bms_soc;
      bms_soc.raw[0] = ((float*) receivedData)[0];
      bms_soc.raw[1] = ((float*) receivedData)[1];
      io_printf(OUT_USB, "Battery percentage: %f \r\n", bms_soc.data.battery_soc);
    }
    else if (pRxHeader.Identifier == 0x423)
    {
      INVERTER_VELOCITY_0x423_0x443 left_inverter_velocity;
      left_inverter_velocity.raw[0] = ((int32_t*) receivedData)[0];
      left_inverter_velocity.raw[1] = ((int32_t*) receivedData)[1];
      io_printf(OUT_USB, "Vehicle Velocity Left: %d \r\n",
          left_inverter_velocity.data.vehicle_velocity);
    }
    else if (pRxHeader.Identifier == 0x443)
    {
      INVERTER_VELOCITY_0x423_0x443 right_inverter_velocity;
      right_inverter_velocity.raw[0] = ((int32_t*) receivedData)[0];
      right_inverter_velocity.raw[1] = ((int32_t*) receivedData)[1];
      io_printf(OUT_USB, "Vehicle Velocity Right: %d \r\n",
          right_inverter_velocity.data.vehicle_velocity);
    }
    else if (pRxHeader.Identifier == 0x244)
    {
      // Ensure string is terminated
      receivedData[pRxHeader.DataLength >> 16] = '\0';
      io_printf(OUT_USB, "%s\r\n", receivedData);
    }
    else
    {
      io_printf(OUT_USB, "Unknown message\r\n");
    }
  }
}

/* USER CODE END 1 */
