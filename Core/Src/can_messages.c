#include "main.h"
#include "fdcan.h"

#include "can_messages.h"

void CAN_Messages_Init(void)
{
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
}

void CAN_Messages_Callback(void)
{
  FDCAN_RxHeaderTypeDef pRxHeader;
  uint8_t receivedData[100];
  /* Retrieve Rx messages from RX FIFO0 */
  if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &pRxHeader, receivedData)
      != HAL_OK)
  {
    Error_Handler();
  }
#if(_CAN_DEBUG==1)
  io_printf(OUT_USB, "New Message | ID:0x%X | ", pRxHeader.Identifier);
#endif
  if (pRxHeader.Identifier == 0x118)
  {
    ECU_0x118 ecu_0x118;
    ecu_0x118.raw[0] = receivedData[0];
    ecu_0x118.raw[1] = receivedData[1];
    UNUSED(ecu_0x118);
#if(_CAN_DEBUG==1)
    io_printf(OUT_USB,
        ecu_0x118.data.left_indicator_switch ?
            "Left indicator on\n" : "Left indicator off\r\n");
#endif
  }
  else if (pRxHeader.Identifier == 0x6F4)
  {
    BMS_SOC_0x6F4 bms_soc;
    bms_soc.raw[0] = ((float*) receivedData)[0];
    bms_soc.raw[1] = ((float*) receivedData)[1];
    updateSoC(bms_soc.data.battery_soc);
#if(_CAN_DEBUG==1)
    io_printf(OUT_USB, "Battery percentage: %f \r\n", bms_soc.data.battery_soc);
#endif
  }
  else if (pRxHeader.Identifier == 0x423)
  {
    INVERTER_VELOCITY_0x423_0x443 left_inverter_velocity;
    left_inverter_velocity.raw[0] = ((int32_t*) receivedData)[0];
    left_inverter_velocity.raw[1] = ((int32_t*) receivedData)[1];
    updateSpeed(left_inverter_velocity.data.vehicle_velocity);
#if(_CAN_DEBUG==1)
    io_printf(OUT_USB, "Vehicle Velocity Left: %d \r\n",
        left_inverter_velocity.data.vehicle_velocity);
#endif
  }
  else if (pRxHeader.Identifier == 0x443)
  {
    INVERTER_VELOCITY_0x423_0x443 right_inverter_velocity;
    right_inverter_velocity.raw[0] = ((int32_t*) receivedData)[0];
    right_inverter_velocity.raw[1] = ((int32_t*) receivedData)[1];
    UNUSED(right_inverter_velocity);
#if(_CAN_DEBUG==1)
    io_printf(OUT_USB, "Vehicle Velocity Right: %d \r\n",
        right_inverter_velocity.data.vehicle_velocity);
#endif
  }
  else if (pRxHeader.Identifier == 0x244)
  {
    // Ensure string is terminated
    receivedData[pRxHeader.DataLength >> 16] = '\0';
#if(_CAN_DEBUG==1)
    io_printf(OUT_USB, "%s\r\n", receivedData);
#endif
  }
  else
  {
#if(_CAN_DEBUG==1)
    io_printf(OUT_USB, "Unknown message\r\n");
#endif
  }
}
