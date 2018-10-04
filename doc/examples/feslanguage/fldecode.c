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
#include "fesdecode.h"
#include "feslanguage.h"

int main(void) {

	char hex[17] = "0x012345678abcdef";
	unsigned char field[15];
	int i = 0;
	for(i = 2; i < 17; i++) {
		char c = hex[i];
		char s[3];
		s[0] = '0';
		s[1] = 'x';
		s[2] = c;
		unsigned int v = 0;
		sscanf(s, "%x", &v);
		field[i-2] = v;
	}
	for(i = 0; i < 15; i++) {
		printf("%u\n", field[i]);
	}
	return 0;
}
