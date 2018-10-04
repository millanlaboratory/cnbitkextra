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

#ifndef FEEDBACKBARSCONTINUOUS_HPP
#define FEEDBACKBARSCONTINUOUS_HPP

#include "CSmrBars.hpp"
#include "CSmrEngine.hpp"
#include <dtk_colors.h>
#include <cnbiprotocol/CpProbability.hpp>
#include <assert.h>

/*! \brief The most sexy BCI feedback with bars and arrows ever, now featuring
 * continuous control
 * TODO
 */
class CSmrCBars : public CSmrBars {
	public:
		CSmrCBars(int winw = 800, int winh = 600, CpTrigger* trigger = NULL); 
		CSmrCBars(CCfgConfig* configuration, CpTrigger* trigger);
		virtual ~CSmrCBars(void);
		
		void Setup(void);
		
		virtual void SetThrottle(float max = 1.80f);
		virtual void ControlContinuous(CpProbability* prob, CSmrBarsClass sclass,
				float reliability = 0.625f, float target = 0.00f);
		virtual void DisableContinuous(void);

		virtual void ShowTeaser(float step = 0.00f);
		virtual void HideTeaser(void);

	protected:
		virtual void RenderThrottle(float prob, float reliability,
				CSmrBarsClass sclass);

		virtual void RenderLimit(CSmrBarsAlpha alpha);
		virtual void RenderTarget(float prob, CSmrBarsAlpha alpha);
		virtual void RenderTeaser(unsigned int step);

	protected:
		float _throttlemax;
};

#endif
