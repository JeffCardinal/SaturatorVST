//------------------------------------------------------------------------
// Copyright(c) 2024 .
//------------------------------------------------------------------------

#include <cmath>

#include "processor.h"
#include "cids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#include "params.h"

using namespace Steinberg;

namespace MyCompanyName {

//------------------------------------------------------------------------
// VST3_Test_SteinbergGUIProcessor
//------------------------------------------------------------------------
VST3_Test_SteinbergGUIProcessor::VST3_Test_SteinbergGUIProcessor ()
{
	//--- set the wanted controller for our processor
	setControllerClass (kVST3_Test_SteinbergGUIControllerUID);
}

//------------------------------------------------------------------------
VST3_Test_SteinbergGUIProcessor::~VST3_Test_SteinbergGUIProcessor ()
{}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIProcessor::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated
	
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	// if everything Ok, continue
	if (result != kResultOk)
	{
		return result;
	}

	//--- create Audio IO ------
	addAudioInput (STR16 ("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
	addAudioOutput (STR16 ("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

	/* If you don't need an event bus, you can remove the next line */
	addEventInput (STR16 ("Event In"), 1);

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIProcessor::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!
	
	//---do not forget to call parent ------
	return AudioEffect::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIProcessor::setActive (TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
	return AudioEffect::setActive (state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIProcessor::process (Vst::ProcessData& data)
{
	//--- First : Read inputs parameter changes-----------

	if (data.inputParameterChanges)
	{
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			if (auto* paramQueue = data.inputParameterChanges->getParameterData (index))
			{
				Vst::ParamValue value;
				int32 sampleOffset;
				int32 numPoints = paramQueue->getPointCount ();
				if (paramQueue -> getPoint(numPoints-1, sampleOffset, value) == kResultTrue) {
					switch (paramQueue->getParameterId()) {
						case kDriveId:
							fDriveTarget = float(value);
							// fDrive = float(value);
    						// fDrive = driveSmoother.smooth(float(value));
							break;
						case kShapeId:
							fShapeTarget = float(value);
							// fShape = shapeSmoother.smooth(float(value));
							break;
						case kOutputLevelId:  // Output level knob parameter
                            fOutputLevel = float(value);  // Get the output level value
                            break;
					}
				}
			}
		}
	}

	//--- Here you have to implement your processing

	for (int32 i = 0; i < data.numInputs; i++) {
    	int32 minChan = std::min(data.inputs[i].numChannels, data.outputs[i].numChannels);
		for (int32 c = 0; c < minChan; c++) {
			float* input = data.inputs[i].channelBuffers32[c];
			float* output = data.outputs[i].channelBuffers32[c];

			memset(output, 0, data.numSamples * sizeof(float));

			float& currentDrive = (c == 0) ? fDriveLeft : fDriveRight;
			float& currentShape = (c == 0) ? fShapeLeft : fShapeRight;
			float driveDelta = (fDriveTarget - currentDrive) / data.numSamples;
			float shapeDelta = (fShapeTarget - currentShape) / data.numSamples;

			for (int32 sample = 0; sample < data.numSamples; sample++) {
                currentDrive += driveDelta;
                currentShape += shapeDelta;
				
				float outputGain = 10.0f * std::pow(10.0f, fOutputLevel * -36.0f / 20.0f);

				if (currentDrive <= 0.05f) {
					output[sample] = input[sample] * outputGain;
				} else {
					float scaledSample = currentDrive * 20.0f * input[sample];
					// float waveshapedSample = std::tanh(scaledSample);
					float sineShape = std::sin(currentShape * scaledSample); 
                	float waveshapedSample = sineShape;
					float compensation = 1.0f / std::tanh(20.0f * 0.05f); // Is this right?
					output[sample] = waveshapedSample * compensation * outputGain;
				}
			}
		}
	}

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	// //--- called before any processing ----
	// return AudioEffect::setupProcessing (newSetup);
	sampleRate = newSetup.sampleRate;
	driveSmoother.setSampleRate(sampleRate);
	shapeSmoother.setSampleRate(sampleRate);
	return AudioEffect::setupProcessing(newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	// by default kSample32 is supported
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIProcessor::setState (IBStream* state)
{
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);
	
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API VST3_Test_SteinbergGUIProcessor::getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);

	return kResultOk;
}

//------------------------------------------------------------------------

} // namespace MyCompanyName
