/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SpectrumPlotObject.h"
//==============================================================================
/**
*/
class SpectrumComponent  : public juce::Component,
private juce::Timer
{
public:
    SpectrumComponent (DynResoSuppressorProcessor& p);
    ~SpectrumComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    void timerCallback() override;
    
    // #####################
    SpectrumPlot PlotObject;
    juce::Image SpectogramImageProcessor;
    void mouseMove (const juce::MouseEvent &event) override;
    void mouseDown(const juce::MouseEvent &event) override;
    void repaintCallback();
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DynResoSuppressorProcessor& audioProcessor;
    int mouseXPosition;
    int mouseYPosition;
    
    int ColorBackground[4]      = {22, 27, 34, 255};
    int ColorText[4]            = {160, 170, 180, 255};
    int ColorCursor[4]          = {255, 121, 198, 255};
    int ColorLines[4]           = {255, 255, 255, 40};
    int ColorSpectrumLine[4]    = {0, 255, 255, 255};
    int ColorSpectrumSurface[4] = {0, 255, 255, 45};
    
    
    float PosStart   = 2.99573227355; //std::log(20);
    float PosEnd     = 9.90348755254; //std::log(20000);
    float FrequencyPosLabels[9] = {50,100,200,500,1000,2000,5000,8000,15000};
    std::string FrequencyLabels[9] = {"50","100","200","500","1k","2k","5k","8k","15k"};
    float FrequencyLines[27] = {20,30,40,50,60,80,90,100,200,300,400,500,600,700,800,900,1000,2000,3000,
                                4000,5000,6000,7000,8000,9000,10000,20000};
    std::string AmplitudeLabels[9] = {"20 dB","16 dB","12 dB","8 dB","4 dB","0 dB","-4 dB","-8 dB","-12 dB"};
    
    std::unique_ptr<float[]> logIdx;
    // Natural logarithms (base e) of the originals
    float FrequencyPosLabelsLog[9] = {
        3.912023005f, 4.605170186f, 5.298317367f, 6.214608098f, 6.907755279f,
        7.600902460f, 8.517193191f, 8.987196821f, 9.615805480f
    };
    
    float FrequencyLinesLog[27] = {
        2.995732274f, 3.401197382f, 3.688879454f, 3.912023005f, 4.094344562f,
        4.382026635f, 4.499809670f, 4.605170186f, 5.298317367f, 5.703782475f,
        5.991464547f, 6.214608098f, 6.396929655f, 6.551080335f, 6.684611728f,
        6.802394763f, 6.907755279f, 7.600902460f, 8.006367568f, 8.294049640f,
        8.517193191f, 8.699514748f, 8.853665428f, 8.987196821f, 9.104979856f,
        9.210340372f, 9.903487553f
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumComponent)
};
