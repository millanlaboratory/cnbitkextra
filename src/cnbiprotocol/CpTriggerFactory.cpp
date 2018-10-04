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

#ifndef CCTRIGGERFACTORY_CPP
#define CCTRIGGERFACTORY_CPP

#include "CpTriggerFactory.hpp"
#include <cnbicore/CcBasic.hpp>
#include <stdio.h>

CpTrigger* CpTriggerFactory::Find(CCfgConfig* configuration) {
	CpTrigger* trigger = NULL;
	int forcetype = 4; // Auto-detect by default

	if(configuration != NULL) {
		try {
			forcetype = 
				configuration->RootEx()->QuickIntEx("configuration/cptriggerforce");
		} catch(XMLException e) {
			CcLogErrorS("Configuration failed, forcing auto-detect: " << e.Info());
			forcetype = 4;
		}
	} else {
		CcLogConfig("Configuration unknown, forcing auto-detect");
		forcetype = 4;
	}


	switch(forcetype) {
		case 1: {
			try {
				if(configuration != NULL) {
					trigger = new CpTriggerLPT(configuration);
				} else {
					trigger = new CpTriggerLPT();
				}
				trigger->Open();
				CcLogConfig("LPT trigger capabilities selected and detected");
			} catch(CcException e) {
				CcLogErrorS("LPT trigger capabilities selected but could not be detected: " << e.Info());
				delete trigger;
				trigger = NULL;
			}
			break;		
		}
		case 2: {
			try {
				if(configuration != NULL) {
					trigger = new CpTriggerSerial(configuration);
				} else {
					trigger = new CpTriggerSerial();
				}
				trigger->Open();
				CcLogConfig("Serial trigger capabilities selected and detected");
			} catch(CcException e) {
				CcLogErrorS("Serial trigger capabilities selected but could not be detected: " << e.Info());
				delete trigger;
				trigger = NULL;
			}
			break;		
		}
		case 3: {
			try {
				trigger = new CpTriggerBeep();
				trigger->Open();
				CcLogConfig("Beep trigger capabilities selected and detected");
			} catch(CcException e) {
				CcLogErrorS("Beep trigger capabilities selected but could not be detected: " << e.Info());
				delete trigger;
				trigger = NULL;
			}
			break;		
		}
		default : {
			try {
				CcLogConfig("Auto-detecting LPT trigger.");
				if(configuration != NULL) {
					trigger = new CpTriggerLPT(configuration);
				} else {
					trigger = new CpTriggerLPT();
				}
				trigger->Open();
				CcLogConfig("LPT trigger capabilities detected");
			} catch(CcException e) {
				CcLogErrorS("LPT trigger capabilities could not be detected: " << e.Info());
				try {
					CcLogConfig("Auto-detecting serial trigger.");
					if(configuration != NULL) {
						trigger = new CpTriggerSerial(configuration);
					} else {
						trigger = new CpTriggerSerial();
					}
					trigger->Open();
					CcLogConfig("Serial trigger capabilities detected");					
				} catch(CcException e) {
					CcLogErrorS("Serial trigger capabilities could not be detected: " << e.Info());
					try {
						CcLogConfig("Auto-detecting beep trigger.");
						trigger = new CpTriggerBeep();
						trigger->Open();
						CcLogConfig("Beep trigger capabilities detected");
					} catch(CcException e) {
						CcLogErrorS("Beep trigger capabilities could not be detected: " << e.Info());
						delete trigger;
						trigger = NULL;
						CcLogConfig("Could not detect any type of hardware trigger!");
					}
				}
			}
			break;
		}
	}
	return trigger;
}

#endif
