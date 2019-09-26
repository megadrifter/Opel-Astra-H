//********************Filling an array with USART characters**************************
String Data_USART() {
  String Buffer_USART;
  char u;
  while (Serial2.available() > 0 && u != '\n') { //read serial buffer until \n
    char u = Serial2.read();
    if (u != 0xD) Buffer_USART += u;  // skip \r
  }
  Buffer_USART.remove(Buffer_USART.length() - 1);
  return Buffer_USART;
}
//***************************************************************************************
void SendClimateData() {
  char buf[8];
  Serial2.print("<C:");
  buf[0] = (CTemp[0]);
  buf[1] = (CTemp[1]);
  buf[2] = (CNapr);
  buf[3] = (CSpeed);
  buf[4] = (CEco);
  if (COutT < 0) {
    buf[5] = (COutT / -100 + '0');
    buf[6] = ((COutT % -100) / 10 + '0');
    buf[7] = (COutT % -10 + '0');
  }
  else
  {
    buf[5] = (COutT / 100 + '0');
    buf[6] = ((COutT % 100) / 10 + '0');
    buf[7] = (COutT % 10 + '0');
  }
  Serial2.print(buf);
  Serial2.print(">\r");
}
