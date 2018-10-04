#include "rex.hpp"

Rex::Rex(char* port)
{
  std::cout << "Creating a Rex object on port " << port << std::endl;

	// Initialise stationary velocities;
	v_tx = 0x80;
	w_tx = 0x80;
	v_rx = 0x80;
	w_rx = 0x80;

  this->setupPort(port);

}

Rex::~Rex()
{

}



bool Rex::seekByte(char b, int search_limit = 24)
{
  int myerrno, nread;
  unsigned int ndata, totalread, tries, maxtries;
  unsigned char sbuf[10];

  bool found = false;

  for( totalread = 0, nread = 0, tries = 0, maxtries = 24;
      totalread < search_limit && tries < maxtries; tries++ ) {

    nread = read( this->fd, sbuf, 1);

    unsigned char c = sbuf[0];
    printf("0x%X 0x%X \n", (unsigned char)b, c);

    if ( nread == -1 ) {
      myerrno = errno;
      printf( "read() failed: %s\n", strerror( myerrno ) );
      return myerrno;
    }
    if ( nread == 0 ) {
      printf( "read() failed, unexpected EOF\n" );
      return ENODATA;
    }

    if (sbuf[0] ==  (unsigned char)b){
      found = true;
      break;
    }
  }

  return found;
}

/* Writes to device and automatically generates and appends a sequence byte and Dynamics checksum */
int Rex::writeToDevice(int n_bytes, unsigned char* data){
  int retval = 0;
  unsigned char crem;
  static int sequence = 0;

	unsigned int len_packet = n_bytes + 2;

  unsigned char sdata[len_packet];
  unsigned char csum = 0x00;

  for (int i = 0; i < n_bytes; i++){
    sdata[i] = data[i];
    csum += sdata[i];
  }

  sequence++;
	if (sequence >= 255){
		sequence = 0;
	}

  sdata[n_bytes] = sequence;
  csum += sequence;

  crem = csum % 0xFF;

  sdata[n_bytes + 1] = 0xFE - crem;

  tcflush( this->fd, TCOFLUSH );
  retval = write(this->fd, sdata, len_packet);
  tcflush( this->fd, TCOFLUSH );
  return retval;
  		  // FOR DEBUG
  printf("Sent packet:     0x");
  for (int i = 0; i < len_packet; i++){
    printf("%02X ", sdata[i]);
  }
  printf("\n");

  return retval;
}

/* Writes plain bytes to device without a checksum */
int Rex::writeToDeviceNOCHKSUM(int n_bytes, unsigned char* data){
  int retval = 0;

  tcflush( this->fd, TCOFLUSH );
  retval = write(this->fd, data, n_bytes);
  tcflush( this->fd, TCOFLUSH );
  return retval;
  		  // FOR DEBUG
   printf("Sent packet:     0x");
   for (int i = 0; i < n_bytes; i++){
     printf("%02X ", data[i]);
   }
   printf("\n");

  return retval;
}

/* Writes plain bytes to device without a checksum */
int Rex::writeToDeviceLittleNOCHKSUM(int n_bytes, unsigned char* data){
  int retval = 0;
  char little[n_bytes];

	for (int i = 0; i < n_bytes; i++){
		little[i] = data[n_bytes - i - 1];
	}

  tcflush( this->fd, TCOFLUSH );
  retval = write(this->fd, little, n_bytes);
  tcflush( this->fd, TCOFLUSH );
  return retval;
		  // FOR DEBUG
   printf("Sent packet:     0x");
   for (int i = 0; i < n_bytes; i++){
     printf("%02X ", data[i]);
   }
   printf("\n");

  return retval;
}

/* Reads regular packets from DX-GPSB */
int Rex::readDevice(unsigned char *return_data)
{
  int myerrno, nread;
  unsigned int ndata, totalread, tries, maxtries;

  const int MAX_DATA = 24;
	int NDATA = 0;
	unsigned char sbuf[MAX_DATA];

	// DEBUG STOP!!!!!
		return -1;
		printf("reading\n");
  // Check for start byte
		 if (!seekByte(0xFB)){
   //if (!seekByte('%')){
     printf( "Missing sample from Rex (start byte not found)\n" );
     return -1;
  } else printf("seekByte [OK]\n");

  // Get Rex readings

  for( totalread = 0, nread = 0, tries = 0, maxtries = 24;
      totalread < NDATA  && tries < maxtries; tries++ ) {

    nread = read( this->fd, sbuf + totalread, NDATA - totalread);

    if ( nread == -1 ) {
      myerrno = errno;
      printf( "read() failed: %s\n", strerror( myerrno ) );
      return myerrno;
    }
    if ( nread == 0 ) {
      printf( "read() failed, unexpected EOF\n" );
      return ENODATA;
    }
    printf("totalread: %d, sbuf: %s\n",totalread,sbuf);
    totalread += nread;
  }

   NDATA = read( this->fd, sbuf, MAX_DATA);

   printf("Received packet: ");
   for (int i = 0; i < NDATA; i++){
     printf("%02X ", sbuf[i]);
     return_data[i] = sbuf[i];
   }
   printf("\n");

  return 0;

}

int Rex::setupPort(char* port)
{
  int myerrno;
  struct termios termInfo;

  this->fd = open( port, O_RDWR | O_NOCTTY | O_SYNC );
  //this->fd = open( port, O_RDWR | O_NOCTTY | O_NDELAY );
  if ( this->fd == -1 ) {
    myerrno = errno;
    printf( "open() of %s failed: %s\n", port, strerror( myerrno ) );
    return myerrno;
  }

  if ( ioctl( this->fd , TCGETA, & termInfo ) == -1 ) {
    myerrno = errno;
    printf( "ioctl(TCGETA) failed: %s\n", strerror( myerrno ) );
    return myerrno;
  }

  if( tcgetattr( this->fd, & termInfo ) == -1 ) {
    myerrno = errno;
    printf( "tcgetattr() failed: %s\n", strerror( myerrno ) );
    return myerrno;
  }

  //cfsetispeed( & termInfo , B38400 );
  cfsetspeed( & termInfo , B115200 );

  termInfo.c_cflag |=  ( CLOCAL | CREAD );
  termInfo.c_lflag &= ~( ICANON | ECHO | ECHOE | ISIG );
  termInfo.c_cflag &= ~ PARENB;
  termInfo.c_cflag &=  ~CSTOPB;
  termInfo.c_cflag &= ~ CSIZE;
  termInfo.c_cflag |=   CS8;
  termInfo.c_iflag  =    IGNBRK | IGNPAR;
  termInfo.c_oflag &= ~OPOST;

  tcflush( this->fd, TCIFLUSH );

  if ( tcsetattr( this->fd, TCSANOW, &termInfo) == -1 ) {
    myerrno = errno;
    printf( "tcsetattr() failed: %s\n", strerror( myerrno ) );
    return myerrno;
  }

  printf("Connected to a Rex on port: %s\n", port);

  unsigned char wbuf[24];
  int wn = 0;
  unsigned char rbuf[24];
  unsigned char tmp;

  printf("Initialise link... \n");
  // Initialise
  wbuf[0] = 0xFB;
  wbuf[1] = 0x07;
  wbuf[2] = 0x00;
  wbuf[3] = 0x44;
  wbuf[4] = 0x4E;
  wbuf[5] = 0x54;
  wbuf[6] = 0x43;
  wbuf[7] = 0x46;
  wbuf[8] = 0x47;
  wn = 9;

  writeToDeviceNOCHKSUM(wn, wbuf);
  readDevice(rbuf);


	usleep(500000);


	this->rl_set_idle();







  printf("\nFinished setupPort()\n");

  return 0;
}

int Rex::rl_update()
{

	return 0;
}



int Rex::rl_read_velocities()
{
	unsigned char rbuf[24];

	// Read Joystick
	readDevice(rbuf);
	printf("Received packet: 0x40 ");
	for (int j = 0; j < 4; j++){
		printf("%02X ", rbuf[j]);
	}
	printf("\n");
	this->v_rx = rbuf[0];
	this->w_rx = rbuf[1];

	return 0;
}

void Rex::sendCommand()
{

	if ((unsigned char)this->w_tx > (unsigned char) 0xC0){
		std::cout << "REX: Full East" << std::endl;
		rl_send_fulleast();
	} else if ((unsigned char)this->w_tx < (unsigned char) 0x40){
		std::cout << "REX: Full West" << std::endl;
		rl_send_fullwest();
	} else if ((unsigned char)this->v_tx > 0xC0){
	  std::cout << "REX: Full South" << std::endl;
		rl_send_fullsouth();
	} else if ((unsigned char)this->v_tx < 0x40){
		std::cout << "REX: Full North" << std::endl;
		rl_send_fullnorth();

	} else {
		rl_set_xyzero();
	}


}

int Rex::rl_set_velocities(float v, float w)
{

	this->v_tx = (char)(v * 127.0 + 128.0);
	this->w_tx = (char)(w * 127.0 + 128.0);
	sendCommand();
	return 0;
}

int Rex::rl_set_velocities(char v, char w)
{

	this->v_tx = v;
	this->w_tx = w;
	sendCommand();
	return 0;
}



int Rex::rl_get_velocities(char &v, char &w)
{
	v = this->v_rx;
	w = this->w_rx;

	return 0;

}

int Rex::rl_get_velocities(float &v, float &w)
{
	v = (this->v_rx - 128) / 127.0;
	w = (this->w_rx - 128) / 128.0;

	return 0;

}

void Rex::sendHold()
{
	unsigned char wbuf[24];
	int wn = 0;
	unsigned char rbuf[24];

	// Hold
	wbuf[0] = 0xFB;
	wbuf[1] = 0x08;
	wbuf[2] = 0x07;
	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x0E;
	wbuf[7] = 0x05;
	wbuf[8] = 0x01;
	wbuf[9] = 0x00;
	wn = 10;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);
}

void Rex::sendRelease()
{
	unsigned char wbuf[24];
	int wn = 0;
	unsigned char rbuf[24];

	// Set Release
	wbuf[0] = 0xFB;
	wbuf[1] = 0x08;
	wbuf[2] = 0x07;
 	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x0E;
	wbuf[7] = 0x05;
	wbuf[8] = 0x01;
	wbuf[9] = 0x01;
	wn = 10;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

}

int Rex::rl_set_idle()
{
	unsigned char wbuf[24];
	int wn = 0;
	unsigned char rbuf[24];

	// SET IDLE

	// // Set X 0
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1B;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0x00;
	wbuf[10] = 0x08;
	wn = 11;
	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// Set Y 0
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
 	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1D;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0x00;
	wbuf[10] = 0x08;
	wn = 11;
	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// ZERO CW
	wbuf[0] = 0xFB;
	wbuf[1] = 0x08;
	wbuf[2] = 0x07;
 	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x10;
	wbuf[7] = 0x05;
	wbuf[8] = 0x01;
	wbuf[9] = 0x01;
	wn = 10;
	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// ZERO CCW
	wbuf[0] = 0xFB;
	wbuf[1] = 0x08;
	wbuf[2] = 0x07;
 	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x0F;
	wbuf[7] = 0x05;
	wbuf[8] = 0x01;
	wbuf[9] = 0x00;
	wn = 10;
	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	return 0;
}

int Rex::rl_set_xyzero()
{
	unsigned char wbuf[24];
	int wn = 0;
	unsigned char rbuf[24];

	// SET XY 0

	// Hold
	sendHold();

	// Set X 0
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
  	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1B;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0x00;
	wbuf[10] = 0x08;
	wn = 11;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// Set Y
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
 	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1D;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0x00;
	wbuf[10] = 0x08;
	wn = 11;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// Set Release
	sendRelease();

	return 0;
}

int Rex::rl_send_fullnorth()
{

	unsigned char wbuf[24];
	int wn = 0;
	unsigned char rbuf[24];

	// North

	// Hold
	sendHold();

	// Set X
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
  	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1B;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0x00;
	wbuf[10] = 0x08;
	wn = 11;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// Set Y
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
 	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1D;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0xEC;
	wbuf[10] = 0x01;
	wn = 11;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// Set Release
	sendRelease();

	return 0;

}

int Rex::rl_send_fullsouth()
{

		unsigned char wbuf[24];
		int wn = 0;
		unsigned char rbuf[24];

		// South

	// Hold
	sendHold();

	// Set X
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
  	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1B;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0x00;
	wbuf[10] = 0x08;
	wn = 11;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// Set Y
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
 	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1D;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0x14;
	wbuf[10] = 0x0E;
	wn = 11;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// Set Release
	sendRelease();

	return 0;
}

int Rex::rl_send_fulleast()
{
		unsigned char wbuf[24];
		int wn = 0;
		unsigned char rbuf[24];

		// East

	// Hold
	sendHold();

	// Set X
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
 	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1B;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0xEC;
	wbuf[10] = 0x01;
	wn = 11;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// Set Y
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
  	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1D;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0x00;
	wbuf[10] = 0x08;
	wn = 11;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// Set Release
	sendRelease();

	return 0;
}

int Rex::rl_send_fullwest()
{
		unsigned char wbuf[24];
		int wn = 0;
		unsigned char rbuf[24];

		// West

	// Hold
	sendHold();

	// Set X
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1B;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0x14;
	wbuf[10] = 0x0E;
	wn = 11;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// Set Y
	wbuf[0] = 0xFB;
	wbuf[1] = 0x09;
	wbuf[2] = 0x07;
 	wbuf[3] = 0xA6;
	wbuf[4] = 0x04;
	wbuf[5] = 0x00;
	wbuf[6] = 0x1D;
	wbuf[7] = 0x05;
	wbuf[8] = 0x02;
	wbuf[9] = 0x00;
	wbuf[10] = 0x08;
	wn = 11;

	writeToDeviceNOCHKSUM(wn, wbuf);
	readDevice(rbuf);

	// Set Release
	sendRelease();

	return 0;
}

