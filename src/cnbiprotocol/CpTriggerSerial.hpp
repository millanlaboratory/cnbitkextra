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

#ifndef CPTRIGGERSERIAL_HPP
#define CPTRIGGERSERIAL_HPP

#include "CpTrigger.hpp"
#include <cnbiconfig/CCfgConfig.hpp>
#include <iostream>
#include <termios.h>    // POSIX terminal control definitions

/*! \brief Serial trigger
 */
class CpTriggerSerial : public CpTrigger {
	public:
		CpTriggerSerial(std::string port = "/dev/ttyACM0");
		CpTriggerSerial(CCfgConfig* config);
		virtual ~CpTriggerSerial(void);
		virtual void Open(void);
		virtual void Close(void);
		virtual void Send(CpTriggerByte data = 0);
	private:
		int _fid;
		std::string _port;
		struct termios _tty;
		bool _open;
};

#endif
