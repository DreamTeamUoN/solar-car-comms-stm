/*
 * can_messages.h
 *
 *  Created on: 25 Apr 2022
 *      Author: oabou
 */

#ifndef INC_CAN_MESSAGES_H_
#define INC_CAN_MESSAGES_H_

typedef union
{
  int32_t raw[2];
  struct
  {
    int32_t motor_angular_speed;
    int32_t vehicle_velocity;
  } data;
} INVERTER_VELOCITY_0x423_0x443;

typedef union
{
  float raw[2];
  struct
  {
    float battery_soc;
    float balance_soc;
  } data;
} BMS_SOC_0x6F4;

// For testing
typedef union
{
  uint8_t raw[2];
  struct
  {
    uint8_t left_indicator_switch :1;
    uint8_t right_indicator_switch :1;
    uint8_t front_and_rear_lights_switch :1;
    uint8_t brake_lights_switch :1;
    uint8_t reverse_lights_switch :1;
    uint8_t daylight_lights_switch :1;
    uint8_t camera_switch :1;
    uint8_t dash_switch :1;

    uint8_t ECU_switch :1;
    uint8_t MPPT_switch :1;
    uint8_t tritium_switch :1;
    uint8_t charger_switch :1;
    uint8_t unused :4;
  } data;
} ECU_0x118;

void CAN_Messages_Init(void);
void CAN_Messages_Callback(void);

#endif /* INC_CAN_MESSAGES_H_ */
