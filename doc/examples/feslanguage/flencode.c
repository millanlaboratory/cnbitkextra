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

#include "fesencode.h"

int main(void) {
	fl_message message;
	
	printf("Play1\n");
	fl_play1(&message);
	fl_dump(&message);
	
	printf("Play2\n");
	fl_play2(&message);
	fl_dump(&message);
	
	printf("Activate channel\n");
	fl_activate(&message, 1, 20, 1.00f);
	fl_dump(&message);

	printf("Deactivate channel\n");
	fl_deactivate(&message, FESMESSAGE_CH1|FESMESSAGE_CH2, 1);
	fl_dump(&message);

	fl_lock(&message);
	fl_unlock(&message);
	fl_init(&message);
	fl_reset(&message);
	fl_activate(&message, 1, 15, 1);
	fl_activate(&message, 2, 15, 1);
	fl_activate(&message, 3, 15, 1);
	fl_activate(&message, 4, 15, 1);
	fl_activate(&message, 5, 15, 1);
	fl_activate(&message, 6, 15, 1);
	fl_activate(&message, 7, 15, 1);
	fl_activate(&message, 8, 15, 1);
	fl_deactivate(&message, FESMESSAGE_CH8, 0);
	fl_useadin(&message, 5, FESMESSAGE_CH8);
	fl_freeze(&message, FESMESSAGE_CH8);
	fl_setfrequency(&message, 40);
	fl_usepresaved(&message);
	fl_togglepause(&message);
	
	fl_readchannel(&message, 1);
	fl_readjoystick(&message);
	fl_readactive(&message);
	fl_readcurrent(&message);
	fl_readpulsewidth(&message);
	fl_readfrequency(&message);
	fl_readpause(&message);
	fl_readcalibration(&message);
	fl_readstatus(&message);
	fl_togglepause(&message);
	fl_useanin(&message, 2| FESMESSAGE_CH2, 1023);

	return 0;
}
