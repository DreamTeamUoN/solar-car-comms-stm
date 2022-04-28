/*
 * data_handling.c
 *
 *  Created on: 28 Apr 2022
 *      Author: oabou
 */

#include "data_handling.h"
#include "util.h"
#include <string.h>

DataOut_t DataOut;
DataIn_t DataIn;

char jsonBuffer[JSON_BUFFER_SIZE];

void Data_Handling_Init(void)
{
  DataOut.speed_Valid = false;
  DataOut.SoC_Valid = false;
  DataOut.GNMRC_Valid = false;

  DataIn.targetSpeed_Valid = false;
}

void Data_Handling_Process(void)
{
  // TODO Make all transmission non-blocking
  // TODO Add parity to uart1

  if (DataOut.speed_Valid && DataOut.SoC_Valid && DataOut.GNMRC_Valid)
  {
    encodeData(jsonBuffer, JSON_BUFFER_SIZE, &DataOut);

    io_printf(OUT_XBee, "%s\r\n", jsonBuffer);
    io_printf(OUT_USB, "%s\r\n", jsonBuffer);
    DataOut.speed_Valid = false;
    DataOut.SoC_Valid = false;
    DataOut.GNMRC_Valid = false;
  }

  if (DataIn.targetSpeed_Valid)
  {
    io_printf(OUT_USB, "Target Speed: %d\r\n", DataIn.targetSpeed);

    // Transmit target speed on CAN
    if (CAN_Transmit_ID((uint8_t*) &DataIn.targetSpeed, sizeof(DataIn.targetSpeed),
        0x244) != HAL_OK)
    {
      io_printf(OUT_USB, "Failed to transmit CAN\r\n");
    }

    DataIn.targetSpeed_Valid = false;
  }

  if (DataIn.GPSSpeed_Valid)
  {
    io_printf(OUT_USB, "GPS Speed: %d\r\n", DataIn.GPSSpeed);

    // Transmit GPS speed on CAN
    // TODO Change ID, set to 0x423 for testing
    if (CAN_Transmit_ID((uint8_t*) &DataIn.GPSSpeed, sizeof(DataIn.GPSSpeed), 0x423)
        != HAL_OK)
    {
      io_printf(OUT_USB, "Failed to transmit CAN\r\n");
    }

    DataIn.GPSSpeed_Valid = false;
  }
}

void updateSpeed(int32_t speed)
{
  DataOut.speed = speed;
  DataOut.speed_Valid = true;
}

void updateSoC(float SoC)
{
  DataOut.SoC = SoC;
  DataOut.SoC_Valid = true;
}

void updateGNRMC(char *gnrmc)
{
  strcpy(DataOut.GNRMC, gnrmc);
  DataOut.GNMRC_Valid = true;
}

void updateTargetSpeed(int32_t targetSpeed)
{
  DataIn.targetSpeed = targetSpeed;
  DataIn.targetSpeed_Valid = true;
}

void updateGPSSpeed(int32_t GPSSpeed)
{
  DataIn.GPSSpeed = GPSSpeed;
  DataIn.GPSSpeed_Valid = true;
}
