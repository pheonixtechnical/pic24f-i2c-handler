#ifndef I2C_HANDLE_H
#define	I2C_HANDLE_H

#include "i2c_operations.h"

extern void runI2cHandler(void);
extern uint8_t requestStart(eI2cOperationResult *ptrStatus);
extern uint8_t requestRepeatedStart(eI2cOperationResult *ptrStatus);
extern uint8_t requestStop(eI2cOperationResult *ptrStatus);
extern uint8_t sendData(uint8_t *ptrData, eI2cOperationResult *ptrStatus);
extern uint8_t receiveData(uint8_t *ptrData, uint8_t sendAck, eI2cOperationResult *ptrStatus);
extern uint8_t lockBus(void);
extern uint8_t unlockBus(void);

#endif	/* I2C_HANDLE_H */