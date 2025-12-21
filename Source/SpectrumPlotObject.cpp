/*
  ==============================================================================

    SpectrumPlotObject.cpp
    Created: 18 Nov 2020 7:46:53pm
    Author:  Thiago Lobato

  ==============================================================================
*/

#include "SpectrumPlotObject.h"

void SpectrumPlot::initialize(int _fftSize,SpectrumPlotType Type)
{
    fftSize   = _fftSize;
    ScopeSize = fftSize/2;
    fftOrder  = static_cast<int>(log2(fftSize));
    scopeData        = std::make_unique<float[]>(ScopeSize);
    scopeDataOld     = std::make_unique<float[]>(ScopeSize);
    fifo             = std::make_unique<float[]>(fftSize);
    fftData          = std::make_unique<float[]>(fftSize);
    forwardFFT = juce::dsp::FFT(fftOrder);
    nextFFTBlockReady = false;
    fifoIndex = 0;

    std::fill(scopeData.get(), scopeData.get() + ScopeSize, 0.0f);
    std::fill(scopeDataOld.get(), scopeDataOld.get() + ScopeSize, 0.0f);
    std::fill(fifo.get(), fifo.get() + fftSize, 0.0f);
    std::fill(fftData.get(), fftData.get() + fftSize, 0.0f);
}
void SpectrumPlot::getNextAudioBlock ( juce::AudioBuffer<float>& buffer,
                              int totalNumInputChannels,int totalNumOutputChannels)
    {
        
        for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
            buffer.clear (i, 0, buffer.getNumSamples());

        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer (channel);

            for(int idx=0;idx<buffer.getNumSamples();idx++)
            {
                pushNextSampleIntoFifo( channelData[idx]);
            }
        }
        
        
    }

void SpectrumPlot::getNextAudioBlock (float *buffer,int NumberOfSamples)
{
    for(int idx=0;idx<NumberOfSamples;idx++)
    {
        pushNextSampleIntoFifo( buffer[idx]);
    }
}


inline void SpectrumPlot::pushNextSampleIntoFifo (float sample)
    {

        if (fifoIndex == fftSize)
        {
            if (! nextFFTBlockReady)
            {
                for(int idx=0;idx<fftSize;idx++)
                    fftData[idx] = fifo[idx];
                
                nextFFTBlockReady = true;
            }
 
            fifoIndex = 0;
        }
 
        fifo[(size_t) fifoIndex++] = sample;
    }

void SpectrumPlot::drawNextLineOfSpectrogram()
    {
        bool SignalIsSpectrum = true;
        if (!SignalIsSpectrum)
        {
        window.multiplyWithWindowingTable (&fftData[0], fftSize);
        forwardFFT.performFrequencyOnlyForwardTransform (&fftData[0]);
        }
        
        switch (PlotType)
        {
            case(SpectrumPlotType::Spectogram):
            {
                auto rightHandEdge = spectrogramImage.getWidth() - 1;
                auto imageHeight   = spectrogramImage.getHeight();
         
                 
                spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);
         

                auto maxLevel = juce::FloatVectorOperations::findMinAndMax (&fftData[0], fftSize / 2);
         
                for (auto y = 1; y < imageHeight; ++y)
                {
                    auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.2f);
                    auto fftDataIndex = (size_t) juce::jlimit (0, fftSize / 2, (int) (skewedProportionY * fftSize / 2));
                    auto level = juce::jmap (fftData[fftDataIndex], 0.0f, juce::jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);
         
                    spectrogramImage.setPixelAt (rightHandEdge, y, juce::Colour::fromHSV (level, 1.0f, level, 1.0f));
                }
                    break;
            }
            case(SpectrumPlotType::Spectrum):
            {
                
                auto mindB = -20.0f;
                auto maxdB =  20.f;
             
                for (int i = 0; i < ScopeSize; ++i)
                    {

                        auto level = juce::jmap( juce::jlimit (mindB, maxdB, juce::Decibels::gainToDecibels (fftData[i]) ),
                                                 mindB, maxdB, 0.0f, 1.0f);
                        scopeData[i] = level*ExponentialAverageFactor+(1-ExponentialAverageFactor)*scopeDataOld[i];
                        scopeDataOld[i] =scopeData[i];
                    }
                
                break;
                
            }
            
        }
        
        
        
    }
