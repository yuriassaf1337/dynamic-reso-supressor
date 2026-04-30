/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "DynamicEqualizer.h"
//==============================================================================
/**
*/
class DynResoSuppressorProcessor  : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    DynResoSuppressorProcessor();
    ~DynResoSuppressorProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Sound buffer for plot
    int FFTSize = 2048;
    int N       = 0;
    std::unique_ptr<float[]> bufferToPlot;
    std::unique_ptr<float[]> bufferToPlotTemp;  
    void PushSampleInBlockForPlot(float sample);
    double Fs = 48000;
    float Fmin = 100;
    float Fmax = 10000;
    // WOLA with 75% overlap is enough to avoid almost all artefacts
    int overlapIdx = 4;
    OLA::windowTypeIndex windowType = OLA::windowTypeIndex::windowTypeHannSquared;
    
    DynamicEqualizer DynamicResonator;
    
    // GUI parameters
    juce::AudioProcessorValueTreeState apvts;
    
    void parameterChanged(const juce::String &parameterID, float newValue) override;
    void FrequencyLimitsFromPlot(float Frequency,bool IsFmin);

    juce::CriticalSection processingLock;
    
private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DynResoSuppressorProcessor)
};
