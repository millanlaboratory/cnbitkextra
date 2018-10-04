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

#include "CpTriggerLPT.hpp"
#include "CpTriggerSerial.hpp"
#include "CpTriggerBeep.hpp"
#include <cnbicore/CcCore.hpp>
#include <cnbicore/CcException.hpp>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main(void) {
	printf("[cp_triggersend] Allocating CcTrigger objects\n");
	CpTriggerLPT* lpt = new CpTriggerLPT(-1, 15, 0);
	CpTriggerSerial* ser = new CpTriggerSerial("/dev/ttyACM0");
	CpTriggerBeep* beep = new CpTriggerBeep();

	try {
		lpt->Open();
	} catch(CcException e) {
		cout << e << endl;
		delete lpt;
		lpt = NULL;
	}

	try {
		ser->Open();
	} catch(CcException e) {
		cout << e << endl;
		delete ser;
		ser = NULL;
	}

	try {
		beep->Open();
	} catch(CcException e) {
		cout << e << endl;
		delete beep;
		beep = NULL;
	}

	printf("[cp_triggersend] Entering main loop\n");
	string input;
	while(true) {
		printf(">> ");
		getline(cin, input);
	
		if(input.compare("quit") == 0)
			break;
		if(input.compare("q") == 0)
			break;
		
		if(lpt)
			lpt->Send(1);
		if(ser)
			ser->Send(1);
		if(beep)
			beep->Send();
	}
	
	if(lpt)
		delete lpt;
	if(ser)
		delete ser;
	if(beep)
		delete beep;
	return 0;
}
