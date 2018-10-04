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

#ifndef FESENCODE_H
#define FESENCODE_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "feslanguage.h"

void fl_dump(fl_message* message);

/* Sounds, useful for testing the communication */
fl_message* fl_play1(fl_message* message);
fl_message* fl_play2(fl_message* message);

/* Stimulation */
fl_message* fl_activate(fl_message* message, fl_tbyte channel, fl_tbyte current, 
		double pw);
fl_message* fl_deactivate(fl_message* message, fl_tbyte chbf, fl_tbyte hard);
fl_message* fl_freeze(fl_message* message, fl_tbyte chbf);

/* Orthosis */
fl_message* fl_lock(fl_message* message);
fl_message* fl_unlock(fl_message* message);
fl_message* fl_togglelock(fl_message* message);
fl_message* fl_togglehandarm(fl_message* message);
fl_message* fl_togglepause(fl_message* message);
fl_message* fl_emergency(fl_message* message);
fl_message* fl_init(fl_message* message);
fl_message* fl_reset(fl_message* message);
fl_message* fl_useadin(fl_message* message, fl_tbyte adin, fl_tbyte channel);
fl_message* fl_useanin(fl_message* message, fl_tbyte chbf, fl_tlarge value);
fl_message* fl_setfrequency(fl_message* message, fl_tbyte f);
fl_message* fl_usepresaved(fl_message* message);

/* Requests */
fl_message* fl_readchannel(fl_message* message, fl_tbyte channel);
fl_message* fl_readjoystick(fl_message* message);
fl_message* fl_readactive(fl_message* messagechannel);
fl_message* fl_readcurrent(fl_message* message);
fl_message* fl_readpulsewidth(fl_message* message);
fl_message* fl_readfrequency(fl_message* message);
fl_message* fl_readstatus(fl_message* message);
fl_message* fl_readpause(fl_message* message);
fl_message* fl_readcalibration(fl_message* message);

#ifdef __cplusplus
}
#endif

#endif
