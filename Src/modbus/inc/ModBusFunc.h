/* 
 * FreeModbus Libary: A portable Modbus implementation for Modbus ASCII/RTU.
 * Copyright (c) 2006 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * File: $Id: mbfunc.h,v 1.12 2006/12/07 22:10:34 wolti Exp $
 */

#ifndef _MB_FUNC_H
#define _MB_FUNC_H
#include "ModBusRTU.h"


#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif
#if MB_FUNC_OTHER_REP_SLAVEID_BUF > 0
    eMBException eMBFuncReportSlaveID( uint8_t * pucFrame, uint16_t * usLen );
#endif

#if MB_FUNC_READ_INPUT_ENABLED > 0
eMBException    eMBFuncReadInputRegister( uint8_t * pucFrame, uint16_t * usLen );
#endif

#if MB_FUNC_READ_HOLDING_ENABLED > 0
eMBException    eMBFuncReadHoldingRegister( uint8_t * pucFrame, uint16_t * usLen );
#endif

#if MB_FUNC_WRITE_HOLDING_ENABLED > 0
eMBException    eMBFuncWriteHoldingRegister( uint8_t * pucFrame, uint16_t * usLen );
#endif

#if MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED > 0
eMBException    eMBFuncWriteMultipleHoldingRegister( uint8_t * pucFrame, uint16_t * usLen );
#endif

#if MB_FUNC_READ_COILS_ENABLED > 0
eMBException    eMBFuncReadCoils( uint8_t * pucFrame, uint16_t * usLen );
#endif

#if MB_FUNC_WRITE_COIL_ENABLED > 0
eMBException    eMBFuncWriteCoil( uint8_t * pucFrame, uint16_t * usLen );
#endif

#if MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED > 0
eMBException    eMBFuncWriteMultipleCoils( uint8_t * pucFrame, uint16_t * usLen );
#endif

#if MB_FUNC_READ_DISCRETE_INPUTS_ENABLED > 0
eMBException    eMBFuncReadDiscreteInputs( uint8_t * pucFrame, uint16_t * usLen );
#endif

#if MB_FUNC_READWRITE_HOLDING_ENABLED > 0
eMBException    eMBFuncReadWriteMultipleHoldingRegister( uint8_t * pucFrame, uint16_t * usLen );
#endif


/* ----------------------- Defines ------------------------------------------*/
#define BITS_uint8_t      8U

#define MB_PDU_FUNC_READ_ADDR_OFF           ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_READ_COILCNT_OFF        ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE               ( 4 )
#define MB_PDU_FUNC_READ_COILCNT_MAX        ( 0x07D0 )

#define MB_PDU_FUNC_WRITE_ADDR_OFF          ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_WRITE_VALUE_OFF         ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_SIZE              ( 4 )

#define MB_PDU_FUNC_WRITE_MUL_ADDR_OFF      ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_WRITE_MUL_COILCNT_OFF   ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF   ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_WRITE_MUL_VALUES_OFF    ( MB_PDU_DATA_OFF + 5 )
#define MB_PDU_FUNC_WRITE_MUL_SIZE_MIN      ( 5 )
#define MB_PDU_FUNC_WRITE_MUL_COILCNT_MAX   ( 0x07B0 )


#define MB_PDU_FUNC_READ_ADDR_OFF           ( MB_PDU_DATA_OFF )
#define MB_PDU_FUNC_READ_DISCCNT_OFF        ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE               ( 4 )
#define MB_PDU_FUNC_READ_DISCCNT_MAX        ( 0x07D0 )


#define MB_PDU_FUNC_READ_REGCNT_OFF             ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE                   ( 4 )
#define MB_PDU_FUNC_READ_REGCNT_MAX             ( 0x007D )

#define MB_PDU_FUNC_WRITE_VALUE_OFF             ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_SIZE                  ( 4 )

#define MB_PDU_FUNC_WRITE_MUL_REGCNT_OFF        ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_WRITE_MUL_BYTECNT_OFF       ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_WRITE_MUL_VALUES_OFF        ( MB_PDU_DATA_OFF + 5 )
#define MB_PDU_FUNC_WRITE_MUL_SIZE_MIN          ( 5 )
#define MB_PDU_FUNC_WRITE_MUL_REGCNT_MAX        ( 0x0078 )

#define MB_PDU_FUNC_READWRITE_READ_ADDR_OFF     ( MB_PDU_DATA_OFF + 0 )
#define MB_PDU_FUNC_READWRITE_READ_REGCNT_OFF   ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READWRITE_WRITE_ADDR_OFF    ( MB_PDU_DATA_OFF + 4 )
#define MB_PDU_FUNC_READWRITE_WRITE_REGCNT_OFF  ( MB_PDU_DATA_OFF + 6 )
#define MB_PDU_FUNC_READWRITE_BYTECNT_OFF       ( MB_PDU_DATA_OFF + 8 )
#define MB_PDU_FUNC_READWRITE_WRITE_VALUES_OFF  ( MB_PDU_DATA_OFF + 9 )
#define MB_PDU_FUNC_READWRITE_SIZE_MIN          ( 9 )

#define MB_PDU_FUNC_READ_REGCNT_OFF         ( MB_PDU_DATA_OFF + 2 )
#define MB_PDU_FUNC_READ_SIZE               ( 4 )
#define MB_PDU_FUNC_READ_REGCNT_MAX         ( 0x007D )

#define MB_PDU_FUNC_READ_RSP_BYTECNT_OFF    ( MB_PDU_DATA_OFF )


#ifdef __cplusplus
PR_END_EXTERN_C
#endif
#endif
