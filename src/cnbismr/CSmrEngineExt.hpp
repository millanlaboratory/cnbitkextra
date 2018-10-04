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

#ifndef CSMRENGINEEXT_HPP 
#define CSMRENGINEEXT_HPP 

#include "CSmrEngine.hpp"
#include "CSmrEvents.hpp"
#include "CSmrTrigger.hpp"
#include <cnbicore/CcThreadSafe.hpp>
#include <cnbiprotocol/CpTrigger.hpp>
#include <cnbiprotocol/CpTriggerFactory.hpp>

class CSmrEngineExt : public CSmrEngine, public CSmrEventsProxy {
	public:
		CSmrEngineExt(const std::string& caption = "libcnbismr::CSmrEngineExt",
				int width = 800, int height = 600, CpTrigger* trigger = NULL);
		CSmrEngineExt(CCfgConfig* configuration, CpTrigger* trigger);
		virtual ~CSmrEngineExt(void);
		virtual void Open(void);
		virtual void Close(void);
		virtual bool ScreenSync(CpTriggerByte value);
	protected:
		virtual void HandleKeyboard(CSmrEvents* caller, CSmrKeyboard event);
		virtual void HandleQuit(CSmrEvents* caller);
		virtual void HandleRedraw(CSmrEvents* caller);
	private:
		void Init(CpTrigger* trigger);
	public:
		CcThreadSafe<unsigned int> eventkeyboard;
		CcThreadSafe<bool> eventquit;
		CcThreadSafe<bool> eventredraw;
	protected:
		CpTrigger* _trigger;
		CSmrEvents* _events;
		CSmrTrigger* _screensync;
};

#endif
