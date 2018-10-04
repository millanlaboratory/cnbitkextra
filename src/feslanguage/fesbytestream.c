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

#define FESMESSAGE_HSIZE		10

void fl_clear(fl_message* message) {
	memset(message, 0, sizeof(fl_message));
}

void fl_write_sof(fl_message* message) {
	message->buffer[FESMESSAGE_ISOF_00] = (fl_tbyte)FESPROTOCOL_SOF;
	message->buffer[FESMESSAGE_ISOF_01] = (fl_tbyte)FESPROTOCOL_SOF;
	message->buffer[FESMESSAGE_ISOF_02] = (fl_tbyte)FESPROTOCOL_SOF;
	message->buffer[FESMESSAGE_ISOF_03] = (fl_tbyte)FESPROTOCOL_SOF;
}

void fl_write_dl(fl_message* message, fl_tbyte lenght) {
	message->buffer[FESMESSAGE_IDL] = lenght;
	message->bytes = FESMESSAGE_HSIZE + lenght;
}

void fl_write_cid(fl_message* message, fl_tbyte id) {
	message->buffer[FESMESSAGE_ICID] = id;
	message->cid = id;
}

void fl_write_cs(fl_message* message) {
	fl_tlarge checksum = 0;
	int i;

	for(i = FESMESSAGE_IDL; i < 6 + message->buffer[FESMESSAGE_IDL]; i++)
		checksum += message->buffer[i];

	fl_tbyte cs[4];
	memcpy(cs, &checksum, 4*sizeof(fl_tbyte));
	message->buffer[message->bytes - 4] = (fl_tbyte)cs[3];
	message->buffer[message->bytes - 3] = (fl_tbyte)cs[2];
	message->buffer[message->bytes - 2] = (fl_tbyte)cs[1];
	message->buffer[message->bytes - 1] = (fl_tbyte)cs[0];
}
