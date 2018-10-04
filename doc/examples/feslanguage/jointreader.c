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

#include "serialport.h"
#include "feslanguage.h"
#include "fesencode.h"
#include "fesdecode.h"
#include <stdio.h>
#include <unistd.h>

int main(void) {
	int i;
	
	fl_tbyte locked = 0;
	fl_tlarge angle = 0;
	fl_tbyte active = 0;

	sp_tty tty;
	sp_init(&tty);
	if(sp_open(&tty, "/dev/ttyUSB0", 1) <= 0) {
		printf("Cannot open device\n");
		return 1;
	}
	
	fl_message message;
	fl_reply reply;
	
	for(i = 0; i < 1000; i++) {
		fl_readstatus(&message);
		sp_writem(&tty, &message);
		sp_waitr(&tty, &reply);
		fl_decode_status(&reply, &locked, &angle, &active);
		fl_inspect(&reply);
		printf("Status: %d, %d, %d\n",
 			locked, (int)angle, active);
		sp_display();
	}

	sp_close(&tty);
	
	return 0;
}
