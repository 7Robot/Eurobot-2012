/*
  ax12.h - arbotiX Library for AX-12 Servos
  Copyright (c) 2008,2009 Michael E. Ferguson.  All right reserved.
  Modificada el 15/11/09 por Pablo Gindel.
*/

#ifndef ax12_h
#define ax12_h

#define AX12_MAX_SERVOS          18
#define AX12_BUFFER_SIZE         32

/** EEPROM AREA **/
#define MODEL_NUMBER             0
#define VERSION                  2
#define ID                       3
#define BAUD_RATE                4
#define RETURN_DELAY_TIME        5
#define CW_ANGLE_LIMIT           6
#define CCW_ANGLE_LIMIT          8
#define LIMIT_TEMPERATURE        11
#define DOWN_LIMIT_VOLTAGE       12
#define UP_LIMIT_VOLTAGE         13
#define MAX_TORQUE               14
#define RETURN_LEVEL             16
#define ALARM_LED                17
#define ALARM_SHUTDOWN           18
#define DOWN_CALIBRATION         20
#define UP_CALIBRATION           22

/** RAM AREA **/
#define TORQUE_ENABLE            24
#define AX_LED                   25
#define CW_COMPLIANCE_MARGIN     26
#define CCW_COMPLIANCE_MARGIN    27
#define CW_COMPLIANCE_SLOPE      28
#define CCW_COMPLIANCE_SLOPE     29
#define GOAL_POSITION            30
#define MOVING_SPEED             32
#define TORQUE_LIMIT             34
#define PRESENT_POSITION         36
#define PRESENT_SPEED            38
#define PRESENT_LOAD             40
#define PRESENT_VOLTAGE          42
#define PRESENT_TEMPERATURE      43
#define REGISTERED_INSTRUCTION   44
#define MOVING                   46
#define LOCK                     47
#define PUNCH                    48

/** Status Return Levels **/
#define AX_RETURN_NONE              0
#define AX_RETURN_READ              1
#define AX_RETURN_ALL               2

/** Instruction Set **/
#define AX_PING                     1
#define AX_READ_DATA                2
#define AX_WRITE_DATA               3
#define AX_REG_WRITE                4
#define AX_ACTION                   5
#define AX_RESET                    6
#define AX_SYNC_WRITE               131

typedef unsigned char byte;
typedef unsigned char boolean;

class AX12
{
  public:

    AX12 (long baud, byte motor_id, boolean inv);
    AX12 (byte motor_id, boolean inv);
    AX12 (long baud, byte motor_id);
    AX12 (byte motor_id);
    AX12 ();

    byte id;
    boolean inverse;
    int status_id;                                   // ID del paquete de retorno
    int status_error;                                // error del paquete de retorno
    int status_data;                                 // data del paquete de retornostatic void setTX();

    static byte ax_rx_buffer[AX12_BUFFER_SIZE];      // buffer de recepción
    static volatile byte ax_rx_Pointer;              // making these volatile keeps the compiler from optimizing loops of available()

    static void init (long baud);
    static void autoDetect (int* list_motors, byte num_motors);

    byte ping ();
    byte reset ();
    byte readData (byte regstart, byte reglength);
    byte writeData (byte regstart, byte reglength, int value);
    byte action ();
    byte regWrite (byte regstart, byte reglength, int value);
    byte readInfo (byte regstart);
    byte writeInfo (byte regstart, int value);
    void setEndlessTurnMode (boolean onoff);
    void endlessTurn (int velocidad);
    byte presentPSL (int* PSL);

  private:

    static void setTX();
    static void setRX();
    static void setNone();
    static byte ax12writeB(byte data);
    static void ax12SendPacket (byte _id, byte datalength, byte instruction, byte* data);
    static byte ax12ReadPacket(int* status_id, int* status_error, int* status_data);

};

boolean sign2bin (int numero);
char bin2sign (boolean var);
int makeInt (byte l, byte h);


#endif









