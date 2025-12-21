/*
  ==============================================================================

 SpectrumPlotObject.h
    Created: 18 Nov 2020 7:46:53pm
    Author:  Thiago Lobato

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

enum SpectrumPlotType{Spectogram,Spectrum};
class SpectrumPlot
{
public:
    SpectrumPlot():forwardFFT(fftOrder),spectrogramImage (juce::Image::RGB, 512, 512, true),window(fftSize,juce::dsp::WindowingFunction<float>::hann)
    {
        PlotType=SpectrumPlotType::Spectrum;
    }
    
    int fftOrder = 11;
    int fftSize  = 2048;
    int ScopeSize = 1024; // How many points by the 2d spectrum, half FFT size
    
public:
    void getNextAudioBlock ( juce::AudioBuffer<float>& bufferToFill,int totalNumInputChannels,int totalNumOutputChannels);
    void getNextAudioBlock (float *buffer,int NumberOfSamples);
    inline void pushNextSampleIntoFifo (float sample);
    void drawNextLineOfSpectrogram();
    void initialize(int fftSize,SpectrumPlotType Type = SpectrumPlotType::Spectrum);
    bool nextFFTBlockReady = false;
    juce::Image spectrogramImage;
    SpectrumPlotType PlotType; // If 3D or 2D spectogram
    std::unique_ptr<float[]> scopeData;
    std::unique_ptr<float[]> scopeDataOld;  
    
private:
    juce::dsp::FFT forwardFFT;                          
    juce::dsp::WindowingFunction<float> window;
 
    std::unique_ptr<float[]> fifo;                    // temporary fft data
    std::unique_ptr<float[]> fftData;             // FFT data to plot

    float ExponentialAverageFactor = 0.2;
    int fifoIndex = 0;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumPlot)
};
