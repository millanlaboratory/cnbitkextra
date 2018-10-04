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

#ifndef CPSOUND_CPP
#define CPSOUND_CPP

#include "CpSound.hpp"

CpSound::CpSound(void) {
	this->_data = NULL;
}

CpSound::~CpSound(void) {
}

void CpSound::Load(std::string filename) {
	this->_filename = filename;
	if(this->_data != NULL)
		this->Free();

	this->_data = Mix_LoadWAV(this->_filename.c_str());
	
	if(this->_data != NULL)
		return;

	printf("[CpSound::Load] Unable to load %s: %s\n", this->_filename.c_str(), Mix_GetError());
}

void CpSound::Free(void) {
	if(this->_data == NULL) 
		return;

	Mix_FreeChunk(this->_data);
	this->_data = NULL;
}

#endif
