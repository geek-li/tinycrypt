/******************************************************************************
Written and Copyright (C) by Dirk Klose
and the EmSec Embedded Security group of Ruhr-Universitaet Bochum. 
All rights reserved.

Contact lightweight@crypto.rub.de for comments & questions.
This program is free software; You may use it or parts of it or
modifiy it under the following terms:

(1) Usage and/or redistribution and/or modification of the software 
or parts of the software is permitted for non-commercial use only.

(2a) If this software or parts are used as part of a new software, you
must license the entire work, as a whole, under this License to anyone
who comes into possession of a copy. This License will therefore
apply, to the whole of the work, and all its parts, regardless of how
they are packaged.

(2b) You may expand this license by your own license. In this case this
license still applies to the software as mentioned in (2a) and must
not be changed. The expansion must be clearly recognizable as such. In
any case of collision between the license and the expansion the
license is superior to the expansion.

(3) If this software or parts are used as part of a new software, you
must provide equivalent access to the source code of the entire work,
as a whole, to anyone who comes into possession of a copy, in the same
way through the same place at no further charge, as for the binary
version.

(4) This program is distributed in the hope that it will be useful,
but   WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
(5) These notices must be retained in any copies of any part of this
documentation and/or software.

(6) If this software is used credit must be given to the
"Embedded Security Group of Ruhr-Universitaet Bochum, Germany" as
the authors of the parts of the software used. This can be in the form
of a textual message at program startup or  at *beginning* of the
documentation (online or textual) provided with the package.

If you are interested in a commercial use 
please contact '''lightweigth@crypto.rub.de'''
******************************************************************************/

// Include-file
#include<stdint.h>

int main(void)
{
//	4Bit sBox used for key scheduling
const uint8_t sBox4[]	=	{
							0xc0,0x50,0x60,0xb0,0x90,0x00,0xa0,0xd0,0x30,0xe0,0xf0,0x80,0x40,0x70,0x10,0x20
							};
//	8Bit sBox used for the sBox functionality
const uint8_t sBox8[]	=	{
							204, 197, 198, 203, 201, 192, 202, 205, 
							195, 206, 207, 200, 196, 199, 193, 194, 
							92, 85, 86, 91, 89, 80, 90, 93, 
							83, 94, 95, 88, 84, 87, 81, 82, 
							108, 101, 102, 107, 105, 96, 106, 109, 
							99, 110, 111, 104, 100, 103, 97, 98, 
							188, 181, 182, 187, 185, 176, 186, 189, 
							179, 190, 191, 184, 180, 183, 177, 178, 
							156, 149, 150, 155, 153, 144, 154, 157, 
							147, 158, 159, 152, 148, 151, 145, 146, 
							12, 5, 6, 11, 9, 0, 10, 13, 
							3, 14, 15, 8, 4, 7, 1, 2, 
							172, 165, 166, 171, 169, 160, 170, 173, 
							163, 174, 175, 168, 164, 167, 161, 162, 
							220, 213, 214, 219, 217, 208, 218, 221, 
							211, 222, 223, 216, 212, 215, 209, 210, 
							60, 53, 54, 59, 57, 48, 58, 61, 
							51, 62, 63, 56, 52, 55, 49, 50, 
							236, 229, 230, 235, 233, 224, 234, 237, 
							227, 238, 239, 232, 228, 231, 225, 226, 
							252, 245, 246, 251, 249, 240, 250, 253, 
							243, 254, 255, 248, 244, 247, 241, 242, 
							140, 133, 134, 139, 137, 128, 138, 141, 
							131, 142, 143, 136, 132, 135, 129, 130, 
							76, 69, 70, 75, 73, 64, 74, 77, 
							67, 78, 79, 72, 68, 71, 65, 66, 
							124, 117, 118, 123, 121, 112, 122, 125, 
							115, 126, 127, 120, 116, 119, 113, 114, 
							28, 21, 22, 27, 25, 16, 26, 29, 
							19, 30, 31, 24, 20, 23, 17, 18, 
							44, 37, 38, 43, 41, 32, 42, 45, 
							35, 46, 47, 40, 36, 39, 33, 34
							};

const uint8_t invsBox8[]	=	{
								85, 94, 95, 88, 92, 81, 82, 93, 
								91, 84, 86, 83, 80, 87, 89, 90, 
								229, 238, 239, 232, 236, 225, 226, 237, 
								235, 228, 230, 227, 224, 231, 233, 234, 
								245, 254, 255, 248, 252, 241, 242, 253, 
								251, 244, 246, 243, 240, 247, 249, 250, 
								133, 142, 143, 136, 140, 129, 130, 141, 
								139, 132, 134, 131, 128, 135, 137, 138, 
								197, 206, 207, 200, 204, 193, 194, 205, 
								203, 196, 198, 195, 192, 199, 201, 202, 
								21, 30, 31, 24, 28, 17, 18, 29, 
								27, 20, 22, 19, 16, 23, 25, 26, 
								37, 46, 47, 40, 44, 33, 34, 45, 
								43, 36, 38, 35, 32, 39, 41, 42, 
								213, 222, 223, 216, 220, 209, 210, 221, 
								219, 212, 214, 211, 208, 215, 217, 218, 
								181, 190, 191, 184, 188, 177, 178, 189, 
								187, 180, 182, 179, 176, 183, 185, 186, 
								69, 78, 79, 72, 76, 65, 66, 77, 
								75, 68, 70, 67, 64, 71, 73, 74, 
								101, 110, 111, 104, 108, 97, 98, 109, 
								107, 100, 102, 99, 96, 103, 105, 106, 
								53, 62, 63, 56, 60, 49, 50, 61, 
								59, 52, 54, 51, 48, 55, 57, 58, 
								5, 14, 15, 8, 12, 1, 2, 13, 
								11, 4, 6, 3, 0, 7, 9, 10, 
								117, 126, 127, 120, 124, 113, 114, 125, 
								123, 116, 118, 115, 112, 119, 121, 122, 
								149, 158, 159, 152, 156, 145, 146, 157, 
								155, 148, 150, 147, 144, 151, 153, 154, 
								165, 174, 175, 168, 172, 161, 162, 173, 
								171, 164, 166, 163, 160, 167, 169, 170
								};

// Counter
	uint8_t k;
// Variables Key scheduling
	uint8_t temp;														// temporary variable
// Input values
	uint8_t key[]   = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	volatile uint8_t state[] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};		// volatile = compiler command to not delete this value while optimizing
// Counter
	uint8_t i;															// Used for subkey storage (key scheduling)
// Variables p-Layer
	uint8_t temp_pLayer[8];												// Temporary array
// Variables Key scheduling
	uint8_t round;														// Round counter
	uint8_t save1;														// Temporary variable
	uint8_t save2;														// Temporary variable
	uint8_t subkey[32][8];												// Array of precomputed Keys (32 rounds)
    volatile int eingabe;												// 1 für enc, 2 für dec (manuell setzen)
//	****************** Key scheduling **********************
	for(round=2;round<=9;round++)
	{
		subkey[0][round-2] = key[round];								// First subkey storage
	}
	for(round = 0;round <= 31;round++)
	{
		save1  = key[0];												// 61 Bit Left-shift	
		save2  = key[1];
		key[0] = key[2];
		key[1] = key[3];
		key[2] = key[4];
		key[3] = key[5];
		key[4] = key[6];
		key[5] = key[7];
		key[6] = key[8];
		key[7] = key[9];
		key[8] = save1;
		key[9] = save2;
		temp = key[0] & 7;
		key[0] = key[0] >> 3 | key[1] << 5;
		key[1] = key[1] >> 3 | key[2] << 5;
		key[2] = key[2] >> 3 | key[3] << 5;
		key[3] = key[3] >> 3 | key[4] << 5;
		key[4] = key[4] >> 3 | key[5] << 5;
		key[5] = key[5] >> 3 | key[6] << 5;
		key[6] = key[6] >> 3 | key[7] << 5;
		key[7] = key[7] >> 3 | key[8] << 5;
		key[8] = key[8] >> 3 | key[9] << 5;
		key[9] = key[9] >> 3 | temp << 5;

		key[9] = sBox4[key[9]>>4] | (key[9] & 15);						// sBox4 applied on Bits 79-76


		if((round+1) % 2 == 1)
			key[1] ^= 128;
		key[2] = ((((round+1)>>1) ^ (key[2] & 15)) | (key[2] & 240));	// Salt (XOR with roundcounter)

		for(i=2;i<=9;i++)												// Subkey storage
		{
			subkey[round+1][i-2] = key[i];
		}
	}		
//	****************** End Key Scheduling ******************
//	****************** Encryption **************************
	if(eingabe == 1)
	{
	round=0;															// Initializing counter
	do																	// round loop
	{
//	****************** addRoundkey + sBox ******************

		state[0] = sBox8[state[0] ^ subkey[round][0]];					// 8Bit sBox lookup + key XOR
		state[1] = sBox8[state[1] ^ subkey[round][1]];
		state[2] = sBox8[state[2] ^ subkey[round][2]];
		state[3] = sBox8[state[3] ^ subkey[round][3]];
		state[4] = sBox8[state[4] ^ subkey[round][4]];
		state[5] = sBox8[state[5] ^ subkey[round][5]];
		state[6] = sBox8[state[6] ^ subkey[round][6]];
		state[7] = sBox8[state[7] ^ subkey[round][7]];

//	****************** End addRoundkey *********************
//	****************** pLayer ******************************
		asm volatile("clr %0				\n"
					 "clr %1				\n"
					 "clr %2				\n"
					 "clr %3				\n"
					 "clr %4				\n"
					 "clr %5				\n"
					 "clr %6				\n"
					 "clr %7				\n"
					 "sbrc %8,0				\n"
					 "sbr  %0,1				\n"
				     "sbrc %8,1				\n"
					 "sbr  %2,1				\n"
				     "sbrc %8,2				\n"
					 "sbr  %4,1				\n"
				     "sbrc %8,3				\n"
					 "sbr  %6,1				\n"
				     "sbrc %8,4				\n"
					 "sbr  %0,2				\n"
				     "sbrc %8,5				\n"
					 "sbr  %2,2				\n"
				     "sbrc %8,6				\n"
					 "sbr  %4,2				\n"
				     "sbrc %8,7				\n"
					 "sbr  %6,2				\n"

				     "sbrc %9,0				\n"
					 "sbr  %0,4				\n"
				     "sbrc %9,1				\n"
					 "sbr  %2,4				\n"
				     "sbrc %9,2				\n"
					 "sbr  %4,4				\n"
				     "sbrc %9,3				\n"
					 "sbr  %6,4				\n"
				     "sbrc %9,4				\n"
					 "sbr  %0,8				\n"
				     "sbrc %9,5				\n"
					 "sbr  %2,8				\n"
				     "sbrc %9,6				\n"
					 "sbr  %4,8				\n"
				     "sbrc %9,7				\n"
					 "sbr  %6,8				\n"

				     "sbrc %10,0			\n"
					 "sbr  %0,16			\n"
				     "sbrc %10,1			\n"
					 "sbr  %2,16			\n"
				     "sbrc %10,2			\n"
					 "sbr  %4,16			\n"
				     "sbrc %10,3			\n"
					 "sbr  %6,16			\n"
				     "sbrc %10,4			\n"
					 "sbr  %0,32			\n"
				     "sbrc %10,5			\n"
					 "sbr  %2,32			\n"
				     "sbrc %10,6			\n"
					 "sbr  %4,32			\n"
				     "sbrc %10,7			\n"
					 "sbr  %6,32			\n"

				     "sbrc %11,0			\n"
					 "sbr  %0,64			\n"
				     "sbrc %11,1			\n"
					 "sbr  %2,64			\n"
				     "sbrc %11,2			\n"
					 "sbr  %4,64			\n"
				     "sbrc %11,3			\n"
					 "sbr  %6,64			\n"
				     "sbrc %11,4			\n"
					 "sbr  %0,128			\n"
				     "sbrc %11,5			\n"
					 "sbr  %2,128			\n"
				     "sbrc %11,6			\n"
					 "sbr  %4,128			\n"
				     "sbrc %11,7			\n"
					 "sbr  %6,128			\n"

				     "sbrc %12,0			\n"
					 "sbr  %1,1				\n"
				     "sbrc %12,1			\n"
					 "sbr  %3,1				\n"
				     "sbrc %12,2			\n"
					 "sbr  %5,1				\n"
				     "sbrc %12,3			\n"
					 "sbr  %7,1				\n"
				     "sbrc %12,4			\n"
					 "sbr  %1,2				\n"
				     "sbrc %12,5			\n"
					 "sbr  %3,2				\n"
				     "sbrc %12,6 			\n"
					 "sbr  %5,2				\n"
				     "sbrc %12,7			\n"
					 "sbr  %7,2				\n"

				     "sbrc %13,0			\n"
					 "sbr  %1,4				\n"
				     "sbrc %13,1			\n"
					 "sbr  %3,4				\n"
				     "sbrc %13,2			\n"
					 "sbr  %5,4				\n"
				     "sbrc %13,3			\n"
					 "sbr  %7,4				\n"
				     "sbrc %13,4			\n"
					 "sbr  %1,8				\n"
				     "sbrc %13,5			\n"
					 "sbr  %3,8				\n"
				     "sbrc %13,6			\n"
					 "sbr  %5,8				\n"
				     "sbrc %13,7			\n"
					 "sbr  %7,8				\n"

				     "sbrc %14,0			\n"
					 "sbr  %1,16			\n"
				     "sbrc %14,1			\n"
					 "sbr  %3,16			\n"
				     "sbrc %14,2			\n"
					 "sbr  %5,16			\n"
				     "sbrc %14,3			\n"
					 "sbr  %7,16			\n"
				     "sbrc %14,4			\n"
					 "sbr  %1,32			\n"
				     "sbrc %14,5			\n"
					 "sbr  %3,32			\n"
				     "sbrc %14,6			\n"
					 "sbr  %5,32			\n"
				     "sbrc %14,7			\n"
					 "sbr  %7,32			\n"

				     "sbrc %15,0			\n"
					 "sbr  %1,64			\n"
				     "sbrc %15,1			\n"
					 "sbr  %3,64			\n"
				     "sbrc %15,2			\n"
					 "sbr  %5,64			\n"
				     "sbrc %15,3			\n"
					 "sbr  %7,64			\n"
				     "sbrc %15,4			\n"
					 "sbr  %1,128			\n"
				     "sbrc %15,5			\n"
					 "sbr  %3,128			\n"
				     "sbrc %15,6			\n"
					 "sbr  %5,128			\n"
				     "sbrc %15,7			\n"
					 "sbr  %7,128			\n"

					:"=&r"	(temp_pLayer[0]),"=&r"	(temp_pLayer[1]),"=&r"	(temp_pLayer[2]),"=&r"	(temp_pLayer[3]),"=&r"	(temp_pLayer[4]),"=&r"	(temp_pLayer[5]),"=&r"	(temp_pLayer[6]),"=&r"	(temp_pLayer[7])
					:"r"	(state[0]),"r"	(state[1]),"r"	(state[2]),"r"	(state[3]),"r"	(state[4]),"r"	(state[5]),"r"	(state[6]),"r"	(state[7])		
		);
		state[0] = temp_pLayer[0];										// Store temporary values in state variable
		state[1] = temp_pLayer[1];
		state[2] = temp_pLayer[2];
		state[3] = temp_pLayer[3];
		state[4] = temp_pLayer[4];
		state[5] = temp_pLayer[5];
		state[6] = temp_pLayer[6];
		state[7] = temp_pLayer[7];
		round++;														// Increment roundcounter
//	****************** End pLayer **************************
	}
	while(round<31);													// End round loop
//	****************** addRoundkey *************************
	state[0] ^= subkey[round][0];										// Final key XOR
	state[1] ^= subkey[round][1];
	state[2] ^= subkey[round][2];
	state[3] ^= subkey[round][3];
	state[4] ^= subkey[round][4];
	state[5] ^= subkey[round][5];
	state[6] ^= subkey[round][6];
	state[7] ^= subkey[round][7];
//	****************** End addRoundkey *********************
	}
//	****************** End Encryption **********************
//	****************** Decryption **************************
	else if(eingabe == 2)
	{
	i=0;																		// Initializing counter
	k=31;																		// Initializing key index
	state[0] ^= subkey[k][0];													// Initial key XOR
	state[1] ^= subkey[k][1];													// Initial key XOR
	state[2] ^= subkey[k][2];													// Initial key XOR
	state[3] ^= subkey[k][3];													// Initial key XOR
	state[4] ^= subkey[k][4];													// Initial key XOR
	state[5] ^= subkey[k][5];													// Initial key XOR
	state[6] ^= subkey[k][6];													// Initial key XOR
	state[7] ^= subkey[k][7];													// Initial key XOR
	do
	{
		k--;																	// Decrement key index
//	****************** pLayer ******************************
		asm volatile("clr %0				\n"
					 "clr %1				\n"
					 "clr %2				\n"
					 "clr %3				\n"
					 "clr %4				\n"
					 "clr %5				\n"
					 "clr %6				\n"
					 "clr %7				\n"
					 "sbrc %8,0				\n"
					 "sbr  %0,1				\n"
				     "sbrc %8,1				\n"
					 "sbr  %0,16			\n"
				     "sbrc %8,2				\n"
					 "sbr  %1,1				\n"
				     "sbrc %8,3				\n"
					 "sbr  %1,16			\n"
				     "sbrc %8,4				\n"
					 "sbr  %2,1				\n"
				     "sbrc %8,5				\n"
					 "sbr  %2,16			\n"
				     "sbrc %8,6				\n"
					 "sbr  %3,1				\n"
				     "sbrc %8,7				\n"
					 "sbr  %3,16			\n"

				     "sbrc %9,0				\n"
					 "sbr  %4,1				\n"
				     "sbrc %9,1				\n"
					 "sbr  %4,16			\n"
				     "sbrc %9,2				\n"
					 "sbr  %5,1				\n"
				     "sbrc %9,3				\n"
					 "sbr  %5,16			\n"
				     "sbrc %9,4				\n"
					 "sbr  %6,1				\n"
				     "sbrc %9,5				\n"
					 "sbr  %6,16			\n"
				     "sbrc %9,6				\n"
					 "sbr  %7,1				\n"
				     "sbrc %9,7				\n"
					 "sbr  %7,16			\n"

				     "sbrc %10,0			\n"
					 "sbr  %0,2				\n"
				     "sbrc %10,1			\n"
					 "sbr  %0,32			\n"
				     "sbrc %10,2			\n"
					 "sbr  %1,2				\n"
				     "sbrc %10,3			\n"
					 "sbr  %1,32			\n"
				     "sbrc %10,4			\n"
					 "sbr  %2,2				\n"
				     "sbrc %10,5			\n"
					 "sbr  %2,32			\n"
				     "sbrc %10,6			\n"
					 "sbr  %3,2				\n"
				     "sbrc %10,7			\n"
					 "sbr  %3,32			\n"

				     "sbrc %11,0			\n"
					 "sbr  %4,2				\n"
				     "sbrc %11,1			\n"
					 "sbr  %4,32			\n"
				     "sbrc %11,2			\n"
					 "sbr  %5,2				\n"
				     "sbrc %11,3			\n"
					 "sbr  %5,32			\n"
				     "sbrc %11,4			\n"
					 "sbr  %6,2				\n"
				     "sbrc %11,5			\n"
					 "sbr  %6,32			\n"
				     "sbrc %11,6			\n"
					 "sbr  %7,2				\n"
				     "sbrc %11,7			\n"
					 "sbr  %7,32			\n"

				     "sbrc %12,0			\n"
					 "sbr  %0,4				\n"
				     "sbrc %12,1			\n"
					 "sbr  %0,64			\n"
				     "sbrc %12,2			\n"
					 "sbr  %1,4				\n"
				     "sbrc %12,3			\n"
					 "sbr  %1,64			\n"
				     "sbrc %12,4			\n"
					 "sbr  %2,4				\n"
				     "sbrc %12,5			\n"
					 "sbr  %2,64			\n"
				     "sbrc %12,6 			\n"
					 "sbr  %3,4				\n"
				     "sbrc %12,7			\n"
					 "sbr  %3,64			\n"

				     "sbrc %13,0			\n"
					 "sbr  %4,4				\n"
				     "sbrc %13,1			\n"
					 "sbr  %4,64			\n"
				     "sbrc %13,2			\n"
					 "sbr  %5,4				\n"
				     "sbrc %13,3			\n"
					 "sbr  %5,64			\n"
				     "sbrc %13,4			\n"
					 "sbr  %6,4				\n"
				     "sbrc %13,5			\n"
					 "sbr  %6,64			\n"
				     "sbrc %13,6			\n"
					 "sbr  %7,4				\n"
				     "sbrc %13,7			\n"
					 "sbr  %7,64			\n"

				     "sbrc %14,0			\n"
					 "sbr  %0,8				\n"
				     "sbrc %14,1			\n"
					 "sbr  %0,128			\n"
				     "sbrc %14,2			\n"
					 "sbr  %1,8				\n"
				     "sbrc %14,3			\n"
					 "sbr  %1,128			\n"
				     "sbrc %14,4			\n"
					 "sbr  %2,8				\n"
				     "sbrc %14,5			\n"
					 "sbr  %2,128			\n"
				     "sbrc %14,6			\n"
					 "sbr  %3,8				\n"
				     "sbrc %14,7			\n"
					 "sbr  %3,128			\n"

				     "sbrc %15,0			\n"
					 "sbr  %4,8				\n"
				     "sbrc %15,1			\n"
					 "sbr  %4,128			\n"
				     "sbrc %15,2			\n"
					 "sbr  %5,8				\n"
				     "sbrc %15,3			\n"
					 "sbr  %5,128			\n"
				     "sbrc %15,4			\n"
					 "sbr  %6,8				\n"
				     "sbrc %15,5			\n"
					 "sbr  %6,128			\n"
				     "sbrc %15,6			\n"
					 "sbr  %7,8				\n"
				     "sbrc %15,7			\n"
					 "sbr  %7,128			\n"

					:"=&r"	(temp_pLayer[0]),"=&r"	(temp_pLayer[1]),"=&r"	(temp_pLayer[2]),"=&r"	(temp_pLayer[3]),"=&r"	(temp_pLayer[4]),"=&r"	(temp_pLayer[5]),"=&r"	(temp_pLayer[6]),"=&r"	(temp_pLayer[7])
					:"r"	(state[0]),"r"	(state[1]),"r"	(state[2]),"r"	(state[3]),"r"	(state[4]),"r"	(state[5]),"r"	(state[6]),"r"	(state[7])		
		);
		state[0] = temp_pLayer[0];												// Store temporary values in state state variable
		state[1] = temp_pLayer[1];												// Store temporary values in state state variable
		state[2] = temp_pLayer[2];												// Store temporary values in state state variable
		state[3] = temp_pLayer[3];												// Store temporary values in state state variable
		state[4] = temp_pLayer[4];												// Store temporary values in state state variable
		state[5] = temp_pLayer[5];												// Store temporary values in state state variable
		state[6] = temp_pLayer[6];												// Store temporary values in state state variable
		state[7] = temp_pLayer[7];												// Store temporary values in state state variable
//	****************** End pLayer **************************
		state[0] = invsBox8[state[0]] ^ subkey[k][0];						// 8Bit sBox lookup + key XOR							
		state[1] = invsBox8[state[1]] ^ subkey[k][1];						// 8Bit sBox lookup + key XOR
		state[2] = invsBox8[state[2]] ^ subkey[k][2];						// 8Bit sBox lookup + key XOR
		state[3] = invsBox8[state[3]] ^ subkey[k][3];						// 8Bit sBox lookup + key XOR
		state[4] = invsBox8[state[4]] ^ subkey[k][4];						// 8Bit sBox lookup + key XOR
		state[5] = invsBox8[state[5]] ^ subkey[k][5];						// 8Bit sBox lookup + key XOR
		state[6] = invsBox8[state[6]] ^ subkey[k][6];						// 8Bit sBox lookup + key XOR
		state[7] = invsBox8[state[7]] ^ subkey[k][7];						// 8Bit sBox lookup + key XOR

//	****************** End addRoundkey *********************
		i++;																// Increment roundcounter
	}
	while(i<31);															// End round loop
	}
	return 0;		
//	****************** End Decryption **********************
}
