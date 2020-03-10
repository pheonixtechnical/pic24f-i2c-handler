#include <xc.h>
#include <p24fj256ga705.h>
#include "i2c_operations.h"

typedef union {
    struct {
        unsigned busLocked:1;
        unsigned inUse:1;
        unsigned request:1;
    };
    uint8_t byte;
} tI2cFlags;

typedef struct {
    uint8_t ack;
    eI2cOperationType opType;
    eI2cOperationResult *ptrStatus;
    uint8_t *ptrData;
    tI2cFlags flags;
} tI2cOperation;

typedef enum {
    I2C_STATE_NONE,
    I2C_STATE_WAIT_FOR_OPERATION,
    I2C_STATE_HANDLE_OPERATION,
    I2C_STATE_SEND_START_BIT,
    I2C_STATE_WAIT_FOR_START_COMPLETE,
    I2C_STATE_SEND_REPEATED_START,
    I2C_STATE_WAIT_FOR_REPEATED_START_COMPLETE,
    I2C_STATE_SEND_STOP_BIT,
    I2C_STATE_WAIT_FOR_STOP_COMPLETE,
    I2C_STATE_SEND_DATA,
    I2C_STATE_WAIT_DATA_SENT,
    I2C_STATE_RECEIVE_DATA,
    I2C_STATE_WAIT_DATA_RECEIVED,
    I2C_STATE_HANDLE_ACK_NACK,
    I2C_STATE_WAIT_ACK_COMPLETE,
    I2C_STATE_HANDLE_DATA_RECEIVED,
    I2C_STATE_MARK_OPERATION_COMPLETE,
    I2C_STATE_WAIT_CLEAR,
    I2C_STATE_RESET,
    I2C_STATE_ERROR,
} eI2cState;

tI2cOperation i2cOperation;
eI2cState i2cState;

//                               API Methods                                  //
void runI2cHandler(void);
uint8_t requestStart(uint8_t *ptrStatus);
uint8_t requestRepeatedStart(uint8_t *ptrStatus);
uint8_t requestStop(uint8_t *ptrStatus);
uint8_t sendData(uint8_t *ptrData, uint8_t *ptrStatus);
uint8_t receiveData(uint8_t *ptrData, uint8_t sendAck, uint8_t *ptrStatus);
uint8_t lockBus(void);
uint8_t unlockBus(void);

//                            Prototype Methods                               //


uint8_t requestStart(eI2cOperationResult *ptrStatus) {
    if(i2cOperation.flags.inUse) {
        return 1;
    }
    
    i2cOperation.opType = I2C_OPERATION_START;
    i2cOperation.ptrStatus = ptrStatus;
    i2cOperation.flags.inUse = 1;
}

uint8_t requestRepeatedStart(eI2cOperationResult *ptrStatus) {
    if(i2cOperation.flags.inUse) {
        return 1;
    }
    
    i2cOperation.opType = I2C_OPERATION_REPEATED_START;
    i2cOperation.ptrStatus = ptrStatus;
    i2cOperation.flags.inUse = 1;
}

uint8_t requestStop(eI2cOperationResult *ptrStatus) {
    if(i2cOperation.flags.inUse) {
        return 1;
    }
    
    i2cOperation.opType = I2C_OPERATION_STOP;
    i2cOperation.ptrStatus = ptrStatus;
    i2cOperation.flags.inUse = 1;
}

uint8_t sendData(uint8_t *ptrData, eI2cOperationResult *ptrStatus) {
    if(i2cOperation.flags.inUse) {
        return 1;
    }
    i2cOperation.opType = I2C_OPERATION_STOP;
    i2cOperation.ptrData = ptrData;
    i2cOperation.ptrStatus = ptrStatus;
    i2cOperation.flags.inUse = 1; 
}

uint8_t receiveData(uint8_t *ptrData, uint8_t sendAck, eI2cOperationResult *ptrStatus) {
    if(i2cOperation.flags.inUse) {
        return 1;
    }
    
    i2cOperation.ack = sendAck;
    i2cOperation.opType = I2C_OPERATION_STOP;
    i2cOperation.ptrData = ptrData;
    i2cOperation.ptrStatus = ptrStatus;
    i2cOperation.flags.inUse = 1;
}

uint8_t lockBus(void){
    if(i2cOperation.flags.busLocked) {
        return 1;
    } else {
        i2cOperation.flags.busLocked = 1;
        return 0;
    }
}

uint8_t unlockBus(void) {
    if(!i2cOperation.flags.busLocked) {
        return 1;
    } else {
        i2cOperation.flags.busLocked = 0;
        return 0;
    }
}

void runI2cHandler(void) {
    switch (i2cState) {
        case I2C_STATE_NONE:
            i2cState = I2C_STATE_WAIT_FOR_OPERATION;
            break;
            
        case I2C_STATE_WAIT_FOR_OPERATION:
            if(i2cOperation.flags.inUse) {
                i2cState = I2C_STATE_HANDLE_OPERATION;
            }
            break;
            
        case I2C_STATE_HANDLE_OPERATION:
            switch(i2cOperation.opType) {
                case I2C_OPERATION_START:
                    i2cState = I2C_STATE_SEND_START_BIT;
                    break;
                    
                case I2C_OPERATION_REPEATED_START:
                    i2cState = I2C_STATE_SEND_REPEATED_START;
                    break;
                    
                case I2C_OPERATION_STOP:
                    i2cState = I2C_STATE_SEND_STOP_BIT;
                    break;
                    
                case I2C_OPERATION_SEND:
                    i2cState = I2C_STATE_SEND_DATA;
                    break;
                    
                case I2C_OPERATION_RECEIVE:
                    i2cState = I2C_STATE_RECEIVE_DATA;
                    break;
                    
                default:
                    i2cState = I2C_STATE_ERROR;
                    break;
            }
            break;
            
        case I2C_STATE_SEND_START_BIT:
            I2C1CONLbits.SEN = 1;
            break;
            
        case I2C_STATE_WAIT_FOR_START_COMPLETE:
            if(!I2C1CONLbits.SEN) {
                i2cState = I2C_STATE_MARK_OPERATION_COMPLETE;
            }
            break;
            
        case I2C_STATE_SEND_REPEATED_START:
            I2C1CONLbits.RSEN = 1;
            break;
            
        case I2C_STATE_WAIT_FOR_REPEATED_START_COMPLETE:
            if(!I2C1CONLbits.RSEN) {
                i2cState = I2C_STATE_MARK_OPERATION_COMPLETE;
            }
            break;
            
        case I2C_STATE_SEND_STOP_BIT:
            I2C1CONLbits.PEN = 1;
            break;
            
        case I2C_STATE_WAIT_FOR_STOP_COMPLETE:
            if(!I2C1CONLbits.PEN) {
                i2cState = I2C_STATE_MARK_OPERATION_COMPLETE;
            }
            break;
            
        case I2C_STATE_SEND_DATA:
            I2C1TRN = *i2cOperation.ptrData;
            i2cState = I2C_STATE_WAIT_DATA_SENT;
            break;
            
        case I2C_STATE_WAIT_DATA_SENT:
            if(!I2C1STATbits.TBF) {
                i2cState = I2C_STATE_MARK_OPERATION_COMPLETE;
            }
            break;
            
        case I2C_STATE_RECEIVE_DATA:
            I2C1CONLbits.RCEN = 1;
            i2cState = I2C_STATE_WAIT_DATA_RECEIVED;
            break;
            
        case I2C_STATE_WAIT_DATA_RECEIVED:
            if(!I2C1CONLbits.RCEN) {
                i2cState = I2C_STATE_HANDLE_ACK_NACK;
            }
            break;
            
        case I2C_STATE_HANDLE_ACK_NACK:
            I2C1CONLbits.ACKDT = !i2cOperation.ack;
            I2C1CONLbits.ACKEN = 1;
            i2cState = I2C_STATE_WAIT_ACK_COMPLETE;
            break;
            
        case I2C_STATE_WAIT_ACK_COMPLETE:
            if(!I2C1CONLbits.ACKEN) {
                i2cState = I2C_STATE_HANDLE_DATA_RECEIVED;
            }
            break;
            
        case I2C_STATE_HANDLE_DATA_RECEIVED:
            *i2cOperation.ptrData = I2C1RCV;
            break;
            
        case I2C_STATE_MARK_OPERATION_COMPLETE:
            i2cOperation.ptrStatus = I2C_OPERATION_RESULT_SUCCESS;
            i2cState = I2C_STATE_WAIT_CLEAR;
            break;
            
        case I2C_STATE_WAIT_CLEAR:
            if(i2cOperation.ptrStatus == 0) {
                i2cState = I2C_STATE_RESET;
            }
            break;
            
        case I2C_STATE_RESET:
            i2cOperation.flags.inUse = 0;
            i2cState = I2C_STATE_WAIT_FOR_OPERATION;
            break;
            
        case I2C_STATE_ERROR:
            break;
    }
}
