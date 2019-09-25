#include "HardwareCAN.h"
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                                  ASTRA H DEFINITIONS                                //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
// Define the values of the MS CAN identifiers
#define MS_MEDIA_BUTTONS_ID 0x201
#define MS_BTN_BC 0x01
#define MS_BTN_1 0x31
#define MS_BTN_2 0x32
#define MS_BTN_3 0x33
#define MS_BTN_4 0x34
#define MS_BTN_5 0x35
#define MS_BTN_6 0x36
#define MS_BTN_7 0x37
#define MS_BTN_8 0x38
#define MS_BTN_9 0x39
#define MS_BTN_OK 0x6F
#define MS_BTN_RIHGT 0x6C
#define MS_BTN_LEFT 0x6D
#define MS_BTN_CD_RADIO 0xE0
#define MS_BTN_SETTINGS 0xFF
#define MS_WHEEL_BUTTONS_ID 0x206
#define MS_BTN_STATION 0x81
#define MS_BTN_MODE 0x82
#define MS_BTN_LEFT_WHEEL_TURN 0x83
#define MS_BTN_LEFT_WHEEL 0x84
#define MS_BTN_NEXT 0x91
#define MS_BTN_PREV 0x92
#define MS_BTN_VOL 0x93
#define MS_MEDIA_ID 0x6C1
#define MS_CLIMATE_INFO_ID 0x6C8
#define MS_ECC_ID 0x548
#define MS_BATTERY 0x07
#define MS_ENGINE_TEMP 0x10
#define MS_RPM_SPEED 0x10
#define MS_IGNITION_STATE_ID 0x450
#define MS_IGNITION_NO_KEY 0x00
#define MS_IGNITION_KEY_PRESENT 0x04
#define MS_IGNITION_1 0x05
#define MS_IGNITION_START 0x06
#define MS_SPEED_RPM_ID 0x4E8
#define MS_RANGE_ID 0x4EE
#define MS_BACKWARDS_BIT 0x04
#define MS_CLEAR_DISPLAY 0x0691
#define MS_WAKEUP_ID 0x697
#define MS_TIME_CLOCK_ID 0x180
#define MS_WINDOW_ID 0x2B0
#define MS_TEMP_OUT_DOOR_ID 0x683
// Define buttons state bytes
#define BTN_PRESSED 0x01
#define BTN_RELEASED 0x00
#define WHEEL_PRESSED 0x08
#define WHEEL_TURN_DOWN 0xFF
#define WHEEL_TURN_UP 0x01
// Limit time to flag a CAN error
#define CAN_TIMEOUT 100
#define CAN_DELAY 0 // 10        // ms between two processings of incoming messages
#define CAN_SEND_RATE 10 // 200   // ms between two successive sendings
#define PC13ON 0
#define PC13OFF 1
#define DELAY 250
//#define DEBUG  //Debug activation will increase the ant of memory by ~16k

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                         ASTRA H VARIABLES AND FUNCTIONS                             //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//****************************************Variables************************************//
char CTemp[2];
bool key_acc = 0;
bool test_mode = 0;
bool alarm = 0;
bool Blink = 0;
bool REVERSE = 0; //задний ход вкл/выкл
int VOLTAGE = 131;
int p_VOLTAGE = 0;
int T_ENG = 1000;
int p_T_ENG = 0;
int SPEED = 0;
//int p_SPEED = 0;
int RPM = 0;
//int p_RPM = 0;
int DAY = 0;
int MONTH = 0;
int YEAR = 0;
byte data2 = 0;
byte data4 = 0;
int RANGE = 0;
int p_RANGE = 0;
int window = 0;
int CNapr;
int CSpeed;
int CEco;
int COutT;
uint32_t btn = 0;
uint32_t time_request_ecc = 0;
uint32_t time_send = 0;
uint32_t time_send_album = 0;
uint32_t time_send_artist = 0;
uint32_t Time_USART = 0;
uint32_t Time_Update_Message = 0;
String Prev_Message;
String Message_USART;
String message = "OPEL";
String message_album = "";
String message_artist = "";
String p_message_album = "";
String p_message_artist = "";
String p_message = "";
String message_temp = "   ";
//********************************Tab function prototypes*****************************//
//Announcement of function prototypes from other tabs for correct function call.
void message_to_DIS (String);
void message_to_DIS_album (String);
void message_to_DIS_artist (String);
String Bold(String);
String Normal(String);
String Right(String);
String Central(String);
void CAN_message_process(CanMsg*);
String data_to_str(int, int);
String data_to_time(int);
//************************************************************************************//
//********************Filling an array with USART characters**************************
String Data_USART() {
  String Buffer_USART;
  char u;
  while (Serial2.available() > 0 && u != '\n') { //read serial buffer until \n
    char u = Serial2.read();
    if (u != 0xD) Buffer_USART += u;  // skip \r
  }
#ifdef DEBUG
  Serial2.println(Buffer_USART);
#endif
  Buffer_USART.remove(Buffer_USART.length() - 1);
  return Buffer_USART;
}
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
//*********************Generate alarm text******************************************
String Alarm(bool event) {
  if (event) return (data_to_str(T_ENG, 0));
  else       return ("   ");
}
//*************************************************************************************//
//Function prototypes and variables for working with CAN bus, function body in the "Init CAN" tab
#define CAN_RX_QUEUE_SIZE 36
HardwareCAN canBus(CAN1_BASE);
void CANSetup(void);
void SendCANmessage(long, byte, byte, byte, byte, byte, byte, byte, byte, byte);
void btn_function(byte, byte);
//*************************************************************************************//

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                                  SETUP FUNCTIONS                                    //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void setup() {
  Serial2.begin(115200); // USART2 on A2-A3 pins
  CANSetup() ;
  pinMode(PB12, OUTPUT_OPEN_DRAIN);
  pinMode(PB13, OUTPUT_OPEN_DRAIN);
  pinMode(PB14, OUTPUT_OPEN_DRAIN);
  pinMode(PB15, OUTPUT_OPEN_DRAIN);
  pinMode(PC13, OUTPUT); // LED
  pinMode(PC14, OUTPUT); // LED ERR
  digitalWrite(PC13, PC13ON);
  digitalWrite(PC13, PC13OFF);
  digitalWrite(PB12, HIGH);
  digitalWrite(PB13, HIGH);
  digitalWrite(PB14, HIGH);
  digitalWrite(PB15, HIGH);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//                                   LOOP FUNCTIONS                                    //
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
void loop() {
  if ((millis() - btn) > 150) {
    digitalWrite(PB12, HIGH);
    digitalWrite(PB13, HIGH);
    digitalWrite(PB14, HIGH);
    digitalWrite(PB15, HIGH);
    digitalWrite(PC13, HIGH);
  }
  //*******************************Request for data from the ECC************************
  if ((key_acc == 1) &&  ((millis() - time_request_ecc) > 1000)) {
    SendCANmessage(0x0248, 8, 0x06, 0xAA, 0x01, 0x01, 0x07, 0x10, 0x11, 0x00);
    time_request_ecc = millis();
  }
  //*******************Generate an engine temperature alarm*****************************
  if (T_ENG > 1080) {
    alarm = 1;
    message_temp = Alarm(Blink);
  }
  else alarm = 0;
  if (alarm == 0) {
    message_temp = data_to_str(T_ENG, 0);
  }
  //*************************Receiving a message with USART2****************************
  if ((key_acc == 1) && (millis() - Time_USART > 200)) {
    Message_USART = Data_USART();
    Time_USART = millis();
  }
  if ((key_acc == 1) && (Message_USART != "")) {
    if ( Message_USART == "run_bc") {
      btn_function(MS_BTN_BC, 0x00);
    }
    if ( Message_USART == "run_settings") {
      btn_function(MS_BTN_SETTINGS, 0x00);
    }
    if ((Message_USART != "run_bc") && ( Message_USART != "run_settings")) {
      message = Central(Bold(Message_USART));
    }
    Time_Update_Message = millis();
  }
  //******************************* Parameter display **********************************
  if ((key_acc == 1) && ((millis() - Time_Update_Message) > 500)) {
    message_album = Central((message_temp) + "°C" + " " + data_to_str(VOLTAGE, 1) + "V" + " " + String(SPEED) + "km/h" + " " + String(RPM) + "rpm");
    if (test_mode == 1) {
      Message_USART = "TEST MODE";
    }
    else Message_USART = "OPEL MEDIA SYSTEM";
    Time_Update_Message = millis();
  }
  //***************Check CAN message buffer and process message*************************
  while (canBus.available() > 0)
  { CAN_message_process(canBus.recv());
    canBus.free();
  }
  //******************************* Update display string title **********************************
  if ((key_acc == 1) && ((millis() - time_send) > 500) || (p_message != message )) {
    message_to_DIS(message);
    p_message = message;
    time_send = millis();
  }
  //******************************* Update display string album **********************************
  if ((key_acc == 1) && ((millis() - time_send_album) > 500) || (p_message_album != message_album )) {
    message_to_DIS_album(message_album);
    p_message_album = message_album;
    time_send_album = millis();
    if (Blink) {
      Blink = 0;
    }
    else Blink = 1;
  }
  //******************************* Update display string artist **********************************
  if ((key_acc == 1) && ((millis() - time_send_artist) > 500) || (p_message_artist != message_artist )) {
    message_artist = Central(("OCTATOK ") + String(RANGE) + "km" + "    " + data_to_time(DAY) + "." + data_to_time(MONTH) + "." + "20" + data_to_time(YEAR));
    message_to_DIS_artist(message_artist);
    p_message_artist = message_artist;
    time_send_artist = millis();
  }
}
