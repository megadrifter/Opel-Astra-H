//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                             CAN-BUS VARIABLES AND FUNCTIONS                         //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//********************************************************************************************//
//FOR CORRECT WORK, IT IS REQUIRED TO INCLUDE CAN_MCR_TXFP In the HardwareCAN.cpp FILE line 26//
//********************************************************************************************//
void CANSetup(void)
{
  CAN_STATUS Stat ;
  // Initialize CAN module
  canBus.map(CAN_GPIO_PB8_PB9);
  Stat = canBus.begin(CAN_SPEED_95, CAN_MODE_NORMAL);
  //canBus.filter(0, CAN_FIFO0, 0, 0); //без фильтра
  canBus.filter(0, CAN_FIFO0, MS_WINDOW_ID, 0xFFFFFFFF);
  canBus.filter(1, CAN_FIFO1, MS_WHEEL_BUTTONS_ID, 0xFFFFFFFF);
  canBus.filter(2, CAN_FIFO0, MS_ECC_ID, 0xFFFFFFFF);
  canBus.filter(3, CAN_FIFO1, MS_MEDIA_ID, 0xFFFFFFFF);
  canBus.filter(4, CAN_FIFO0, MS_IGNITION_STATE_ID, 0xFFFFFFFF);
  canBus.filter(5, CAN_FIFO1, MS_TIME_CLOCK_ID, 0xFFFFFFFF);
  canBus.filter(6, CAN_FIFO0, MS_SPEED_RPM_ID, 0xFFFFFFFF);
  canBus.filter(7, CAN_FIFO1, MS_RANGE_ID, 0xFFFFFFFF);
  canBus.filter(8, CAN_FIFO0, MS_TEMP_OUT_DOOR_ID, 0xFFFFFFFF);
  canBus.filter(9, CAN_FIFO1, MS_CLIMATE_INFO_ID, 0xFFFFFFFF);
  canBus.set_irq_mode();
  nvic_irq_set_priority(NVIC_CAN_RX1, 0);
  nvic_irq_set_priority(NVIC_USB_LP_CAN_RX0, 0);
  nvic_irq_set_priority(NVIC_USART2, 1);
  Stat = canBus.status();
#ifdef DEBUG
  if (Stat != CAN_OK) Serial2.print("Initialization failed");
#endif
}

void SendCANmessage(long id = 0x100, byte dlength = 8, byte d0 = 0x00, byte d1 = 0x00, byte d2 = 0x00, byte d3 = 0x00, byte d4 = 0x00, byte d5 = 0x00, byte d6 = 0x00, byte d7 = 0x00)
{ CanMsg msg ;
  msg.IDE = CAN_ID_STD;
  msg.RTR = CAN_RTR_DATA;
  msg.ID = id ;
  msg.DLC = dlength;
  msg.Data[0] = d0 ;
  msg.Data[1] = d1 ;
  msg.Data[2] = d2 ;
  msg.Data[3] = d3 ;
  msg.Data[4] = d4 ;
  msg.Data[5] = d5 ;
  msg.Data[6] = d6 ;
  msg.Data[7] = d7 ;
  uint32_t time = millis();
  CAN_TX_MBX MBX;
  do    {
    MBX = canBus.send(&msg);
  }
  while ((MBX == CAN_TX_NO_MBX) && ((millis() - time) < 200)) ;
  if ((millis() - time) >= 200) {
    canBus.cancel(CAN_TX_MBX0);
    canBus.cancel(CAN_TX_MBX1);
    canBus.cancel(CAN_TX_MBX2);
#ifdef DEBUG
    Serial2.print("CAN-Bus send error");
#endif
  }
}
void btn_function(byte data4, byte data2) {
  SendCANmessage(0x201, 3, 0x01, data4, data2);
  SendCANmessage(0x201, 3, 0x00, data4, data2);
}
