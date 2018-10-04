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

#include "fesbytestream.h"
#include "fesencode.h"

#define FESMESSAGE_PW_MIN		0
#define FESMESSAGE_PW_MAX		500

void fl_dump(fl_message* message) {
	int i;
	printf("[fl_dump] Dumping %d chars:\n", message->bytes);
	for(i = 0; i < message->bytes; i++)
		printf("  %.3d:  %hhu\n", i, message->buffer[i]);
}

fl_message* fl_play1(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_PLAYSOUND1);
	fl_write_cs(message);
	return message;
}

fl_message* fl_play2(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_PLAYSOUND2);
	fl_write_cs(message);
	return message;
}


fl_message* fl_activate(fl_message* message, 
		fl_tbyte channel, fl_tbyte current, double pw) {
	fl_tsmall pulsewidth;
	
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)4);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_ACTIVATECHANNEL);
	
	message->buffer[6] = channel;
	message->buffer[7] = current;

	/* 2009-11-19  Michele Tavella <michele.tavella@epfl.ch>
	 * Oki, doggie code here... But I'm rushing, so I assume 
	 * I'm excused :-)
	 */
	pulsewidth = (fl_tsmall)(pw * FESMESSAGE_PW_MAX);
	if(pulsewidth < FESMESSAGE_PW_MIN)
		pulsewidth = FESMESSAGE_PW_MIN;
	if(pulsewidth > FESMESSAGE_PW_MAX)
		pulsewidth = FESMESSAGE_PW_MAX;

	fl_tbyte tpulsewidth[2];
	memcpy(tpulsewidth, &pulsewidth, 2);
	message->buffer[8] = (fl_tbyte)tpulsewidth[1];
	message->buffer[9] = (fl_tbyte)tpulsewidth[0];
	
	fl_write_cs(message);

	return message;
}

fl_message* fl_deactivate(fl_message* message, fl_tbyte chbf, fl_tbyte hard) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)2);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_DEACTIVATECHANNEL);
	
	message->buffer[6] = chbf;
	message->buffer[7] = hard;

	fl_write_cs(message);

	return message;
}


fl_message* fl_togglelock(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_TOGGLELOCK);
	fl_write_cs(message);
	return message;
}

fl_message* fl_lock(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_LOCKORTHOSIS);
	fl_write_cs(message);
	return message;
}

fl_message* fl_unlock(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_UNLOCKORTHOSIS);
	fl_write_cs(message);
	return message;
}

fl_message* fl_togglehandarm(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)1);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_TOGGLEHANDARM);
	message->buffer[6] = 0;
	fl_write_cs(message);
	return message;
}

fl_message* fl_togglepause(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_TOGGLEPAUSE);
	fl_write_cs(message);
	return message;
}

fl_message* fl_emergency(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_UNLOCKORTHOSISEMERGENCY);
	fl_write_cs(message);
	return message;
}

fl_message* fl_init(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_INIT);
	fl_write_cs(message);
	return message;
}

fl_message* fl_reset(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_RESET);
	fl_write_cs(message);
	return message;
}

fl_message* fl_useadin(fl_message* message, fl_tbyte adin, fl_tbyte channel) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)2);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_USEADIN);
	message->buffer[6] = adin;
	message->buffer[7] = channel;
	fl_write_cs(message);
	return message;
}

fl_message* fl_freeze(fl_message* message, fl_tbyte chbf) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)1);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_FREEZEPULSEWIDTHS);
	message->buffer[6] = chbf;
	fl_write_cs(message);
	return message;
}

// Value max 1023
fl_message* fl_useanin(fl_message* message, fl_tbyte chbf, fl_tlarge value) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)3);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_USEANIN);

	fl_tbyte tvalue[2];
	memcpy(tvalue, &value, 2);
	message->buffer[6] = (fl_tbyte)tvalue[1];
	message->buffer[7] = (fl_tbyte)tvalue[0];
	message->buffer[8] = chbf;

	fl_write_cs(message);
	return message;
}

fl_message* fl_setfrequency(fl_message* message, fl_tbyte f) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)2);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_SETFREQUENCY);

	message->buffer[6] = f;

	fl_write_cs(message);
	return message;
}

fl_message* fl_usepresaved(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESMESSAGE_USEPRESAVED);
	fl_write_cs(message);
	return message;
}

fl_message* fl_readchannel(fl_message* message, fl_tbyte channel) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)1);
	fl_write_cid(message, (fl_tbyte)FESREQUEST_CHANNEL);
	message->buffer[6] = channel;
	fl_write_cs(message);
	return message;
}
fl_message* fl_readjoystick(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESREQUEST_JOYSTICK);
	fl_write_cs(message);
	return message;
}
fl_message* fl_readactive(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESREQUEST_ACTIVE);
	fl_write_cs(message);
	return message;
}
fl_message* fl_readcurrent(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESREQUEST_CURRENT);
	fl_write_cs(message);
	return message;
}
fl_message* fl_readpulsewidth(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESREQUEST_PULSEWIDTH);
	fl_write_cs(message);
	return message;
}
fl_message* fl_readfrequency(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESREQUEST_FREQUENCY);
	fl_write_cs(message);
	return message;
}
fl_message* fl_readstatus(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESREQUEST_STATUS);
	fl_write_cs(message);
	return message;
}
fl_message* fl_readpause(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESREQUEST_PAUSE);
	fl_write_cs(message);
	return message;
}
fl_message* fl_readcalibration(fl_message* message) {
	fl_clear(message);
	fl_write_sof(message);
	fl_write_dl(message,  (fl_tbyte)0);
	fl_write_cid(message, (fl_tbyte)FESREQUEST_CALIBRATION);
	fl_write_cs(message);
	return message;
}
