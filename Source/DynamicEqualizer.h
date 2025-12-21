/*
  ==============================================================================

    DynamicEqualizer.h
    Created: 22 Nov 2020 3:58:07pm
    Author:  Thiago Lobato

  ==============================================================================
*/

#pragma once
#include "OLA.h"
#include <complex>


//==============================================================================

//const double PI = 3.141592653589793;


class DynamicEqualizer : public OLA
{
public:
    DynamicEqualizer();
    //~DynamicEqualizer() override;
    void UpdateParameters(float _Prominence, float _Width,float _PeakReductiondB,
                          float _makeUpDB,float _Wet,bool _OnlyDiff, float fmin,
                          float fmax,float speed, float _SamplingRate,bool _passThrough=false,int _fftSize=2048);
    
    
    
private:
    void  modification() override;
    float Prominence       = 3;
    float Width            = 1.1;
    float logW             = 0.0953101798;
    int   lowFreqIdx       = 3;
    int   HighFreqIdx      = 500;
    float PeakReductionLin = 0.707;
    float df;
    float SamplingRate = 48000;
    float Wet = 1.0f;
    float makeUp = 1;
    bool passThrough = false;
    float AverageRatioTF = 0.15;             // How fast the transfer function will update
    int   fftSize          = 2048;
    
    // Variables to generate the function
    void InitializeNewFFTSize();
    std::unique_ptr<float[]> LogFreqVector; // Frequency vector in a logarithmic scale
    std::unique_ptr<float[]> FreqList;      // Time varying list containing the Frequencies to be filtered
    std::unique_ptr<int[]> FreqIntList;      // Time varying list containing the indices of frequencies to be filtered
    std::unique_ptr<float[]> AmpList;       // Time varying list containing the amplitudes to reduce the frequencies
    std::unique_ptr<float[]> TF;            // Transfer function to filter the signal
    std::unique_ptr<float[]> Abs2FFTVec;
    juce::HeapBlock<juce::dsp::Complex<float>> TFreal;            // (real) Transfer function to filter the signal

    
    
public:
    void setFFTSize(int _fftSize);
    int getFFTSize(){return fftSize;}
    
    std::unique_ptr<float[]> TFOld;         // Transfer function buffer used for plot
    bool OnlyDiff = false;                  //  Important for the plot so it needs to be public
    juce::HeapBlock<juce::dsp::Complex<float>> TFrealOld;       
    

};
