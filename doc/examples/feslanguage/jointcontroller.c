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
	ssize_t bytes;
	float pw, dpw;
	int status;
	
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
	

	pw = 0.1f;
	dpw = 0.10f;

	while(1) {
		fl_activate(&message, 5, 20, pw);
		bytes = sp_writem(&tty, &message);
		status = sp_waitr(&tty, &reply);
		//sp_display();

		fl_readstatus(&message);
		sp_writem(&tty, &message);
		sp_waitr(&tty, &reply);
		fl_decode_status(&reply, &locked, &angle, &active);
		
		printf("Status: %d, %d, %d PW=%f",
 			locked, (int)angle, active, pw);


		if(angle > 400) {
			printf("---\n");
			pw -= dpw;
		} else {
			printf("+++\n");
			pw += dpw;
		}
		
		if(pw > 1)
			pw = 1;
		if(pw < 0)
			pw = 0;
	}

	sp_close(&tty);
	
	return 0;
}
