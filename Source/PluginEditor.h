/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SpectrumComponent.h"
#include "StyleSheet.h"
//==============================================================================
/**
*/
class DynResoSuppressorEditor  : public juce::AudioProcessorEditor
{
public:
    DynResoSuppressorEditor (DynResoSuppressorProcessor&);
    ~DynResoSuppressorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    SpectrumComponent Spectrum;

private:
    // Sliders
    juce::Slider sliderSensibility;
    juce::Slider sliderSharpness;
    juce::Slider sliderGain;
    juce::Slider sliderWet;
    juce::Slider sliderMakeUp;
    juce::Slider sliderFmin;
    juce::Slider sliderFmax;
    juce::Slider sliderSpeed;
    juce::ToggleButton ButtonHearDiff;
    juce::ComboBox ComboBoxResolution;
    juce::ToggleButton ButtonPassThrough;
    juce::Label  labelPassThrough;
    juce::Label  labelResolution;
    juce::Label  labelSensibility;
    juce::Label labelSharpness;
    juce::Label labelGain;
    juce::Label labelWet;
    juce::Label labelMakeUp;
    juce::Label labelHearDiff;
    juce::Label labelEqualizer;
    juce::Label labelsliderFmin;
    juce::Label labelsliderFmax;
    juce::Label labelsliderSpeed;
    juce::Label labelCredits;
    
    // Attachments 
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderSensibilityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderSharpnessAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderWetAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderSpeedAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderMakeUpAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderFminAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderFmaxAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> HearDiffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ResolutionAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> PassThroughAttachment;

    DynResoSuppressorProcessor& audioProcessor;
    
    CustomLookAndFeel LookAndFeel;
    
    const int GUI_HEIGHT = 400;
    const int GUI_WIDTH  = 1200;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DynResoSuppressorEditor)
};
