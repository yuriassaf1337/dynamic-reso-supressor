/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DynamicEqualizerAudioProcessor::DynamicEqualizerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true) //juce::AudioChannelSet::stereo()
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                     #endif
                       ),apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
    
    // Add listener
    apvts.addParameterListener("prominence",this);
    apvts.addParameterListener("onlyDiff",this);
    apvts.addParameterListener("wet",this);
    apvts.addParameterListener("gain",this);
    apvts.addParameterListener("width",this);
    apvts.addParameterListener("makeup",this);
    apvts.addParameterListener("fmin",this);
    apvts.addParameterListener("fmax",this);
    apvts.addParameterListener("speed",this);
    apvts.addParameterListener("passThrough", this);
    apvts.addParameterListener("fftsize", this);
}

DynamicEqualizerAudioProcessor::~DynamicEqualizerAudioProcessor()
{
    apvts.removeParameterListener ("prominence", this);
    apvts.removeParameterListener ("onlyDiff", this);
    apvts.removeParameterListener ("wet", this);
    apvts.removeParameterListener ("gain", this);
    apvts.removeParameterListener ("width", this);
    apvts.removeParameterListener ("makeup", this);
    apvts.removeParameterListener ("fmin", this);
    apvts.removeParameterListener ("fmax", this);
    apvts.removeParameterListener ("speed", this);
    apvts.removeParameterListener ("passThrough", this);
    apvts.removeParameterListener ("fftsize", this);
}

//==============================================================================
const juce::String DynamicEqualizerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DynamicEqualizerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DynamicEqualizerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DynamicEqualizerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DynamicEqualizerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DynamicEqualizerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DynamicEqualizerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DynamicEqualizerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String DynamicEqualizerAudioProcessor::getProgramName (int index)
{
    return {};
}

void DynamicEqualizerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void DynamicEqualizerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Plot buffers
    int maxFFTSize = 16384;
    bufferToPlot     = std::make_unique<float[]>(maxFFTSize); // Allocate everything beforehand to avoid multiple allocations
    bufferToPlotTemp =std::make_unique<float[]>(maxFFTSize);
    Fs               = sampleRate;
    DynamicResonator.setup (getTotalNumInputChannels());
    // Method from base OLA class
    DynamicResonator.updateParameters(FFTSize,
                                      overlapIdx,
                                      windowType);
}

void DynamicEqualizerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DynamicEqualizerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     //&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()
        )
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DynamicEqualizerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedLock lock(processingLock);
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();
    
    
    // clean channels
    for (int channel = totalNumInputChannels; channel < totalNumOutputChannels; ++channel)
        buffer.clear (channel, 0, numSamples);
    //======================================

    DynamicResonator.processBlock (buffer);

    //======================================
    // Buffer for plot
    const float eps = 1e-8;
    int halfFftSize = DynamicResonator.getFFTSize()/2;
    if(DynamicResonator.OnlyDiff)
    {
        for(int idx=0;idx<halfFftSize;idx++)
        {
            
            PushSampleInBlockForPlot(1./( DynamicResonator.TFOld[idx] +eps));
        }
    }
    else
    {
        for(int idx=0;idx<halfFftSize;idx++)
        {
            
            PushSampleInBlockForPlot(DynamicResonator.TFOld[idx]);
        }
    }





}

void DynamicEqualizerAudioProcessor::PushSampleInBlockForPlot(float sample)
{

    if (N == FFTSize)
    {
        std::copy(bufferToPlotTemp.get(),
                  bufferToPlotTemp.get() + FFTSize,
                  bufferToPlot.get());
        std::fill(bufferToPlotTemp.get(),
                  bufferToPlotTemp.get() + FFTSize,
                  0.0f);
        N = 0;
    }

    bufferToPlotTemp[static_cast<size_t>(N++)] = sample;

}

//==============================================================================
bool DynamicEqualizerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DynamicEqualizerAudioProcessor::createEditor()
{
    return new DynamicEqualizerAudioProcessorEditor (*this);
}

//==============================================================================

void DynamicEqualizerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ValueTree stateCopy = apvts.copyState();
    stateCopy.setProperty ("PlotFmin", Fmin, nullptr);
    stateCopy.setProperty ("PlotFmax", Fmax, nullptr);

    if (auto xml = stateCopy.createXml())
        copyXmlToBinary (*xml, destData);
}

void DynamicEqualizerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState == nullptr)
        return;

    juce::ValueTree stateTree = juce::ValueTree::fromXml (*xmlState);

    Fmin = (float) stateTree.getProperty ("PlotFmin", Fmin);
    Fmax = (float) stateTree.getProperty ("PlotFmax", Fmax);

    apvts.replaceState (stateTree);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DynamicEqualizerAudioProcessor();
}


juce::AudioProcessorValueTreeState::ParameterLayout DynamicEqualizerAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    params.push_back(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID("fftsize",1),
                                                                  "FFTSize",
                                                                  juce::StringArray{"1024","2048","4096","8192","16384"},
                                                                  1
                                                                  ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("prominence",1), // parameterID
                                                                 "Sensitivity", // parameter name
                                                                 1.0f,   // minimum value
                                                                 10.0f,   // maximum value
                                                                 8.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("width",1), // parameterID
                                                                "Sharpness", // parameter name
                                                                0.38f,   // minimum value
                                                                0.68f,   // maximum value  
                                                                0.6f)); // default value
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("gain",1), // parameterID
                                                                "Gain", // parameter name
                                                                -20.0f,   // minimum value
                                                                20.0f,   // maximum value
                                                                -3.0f)); // default value
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("wet",1), // parameterID
                                                                "Wet", // parameter name
                                                                0.0f,   // minimum value
                                                                1.0f,   // maximum value
                                                                1.0f)); // default value
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("makeup",1), //parameterID
                                                                "Make Up gain dB", // parameter name
                                                                -10.0f,   // minimum value
                                                                10.0f,   // maximum value
                                                                0.0f)); // default value
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("speed",1), //parameterID
                                                                "Speed", // parameter name
                                                                0.05f,   // minimum value
                                                                0.9999f,   // maximum value
                                                                0.85f)); // default value
    
    auto normRangeMin = juce::NormalisableRange<float>(150.0f,4000.0f);
    normRangeMin.setSkewForCentre(1000);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fmin",1), // parameterID
                                                                "Fmin", // parameter name
                                                                 normRangeMin,   // maximum value
                                                                150.0f)); // default value

    
    auto normRange = juce::NormalisableRange<float>(5000.0f, 19000.0f);
    normRange.setSkewForCentre(10000);
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("fmax",1), // parameterID
                                                                "Fmax", // parameter name
                                                                 normRange,   // maximum value
                                                                10000.0f)); // default value
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("passThrough",1), // parameterID
                                                                "PassThrough", // parameter name
                                                                false)); // default value
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("onlyDiff",1), // parameterID
                                                                "Only Difference", // parameter name
                                                                false)); // default value
    
    return {params.begin(),params.end()};
}

void DynamicEqualizerAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue)
{
    // How to actually get the paramters from the parameter tree
    auto Pro                = apvts.getRawParameterValue("prominence");
    auto Width              = apvts.getRawParameterValue("width");
    auto gainDB             = apvts.getRawParameterValue("gain");
    auto MakeUp             = apvts.getRawParameterValue("makeup");
    auto OnlyDiff           = apvts.getRawParameterValue("onlyDiff");
    auto Wet                = apvts.getRawParameterValue("wet");
    auto _Speed             = apvts.getRawParameterValue("speed");
    auto _PassThrough       = apvts.getRawParameterValue("passThrough");
    auto fftIdx             = apvts.getRawParameterValue("fftsize");
    
    int newFFTSize = 1024 << static_cast<int>( fftIdx->load() );

    // Lock processing to reasign buffers
    juce::ScopedLock lock(processingLock);
    if (newFFTSize != FFTSize)
    {
        FFTSize = newFFTSize;
        N = 0;
        // Update of OLA superclass
        DynamicResonator.updateParameters(FFTSize,
                                          overlapIdx,
                                          windowType);
        
        // Update of DynamicResonator class
        DynamicResonator.UpdateParameters(Pro->load(), Width->load(),gainDB->load(),MakeUp->load(),Wet->load(), OnlyDiff->load(),Fmin,Fmax,
                             _Speed->load(),Fs,_PassThrough->load(),FFTSize);
        
        
    }
    else
    {

        // Update of DynamicResonator class
        DynamicResonator.UpdateParameters(Pro->load(), Width->load(),gainDB->load(),MakeUp->load(),Wet->load(), OnlyDiff->load(),Fmin,Fmax,
                             _Speed->load(),Fs,_PassThrough->load(),FFTSize);
        
        
    }
}

void DynamicEqualizerAudioProcessor::FrequencyLimitsFromPlot(float Frequency,bool IsFmin)
{
    
    if(IsFmin)
    {
        Fmin = Frequency;
    }
    else
    {
        Fmax = Frequency;
    }
    // How to actually get the paramters from the parameter tree
    auto Pro                = apvts.getRawParameterValue("prominence");
    auto Width              = apvts.getRawParameterValue("width");
    auto gainDB             = apvts.getRawParameterValue("gain");
    auto MakeUp             = apvts.getRawParameterValue("makeup");
    auto OnlyDiff           = apvts.getRawParameterValue("onlyDiff");
    auto Wet                = apvts.getRawParameterValue("wet");
    auto _Speed             = apvts.getRawParameterValue("speed");
    auto _PassThrough       = apvts.getRawParameterValue("passThrough");
    auto fftIdx             = apvts.getRawParameterValue("fftsize");
    
    int newFFTSize = 1024 << static_cast<int>( fftIdx->load() );
    juce::ScopedLock lock(processingLock);
    if (newFFTSize != FFTSize)
    {
        
        FFTSize = newFFTSize;
        N = 0;
        
        DynamicResonator.updateParameters(FFTSize,
                                          overlapIdx,
                                          windowType);
        
        DynamicResonator.UpdateParameters(Pro->load(), Width->load(),gainDB->load(),MakeUp->load(),Wet->load(), OnlyDiff->load(),Fmin,Fmax,_Speed->load(),Fs,_PassThrough->load(),FFTSize);
        
        
    }
    else
    {
        
        DynamicResonator.UpdateParameters(Pro->load(), Width->load(),gainDB->load(),MakeUp->load(),Wet->load(), OnlyDiff->load(),Fmin,Fmax,_Speed->load(),Fs,_PassThrough->load(),FFTSize);
        
        
    }
}
