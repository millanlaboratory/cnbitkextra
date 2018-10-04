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

#ifndef CPPLAYER_CPP
#define CPPLAYER_CPP

#include "CpPlayer.hpp"
#include <SDL/SDL_mixer.h>
#include <cnbicore/CcTime.hpp>

CpPlayer::CpPlayer(void) { 
	this->_playing = -1;
}

CpPlayer::~CpPlayer(void) { 
}

void CpPlayer::Play(CpSound* sound, int ch, int loops) {
	this->_playing = Mix_PlayChannel(ch, sound->_data, loops);
	if(this->_playing != -1)
		return;
	printf("[CpPlayer::Play] Unable to play WAV file: %s\n",
			Mix_GetError());
}

bool CpPlayer::IsPlaying(void) {
	return(Mix_Playing(this->_playing) != 0);
}

void CpPlayer::Wait(int ms) {
	while(Mix_Playing(this->_playing) != 0)
		CcTime::Sleep(ms);
	this->_playing = -1;
}

#endif
