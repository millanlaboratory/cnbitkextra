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

#ifndef CPAUDIO_HPP
#define CPAUDIO_HPP

#include <cnbicore/CcObject.hpp>

/*! \brief Audio subsistem
 */
class CpAudio : public CcObject {
	protected:
		CpAudio(void);
		~CpAudio(void);
	public:
		static CpAudio* Open(int rate = 44100, int channels = 2, 
				int buffers = 512);
		void Close(void);
		void SetVolume(float volume);
		float GetVolume(void);
	private:
		static CpAudio* Instance(void);
		void Destroy(void);
		static unsigned int Refcount(void);
		void Release(void);

	private:
		static CpAudio* _instance;
		static unsigned int _refCount;
};


#endif
