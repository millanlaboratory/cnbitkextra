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

#ifndef CSMRTRIGGER_HPP
#define CSMRTRIGGER_HPP

#include "CSmrEngine.hpp"
#include <cnbicore/CcSemaphore.hpp>
#include <cnbiprotocol/CpTrigger.hpp>

class CSmrTrigger : public CSmrEngineProxy {
	public:
		CSmrTrigger(CSmrEngine* engine, CpTrigger* trigger, 
				bool verbose = false);
		virtual void Enable(CpTriggerByte value);
		virtual void Disable(void);
		virtual void HandleDraw(CSmrEngine* caller);

	private:
		CSmrEngine* _engine;
		CpTrigger* _trigger;
		CpTriggerByte _value;
		CcSemaphore _semsend;
		bool _enabled;
		bool _verbose;
};

#endif
