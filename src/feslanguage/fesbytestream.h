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

#ifndef FESBYTESTREAM_H
#define FESBYTESTREAM_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <string.h>
#include "feslanguage.h"

#define FESMESSAGE_ISOF_00		0	
#define FESMESSAGE_ISOF_01		1	
#define FESMESSAGE_ISOF_02		2	
#define FESMESSAGE_ISOF_03		3	
#define FESMESSAGE_IDL			4
#define FESMESSAGE_ICID			5

void fl_clear(fl_message* message);

void fl_write_sof(fl_message* message);
void fl_write_dl(fl_message* message, fl_tbyte lenght);
void fl_write_cid(fl_message* message, fl_tbyte id);
void fl_write_cs(fl_message* message);

#ifdef __cplusplus
}
#endif

#endif
