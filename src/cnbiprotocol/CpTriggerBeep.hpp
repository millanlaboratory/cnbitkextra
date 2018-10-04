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

#ifndef CPTRIGGERBEEP_HPP
#define CPTRIGGERBEEP_HPP

#include "CpTrigger.hpp"

/* 2009-10-17  Michele Tavella <michele.tavella@epfl.ch>
 * Tricky piece of doggie code to debug the trigger on a LPT-free machine
 */
 /*! \brief Beeper trigger for debugging
 */
class CpTriggerBeep : public CpTrigger {
	public:
		CpTriggerBeep(void);
		virtual ~CpTriggerBeep(void);

		virtual void Open(void);
		virtual void Close(void);
		virtual void Send(unsigned char data = 0);

	private:
		bool _open;
};

#endif
