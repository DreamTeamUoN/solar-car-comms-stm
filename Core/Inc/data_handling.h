/*
 * data_handling.h
 *
 *  Created on: 28 Apr 2022
 *      Author: oabou
 */

#ifndef INC_DATA_HANDLING_H_
#define INC_DATA_HANDLING_H_

#define JSON_BUFFER_SIZE 200

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  int32_t speed;
  float SoC;
  char GNRMC[100];

  bool speed_Valid;
  bool SoC_Valid;
  bool GNMRC_Valid;
} DataOut_t;

typedef struct
{
  int32_t targetSpeed;

  bool targetSpeed_Valid;
} DataIn_t;

extern DataOut_t DataOut;
extern DataIn_t DataIn;


void Data_Handling_Init(void);
void Data_Handling_Process(void);

void updateSpeed(int32_t speed);
void updateSoC(float SoC);
void updateGNRMC(char *gnrmc);

void updateTargetSpeed(int32_t targetSpeed);

#endif /* INC_DATA_HANDLING_H_ */
