/*
    Copyright (C) 2009-2011  EPFL (Ecole Polytechnique Fédérale de Lausanne)
    Michele Tavella <michele.tavella@epfl.ch>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FESLANGUAGE_H
#define FESLANGUAGE_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdint.h>

/* Bitfields for channel requests */
#define FESMESSAGE_CH1 			128
#define FESMESSAGE_CH2 			64
#define FESMESSAGE_CH3 			32
#define FESMESSAGE_CH4 			16
#define FESMESSAGE_CH5 			8
#define FESMESSAGE_CH6 			4
#define FESMESSAGE_CH7 			2
#define FESMESSAGE_CH8 			1

/* Size of buffer for writing/reading 
 * messages over the serial port 
 */
#define FESREPLY_BSIZE		512
#define FESMESSAGE_FSIZE		20

/* Motionstim firmware types */
typedef unsigned char fl_tbyte;			// 1 byte
typedef unsigned short int fl_tsmall;	// 2 bytes
typedef unsigned int fl_tlarge;			// 4 byte 

/* Simple message structure */
typedef struct fl_message_struct {
	fl_tsmall cid;
	fl_tbyte buffer[FESREPLY_BSIZE];
	fl_tbyte bytes;
} fl_message;

/* Reply structures 
 * - <sof>F0F0F0F0:dl:01:cid:C9:data:66:chk:00000130</sof>
 */
typedef struct fl_reply_struct {
	fl_tlarge sof;
	fl_tbyte  dl;
	fl_tsmall cid;
	fl_tsmall data;
	fl_tlarge cs;
	fl_tbyte buffer[FESREPLY_BSIZE];
	fl_tbyte bytefield[FESMESSAGE_FSIZE];
	uint32_t rawdata;
} fl_reply;

/* Stimulation instructions */
#define FESMESSAGE_ACTIVATECHANNEL				103
#define FESMESSAGE_DEACTIVATECHANNEL			105
#define FESMESSAGE_FREEZEPULSEWIDTHS			104
#define FESMESSAGE_PLAYSOUND1					120
#define FESMESSAGE_PLAYSOUND2					121
#define FESMESSAGE_SETFREQUENCY					106
#define FESMESSAGE_USEADIN						101
#define FESMESSAGE_USEANIN						102
#define FESMESSAGE_USEPRESAVED					107

/* Elbow Orthosis instructions */
#define FESMESSAGE_TOGGLELOCK	              	140
#define FESMESSAGE_LOCKORTHOSIS		      		141
#define FESMESSAGE_UNLOCKORTHOSIS		      	142
#define FESMESSAGE_TOGGLEHANDARM		      	143
#define FESMESSAGE_TOGGLEPAUSE              	144
#define FESMESSAGE_UNLOCKORTHOSISEMERGENCY 		145
#define FESMESSAGE_INIT							146
#define FESMESSAGE_RESET						147

#define FESREQUEST_CHANNEL						1
#define FESREQUEST_JOYSTICK						2
#define FESREQUEST_ACTIVE						3
#define FESREQUEST_CURRENT						4
#define FESREQUEST_PULSEWIDTH					5	
#define FESREQUEST_FREQUENCY					6	
#define FESREQUEST_STATUS						7
#define FESREQUEST_PAUSE						8
#define FESREQUEST_CALIBRATION					9

/* Reply instructions  */
#define FESREPLY_SENDAGAIN						200
#define FESREPLY_RECVOK							201
#define FESREPLY_REJECTED						202
#define FESREPLY_HANDMODE						203
#define FESREPLY_ARMMODE						204
#define FESREPLY_LOCKED							205
#define FESREPLY_UNLOCKED						206

/* Protocol related stuff */
#define FESPROTOCOL_SOF							240

#ifdef __cplusplus
}
#endif

#endif

