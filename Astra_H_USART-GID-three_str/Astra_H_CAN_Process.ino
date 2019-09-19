//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                                  PROCESSING CAN-MESSAGES                            //
//                All processing and decoding of CAN messages is here.                 //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void CAN_message_process(CanMsg *can_msg) {
  switch (can_msg->ID)  {
    case MS_WHEEL_BUTTONS_ID: {
        switch (can_msg->Data[1]) {
          case MS_BTN_VOL: {
              if (can_msg->Data[0] == WHEEL_PRESSED) {
                if (can_msg->Data[2] == WHEEL_TURN_UP) {
                  digitalWrite(PB12, LOW); btn = millis();
                  Message_USART = "VOLUME UP";
                }
              }
              if (can_msg->Data[2] == WHEEL_TURN_DOWN) {
                digitalWrite(PB13, LOW); btn = millis();
                Message_USART = "VOLUME DOWN";
              }
              break;
            }
          case MS_BTN_NEXT: {
              if ((can_msg->Data[0] == BTN_PRESSED) && (can_msg->Data[2] == BTN_RELEASED)) {
                digitalWrite(PB14, LOW); btn = millis();
                Message_USART = "NEXT TRACK";
              }
              break;
            }
          case MS_BTN_PREV: {
              if ((can_msg->Data[0] == BTN_PRESSED) && (can_msg->Data[2] == BTN_RELEASED)) {
                digitalWrite(PB15, LOW); btn = millis();
                Message_USART = "PREVIOUS TRACK";
              }
              break;
            }
          case MS_BTN_STATION: {
              if (can_msg->Data[0] == BTN_PRESSED) {
                if ((can_msg->Data[2]) == BTN_RELEASED) {
                  //
                  btn = millis();
                }
                if ((can_msg->Data[2]) == 0x03) {
                  btn_function(MS_BTN_BC, BTN_RELEASED);
                  btn = millis();
                }
              }
              break;
            }
          case MS_BTN_MODE: {
              if (can_msg->Data[0] == BTN_PRESSED) {
                if  ((can_msg->Data[2]) == BTN_RELEASED) {
                  btn_function(MS_BTN_SETTINGS, BTN_RELEASED);
                  btn = millis();
                }
                if (data2 != can_msg->Data[2])  {
                  data2 = can_msg->Data[2];
                  if (data2 == 0x3C) {
                    btn_function(MS_BTN_SETTINGS, BTN_RELEASED);
                    btn_function(MS_BTN_SETTINGS, data2);
                    btn = millis();
                    if (test_mode == 0)  {
                      test_mode = 1;
                      Message_USART = "TEST MODE ON";
                    }
                    else if (test_mode == 1 )  {
                      test_mode = 0;
                      Message_USART = "TEST MODE OFF";
                    }
                  }
                }
              }
              break;
            }
        }
        break;
      }
    case MS_ECC_ID: {
        if (can_msg->Data[0] == MS_BATTERY) {
          VOLTAGE = (can_msg->Data[2]);
        }
        if (can_msg->Data[0] == MS_ENGINE_TEMP) {
          T_ENG = (((can_msg->Data[3]) * 256) + (can_msg->Data[4]));
        }
        break;
      }
    case MS_SPEED_RPM_ID: {
        if (can_msg->Data[0] == 0x46) {
          SPEED = ((((can_msg->Data[4]) << 8) + (can_msg->Data[5])) >> 7);
          RPM = ((((can_msg->Data[2]) << 8) + (can_msg->Data[3])) >> 2);
          //          if ((can_msg->Data[6]) == 0x04) { //задний ход вкл/выкл
          //            REVERSE = 1;
          //          }
        }
        break;
      }
    case MS_TIME_CLOCK: {
        if (can_msg->Data[0] == 0x46) {
          DAY = ((can_msg->Data[4]) >> 3);
          MONTH = (can_msg->Data[3]);
          YEAR = (can_msg->Data[2]);
        }
        break;
      }
    case 0x4EE: {
        if (can_msg->Data[0] == 0x46) {
          RANGE = ((((can_msg->Data[2]) << 8) + (can_msg->Data[3])) / 2);
        }
        break;
      }
    case MS_IGNITION_STATE_ID: {
        if (can_msg->Data[2] ==  MS_IGNITION_NO_KEY) {
          key_acc = 0;
        }
        if ((can_msg->Data[2] ==  MS_IGNITION_KEY_PRESENT) || (can_msg->Data[2] == MS_IGNITION_START)) {
          key_acc = 1;
        }
        break;
      }
  }
#ifdef DEBUG
  char scan[40];
  sprintf (scan, "\n % d: % 04X # %02X %02X %02X %02X %02X %02X %02X %02X ", millis(),
           can_msg->ID, can_msg->Data[0], can_msg->Data[1], can_msg->Data[2], can_msg->Data[3],
           can_msg->Data[4], can_msg->Data[5], can_msg->Data[6], can_msg->Data[7]);
  Serial2.print(scan);
#endif
}
