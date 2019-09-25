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
                if (( window != 2) && ((can_msg->Data[2]) == BTN_RELEASED)) {
                  //
                  btn = millis();
                }
                if (( window == 2) && ((can_msg->Data[2]) == BTN_RELEASED)) {
                  btn_function(MS_BTN_BC, 0x00);
                  btn = millis();
                }
              }
              break;
            }
          case MS_BTN_MODE: {
              if (can_msg->Data[0] == BTN_PRESSED) {
                if  ((can_msg->Data[2]) == 0x00) {
                  btn_function(MS_BTN_SETTINGS, 0x00);
                  btn = millis();
                }
                if (data2 != can_msg->Data[2])  {
                  data2 = can_msg->Data[2];
                  if (data2 == 0x3C) {
                    btn_function(MS_BTN_SETTINGS, 0x00);
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
          if (VOLTAGE != p_VOLTAGE) {
            p_VOLTAGE = VOLTAGE;
            Serial2.println("<voltage:" + data_to_str(p_VOLTAGE, 1) + ">");
          }
        }
        if (can_msg->Data[0] == MS_ENGINE_TEMP) {
          T_ENG = (((can_msg->Data[3]) * 256) + (can_msg->Data[4]));
          if (T_ENG != p_T_ENG) {
            p_T_ENG = T_ENG;
            Serial2.println("<t_eng:" + data_to_str(p_T_ENG, 0) + ">");
          }
        }
        break;
      }
    case MS_SPEED_RPM_ID: {
        if (can_msg->Data[0] == 0x46) {
          SPEED = ((((can_msg->Data[4]) << 8) + (can_msg->Data[5])) >> 7);
          RPM = ((((can_msg->Data[2]) << 8) + (can_msg->Data[3])) >> 2);
          //          if (SPEED != p_SPEED) {
          //            p_SPEED = SPEED;
          //            Serial2.println("<speed:" + String(p_SPEED) + ">");
          //          }
          //          if (RPM != p_RPM) {
          //            p_RPM = RPM;
          //            Serial2.println("<rpm:" + String(p_RPM) + ">");
          //          }
          //          if ((can_msg->Data[6]) != 0x04) { //задний ход выкл
          //            REVERSE = 0;
          //          }
          //          if ((can_msg->Data[6]) == 0x04) { //задний ход вкл
          //            REVERSE = 1;
          //          }
        }
        break;
      }
    case MS_TIME_CLOCK_ID: {
        if (can_msg->Data[0] == 0x46) {
          DAY = ((can_msg->Data[4]) >> 3);
          MONTH = (can_msg->Data[3]);
          YEAR = (can_msg->Data[2]);
        }
        break;
      }
    case MS_RANGE_ID: {
        if (can_msg->Data[0] == 0x46) {
          RANGE = ((((can_msg->Data[2]) << 8) + (can_msg->Data[3])) / 2);
          if (RANGE != p_RANGE) {
            p_RANGE = RANGE;
            Serial2.println("<range:" + String(p_RANGE) + ">");
          }
        }
        break;
      }
    case MS_CLIMATE_INFO_ID: {//***********************ClimateControl_Info
        //*************************REGULAR SEND MESSAGES IN CAN BUS
        if (can_msg->Data[0] == 0x21 && can_msg->Data[1] == 0x00 && can_msg->Data[6] == 0xB0 && can_msg->Data[7] == 0x24) {
          CTemp[0] = can_msg->Data[2];
          CTemp[1] = can_msg->Data[4];
        }
        if (can_msg->Data[0] == 0x22 && can_msg->Data[1] == 0x01 && can_msg->Data[2] == 0xE0) {
          CNapr = can_msg->Data[3] - 0x21;
        }
        if (can_msg->Data[0] == 0x22 && can_msg->Data[4] == 0x25) {
          if (can_msg->Data[5] == 0x01) {
            CEco = 0x30;
          }
          if (can_msg->Data[5] == 0x03) {
            CEco = 0x31;
          }
        }
        if (CEco == 0x30) {//****************************Only in none-Eco
          if (can_msg->Data[0] == 0x23 && can_msg->Data[1] == 0x26) {
            CSpeed = can_msg->Data [6];
            SendClimateData();
            //            Serial2.println("<CSpeed:" + String(CSpeed) + ">");
          }
        }
        if (CEco == 0x31) { //Only in Eco
          if (can_msg->Data[0] == 0x24 && can_msg->Data[1] == 0x50) {//*************************only in eco mode
            CSpeed = can_msg->Data[3];
            SendClimateData();
            //            Serial2.println("<CSpeed:" + String(CSpeed) + ">");
          }
        }
        //SENDS ONLY AFTER CHANGE
        if (can_msg->Data[0] == 0x21 && can_msg->Data[1] == 0x00 && can_msg->Data[6] == 0xB0 && can_msg->Data[7] == 0xA3) {//*********************After change temperature
          CTemp[0] = can_msg->Data[2];
          CTemp[1] = can_msg->Data[4];
          SendClimateData();
          //          Serial2.println("<CTemp:" + String(CTemp) + ">");
        }
        if (can_msg->Data[0] == 0x21 && can_msg->Data[1] == 0xE0 && can_msg->Data[3] == 0xA4) {//****************************After change air-flow
          CNapr = can_msg->Data[2] - 0x21;
          SendClimateData();
          //          Serial2.println("<CNapr:" + String(CNapr) + ">");
        }
        if (can_msg->Data[0] == 0x10 && can_msg->Data[6] == 0x25) {//******************After change mode Eco/none-Eco
          if (can_msg->Data[7] == 0x01) {
            CEco = 0x30;
          }
          if (can_msg->Data[7] == 0x03) {
            CEco = 0x31;
          }
          SendClimateData();
          //          Serial2.println("<CEco:" + String(CEco) + ">");
        }
        if (can_msg->Data[0] == 0x22 && can_msg->Data[1] == 0x50) {//**********************after change speed fan
          CSpeed = can_msg->Data[3];
          SendClimateData();
          //          Serial2.println("<CSpeed:" + String(CSpeed) + ">");
        }
        if ( can_msg->Data[0] == 0x25 && can_msg->Data[1] == 0xA5 && can_msg->Data[2] == 0x02  && can_msg->Data[4] == 0x50 && can_msg->Data[5] == 0x00 && can_msg->Data[6] == 0x41 && can_msg->Data[7] == 0x59) {//***********************Click Auto-Mode
          if (can_msg->Data[3] == 0xE0) {
            CNapr = 0x38;
            CSpeed = 0x41;
            SendClimateData();
            //            Serial2.println("<CNapr:" + String(CNapr) + ">");
            //            Serial2.println("<CSpeed:" + String(CSpeed) + ">");
          }
        }
        break;
      }
    case MS_IGNITION_STATE_ID: {
        if ((can_msg->Data[2] ==  MS_IGNITION_NO_KEY) || (can_msg->Data[2] ==  MS_IGNITION_1)) {
          key_acc = 0;
        }
        if ((can_msg->Data[2] ==  MS_IGNITION_KEY_PRESENT) || (can_msg->Data[2] == MS_IGNITION_START)) {
          key_acc = 1;
        }
        break;
      }
    case MS_TEMP_OUT_DOOR_ID: {
        if ((can_msg->Data[0] == 0x46) && (COutT != (can_msg->Data[2] / 2) - 40)) {
          COutT = (can_msg->Data[2] - 80) * 5 / 10;
          //COutT = (can_msg->Data[2] / 2) - 40;
          SendClimateData();
          //          Serial2.println("<cout:" + String(COutT) + ">");
        }
        break;
      }
    case MS_WINDOW_ID: {
        if (can_msg->Data[1] == 0x0C && can_msg->Data[2] == 0x00 && can_msg->Data[3] == 0xC8) { //main
          window =  1;
        }
        if (can_msg->Data[1] == 0x04 && can_msg->Data[2] == 0x08 && can_msg->Data[3] == 0xC8) { //bc
          window =  2;
        }
        if (((can_msg->Data[1] == 0x0C) || (can_msg->Data[1] == 0x0D)) && can_msg->Data[2] == 0x08 && can_msg->Data[3] == 0xC8) { //settings
          window =  3;
        }
        if (can_msg->Data[1] == 0x0A && can_msg->Data[2] == 0x00 && can_msg->Data[3] == 0xC8) { //climate
          window =  4;
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
