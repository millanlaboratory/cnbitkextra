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

#ifndef FEEDBACKENGINE_HPP
#define FEEDBACKENGINE_HPP

#include <cnbicore/CcBasic.hpp>
#include <cnbicore/CcThread.hpp>
#include <drawtk.h>
#include <cnbiconfig/CCfgConfig.hpp>
#include <list>
#include <map>
#include <utility>
#include <string>

typedef std::map<std::string, dtk_hshape> CSmrShapes;
typedef std::map<std::string, CSmrShapes> CSmrScenes;

class CSmrEngineProxy;

class CSmrEngine : protected CcThread {
	public:
		CSmrEngine(const std::string& caption = "CSmrEngine", 
				int width = 800, int height = 600, int bpp = 24);
		CSmrEngine(CCfgConfig* configuration);
		virtual ~CSmrEngine(void);

		// Creational methods for scenes
		virtual bool AddScene(const std::string& scene);
		virtual bool HasScene(const std::string& scene);
		virtual void DestroyScene(const std::string& scene);

		// Creational methods for shapes 
		virtual bool AddShape(const std::string& scene, const std::string& shape, 
				dtk_hshape hshape = NULL);
		virtual bool HasShape(const std::string& scene, const std::string& shape);
		virtual void DestroyShape(const std::string& scene, const std::string& shape);
		
		// General methods for shapes
		virtual dtk_hshape GetShape(const std::string& scene, const std::string& shape);
		virtual dtk_hshape SetShape(const std::string& scene, const std::string& shape, 
				dtk_hshape hshape);
		virtual void MoveShape(const std::string& scene, const std::string& shape, 
				float dx, float dy);
		virtual void MoveShapeRel(const std::string& scene, const std::string& shape, 
				float dx, float dy);
		
		virtual void Open(void);
		virtual void Close(void);
		virtual bool IsOpen(void);
	
		// Data locks
		virtual void DataWait(void);
		virtual void DataPost(void);
		virtual void WindowWait(void);
		virtual void WindowPost(void);
		// Utility methods
		virtual void Dump(void);
		dtk_hwnd GetWindow(void);
		// Screen update methods
		virtual void SwitchScene(const std::string& scene);

	protected:
		// HbThread methods
		virtual void Main(void);
		
		virtual void RequestUpdate(void);

		virtual bool SetupForImages(void) { return false; }
		virtual void TeardownForImages(void) { }		

	protected:
		// Syncronization methods
		virtual void WaitUpdate(void);
		
		// Low-level methods for blitting stuff
		virtual void BindShapes(void);
		virtual void RenderScene();

	private:
		// Unsafe methods
		virtual bool HasScene_Unsafe(const std::string& scene);
		virtual bool HasShape_Unsafe(const std::string& scene, const std::string& shape);
		virtual void Init(void);
	protected:
		dtk_hwnd _windowPtr;
		std::string _windowCaption;
		int _windowW, _windowH, _windowX, _windowY;
		int _windowBpp;

		CSmrScenes _data;
		std::string _scene;

	protected:
		CcSemaphore _semscene;
		CcSemaphore _semupdate;
		CcSemaphore _semdata;
		CcSemaphore _semwindow;

	public:
		CcCallback1<CSmrEngineProxy, CSmrEngine*> i_OnDraw;
};

class CSmrEngineProxy {
	public:
		virtual void HandleDraw(CSmrEngine* caller) {}
};

#define CB_CSmrEngine(sender_event, receiver_ptr, method) 	\
	sender_event.Register((CSmrEngineProxy*)receiver_ptr, 	\
			&CSmrEngineProxy::method);

#endif
