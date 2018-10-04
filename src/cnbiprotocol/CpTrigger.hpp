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

#ifndef CPTRIGGER_HPP
#define CPTRIGGER_HPP

typedef unsigned char CpTriggerByte;

/*! \brief Interface for trigger objects
 *
 * Provides a  generic interface for triggers (LPT, TCP and so on).
 */
class CpTrigger {
	public:
		CpTrigger(void);
		virtual ~CpTrigger(void);

		virtual void Open(void) = 0;
		virtual void Close(void) = 0;
		
		/* 2009-10-09  Michele Tavella <michele.tavella@epfl.ch>
		 * Not really sure about this.
		 * It holds for the LPT and the TTY triggers, not really sure for the
		 * others...
		 */
		virtual void Send(CpTriggerByte data = 0) = 0;
};

#endif
