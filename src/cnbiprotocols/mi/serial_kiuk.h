// I got this code from : http://www.easysw.com/~mike/serial/serial.html
// Also: http://www.cplusplus.com/doc/tutorial/files/

#include <stdio.h>		//Standard input/output definitions
#include <fcntl.h>		//File control definitions
#include <termios.h>		//POSIX terminal control definitions

//open_port() - Opens serial port and returns file descriptor on success or -1 on error
int open_port(char* portname){
	int fd;		//File descriptor for the port
	struct termios options;

	fd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1){
		return (fd);
	}
	else{
		fcntl(fd, F_SETFL, FNDELAY); // Sets the read() function to return NOW and not wait for data to enter buffer if there isn't anything there.

		//Configure port for 8N1 transmission and set a baudrate
		tcgetattr(fd, &options);				//Gets the current options for the port
		cfsetispeed(&options, B115200);				//Sets the Input Baud Rate
		cfsetospeed(&options, B115200);				//Sets the Output Baud Rate
		options.c_cflag |= (CLOCAL | CREAD);		
		options.c_cflag &= ~PARENB;				// 8N1 serial operations, no parity
		options.c_cflag &= ~CSTOPB;
		options.c_cflag &= ~CSIZE;
		options.c_cflag |= CS8;

		tcsetattr(fd, TCSANOW, &options);			//Set the new options for the port "NOW"
	};

	return (fd);
};

