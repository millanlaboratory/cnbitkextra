#ifndef ARTIFACT_UTILITIES_H
#define ARTIFACT_UTILITIES_H

#include <string>
#include <cnbicore/CcBasic.hpp>
#include <cnbiconfig/CCfgConfig.hpp>

/*********************************** 
 * Struct and function definitions *
 ***********************************/

/**
 * Structure to represent artifact event
 */
typedef struct {
	/*@{*/
	GDFEvent  gdfevent;	/**< the gdfevent event (e.g., 0x010b) **/
	HWTrigger hwtrigger;	/**< the hardware trigger value (e.g., 1) **/
	float 	  threshold;	/**< the threshold associated to the event **/
	float 	  timeout;	/**< the timeout associated to the event **/
	/*@}*/
} artifact_t;

/**
 * Structure to represent artifact configuration
 */
typedef struct {
	/*@{*/
	artifact_t on;		/**< structure for on event **/
	artifact_t off;		/**< structure for off event **/
	/*@}*/
} artcfg_t;

/*!
 Configures artifact events read from xml config structure. It raises exception if errors occur.
 @param[in]	config	Pointer to XML config class 
 @param[in]  	cybcfg	Pointer to artifact config structure
 @return		True if the artcfg is configured correctly
 */
bool artifact_configure_events(CCfgConfig* config, artcfg_t* artcfg);

/*********************************** 
 *    Function implementations     *
 **********************************/
bool artifact_configure_events(CCfgConfig* config, artcfg_t* artcfg) {
	try {
		config->RootEx()->QuickEx("tasks/artifact_on")->SetBranch();
		artcfg->on.gdfevent  = config->BranchEx()->QuickGDFIntEx("gdfevent");
		artcfg->on.hwtrigger = config->BranchEx()->QuickIntEx("hwtrigger");
		config->RootEx()->QuickEx("online/artifact/taskset/threshold")->SetBranch();
		artcfg->on.threshold = config->BranchEx()->QuickFloatEx("artifact_on");
		config->RootEx()->QuickEx("online/artifact/taskset/timeout")->SetBranch();
		artcfg->on.timeout = config->BranchEx()->QuickFloatEx("artifact_on");

		config->RootEx()->QuickEx("tasks/artifact_off")->SetBranch();
		artcfg->off.gdfevent  = config->BranchEx()->QuickGDFIntEx("gdfevent");
		artcfg->off.hwtrigger = config->BranchEx()->QuickIntEx("hwtrigger");
		config->RootEx()->QuickEx("online/artifact/taskset/threshold")->SetBranch();
		artcfg->off.threshold = config->BranchEx()->QuickFloatEx("artifact_off");
		config->RootEx()->QuickEx("online/artifact/taskset/timeout")->SetBranch();
		artcfg->off.timeout = config->BranchEx()->QuickFloatEx("artifact_off");
		
	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
	return true;
}

#endif
