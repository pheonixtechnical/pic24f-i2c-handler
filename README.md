# pic24f-i2c-handler
I2C Handler for PIC24FJ256GA705

# Information
This handler uses I2C1 for performing operations.
Queuing is not implemented yet, but is possible using a ringbuffer for the i2cOperation variable.

# Usage
- Include "i2c_handle.h" to get the API methods
- Include "i2c_operations.h" to get the operation and status enums
- Call runI2cHandler in your main application loop to run the state machine

- To perform an I2C operation, call "request&lt;operation&gt;".
  
  <code>uint8_t request&lt;operation&gt;(eI2cOperationResult *ptrStatus);</code>
  where operation can be
  - Start
  - RepeatedStart
  - Stop
- To send a byte, call sendData 
  
  <code>uint8_t sendData(uint8_t *ptrData, eI2cOperationResult *ptrStatus);</code>
- To receive a byte, call receiveData.  
  
  <code>uint8_t receiveData(uint8_t *ptrData, uint8_t sendAck, eI2cOperationResult *ptrStatus);</code>
  
- Arguments
  - ptrData: pointer to where the receivedbyte should be put
  - sendAck: 1/0 1 = Send ACK after RX is complete 0 = Send NACK after RX is complete
  - ptrStatus: pointer to a status flag of type eI2cOperationResult. 
  
  This is set to I2C_OPERATION_RESULT_RUN whilst running and I2C_OPERATION_RESULT_<status> when complete, where status is SUCCESS or FAILURE. 
  
| WARNING: This must be cleared to 0 or I2C_OPERATION_RESULT_NONE for the state machine to reset after the operation is complete. |
| --- |
  
