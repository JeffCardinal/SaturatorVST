//------------------------------------------------------------------------
// Copyright(c) 2024 .
//------------------------------------------------------------------------

#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

namespace MyCompanyName {

class Smoother {
public:
    Smoother(float initialValue = 0.0f, float smoothingTime = 0.005f)
        : currentValue(initialValue), sampleRate(44100.0), smoothingTime(smoothingTime) {}

    void setSampleRate(double newSampleRate) {
        sampleRate = newSampleRate;
    }

    float smooth(float targetValue) {
        if (sampleRate <= 0.0) return targetValue;
        float alpha = 1.0f - std::exp(-1.0f / (sampleRate * smoothingTime));
        currentValue += alpha * (targetValue - currentValue);
        return currentValue;
    }

private:
    float  currentValue;
    float  smoothingTime;
    double sampleRate;
};

//------------------------------------------------------------------------
//  VST3_Test_SteinbergGUIProcessor
//------------------------------------------------------------------------
class VST3_Test_SteinbergGUIProcessor : public Steinberg::Vst::AudioEffect
{
public:
	VST3_Test_SteinbergGUIProcessor ();
	~VST3_Test_SteinbergGUIProcessor () SMTG_OVERRIDE;

    // Create function
	static Steinberg::FUnknown* createInstance (void* /*context*/) 
	{ 
		return (Steinberg::Vst::IAudioProcessor*)new VST3_Test_SteinbergGUIProcessor; 
	}

	//--- ---------------------------------------------------------------------
	// AudioEffect overrides:
	//--- ---------------------------------------------------------------------
	/** Called at first after constructor */
	Steinberg::tresult PLUGIN_API initialize (Steinberg::FUnknown* context) SMTG_OVERRIDE;
	
	/** Called at the end before destructor */
	Steinberg::tresult PLUGIN_API terminate () SMTG_OVERRIDE;
	
	/** Switch the Plug-in on/off */
	Steinberg::tresult PLUGIN_API setActive (Steinberg::TBool state) SMTG_OVERRIDE;

	/** Will be called before any process call */
	Steinberg::tresult PLUGIN_API setupProcessing (Steinberg::Vst::ProcessSetup& newSetup) SMTG_OVERRIDE;
	// Steinberg::tresult PLUGIN_API setupProcessing (Steinberg::Vst::ProcessSetup& newSetup) {
	// 	sampleRate = newSetup.sampleRate;
	// 	driveSmoother.setSampleRate(sampleRate);
	// 	shapeSmoother.setSampleRate(sampleRate);
	// 	return AudioEffect::setupProcessing(newSetup);
	// };
	
	/** Asks if a given sample size is supported see SymbolicSampleSizes. */
	Steinberg::tresult PLUGIN_API canProcessSampleSize (Steinberg::int32 symbolicSampleSize) SMTG_OVERRIDE;

	/** Here we go...the process call */
	Steinberg::tresult PLUGIN_API process (Steinberg::Vst::ProcessData& data) SMTG_OVERRIDE;
		
	/** For persistence */
	Steinberg::tresult PLUGIN_API setState (Steinberg::IBStream* state) SMTG_OVERRIDE;
	Steinberg::tresult PLUGIN_API getState (Steinberg::IBStream* state) SMTG_OVERRIDE;

//------------------------------------------------------------------------
protected:
    float fDrive;
    float fDriveLeft = 0.0f;
    float fDriveRight = 0.0f;
    float fDriveTarget;
	float fShape;
	float fShapeTarget;
	Smoother driveSmoother;
	Smoother shapeSmoother;

private:
	double sampleRate;
};

//------------------------------------------------------------------------
} // namespace MyCompanyName
