
/* 
* This file is part of VL53L1 Platform 
* 
* Copyright (c) 2016, STMicroelectronics - All Rights Reserved 
* 
* License terms: BSD 3-clause "New" or "Revised" License. 
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met: 
* 
* 1. Redistributions of source code must retain the above copyright notice, this 
* list of conditions and the following disclaimer. 
* 
* 2. Redistributions in binary form must reproduce the above copyright notice, 
* this list of conditions and the following disclaimer in the documentation 
* and/or other materials provided with the distribution. 
* 
* 3. Neither the name of the copyright holder nor the names of its contributors 
* may be used to endorse or promote products derived from this software 
* without specific prior written permission. 
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
* 
*/

#include "vl53l1_platform.h"
#include <string.h>
#include <math.h>
#include "../../../../../../../mcu/hal_api/hal_api.h"
#include "../../../../../../../util/api.h"
#include "../../../../../../../mcu/hal_api/i2c.h"
#include "../../../../../../../api/time.h"

#define DEBUG_LOG_MODULE_NAME "DBX_WIIO_APP"
/** To activate logs, configure the following line with "LVL_INFO". */
#define DEBUG_LOG_MAX_LEVEL LVL_DEBUG

#include "../../../../../../../util/debug_log.h"

static i2c_conf_t m_i2c_conf = {
        .clock = 400000,
        .pullup = false,
};

static i2c_res_e VL53L1_I2C_Write(uint8_t p_u8Address, uint8_t * p_pu8Data, uint8_t p_u8DataLength);
static i2c_res_e VL53L1_I2C_WriteAndReadNoStop(uint8_t p_u8Address, uint8_t * p_pu8DataOut,
                                         uint8_t p_u8DataOutLen, uint8_t * p_pu8DataIn, uint8_t p_u8DataInLen);

int8_t VL53L1_WriteMulti( uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count) {
	uint8_t l_au8Index[256u] = { 0u };
    int8_t l_s8Error = 0;
    i2c_res_e res;
   
   if( (count < 0xFD) && (pdata != NULL) )
   {
      l_au8Index[0u] = (uint8_t)((index & 0xFF00)>>8);
      l_au8Index[1u] = (uint8_t) (index & 0x00FF);
      memcpy(&l_au8Index[2u], pdata, count);

       res = VL53L1_I2C_Write((uint8_t) dev, l_au8Index, 3);

       if(res != I2C_RES_OK)
       {
           l_s8Error = -1;
       }
   }
   else
   {
      l_s8Error = -2;
   }
   
   return l_s8Error; // to be implemented
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count){
   uint8_t l_au8Index[2u] = { 0u };
   int8_t l_s8Error = 0;
   
   if( (pdata != NULL) && (count < 0xFF) )
   {
      l_au8Index[0u] = (uint8_t)((index & 0xFF00)>>8);
      l_au8Index[1u] = (uint8_t) (index & 0x00FF);
      if(VL53L1_I2C_WriteAndReadNoStop((uint8_t)dev, l_au8Index, 2u, pdata, (uint8_t)count) != I2C_RES_OK)
      {
         l_s8Error = -1;
      }
   }
   else
   {
      l_s8Error = -2;
   }
   
   return l_s8Error; // to be implemented
}

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data) {
	uint8_t l_au8Index[3u] = { 0u };
   int8_t l_s8Error = 0;
   
   l_au8Index[0u] = (uint8_t)((index & 0xFF00)>>8);
   l_au8Index[1u] = (uint8_t) (index & 0x00FF);
   l_au8Index[2u] = data;
   if((VL53L1_I2C_Write((uint8_t)dev, l_au8Index, 3u) != I2C_RES_OK))
   {
      l_s8Error = -1;
   }
   
   return l_s8Error; // to be implemented
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data) {
   uint8_t l_au8Index[4u] = { 0u };
   int8_t l_s8Error = 0;
   
   l_au8Index[0u] = (uint8_t)((index & 0xFF00)>>8);
   l_au8Index[1u] = (uint8_t) (index & 0x00FF);
   l_au8Index[2u] = (uint8_t)((data & 0xFF00)>>8);
   l_au8Index[3u] = (uint8_t) (data & 0x00FF);
   if((VL53L1_I2C_Write((uint8_t)dev, l_au8Index, 4u) != I2C_RES_OK))
   {
      l_s8Error = -1;
   }
   
   return l_s8Error; // to be implemented
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data) {
   uint8_t l_au8Index[6u] = { 0u };
   int8_t l_s8Error = 0;
   
   l_au8Index[0u] = (uint8_t)((index & 0xFF00)>>8);
   l_au8Index[1u] = (uint8_t) (index & 0x00FF);
   l_au8Index[2u] = (uint8_t)((data & 0xFF000000)>>24);
   l_au8Index[3u] = (uint8_t)((data & 0x00FF0000)>>16);
   l_au8Index[4u] = (uint8_t)((data & 0x0000FF00)>>8);
   l_au8Index[5u] = (uint8_t) (data & 0x000000FF);
   if((VL53L1_I2C_Write((uint8_t)dev, l_au8Index, 6u) != I2C_RES_OK))
   {
      l_s8Error = -1;
   }
   
   return l_s8Error; // to be implemented
}

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *data) {
   uint8_t l_au8Index[2u] = { 0u };
   int8_t l_s8Error = 0;
   
   if(data != NULL)
   {
      l_au8Index[0u] = (uint8_t)((index & 0xFF00)>>8);
      l_au8Index[1u] = (uint8_t) (index & 0x00FF);

      if((VL53L1_I2C_WriteAndReadNoStop((uint8_t)dev, l_au8Index, 2u, data, 1u) != I2C_RES_OK))
      {
         l_s8Error = -1;
      }
   }
   else
   {
      l_s8Error = -2;
   }
   
   return l_s8Error; // to be implemented
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *data) {
   uint8_t l_au8Index[2u] = { 0u };
   int8_t l_s8Error = 0;
   uint8_t l_au8Data[2u] = { 0u };
   if(data != NULL)
   {
      l_au8Index[0u] = (uint8_t)((index & 0xFF00)>>8);
      l_au8Index[1u] = (uint8_t) (index & 0x00FF);
      if((VL53L1_I2C_WriteAndReadNoStop((uint8_t)dev, l_au8Index, 2u, l_au8Data, 2u) != I2C_RES_OK))
      {
         l_s8Error = -1;
      }
      else
      {
         (*data) = (uint16_t)(((uint16_t)l_au8Data[0u] << 8u) + ((uint16_t)l_au8Data[1u]));
      }
   }
   else
   {
      l_s8Error = -2;
   }
   
   return l_s8Error; // to be implemented
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *data) {
   uint8_t l_au8Index[2u] = { 0u };
   int8_t l_s8Error = 0;
   uint8_t l_au8Data[4u] = { 0u };
   
   if(data != NULL)
   {
      l_au8Index[0u] = (uint8_t)((index & 0xFF00)>>8);
      l_au8Index[1u] = (uint8_t) (index & 0x00FF);
      if(VL53L1_I2C_WriteAndReadNoStop((uint8_t)dev, l_au8Index, 2u, l_au8Data, 4u) != I2C_RES_OK)
      {
         l_s8Error = -1;
      }
      else
      {
         (*data) = (uint32_t)(((uint32_t)l_au8Data[0u] << 24u) + ((uint32_t)l_au8Data[1u] << 16u) + ((uint32_t)l_au8Data[2u] << 8u) +((uint32_t)l_au8Data[3u]));
      }
   }
   else
   {
      l_s8Error = -2;
   }
   
   return l_s8Error; // to be implemented
}

/**
    @brief     Blocking wait for a given amount of time.
    @param[in] period Time to wait in ms.
*/
void VL53L1_WaitMs(uint32_t wait_ms){
    app_lib_time_timestamp_hp_t end;
    end = lib_time->addUsToHpTimestamp(lib_time->getTimestampHp(),
                                       wait_ms * 1000);

    /* Active wait until period is elapsed */
    while (lib_time->isHpTimestampBefore(lib_time->getTimestampHp(),
                                         end));
}

/**@brief Function to write with I2C module.
 *
 * @details Description
 * @param[in]  p_u8Address : Address of the I2C module
 *             p_pu8Data : Data you would like to send
 *             p_u8DataLength : Length of data you would like to send
 *
 * @return Error Code, could be SUCCESS, ERROR_BUSY, or ERROR_INTERNAL(Hardware)
 */
static i2c_res_e VL53L1_I2C_Write(uint8_t p_u8Address, uint8_t * p_pu8Data, uint8_t p_u8DataLength) {
    i2c_res_e res;

    res = I2C_init(&m_i2c_conf);

    i2c_xfer_t xfer_tx = {
            .address = p_u8Address,
            .write_ptr = p_pu8Data,
            .write_size = p_u8DataLength,
            .read_ptr = NULL,
            .read_size = 0};

    res = I2C_transfer(&xfer_tx, NULL);
    I2C_close();

    return res;
}

/**@brief Function to write and read with I2C module.
 *
 * @param[in]  p_u8Address : Address of the I2C module
 * @param[in]  p_pu8DataOut : Data you would like to send
 * @param[in]  p_u8DataOutLen : Length of data you would like to send
 * @param[in]  p_u8DataInLen : length of read data
 * @param[out] p_pu8DataIn : Read data from I2C module
 *
 * @return Error Code, could be SUCCESS, ERROR_BUSY, or ERROR_INTERNAL(Hardware)
 */
static i2c_res_e VL53L1_I2C_WriteAndReadNoStop(uint8_t p_u8Address, uint8_t * p_pu8DataOut,
                                       uint8_t p_u8DataOutLen, uint8_t * p_pu8DataIn, uint8_t p_u8DataInLen)
{
    i2c_res_e res;

    res = I2C_init(&m_i2c_conf);
    if (res != I2C_RES_OK)
    {
        LOG(LVL_INFO, "I2C init error %d\r\n", res);
    }


    i2c_xfer_t xfer_rx = {
            .address = p_u8Address,
            .write_ptr = p_pu8DataOut,
            .write_size = p_u8DataOutLen,
            .read_ptr = p_pu8DataIn,
            .read_size = p_u8DataInLen};

    res = I2C_transfer(&xfer_rx, NULL);
    I2C_close();

    if (res != I2C_RES_OK)
    {
        LOG(LVL_INFO, "I2C read error %d\r\n", res);
        LOG(LVL_INFO, "addr = %02X \r\n", xfer_rx.address);

        LOG(LVL_INFO, "write data = ");
        for (uint8_t i= 0; i<xfer_rx.write_size; ++i)
        {
            LOG(LVL_INFO, "%02X", xfer_rx.write_ptr[i]);
        }
        LOG(LVL_INFO, "\r\n");
        LOG(LVL_INFO, "write size = %d\r\n", xfer_rx.write_size);LOG(LVL_INFO, "write data = ");

        LOG(LVL_INFO, "read data = ");
        for (uint8_t i= 0; i<xfer_rx.read_size; ++i)
        {
            LOG(LVL_INFO, "%02X", xfer_rx.read_ptr[i]);
        }
        LOG(LVL_INFO, "\r\n");
        LOG(LVL_INFO, "read size = %d\r\n", xfer_rx.read_size);


    }

    return res;
}