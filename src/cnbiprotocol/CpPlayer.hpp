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

#ifndef CPPLAYER_HPP
#define CPPLAYER_HPP

#include "CpSound.hpp"
#include <cnbicore/CcObject.hpp>

class CpPlayer : public CcObject {
	public:
		CpPlayer(void); 
		virtual ~CpPlayer(void);
		
		void Play(CpSound* sound, int ch = -1, int loops = 0);
		bool IsPlaying(void);
		void Wait(int ms = 5);
	
	private:
		int _playing;
		CpSound* _sound;
};
#endif
