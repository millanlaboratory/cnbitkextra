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
	fl_tbyte ch, a;
	float pw;
	int status;
	

	sp_tty tty;
	sp_init(&tty);
	if(sp_open(&tty, "/dev/ttyUSB0", 1) <= 0) {
		printf("Cannot open device\n");
		return 1;
	}
	
	fl_message message;
	fl_reply reply;
	
	for(a = 1; a <= 15; a++) {
		for(ch = 1; ch <= 8; ch++) {
			for(pw = 0.1; pw <= 1; pw += 0.25) {
				fl_activate(&message, ch, a, pw);
				bytes = sp_writem(&tty, &message);
				status = sp_waitr(&tty, &reply);
				printf("Activete: Ch=%u, A=%u, PW=%f, Status=%d\n", 
						ch, a, pw, status);

				if(status != FESREPLY_RECVOK) {
					printf("Error: failed to send message\n");
					break;
				}
				sp_display();
			}
		}
	}

	sp_close(&tty);
	
	return 0;
}
