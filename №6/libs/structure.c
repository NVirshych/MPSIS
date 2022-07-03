/* --COPYRIGHT--,BSD
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
//******************************************************************************
// structure.c
//
// 5 elements configured as a 100 point slider.
//
//
//******************************************************************************

#include "structure.h"

// Actual demo structures
const struct Element element0 = {
		.inputPxoutRegister = (unsigned char *)&P6OUT, // RC: port output address: PxOUT
				        .inputPxinRegister = (unsigned char *)&P6IN,    // RC: port input address: PxIN
				        .inputPxdirRegister = (unsigned char *)&P6DIR,   // RC+PinOsc: port direction address
				        .referencePxoutRegister = (unsigned char *)&P1OUT, // RC: port output address: PxOUT
				        .referencePxdirRegister = (unsigned char *)&P1DIR,
				        .referenceBits = BIT6,
				        .inputBits = BIT0,
				        .maxResponse = 100,
				        .threshold = 50

};

const struct Element element1 = {
		.inputPxoutRegister = (unsigned char *)&P6OUT, // RC: port output address: PxOUT
		        .inputPxinRegister = (unsigned char *)&P6IN,    // RC: port input address: PxIN
		        .inputPxdirRegister = (unsigned char *)&P6DIR,   // RC+PinOsc: port direction address
		        .referencePxoutRegister = (unsigned char *)&P1OUT, // RC: port output address: PxOUT
		        .referencePxdirRegister = (unsigned char *)&P1DIR,
		        .referenceBits = BIT6,
		        .inputBits = BIT2,
		        .maxResponse = 100,
		        .threshold = 50
};
const struct Element element3 = {
		.inputPxoutRegister = (unsigned char *)&P6OUT, // RC: port output address: PxOUT
		        .inputPxinRegister = (unsigned char *)&P6IN,    // RC: port input address: PxIN
		        .inputPxdirRegister = (unsigned char *)&P6DIR,   // RC+PinOsc: port direction address
		        .referencePxoutRegister = (unsigned char *)&P1OUT, // RC: port output address: PxOUT
		        .referencePxdirRegister = (unsigned char *)&P1DIR,
		        .referenceBits = BIT6,
		        .inputBits = BIT3,
		        .maxResponse = 100,
		        .threshold = 50
};
const struct Element element2 = {//CB3, P6.3
        .inputPxoutRegister = (unsigned char *)&P6OUT, // RC: port output address: PxOUT
        .inputPxinRegister = (unsigned char *)&P6IN,    // RC: port input address: PxIN
        .inputPxdirRegister = (unsigned char *)&P6DIR,   // RC+PinOsc: port direction address
        .referencePxoutRegister = (unsigned char *)&P1OUT, // RC: port output address: PxOUT
        .referencePxdirRegister = (unsigned char *)&P1DIR,
        .referenceBits = BIT6,
        .inputBits = BIT2,
        .maxResponse = 100,
        .threshold = 50
};
const struct Element element4 = {
		.inputPxoutRegister = (unsigned char *)&P6OUT, // RC: port output address: PxOUT
		        .inputPxinRegister = (unsigned char *)&P6IN,    // RC: port input address: PxIN
		        .inputPxdirRegister = (unsigned char *)&P6DIR,   // RC+PinOsc: port direction address
		        .referencePxoutRegister = (unsigned char *)&P1OUT, // RC: port output address: PxOUT
		        .referencePxdirRegister = (unsigned char *)&P1DIR,
		        .referenceBits = BIT6,
		        .inputBits = BIT4,
		        .maxResponse = 100,
		        .threshold = 50
};

//*** CAP TOUCH HANDLER *******************************************************/
// This defines the grouping of sensors, the method to measure change in
// capacitance, and the function of the group

const struct Sensor slider =
               {
                  .halDefinition = RC_PAIR_TA0,
                  .numElements = 5,
                  .baseOffset = 0,
                  // Pointer to elements
                  .arrayPtr[0] = &element0,
                  .arrayPtr[1] = &element1,
                  .arrayPtr[2] = &element2,
                  .arrayPtr[3] = &element3,
                  .arrayPtr[4] = &element4,

                  // Timer Information
                  .measGateSource= GATE_WDT_ACLK,     //  0->SMCLK, 1-> ACLK
                  .accumulationCycles = 50 //
               };

