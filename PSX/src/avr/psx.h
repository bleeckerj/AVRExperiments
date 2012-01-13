int main(void);
void setup(void);
void delay_1s(void);
void delay_500ms(void);
void handleCommand(u08);
//void console_state_machine(void);

// higher level functions to manage the game controller
void GameControllerCommand_ReadData(void);
void GameControllerCommand_SetMode(u08, s08);
void GameControllerCommand_SetModeAndLock(u08);
void GameControllerCommand_ConfigModeEnter(void);
void GameControllerCommand_ConfigModeExit(void);
void GameControllerCommand_VibrationEnable(void);
void GameControllerCommand_ReadDataAndVibrate(u08);
void GameController_Reset(void);

// functions between the PSX board and the game controller
//
// read data from the game controller
u08 readData_GameController(void);
// sends a command byte, ignoring any returned data
void sendCommand_GameController(u08);
// send a command byte and listen on data line, return the data
u08 readDataSendCommand_GameController(u08);


u08 Console_ReadCommand(void);
void sendData_Console(u08);
u08 readCommandSendData_Console(u08);
void sendAcknowledge_Console(void);

