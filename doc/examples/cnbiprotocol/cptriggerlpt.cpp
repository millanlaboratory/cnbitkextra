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

#define EXAMPLE_NAME "[triggerlpt]"

#include "CpTriggerLPT.hpp"
#include <cnbicore/CcThread.hpp>
#include <cnbicore/CcTime.hpp>
#include <cnbicore/CcException.hpp>
#include <stdio.h>
#include <math.h>
#include <signal.h>

int main(void) {
	CpTriggerLPT *trigger = new CpTriggerLPT(-1, 15, 0);
	trigger->Open();
	
	printf("%s Testing pins\n", EXAMPLE_NAME);
	for(int i = 0; i <= 8; i++) {
		int value = (int)pow(2, i);
		printf(" %d", value);
		trigger->Send(value);
		fflush(stdout);
		CcTime::Sleep(500);
	}
	printf("\n");

	printf("%s Sending all values [0,255]\n", EXAMPLE_NAME);
	for(int i = 0; i < 256; i++) {
		trigger->Send(i);
		CcTime::Sleep(25);
	}
	CcTime::Sleep(1000);
	
	delete trigger;

	printf("%s Bye bye\n", EXAMPLE_NAME);
	return 0;
}
