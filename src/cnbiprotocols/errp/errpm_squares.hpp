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

#ifndef ERRPSQUARES_HPP
#define ERRPSQUARES_HPP

#include <cnbiconfig/CCfgConfig.hpp>
#include <cnbiprotocol/CpTrigger.hpp>
#include <cnbismr/CSmrEngineExt.hpp>
#include <drawtk.h>
#include <dtk_colors.h>
#include <string>

class ErrpSquares : public CSmrEngineExt {
	public:
		ErrpSquares(int winw = 800, int winh = 600, CpTrigger* trigger = NULL); 
		ErrpSquares(CCfgConfig* configuration, CpTrigger* trigger = NULL); 
		virtual ~ErrpSquares(void);
		void FixationOn(void);
		void FixationOff(void);
		void MoveFixation(float stepx, float stepy);
		void PresentSquares(short int targetlocation, 
				short int cursorlocation);
		void PresentSquaresSuccess(short int targetlocation, 
				short int cursorlocation); // Success
		void PresentSquaresFailure(short int targetlocation, 
				short int cursorlocation); // Failure
		void WhiteSqOn(void);
		void WhiteSqOff(void);
		void Open(float sqsize);
		void Open(void);
		void Close(void);
		void SetSquares(unsigned int totalsquares);
	protected:
		void RenderFixation(float alpha);
		void RenderWhiteSq(float alpha);
		void RenderFrames(void);
		void RenderDrawableArea(void);
		void RenderTargetSquare(float xpos, float ypos, float alpha);
		void RenderTargetSquareSuccess(float xpos, float ypos, float alpha); // Success
		void RenderTargetSquareFailure(float xpos, float ypos, float alpha); // Failure	
		void RenderCursorSquare(float xpos, float ypos, float alpha);
		void RenderCursorSquareSuccess(float xpos, float ypos, float alpha);// Success
		void RenderCursorSquareFailure(float xpos, float ypos, float alpha);// Failure	
		void Setup(float sqsize);
		void Setup(void);
		void Teardown(void);
	private:
		void Init(void);

	public:
		static const short int NoDraw = 666;
	protected:
		dtk_hshape _fixv;
		dtk_hshape _fixvn;
		dtk_hshape _fixh;
		dtk_hshape _fixhn;
		dtk_hshape _whtsq;
		dtk_hshape _whtsqn;
		dtk_hshape _cframe;
		dtk_hshape _cframen;
		dtk_hshape* _Asquares;
		dtk_hshape* _Asquaresn;
		dtk_hshape* _Bsquares;
		dtk_hshape* _Bsquaresn;
		dtk_hshape _fullsquare;
		dtk_hshape _fullsquaren;
		dtk_hshape _targsq;
		dtk_hshape _targsqn;
		dtk_hshape _curssq;
		dtk_hshape _curssqn;

		std::string* _Asquares_str;
		std::string* _Bsquares_str;

		const float* _color_fixation;
		const float* _color_whitesq;
		const float* _color_target;
		const float* _color_cursor;
		const float* _color_success;
		const float* _color_failure;
		float _rgb_fixation[4];
		float _rgb_whitesq[4];
		float _rgb_targetsq[4];
		float _rgb_cursorsq[4];
		float _rgb_success[4]; // Reached target
		float _rgb_failure[4]; // Reached timeout
		float _cx;
		float _cy;
		float _cwdth;
		float _chght;
		float _sqs;
		float _stpsz;
		unsigned int _totalsquares;
		float trg_xpos;
		float trg_ypos;
		float cur_xpos;
		float cur_ypos;
		float trg_alpha;
		float cur_alpha;
};

#endif
