/*
   Martin d'Allens, starting 2011-09-20.
 */


#ifndef _AX12_H
#define _AX12_H

typedef unsigned char byte;

// EEPROM
#define AX_MODEL_NUMBER             0
#define AX_VERSION                  2
#define AX_ID                       3
#define AX_BAUD_RATE                4
#define AX_RETURN_DELAY_TIME        5
#define AX_CW_ANGLE_LIMIT           6
#define AX_CCW_ANGLE_LIMIT          8
//#define AX_SYSTEM_DATA2             10
#define AX_LIMIT_TEMPERATURE        11
#define AX_DOWN_LIMIT_VOLTAGE       12
#define AX_UP_LIMIT_VOLTAGE         13
#define AX_MAX_TORQUE               14
#define AX_RETURN_LEVEL             16
#define AX_ALARM_LED                17
#define AX_ALARM_SHUTDOWN           18
//#define AX_OPERATING_MODE           19
#define AX_DOWN_CALIBRATION         20
#define AX_UP_CALIBRATION           22

// RAM
#define AX_TORQUE_ENABLE            24
#define AX_LED                      25
#define AX_CW_COMPLIANCE_MARGIN     26
#define AX_CCW_COMPLIANCE_MARGIN    27
#define AX_CW_COMPLIANCE_SLOPE      28
#define AX_CCW_COMPLIANCE_SLOPE     29
#define AX_GOAL_POSITION            30
#define AX_GOAL_SPEED               32
#define AX_TORQUE_LIMIT             34
#define AX_PRESENT_POSITION         36
#define AX_PRESENT_SPEED            38
#define AX_PRESENT_LOAD             40
#define AX_PRESENT_VOLTAGE          42
#define AX_PRESENT_TEMPERATURE      43
#define AX_REGISTERED_INSTRUCTION   44
#define AX_PAUSE_TIME               45
#define AX_MOVING                   46
#define AX_LOCK                     47
#define AX_PUNCH                    48

// Status Return Levels
#define AX_RETURN_NONE              0
#define AX_RETURN_READ              1
#define AX_RETURN_ALL               2

// Instruction Set
#define AX_INST_PING                1
#define AX_INST_READ_DATA           2
#define AX_INST_WRITE_DATA          3
#define AX_INST_REG_WRITE           4
#define AX_INST_ACTION              5
#define AX_INST_RESET               6
#define AX_INST_SYNC_WRITE          131

//TODO : le reste

typedef struct {
    byte id; // ID
    byte error; // Last status returned
} AX12;

void SetupAX();
int RegisterLenAX(byte address);
void PushHeaderAX(AX12 ax, int len, byte inst);
void PushBufferAX(int len, byte* buf);
void PushUSART(byte b);
byte PopUSART();

//ne jamais retourner "error"
byte     PingAX(AX12 ax);
byte     ReadAX(AX12 ax, byte address, int len, byte* buf);
byte    WriteAX(AX12 ax, byte address, int len, byte* buf);
byte RegWriteAX(AX12 ax, byte address, int len, byte* buf);
byte   ActionAX(AX12 ax);
byte    ResetAX(AX12 ax);
// byte SyncWriteAX(AX12 ax, ...);

// Shorthands
 int GetAX(AX12 ax, byte address);
byte PutAX(AX12 ax, byte address, int value);

/*
boolean inverse;

int status_id; stupide il ne doit pas changer    // ID del paquete de retorno
int status_error;                                // error del paquete de retorno

static byte ax_rx_buffer[AX12_BUFFER_SIZE];      // buffer de recepción
static volatile byte ax_rx_Pointer;              // making these volatile keeps the compiler from optimizing loops of available()

}
static void AX12init (long baud);
static void autoDetect (int* list_motors, byte num_motors);


void setEndlessTurnMode (boolean onoff);
void endlessTurn (int velocidad);
byte presentPSL (int* PSL);

static void setTX();
static void setRX();
static void setNone();
*/

#endif /* _AX12_H */
