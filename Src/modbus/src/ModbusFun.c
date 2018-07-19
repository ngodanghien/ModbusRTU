/* ----------------------- System includes ----------------------------------*/
#include "stdlib.h"
#include "string.h"

#include "ModBusRTU.h"
#include "ModBusFunc.h"
#include "Bit.h"

/* -----------------------------------------------------------------------------
* Function : 0x01 
*          : pucFrame
* Length   : usLen              
* READ COILS ...
*/
#if MB_FUNC_READ_COILS_ENABLED > 0
eMBException eMBFuncReadCoils( uint8_t * pucFrame, uint16_t * usLen )
{
    uint16_t          usRegAddress;
    uint16_t          usCoilCnt;
    uint8_t           ucNBytes;
    uint8_t          *pucFrameCur;
    uint8_t           i, j, bit;
    uint8_t           ByteN;
	  uint8_t           BitN;
    
    // Kiem tra chieu dai khung du lieu.
    if( *usLen != ( MB_PDU_FUNC_READ_SIZE + MB_PDU_SIZE_MIN ) )
      return MB_EX_ILLEGAL_DATA_VALUE; 
    
    // Kiem tra so COILs phat hien ./
    usCoilCnt = ( uint16_t )( pucFrame[MB_PDU_FUNC_READ_COILCNT_OFF] << 8 );
    usCoilCnt |= ( uint16_t )( pucFrame[MB_PDU_FUNC_READ_COILCNT_OFF + 1] );
    if(usCoilCnt > REG_COILS_SIZE)
      return MB_EX_ILLEGAL_DATA_VALUE;
    
    // Xac dinh Address COILS
    usRegAddress = (uint16_t)( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8 );
    usRegAddress |= (uint16_t)( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1] );
    if((usRegAddress + usCoilCnt) > (REG_COILS_START + REG_COILS_SIZE )){
      return MB_EX_ILLEGAL_DATA_ADDRESS;
    }
     
    // Data Pointer pucFramCur ...        
    pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF]; 
    
    //
    *pucFrameCur++ = MB_FUNC_READ_COILS;	// Function = 1
    
    //So cuon day quy ra BYTE
    ucNBytes = (uint8_t)(usCoilCnt / 8);
    if(usCoilCnt & 0x0007){
        ucNBytes += 1;
    }
    //Chieu dai du lieu.
    *usLen = ucNBytes + 2;
    *pucFrameCur++ = ucNBytes;
  
    j = usRegAddress;//0; Fix
    ByteN = 0;
    for(i = 0; i < ucNBytes; i++){                   // So BYTE Coils
      for(bit = 0; bit < 8 && j < usCoilCnt; bit++){ // Gia tri bit of BYTE
				BitN = GetBit(*MBCoils[j].Port, MBCoils[j].Bit);
				BitN <<= bit;
        ByteN |= BitN;
        j++;
      }  
      pucFrameCur[i] = ByteN;
      ByteN = 0;
   }
   return MB_EX_NONE;
}
#endif

/* -----------------------------------------------------------------------------
 * Function : 0x05 
 * pucFrame
 *            usLen              
 * WRITE_COIL 
 */
#if MB_FUNC_WRITE_COIL_ENABLED > 0
eMBException eMBFuncWriteCoil( uint8_t * pucFrame, uint16_t * usLen )
{
    uint16_t          usRegAddress;
    uint16_t          usRegValue;

    if( *usLen != ( MB_PDU_FUNC_WRITE_SIZE + MB_PDU_SIZE_MIN ) )
      return MB_EX_ILLEGAL_DATA_VALUE;
    
    usRegValue = pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF] << 8;
    usRegValue |= pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF + 1] ;
    if(usRegValue != 0xff00 && usRegValue != 0x0000)
     return MB_EX_ILLEGAL_DATA_VALUE;

    usRegAddress = ( uint16_t )( pucFrame[MB_PDU_FUNC_WRITE_ADDR_OFF] << 8 );
    usRegAddress |= ( uint16_t )( pucFrame[MB_PDU_FUNC_WRITE_ADDR_OFF + 1] );
    if((usRegAddress) > (REG_COILS_START + REG_COILS_SIZE )){
      return MB_EX_ILLEGAL_DATA_ADDRESS;
    }
    if(usRegValue == 0xff00){
        SetBit(*(MBCoils[usRegAddress].Port), MBCoils[usRegAddress].Bit);
    }
    else{
        ClrBit(*(MBCoils[usRegAddress].Port), MBCoils[usRegAddress].Bit);
    }
    return MB_EX_NONE;
}
#endif


/* -----------------------------------------------------------------------------
 * 功    能： 0x0F（15） 写多个线圈 
 * 参    数： pucFrame去除地址后的数制桢
 *            usLen桢长               
 * 运行状态： 
 */
#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
eMBException eMBFuncWriteMultipleCoils( uint8_t * pucFrame, uint16_t * usLen )
{
    uint16_t          usRegAddress;
    uint16_t          usCoilCnt;
    uint8_t           ByteN;
    uint8_t           Cnt;
    uint8_t           i, j, bit;
    if(*usLen <= (MB_PDU_FUNC_WRITE_SIZE + MB_PDU_SIZE_MIN))
      return MB_EX_ILLEGAL_DATA_VALUE;
      
    ByteN = pucFrame[MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF]; //线圈值数据字节数
    usCoilCnt = ( uint16_t )( pucFrame[MB_PDU_FUNC_WRITE_MUL_COILCNT_OFF] << 8 );
    usCoilCnt |= ( uint16_t )( pucFrame[MB_PDU_FUNC_WRITE_MUL_COILCNT_OFF + 1] );
    //根据线圈数量算出线圈值字节数
    Cnt = ( uint8_t )( usCoilCnt / 8 );
    if(usCoilCnt & 0x0007){//非8整数倍时多算一字节
       Cnt += 1;
    }
    
    usRegAddress = (uint16_t)(pucFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF] << 8 );
    usRegAddress |= (uint16_t)(pucFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF + 1]);
    if((usRegAddress + usCoilCnt) > (REG_COILS_START + REG_COILS_SIZE )){
      return MB_EX_ILLEGAL_DATA_ADDRESS;
    }
    //写的线圈数
    if((usCoilCnt > REG_COILS_SIZE ) || (Cnt != ByteN))
      return MB_EX_ILLEGAL_DATA_VALUE;
       
    j = 0;
    for(i = 0; i < Cnt; i++){
      ByteN = pucFrame[MB_PDU_FUNC_WRITE_MUL_VALUES_OFF + i];
      for(bit = 0; bit < 8 && j < usCoilCnt; bit++){
        if(ByteN & 0x01) 
          SetBit(*MBCoils[j].Port, MBCoils[j].Bit);
        else
          ClrBit(*MBCoils[j].Port, MBCoils[j].Bit);
        ByteN >>= 1;
        j++;
      }       
    }
    *usLen = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;
    return MB_EX_NONE;
}
#endif

/* -----------------------------------------------------------------------------
 * 功    能： 0x02（2） 读多个离散开关输入 
 * 参    数： pucFrame去除地址后的数制桢
 *            usLen桢长               
 * 运行状态： 
 */
#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
eMBException eMBFuncReadDiscreteInputs( uint8_t * pucFrame, uint16_t * usLen )
{
    uint16_t          usRegAddress;
    uint16_t          usCoilCnt;
    uint8_t           ucNBytes;
    uint8_t          *pucFrameCur;
    uint8_t           i, j, bit;
    uint8_t           ByteN;
    uint8_t           BitN;
    //数据桢长度检测
    if( *usLen != ( MB_PDU_FUNC_READ_SIZE + MB_PDU_SIZE_MIN ) )
      return MB_EX_ILLEGAL_DATA_VALUE; 
    
    //读取线圈数量检测
    usCoilCnt = ( uint16_t )( pucFrame[MB_PDU_FUNC_READ_COILCNT_OFF] << 8 );
    usCoilCnt |= ( uint16_t )( pucFrame[MB_PDU_FUNC_READ_COILCNT_OFF + 1] );
    if(usCoilCnt > REG_DISCRETE_SIZE)
      return MB_EX_ILLEGAL_DATA_VALUE;
    
    //线圈地址检测
    usRegAddress = ( uint16_t )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8 );
    usRegAddress |= ( uint16_t )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1] );
    if((usRegAddress + usCoilCnt) > (REG_DISCRETE_START + REG_DISCRETE_SIZE )){
      return MB_EX_ILLEGAL_DATA_ADDRESS;
    }
    
    
    //回复数据桢指针        
    pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF]; 
    
    //写入回复的功能码
    *pucFrameCur++ = MB_FUNC_READ_DISCRETE_INPUTS;
    
    //回复线圈数量字节数
    ucNBytes = (uint8_t)(usCoilCnt / 8);
    if(usCoilCnt & 0x0007){
        ucNBytes += 1;
    }
    //数据长度
    *usLen = ucNBytes + 2;
    *pucFrameCur++ = ucNBytes;
  
    j = 0;
    ByteN = 0;
    for(i = 0; i < ucNBytes; i++){                   //线圈字节数
      for(bit = 0; bit < 8 && j < usCoilCnt; bit++){ //线圈字节中的bit值
				BitN = GetBit(*MBDiscretes[j].Port, MBDiscretes[j].Bit);
				BitN <<= bit;
        ByteN |= BitN;
        j++;
      }  
      pucFrameCur[i] = ByteN;
      ByteN = 0;
   }
   return MB_EX_NONE;}
#endif

/* -----------------------------------------------------------------------------
 * 功    能： 0x06（6） 写单个保持寄存器 
 * 参    数： pucFrame去除地址后的数制桢
 *            usLen桢长               
 * 运行状态： 
 */
#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
eMBException eMBFuncWriteHoldingRegister( uint8_t * pucFrame, uint16_t * usLen )
{
    uint16_t          usRegAddress;
    
    if(*usLen != (MB_PDU_FUNC_WRITE_SIZE + MB_PDU_SIZE_MIN))
      return MB_EX_ILLEGAL_DATA_VALUE;
    
    usRegAddress = ( uint16_t )( pucFrame[MB_PDU_FUNC_WRITE_ADDR_OFF] << 8 );
    usRegAddress |= ( uint16_t )( pucFrame[MB_PDU_FUNC_WRITE_ADDR_OFF + 1] );
    if(usRegAddress > REG_HOLDING_START + REG_HOLDING_NREGS)
      return MB_EX_ILLEGAL_DATA_ADDRESS;
    
    
    usRegHoldingBuf[usRegAddress] = pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF] << 8;
    usRegHoldingBuf[usRegAddress] |= pucFrame[MB_PDU_FUNC_WRITE_VALUE_OFF + 1];

    return MB_EX_NONE;
}
#endif


/* -----------------------------------------------------------------------------
 * 功    能： 0x10（16） 写多个保持寄存器 
 * 参    数： pucFrame去除地址后的数制桢
 *            usLen桢长               
 * 运行状态： 
 */
#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
eMBException eMBFuncWriteMultipleHoldingRegister( uint8_t * pucFrame, uint16_t * usLen )
{
    uint16_t          usRegAddress;
    uint16_t          usRegCount;
    uint16_t          i;
    uint8_t           ucRegByteCount;
    uint8_t           *ucReData;
    //数据桢长检测
    if( *usLen < ( MB_PDU_FUNC_WRITE_MUL_SIZE_MIN + MB_PDU_SIZE_MIN ) )
      return MB_EX_ILLEGAL_DATA_VALUE;
     
    //寄存器数据检测
    usRegCount = ( uint16_t )( pucFrame[MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF] << 8 );
    usRegCount |= ( uint16_t )( pucFrame[MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF + 1] );
    ucRegByteCount = pucFrame[MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF];   
    if((usRegCount > MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX) || (ucRegByteCount !=  (uint8_t)(2 * usRegCount )))
      return MB_EX_ILLEGAL_DATA_VALUE; 
    
    //寄存器地址检测
    usRegAddress = ( uint16_t )( pucFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF] << 8 );
    usRegAddress |= ( uint16_t )( pucFrame[MB_PDU_FUNC_WRITE_MUL_ADDR_OFF + 1] );
    if(usRegAddress + usRegCount > REG_HOLDING_START + REG_HOLDING_NREGS ) 
      return MB_EX_ILLEGAL_DATA_ADDRESS;
    
    //数量
    ucReData = &pucFrame[MB_PDU_FUNC_WRITE_MUL_VALUES_OFF];
    for(i = 0; i < usRegCount; i++){
      usRegHoldingBuf[usRegAddress + i] = ucReData[i * 2] << 8;
      usRegHoldingBuf[usRegAddress + i] |= ucReData[i * 2 + 1];
    }
   
    *usLen = MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF;        
    return MB_EX_NONE;
}
#endif

/* -----------------------------------------------------------------------------
 * 功    能： 0x03（3） 读多个保持寄存器 
 * 参    数： pucFrame去除地址后的数制桢
 *            usLen桢长               
 * 运行状态： 
 */
#if MB_FUNC_READ_HOLDING_ENABLED > 0
eMBException eMBFuncReadHoldingRegister( uint8_t * pucFrame, uint16_t * usLen )
{
    uint16_t          usRegAddress;
    uint16_t          usRegCount;
    uint8_t          *pucFrameCur;
    uint8_t           i;
    //桢长检测
    if(*usLen != (MB_PDU_FUNC_READ_SIZE + MB_PDU_SIZE_MIN))
      return MB_EX_ILLEGAL_DATA_VALUE;
    
    usRegAddress  = (uint16_t)(pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8);
    usRegAddress |= (uint16_t)(pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1]);
    
    usRegCount  = (uint16_t)(pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF] << 8);
    usRegCount |= (uint16_t)(pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF + 1]);
    //寄存器地址检测
    if((usRegAddress + usRegCount) > (REG_HOLDING_START + REG_HOLDING_NREGS))
      return MB_EX_ILLEGAL_DATA_ADDRESS;
    //读的长度检测
    if((usRegCount < 1 ) || (usRegCount > MB_PDU_FUNC_READ_REGCNT_MAX))
      return MB_EX_ILLEGAL_DATA_VALUE;
    //回复数据指针    
    pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
    //回复功能码
    *pucFrameCur++ = MB_FUNC_READ_HOLDING_REGISTER;
    //回复数据字节数
    *pucFrameCur++ =(uint8_t)(usRegCount * 2);
    //回复寄存器内容
    for(i = 0; i < usRegCount; i++){
      pucFrameCur[i * 2] = usRegHoldingBuf[i+usRegAddress] >> 8; //fix: i+usRegAddress
      pucFrameCur[i * 2 + 1] = usRegHoldingBuf[i+usRegAddress] & 0xff;
    }       
    *usLen = usRegCount * 2 + 2;        
    return MB_EX_NONE;
}
#endif


/* -----------------------------------------------------------------------------
 * 功    能： 0x17（23） 先写再读多个保持寄存器 
 * 参    数： pucFrame去除地址后的数制桢
 *            usLen桢长               
 * 运行状态： 
 */
#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0

eMBException eMBFuncReadWriteMultipleHoldingRegister( uint8_t * pucFrame, uint16_t * usLen )
{
    uint16_t          usRegReadAddress;
    uint16_t          usRegReadCount;
    uint16_t          usRegWriteAddress;
    uint16_t          usRegWriteCount;
    uint8_t           ucRegWriteByteCount;
    uint8_t          *pucFrameCur;
    uint8_t           i;
  
    if(*usLen < ( MB_PDU_FUNC_READWRITE_SIZE_MIN + MB_PDU_SIZE_MIN))
      return MB_EX_ILLEGAL_DATA_VALUE;
    
    //读寄存器地址
    usRegReadAddress = ( uint16_t )( pucFrame[MB_PDU_FUNC_READWRITE_READ_ADDR_OFF] << 8U );
    usRegReadAddress |= ( uint16_t )( pucFrame[MB_PDU_FUNC_READWRITE_READ_ADDR_OFF + 1] );
    //读个数
    usRegReadCount = ( uint16_t )( pucFrame[MB_PDU_FUNC_READWRITE_READ_REGCNT_OFF] << 8U );
    usRegReadCount |= ( uint16_t )( pucFrame[MB_PDU_FUNC_READWRITE_READ_REGCNT_OFF + 1] );
    if(( usRegReadAddress + usRegReadCount) > (REG_HOLDING_START + REG_HOLDING_NREGS ) )
      return MB_EX_ILLEGAL_DATA_ADDRESS;
  
   //写地址
    usRegWriteAddress = ( uint16_t )( pucFrame[MB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF] << 8U );
    usRegWriteAddress |= ( uint16_t )( pucFrame[MB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF + 1] );
    //写个数
    usRegWriteCount = ( uint16_t )( pucFrame[MB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF] << 8U );
    usRegWriteCount |= ( uint16_t )( pucFrame[MB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF + 1] );
    if(( usRegWriteAddress + usRegWriteCount) > (REG_HOLDING_START + REG_HOLDING_NREGS ) )
      return MB_EX_ILLEGAL_DATA_ADDRESS;
    
    //写数值的字节数
    ucRegWriteByteCount = pucFrame[MB_PDU_FUNC_READWRITE_BYTECNT_OFF];
    //字节数检测
    if((usRegReadCount > 0x7D) && ( usRegWriteCount > 0x79 ) && ((2 * usRegWriteCount) != ucRegWriteByteCount))
      return MB_EX_ILLEGAL_DATA_VALUE;
    
    //写    
    pucFrameCur = &pucFrame[MB_PDU_FUNC_READWRITE_WRITE_VALUES_OFF];
    for(i = 0; i < usRegWriteCount; i++){
      usRegHoldingBuf[usRegWriteAddress + i] = pucFrameCur[i * 2] << 8;
      usRegHoldingBuf[usRegWriteAddress + i] |= pucFrameCur[i * 2 + 1];
    }
    
    //读        
    pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
    *pucFrameCur++ = MB_FUNC_READWRITE_MULTIPLE_REGISTERS; 
    *pucFrameCur++ = ( uint8_t ) ( usRegReadCount * 2 );
    for(i = 0; i < usRegReadCount; i++){
      pucFrameCur[i * 2] = usRegHoldingBuf[i] >> 8;
      pucFrameCur[i * 2 + 1] = usRegHoldingBuf[i] & 0xff;
    }       
    *usLen = 2 * usRegReadCount + 2;
        
    return MB_EX_NONE;
}
#endif
/* -----------------------------------------------------------------------------
 * 功    能： 0x04（4） 读取输入寄存器 
 * 参    数： pucFrame去除地址后的数制桢
 *            usLen桢长               
 * 运行状态： 
 */
#if MB_FUNC_READ_INPUT_ENABLED > 0
eMBException eMBFuncReadInputRegister( uint8_t * pucFrame, uint16_t * usLen )
{
    uint16_t          usRegAddress;
    uint16_t          usRegCount;
    uint8_t          *pucFrameCur;
    uint8_t           i;

    if( *usLen != ( MB_PDU_FUNC_READ_SIZE + MB_PDU_SIZE_MIN ) )
      return MB_EX_ILLEGAL_DATA_VALUE;
    
    usRegAddress = ( uint16_t )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF] << 8 );
    usRegAddress |= ( uint16_t )( pucFrame[MB_PDU_FUNC_READ_ADDR_OFF + 1] );

    usRegCount = ( uint16_t )( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF] << 8 );
    usRegCount |= ( uint16_t )( pucFrame[MB_PDU_FUNC_READ_REGCNT_OFF + 1] );
        
     if((usRegAddress + usRegCount) > (REG_INPUT_START + REG_INPUT_NREGS))
        return MB_EX_ILLEGAL_DATA_ADDRESS;

     if(usRegCount > MB_PDU_FUNC_READ_REGCNT_MAX)
       return MB_EX_ILLEGAL_DATA_VALUE;
        
    pucFrameCur = &pucFrame[MB_PDU_FUNC_OFF];
    *pucFrameCur++ = MB_FUNC_READ_INPUT_REGISTER;
    *pucFrameCur++ = (uint8_t)(usRegCount * 2);

    for(i = 0; i < usRegCount; i++){
      pucFrameCur[i * 2] = usRegInputBuf[i] >> 8;
      pucFrameCur[i * 2 + 1] = usRegInputBuf[i] & 0xff;
    }       
            
    *usLen = usRegCount * 2 + 2;
    return MB_EX_NONE;
}
#endif

/* -----------------------------------------------------------------------------
 * 功    能： 0x14（17） 设置从ID 
 * 参    数： pucFrame去除地址后的数制桢
 *            usLen桢长               
 * 运行状态： 
 */
#if MB_FUNC_OTHER_REP_SLAVEID_ENABLED > 0
static uint8_t    ucMBSlaveID[MB_FUNC_OTHER_REP_SLAVEID_BUF];
static uint16_t   usMBSlaveIDLen;

eMBErrorCode eMBSetSlaveID( uint8_t ucSlaveID, bool xIsRunning,
               uint8_t const *pucAdditional, uint16_t usAdditionalLen )
{
    eMBErrorCode    eStatus = MB_ENOERR;

    /* the first byte and second byte in the buffer is reserved for
     * the parameter ucSlaveID and the running flag. The rest of
     * the buffer is available for additional data. */
    if( usAdditionalLen + 2 < MB_FUNC_OTHER_REP_SLAVEID_BUF )
    {
        usMBSlaveIDLen = 0;
        ucMBSlaveID[usMBSlaveIDLen++] = ucSlaveID;
        ucMBSlaveID[usMBSlaveIDLen++] = ( uint8_t )( xIsRunning ? 0xFF : 0x00 );
        if( usAdditionalLen > 0 )
        {
            memcpy( &ucMBSlaveID[usMBSlaveIDLen], pucAdditional,
                    ( size_t )usAdditionalLen );
            usMBSlaveIDLen += usAdditionalLen;
        }
    }
    else
    {
        eStatus = MB_ENORES;
    }
    return eStatus;
}

/* -----------------------------------------------------------------------------
 * 功    能： 0x14（17） 报告从ID 
 * 参    数： pucFrame去除地址后的数制桢
 *            usLen桢长               
 * 运行状态： 
 */
eMBException eMBFuncReportSlaveID( uint8_t * pucFrame, uint16_t * usLen )
{
    memcpy( &pucFrame[MB_PDU_DATA_OFF], &ucMBSlaveID[0], ( size_t )usMBSlaveIDLen );
    *usLen = ( uint16_t )( MB_PDU_DATA_OFF + usMBSlaveIDLen );
    return MB_EX_NONE;
}
#endif


