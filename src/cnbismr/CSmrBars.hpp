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

#ifndef CSMRBARS_HPP
#define CSMRBARS_HPP

#include "CSmrEngineExt.hpp"
#include <dtk_colors.h>
#include <cnbiprotocol/CpProbability.hpp>
#include <cnbiprotocol/CpTrigger.hpp>

typedef unsigned int CSmrBarsClass;
typedef int CSmrBarsCue;
typedef float CSmrBarsAlpha;

/*! \brief The most boom BCI feedback with bars and arrows ever
 * TODO
 * 
 * Shape-to-layer order follows:
 * - 0100_bar*
 * - 0110_threshold*
 * - 0200_liquid*
 * - 0300_arrow*
 * - 0300_tail*			
 * - 0400_boom			
 * - 0500_fixation*	
 * - 0600_cue		
 * - 0700_rest
 * - 0900_calibration
 */
class CSmrBars : public CSmrEngineExt {
	public:
		CSmrBars(int winw = 800, int winh = 600, 
				CpTrigger* trigger = NULL); 
		CSmrBars(CCfgConfig* configuration, CpTrigger* trigger = NULL); 
		virtual ~CSmrBars(void);

		/* 2009-11-28  Michele Tavella <michele.tavella@epfl.ch>
		 * Those functions can not be virtual since it strictly depends on the
		 * implementation of the class!
		 */
		bool SetParameters(unsigned int classes = 4, float threshold = 1.00f);
		bool SetParameters2(unsigned int classes, float* threshold);
		void SetAspect(float fw = 0.10f, float fl = 0.70f, float sr = 0.18f);
		void SetPosition(float x = 0.00f, float y = 0.00f);
		void UpdateThreshold(float* threshold);

		/* 2009-10-16  Michele Tavella <michele.tavella@epfl.ch>
		 * Luca, apart from the things written in examples/feedbackbars.cpp,
		 * the only thing you need to know is that:
		 * - the numbers of classes are set in the constructor
		 * - put whatever in the unused control signals 
		 * - the control signal is between 0 and 1, where 0 is nothing and 1 is 
		 * full-bar (foobar!)
		 * - For 4 classes:
		 * 		0.25, 0.25, 0.25, 0.25	initial condition (no bias)
		 * 		1.00, 0.00, 0.00, 0.00 	hard decision first class
		 *
		 * - The classes are always:
		 *               2
		 *               |
		 *            1--+--0
		 *               |
		 *               3
		 */
		virtual void Control(CpProbability* probs, 
				CSmrBarsClass sclass = CSmrBars::ArrowNone, 
				bool boom = false);

		virtual void FixationCue(CSmrBarsCue type = CSmrBars::DrawNone, 
				CSmrBarsClass sclass = CSmrBars::ArrowNone, 
				bool calibration = false);
		virtual void ShowWait(float step);
		virtual void ShowBlock(std::string text = "");
		
		virtual void Open(void);
		virtual void Close(void);

	public:
		static const CSmrBarsCue DrawNone = 0;
		static const CSmrBarsCue DrawCue = 1;
		static const CSmrBarsCue DrawFixation = 2;
		static const CSmrBarsCue DrawRest = 3;
		static const CSmrBarsCue DrawBlock = 4;

		static const CSmrBarsClass ArrowRight = 0;
		static const CSmrBarsClass ArrowLeft = 1;
		static const CSmrBarsClass ArrowUp = 2;
		static const CSmrBarsClass ArrowDown = 3;
		static const CSmrBarsClass ArrowNone = 4;

		static constexpr CSmrBarsAlpha ShapeHidden  = 0.00f; 	// C++11 Compliant
		static constexpr CSmrBarsAlpha ShapeVisible = 1.00f;	// C++11 Compliant

	protected:
		virtual void RenderFixation(float alpha);
		virtual void RenderCue(float rotation, const float* color, float alpha);
		virtual void RenderRest(float alpha);
		virtual void RenderBlock(float alpha, std::string text = "");
		virtual void RenderCalibration(float alpha);
		virtual void RenderBars(void);
		virtual void RenderControl(CpProbability* probs, const float* rgb);
		virtual void RenderThreshold(float* threshold);
		virtual void RenderArrows(float kwidth = 1.80f, float klenght = 2.50f);
		virtual void RenderBoom(const float* color, float alpha);
		virtual	void RenderWait(unsigned int step);
		virtual	void RenderBegin(void);
		virtual	void RenderEnd(void);
		void Setup(void);
		void Teardown(void);
	private:
		void Init(void);

	protected:
		unsigned int _classes;
		float _threshold[4];
		float _x, _y;
		float _boomradius;
		float _feedbackwidth, _feedbacklenght;
		float _cordctrl[4];
		dtk_hfont    _text_font;
		std::string  _block_text;
		const float* _color_bars;
		const float* _color_control;
		const float* _color_controlflash;
		const float* _color_threshold;
		const float* _color_fixation;
		const float* _color_cue;
		const float* _color_rest;
		const float* _color_block;
		const float* _color_calibration;
		const float* _color_cross;
		const float* _colors_arrow[5];
		float _rgbth[4];
		float _rgb_rest[4];
		float _rgb_block[4];
		float _rgb_fixation[4];
		float _rgb_cross[4];
		float _rgb_calibration[4];
		float _cuerotations[4];
};
#endif
