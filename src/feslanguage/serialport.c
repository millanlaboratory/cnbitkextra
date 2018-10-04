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
#include "fesdecode.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void sp_init(sp_tty* device) {
	memset(device, 0, sizeof(sp_tty));
	device->options.baud = B38400;
	device->options.databits = CS8;
	device->options.stopbits = CSTOPB;
	device->options.parity = 0;
	device->fd = -1;
}

int sp_open(sp_tty* device, const char* name, int async) {
	int flags = O_RDWR | O_NOCTTY;
	if(async > 0)
		flags |= O_NDELAY;
	device->fd = open(name, flags);

	if(device->fd < 0)
		return device->fd;

	tcgetattr(device->fd, &(device->termios_old));
	device->termios_new = device->termios_old;

	cfsetispeed(&(device->termios_new), device->options.baud);
	cfsetospeed(&(device->termios_new), device->options.baud);

	device->termios_new.c_cflag |= (CLOCAL | CREAD);
	device->termios_new.c_cflag &= ~PARENB;
	device->termios_new.c_cflag &= ~CSTOPB;
	device->termios_new.c_cflag &= ~CSIZE; 
	device->termios_new.c_cflag |= device->options.databits;
	device->termios_new.c_cflag |= device->options.stopbits;
	device->termios_new.c_cflag &= ~CRTSCTS;
	
	device->termios_new.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	device->termios_new.c_iflag &= ~(IXON | IXOFF | IXANY);
	
	device->termios_new.c_oflag &= ~OPOST;
	
	device->termios_new.c_cc[VMIN]  = 1;
	device->termios_new.c_cc[VTIME] = 0; 

	tcflush(device->fd, TCIFLUSH);
	tcsetattr(device->fd, TCSANOW, &(device->termios_new));

	return device->fd;
}

ssize_t sp_writechar(sp_tty* device, const unsigned char* byte) {
	return write(device->fd, byte, 1);
}

ssize_t sp_write(sp_tty* device, const void* buffer, size_t bsize) {
	return write(device->fd, buffer, bsize);
}

ssize_t sp_writem(sp_tty* device, const fl_message* message) {
	return write(device->fd, message->buffer, message->bytes);
}

ssize_t sp_read(sp_tty* device, void* buffer, size_t bsize) {
	return read(device->fd, buffer, bsize);
}

void sp_close(sp_tty* device) {
	tcsetattr(device->fd, TCSANOW, &(device->termios_old));
	close(device->fd);
	device->fd = -1;
}

ssize_t sp_readm(sp_tty* device, fl_message* message) {
	ssize_t bytes = read(device->fd, message->buffer, FESREPLY_BSIZE);
	if(bytes > 0)
		message->bytes = bytes;
	else
		message->bytes = 0;

	return bytes;
}

ssize_t sp_readr(sp_tty* device, fl_reply* reply) {
	memset(reply->buffer, 0, FESREPLY_BSIZE);
	return read(device->fd, reply->buffer, FESREPLY_BSIZE);
}

int sp_waitr(sp_tty* device, fl_reply* reply) {
	int i = 0;
	ssize_t rbytes = 0, bytes;
	
	memset(reply->buffer, 0, FESREPLY_BSIZE);
	while(1) {
		bytes = read(device->fd, reply->buffer + rbytes, FESREPLY_BSIZE - rbytes);
		if(bytes > 0) {
			rbytes += bytes;
			if(strstr((const char*)reply->buffer, "<sof>") != NULL)
				if(strstr((const char*)reply->buffer, "</sof>") != NULL)
					break;
		}
		usleep(5000);
	}

	for(i = 0; i < rbytes; i++)
		if(reply->buffer[i] == '\n')
			reply->buffer[i] = ' ';

	fl_decode(reply);
	return reply->cid;
} 

void sp_display(void) {
	usleep(50*1000);
}
