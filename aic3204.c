/*****************************************************************************/
/*                                                                           */
/* FILENAME                                                                  */
/* 	 aic3204.c                                                               */
/*                                                                           */
/* DESCRIPTION                                                               */
/*   Setup functions for aic3204 codec on the TMS320C5535 eZdsp.        */
/*                                                                           */
/* REVISION                                                                  */
/*   Revision: 1.00	                                                         */
/*   Author  : Richard Sikora                                                */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* HISTORY                                                                   */
/*                                                                           */
/*   Revision 1.00                                                           */
/*   20th December 2009. Created by Richard Sikora from Spectrum Digital     */
/*                       code. Created new functions for codec read          */
/*                       and write.                                          */
/*                                                                           */
/*****************************************************************************/
/*
 *
 * Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/


#include "ezdsp5535.h"
#include "aic3204.h"
#include "ezdsp5535_gpio.h"
#include "ezdsp5535_i2c.h"
#include "math.h"

float phase = 0.0f; 
const float PI = 3.14159265f;
const float freq = 1000.0f;      
const float fs = 48000.0f;        
const float amplitude = 20000.0f; 
Int16 counter1; // Counters for monitoring real-time operation.
Int16 counter2;

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _AIC3204_rget( regnum, regval )                                         *
 *                                                                          *
 *      Return value of codec register regnum                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 AIC3204_rget(  Uint16 regnum, Uint16* regval )
{
    Int16 retcode = 0;
    Uint8 cmd[2];

    cmd[0] = regnum & 0x007F;       // 7-bit Device Address
    cmd[1] = 0;

    retcode |= EZDSP5535_I2C_write( AIC3204_I2C_ADDR, cmd, 1 );
    retcode |= EZDSP5535_I2C_read( AIC3204_I2C_ADDR, cmd, 1 );

    *regval = cmd[0];
    EZDSP5535_wait( 10 );
    return retcode;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _AIC3204_rset( regnum, regval )                                         *
 *                                                                          *
 *      Set codec register regnum to value regval                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 AIC3204_rset( Uint16 regnum, Uint16 regval )
{
    Uint8 cmd[2];
    cmd[0] = regnum & 0x007F;       // 7-bit Device Address
    cmd[1] = regval;                // 8-bit Register Data

    return EZDSP5535_I2C_write( AIC3204_I2C_ADDR, cmd, 2 );
}


/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  aic3204_enable( )                                                       *
 *                                                                          *
 * ------------------------------------------------------------------------ */  

void aic3204_hardware_init(void)
{
 	SYS_EXBUSSEL |= 0x0020;  // Select A20/GPIO26 as GPIO26
	EZDSP5535_GPIO_init();
	EZDSP5535_GPIO_setDirection(GPIO26, GPIO_OUT);
	EZDSP5535_GPIO_setOutput( GPIO26, 1 );    // Take AIC3204 chip out of reset
	EZDSP5535_I2C_init( );                    // Initialize I2C
	EZDSP5535_wait( 100 );  // Wait  
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  aic3204_disable( )                                                      *
 *                                                                          *
 * ------------------------------------------------------------------------ */

void aic3204_disable(void)
{
    AIC3204_rset( 1, 1 );                   // Reset codec
  	EZDSP5535_GPIO_setOutput( GPIO26, 0 ); // Put AIC3204 into reset
    I2S0_CR = 0x00; 
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  aic3204_codec_read( )                                                   *
 *                                                                          *
 * ------------------------------------------------------------------------ */

void aic3204_codec_read(Int16* left_input, Int16* right_input)
{
    volatile Int16 dummy;
    Int16 sine_value;
    
    while ( !(I2S0_IR & RcvR) );

    
    dummy = I2S0_W0_MSW_R;
    dummy = I2S0_W0_LSW_R;
    dummy = I2S0_W1_MSW_R;
    dummy = I2S0_W1_LSW_R;

    
    sine_value = (Int16)(amplitude * sinf(phase));

    *left_input  = sine_value;
    *right_input = sine_value;


    phase += 2.0f * PI * freq / fs;
    if (phase >= 2.0f * PI) {
        phase -= 2.0f * PI;
    }
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  aic3204_codec_write( )                                                  *
 *                                                                          *
 * ------------------------------------------------------------------------ */
 
void aic3204_codec_write(Int16 left_output, Int16 right_output)
{
	counter2 = 0;
	
    while( !(I2S0_IR & XmitR) )
    {
   	counter2++; // Wait for transmit interrupt
    }	
	I2S0_W0_MSW_W = left_output;         // Left output       
    I2S0_W0_LSW_W = 0;
    I2S0_W1_MSW_W = right_output;        // Right output
    I2S0_W1_LSW_W = 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  End of aic3204.c                                                        *
 *                                                                          *
 * ------------------------------------------------------------------------ */













