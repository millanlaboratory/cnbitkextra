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

#ifndef SERIALPORT_H
#define SERIALPORT_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "feslanguage.h"
#include <termios.h>
#include <string.h>
#include <unistd.h>

typedef struct sp_options_struct {
	long baud;
	long databits;
	long stopbits;
	long parity;
} sp_options;

typedef struct sp_tty_struct {
	int fd;
	struct termios termios_old;
	struct termios termios_new;
	sp_options options;
} sp_tty;

void sp_init(sp_tty* device);
int sp_open(sp_tty* device, const char* name, int async);
ssize_t sp_writechar(sp_tty* device, const unsigned char* byte);
ssize_t sp_write(sp_tty* device, const void* buffer, size_t bsize);
ssize_t sp_writem(sp_tty* device, const fl_message* message);
ssize_t sp_read(sp_tty* device, void* buffer, size_t bsize);
ssize_t sp_readm(sp_tty* device, fl_message* message);
ssize_t sp_readr(sp_tty* device, fl_reply* reply);
void sp_close(sp_tty* device);
int sp_waitr(sp_tty* device, fl_reply* reply);
void sp_display(void);

#ifdef __cplusplus
}
#endif

#endif
