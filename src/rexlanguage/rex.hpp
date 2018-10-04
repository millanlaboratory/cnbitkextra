/*
 * rex.hpp
 *
 *      Author: Tom Carlson
 */


#ifndef REX_H
#define REX_H

#include <iostream>
#include <iomanip>

#include <math.h>

#include <stdio.h>

//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
//#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <sys/time.h>

//#include "median_filter.h"


#ifndef DEG2RAD
#define DEG2RAD (M_PI/180.0)
#endif

#ifndef RAD2DEG
#define RAD2DEG (180.0/M_PI)
#endif

class Rex {
  public:
    Rex(char* port = "/dev/ttyUSB0");
    ~Rex();

		int rl_set_velocities(float v, float w); // v, w E [-1, 1]
		int rl_set_velocities(char v, char w); //0x01 = -MAX, 0x80 = 0, 0xFF = +MAX
		int rl_get_velocities(float &v, float &w);
		int rl_get_velocities(char &v, char &w);
		int rl_read_velocities();
		int rl_update();

		int rl_set_idle();
		int rl_set_xyzero();
		int rl_send_fullnorth();
		int rl_send_fullsouth();
		int rl_send_fulleast();
		int rl_send_fullwest();

  private:
    int fd;

		char v_tx;
		char w_tx;
		char v_rx;
		char w_rx;

		void sendHold();
		void sendRelease();
		void sendCommand();

		int setupPort(char* port);
		int readDevice(unsigned char* return_data);
		bool seekByte(char b, int search_limit);
		int writeToDevice(int n_bytes, unsigned char* data);
		int writeToDeviceNOCHKSUM(int n_bytes, unsigned char* data);
		int writeToDeviceLittleNOCHKSUM(int n_bytes, unsigned char* data);


};


#endif
