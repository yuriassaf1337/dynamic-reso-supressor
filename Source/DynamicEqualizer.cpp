/*
  ==============================================================================

    DynamicEqualizer.cpp
    Created: 22 Nov 2020 3:58:07pm
    Author:  Thiago Lobato

  ==============================================================================
*/

#include "DynamicEqualizer.h"


inline float fast_exp(float x) noexcept
{

    // Polynomial approximation on [−10, 0], which is fine for the kind of exponential used (here way better than standard JUCE fast math).
    // ~1.6x faster than std::exp on M1.
    // p(x) = a5 x^5 + a4 x^4 + a3 x^3 + a2 x^2 + a1 x + a0
    constexpr float a5 = 1.39293734e-04f;
    constexpr float a4 = 4.28787130e-03f;
    constexpr float a3 = 5.09871800e-02f;
    constexpr float a2 = 2.94084271e-01f;
    constexpr float a1 = 8.35344150e-01f;
    constexpr float a0 = 9.66534326e-01f;
    
    // Horner's Method
    float y = (((((a5 * x + a4) * x + a3) * x + a2) * x + a1) * x + a0);

    // Enforce non-negativity
    return std::max(y, 0.0f);
}

DynamicEqualizer::DynamicEqualizer()
{
    InitializeNewFFTSize();
    
};

void DynamicEqualizer::setFFTSize(int _fftSize)
{
    fftSize = _fftSize;
    InitializeNewFFTSize();
    
}

void DynamicEqualizer::InitializeNewFFTSize()
{
   
   // Initialize pointers and look-up values
   int memoryReductionFactor = 2; // 2 due to evaluating only positive frequency + window smoothing
   LogFreqVector = std::make_unique<float[]>(fftSize);   ; // Frequency vector in a logarithmic scale
   FreqList = std::make_unique<float[]>(fftSize) ;      // Pointer containing the frequency peaks to be filtered.
   FreqIntList = std::make_unique<int[]>(fftSize);
   AmpList = std::make_unique<float[]>(fftSize) ;
   Abs2FFTVec = std::make_unique<float[]>(fftSize) ;
   TF = std::make_unique<float[]>(fftSize) ;            // Transfer function to filter the signal
   TFOld = std::make_unique<float[]>(fftSize) ;

   
   TFreal.realloc (fftSize);
   TFreal.clear (fftSize);
    
   TFrealOld.realloc (fftSize);
   TFrealOld.clear (fftSize);
       
   df    = SamplingRate/fftSize;
   LogFreqVector[0] = 0;
   TFOld[0] = 1;
   TF[0] = 1;
   FreqList[0]=0;
   AmpList[0] = 0;
   
   for(int idx=1;idx<fftSize/memoryReductionFactor+1;idx++)
   {
       LogFreqVector[idx] = std::log(df*(float)idx);
       TFOld[idx] = 1;
       TF[idx] = 1;
       TFreal[idx] = {1,0};
       TFrealOld[idx]       = {1,0};
       FreqList[idx] = 0;
       AmpList[idx] = 0;
   }
   TFreal[0] = {0.0,0.0};
   TFreal[ (fftSize/2-1) ] = {0.0,0.0};
   TFrealOld[0] = {0.0,0.0};
   TFrealOld[ (fftSize/2-1) ] = {0.0,0.0};

}

void DynamicEqualizer::modification()
{
    if(passThrough)
        return;
    
    fft->perform (timeDomainBuffer, frequencyDomainBuffer, false);

    // ########### Find FFT Peaks #############
    int NumberOfPeaks = 0;
    const int MinPeakDistance = 3;
    float MaxPeak       = 0;
    const int windowSmoothIdxCompensation = 2; //  Window smooths spectrum, so neighbours can be skipped
    const int halfFFTSize = fftSize / 2-1;

    const int absLowIdx  = std::max(1, lowFreqIdx - windowSmoothIdxCompensation);
    const int absHighIdx = std::min(halfFFTSize, HighFreqIdx + windowSmoothIdxCompensation);

    auto*  in  = frequencyDomainBuffer.getData();
    // Compiler should be able to vectorize it
    for (int i = absLowIdx; i < absHighIdx; ++i)
    {
        const auto& c = in[i];
        const float re = c.real();
        const float im = c.imag();
        Abs2FFTVec[i] = re * re + im * im;
    }

    // Find all peaks
    for (int idx = lowFreqIdx; idx < HighFreqIdx; idx++)
    {
        const float currentAbs = Abs2FFTVec[idx];
        if (Abs2FFTVec[idx + windowSmoothIdxCompensation]*Prominence < currentAbs &&
            Abs2FFTVec[idx - windowSmoothIdxCompensation]*Prominence < currentAbs)
        {
            
            FreqList[NumberOfPeaks] = LogFreqVector[idx];
            AmpList[NumberOfPeaks]  = currentAbs;
            FreqIntList[NumberOfPeaks] = idx;
            NumberOfPeaks +=1;
            idx += MinPeakDistance-1;
            
            if(MaxPeak<currentAbs)
                MaxPeak = currentAbs;
        }

    }

    
    // Calculate amplitude transfer function
    const float logW2 = logW*logW;
    const float CurrentAmp = 1-PeakReductionLin;
    const float Var = logW2;
    const float MinAcceptableValue = 1;
    for(int idx=0;idx<NumberOfPeaks;idx++)
    {
        if(MaxPeak*0.01>AmpList[idx] || MaxPeak<MinAcceptableValue) // Take only peaks at a threshold from maximum
            continue;
        
        const float CurrentFreq = FreqList[idx];
        const int CurrentPeakIdx = FreqIntList[idx];
        
        // Update only a local region for each peak
        const float localDf =LogFreqVector[CurrentPeakIdx+1]-LogFreqVector[CurrentPeakIdx];
        const int interval = static_cast<int>(   40*Var/localDf  ); // 40 works fine
        const int startIdxF = std::max( FreqIntList[idx] - interval,1);
        const int endIdxF   =  std::min( FreqIntList[idx] + interval, halfFFTSize );
        
        for(int idxF = startIdxF; idxF < endIdxF; idxF++)
        {
            const auto diff =LogFreqVector[idxF]-CurrentFreq;
            TF[idxF] *= (1-CurrentAmp*fast_exp(-( ( diff*diff ))/Var));
        }

    }
    
// Time smoothing
for(int idx = 1; idx < halfFFTSize; idx++)
{
    // Exponential moving average for TF
    TF[idx] = (TF[idx])*(1-AverageRatioTF)+TFOld[idx]*AverageRatioTF;
    
    TFOld[idx] = TF[idx];
    
    TFreal[idx] =  TF[idx];

    // Clean array for next iteration
    TF[idx] =1 ;
}
    
    
// ####### Filter the signal
    
    if(OnlyDiff)
    {
        for(int idx = 1; idx < halfFFTSize; idx++)
        {
            const auto gain = makeUp * (1.0f - TFreal[idx]);
            frequencyDomainBuffer[idx]           *=gain;
            frequencyDomainBuffer[fftSize - idx] *=gain;

            TFrealOld[idx] = TFreal[idx];
        }
    }
    else
    {
        const auto oneMinusWet = 1 - Wet;
        
        
        for(int idx = 1; idx < halfFFTSize; idx++)
        {
            
            auto Fac = TFreal[idx];
            auto gainPos = makeUp * (Wet * Fac          + oneMinusWet);
            auto gainNeg = makeUp * (Wet * std::conj(Fac) + oneMinusWet);
            
            frequencyDomainBuffer[idx]           *= gainPos;
            frequencyDomainBuffer[fftSize - idx] *= gainNeg;
            
            TFrealOld[idx] = Fac;

        }
    }
    
 // ########## Get back the time signal ################
    fft->perform (frequencyDomainBuffer, timeDomainBuffer, true);
    
    if (isWOLA)
    {
        for (int index = 0; index < fftSize; ++index) {
            timeDomainBuffer[index] *= fftWindow[index];

        }
    }

}


void DynamicEqualizer::UpdateParameters(float _Prominence, float _Width,float _PeakReductiondB,
                                        float _makeUpDB, float _Wet,bool _OnlyDiff, float fmin,
                                        float fmax, float speed, float _SamplingRate,bool _passThrough,int _fftSize)
{
    
    if(_fftSize != fftSize)
    {
        fftSize = _fftSize;
        InitializeNewFFTSize();
    }
    
    Prominence         = (10.0-_Prominence)*(10.0-_Prominence);
    Width              = 1.7-_Width;
    logW               = std::log(Width);
    PeakReductionLin   = std::pow(10,_PeakReductiondB/20.);
    makeUp             = std::pow(10,_makeUpDB/20.);
    SamplingRate       =  _SamplingRate;
    OnlyDiff           = _OnlyDiff;
    Wet                = _Wet;
    AverageRatioTF     = 1-speed;
    passThrough        = _passThrough;
    const float df     = SamplingRate/fftSize;
    lowFreqIdx         = std::fmax(3, static_cast<int>(fmin/df)  );
    HighFreqIdx        = std::fmin(  fftSize/2, static_cast<int>(fmax/df)  );
    


    

    

    
}
