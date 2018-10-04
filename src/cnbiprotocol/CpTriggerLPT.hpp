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

#ifndef CPTRIGGERLPT_HPP
#define CPTRIGGERLPT_HPP

#include "CpTrigger.hpp"
#include <lpttrigger.h>
#include <cnbiconfig/CCfgConfig.hpp>

/*! \brief LPT trigger via liblpttrigger
 */
class CpTriggerLPT : public CpTrigger {
	public:
		/* 2009-10-12  Michele Tavella <michele.tavella@epfl.ch>
		 * Bubs says 50ms for windows
		 */
		CpTriggerLPT(int port = -1, unsigned int dt = 15, 
				unsigned char level =  0);
		CpTriggerLPT(CCfgConfig* config);
		virtual ~CpTriggerLPT(void);
		virtual void Open(void);
		virtual void Close(void);
		virtual void Send(CpTriggerByte data = 0);
	private:
		void Init(void);

	private:
		int _port;
		unsigned int _dt;
		unsigned char _level;
		struct lpttrigger* _trigger;
		bool _open;
};

#endif
