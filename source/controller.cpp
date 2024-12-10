//------------------------------------------------------------------------
// Copyright(c) 2024 .
//------------------------------------------------------------------------

#include "controller.h"
#include "cids.h"
#include "vstgui/plugin-bindings/vst3editor.h"

#include "params.h"

using namespace Steinberg;

namespace MyCompanyName {

//------------------------------------------------------------------------
// VST3_Test_SteinbergGUIController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIController::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated

	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk)
	{
		return result;
	}

	// Here you could register some parameters
    parameters.addParameter(STR16("SmoothDriveV5LR"), nullptr, 0, 0.0, 
		Vst::ParameterInfo::kCanAutomate, kDriveId);

	// parameters.addParameter(STR16("Shape"), nullptr, 0, 0.0, 
	// 	Vst::ParameterInfo::kCanAutomate, kShapeId);

	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIController::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIController::setComponentState (IBStream* state)
{
	// Here you get the state of the component (Processor part)
	if (!state)
		return kResultFalse;

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIController::setState (IBStream* state)
{
	// Here you get the state of the controller

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIController::getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API VST3_Test_SteinbergGUIController::createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
	{
		// create your editor here and return a IPlugView ptr of it
		auto* view = new VSTGUI::VST3Editor (this, "view", "editor.uidesc");
		return view;
	}
	return nullptr;
}

//------------------------------------------------------------------------
} // namespace MyCompanyName
