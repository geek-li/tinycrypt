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

#include"Encryption_8bit.inc"

void main(void)
{
//  Input values
	unsigned __int64 keyhigh=0x0;
	unsigned __int64 keylow=0x0;
	unsigned __int64 state=0x0;
//  Variables key scheduling
	unsigned __int64 subkey[32];
	unsigned __int64 temp;
//  Variables pLayer
	unsigned __int64 temp_0;
	unsigned __int64 temp_1;
	unsigned __int64 temp_2;
	unsigned __int64 temp_3;
	unsigned __int64 temp_4;
	unsigned __int64 temp_5;
	unsigned __int64 temp_6;
	unsigned __int64 temp_7;
//	****************** Key Scheduling **********************
	for(int round=0;round<32;round++)
	{
		subkey[round] = keyhigh;								//61-bit left shift
		temp = keyhigh;
		keyhigh <<= 61;
		keyhigh |= (keylow<<45);
		keyhigh |= (temp>>19);
		keylow = (temp>>3)&0xFFFF;

		temp = keyhigh>>60;										//S-Box application
		keyhigh &=	0x0FFFFFFFFFFFFFFF;
		temp = sBox4[temp];
		keyhigh |= temp;

		keylow ^= ( ( (round+1) & 0x01 ) << 15 );				//round counter addition
		keyhigh ^= ( (round+1) >> 1 );
	}
//	****************** End Key Scheduling ******************
//	****************** Encryption **************************
	for(int round=0;round<31;round++)
	{
//	****************** addRoundkey *************************
		state ^= subkey[round];
//	****************** End addRoundkey *********************
//	 ****************** 8Bit pLayer + sBox *****************
			temp_1 = state;
			temp_2 = state;
			temp_3 = state;
			temp_4 = state;
			temp_5 = state;
			temp_6 = state;
			temp_7 = state;

			temp_0 = pBox8_0[state&0xFF];						//store new value into temp variable
	
			temp_1 = (temp_1 >> 8);								//shift 1 byte
			temp_1 = pBox8_1[temp_1&0xFF];						//store new value into temp variable
		
			temp_2 = (temp_2 >> 16);							//shift 2 byte
			temp_2 = pBox8_2[temp_2&0xFF];						//store new value into temp variable
	
			temp_3 = (temp_3 >> 24);							//shift 3 byte
			temp_3 = pBox8_3[temp_3&0xFF];						//store new value into temp variable

			temp_4 = (temp_4 >> 32);							//shift 4 byte
			temp_4 = pBox8_4[temp_4&0xFF];						//store new value into temp variable
	
			temp_5 = (temp_5 >> 40);							//shift 5 byte
			temp_5 = pBox8_5[temp_5&0xFF];						//store new value into temp variable
		
			temp_6 = (temp_6 >> 48);							//shift 6 byte
			temp_6 = pBox8_6[temp_6&0xFF];						//store new value into temp variable
	
			temp_7 = (temp_7 >> 56);							//shift 7 byte
			temp_7 = pBox8_7[temp_7&0xFF];						//store new value into temp variable

		state=temp_0|temp_1|temp_2|temp_3|temp_4|temp_5|temp_6|temp_7;	// XOR of the results
//	****************** End pLayer **************************
	}
//	****************** addRoundkey *************************
	state ^= subkey[31];
//	****************** End addRoundkey *********************
}
//	****************** End Encryption **********************

