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

#ifndef FESDECODE_H
#define FESDECODE_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "feslanguage.h"

int fl_decode(fl_reply* reply);
void fl_inspect(fl_reply* reply);

int fl_decode_status(fl_reply* reply, fl_tbyte* locked, fl_tlarge* angle, fl_tbyte* active);

#ifdef __cplusplus
}
#endif

#endif
