/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "SpectrumComponent.h"

//==============================================================================
SpectrumComponent::SpectrumComponent (DynamicEqualizerAudioProcessor& p)
    :  audioProcessor (p) //AudioProcessorEditor (&p)
{
    //addAndMakeVisible(ComponentSpec);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    startTimerHz (30);
    setSize (700, 400);
    mouseXPosition = -1;
    mouseYPosition = -1;
    
    logIdx = std::make_unique<float[]>(16384);
    for(int idx=1;idx<16384;idx++)
        logIdx[idx] = std::log(idx);

    PlotObject.initialize( static_cast<int>(std::log2(2048))   ); 

}

SpectrumComponent::~SpectrumComponent()
{
}

//==============================================================================
void SpectrumComponent::paint (juce::Graphics& g)
{
    if(audioProcessor.FFTSize != PlotObject.fftSize)
        PlotObject.initialize(audioProcessor.FFTSize);
        
    {
        juce::ScopedLock lock(audioProcessor.processingLock);
        const int plotSamples = audioProcessor.FFTSize;
        if (audioProcessor.bufferToPlot != nullptr && plotSamples > 0)
            PlotObject.getNextAudioBlock(audioProcessor.bufferToPlot.get(), plotSamples);
    }

    if(PlotObject.nextFFTBlockReady)
    {
        PlotObject.drawNextLineOfSpectrogram();
        SpectogramImageProcessor = PlotObject.spectrogramImage;
        PlotObject.nextFFTBlockReady = false;
        
    }

    
    //g.fillAll(juce::Colours::white);
    g.fillAll(juce::Colour::fromRGBA(ColorBackground[0] ,ColorBackground[1],
                                           ColorBackground[2],ColorBackground[3]));

    switch (PlotObject.PlotType)
    {
        case(SpectrumPlotType::Spectogram):
        {
            g.drawImageAt(SpectogramImageProcessor, 0, 0);
            break;
        }
        case(SpectrumPlotType::Spectrum):
        {
            
            
            int scopeSize = PlotObject.ScopeSize;
            auto width    = getLocalBounds().getWidth();
            auto height   = getLocalBounds().getHeight();
            
            // Create the relative index of the plot for a logarithmic image
            
            
            float LogStep    = (PosEnd-PosStart)/width;
            float df         = audioProcessor.getSampleRate()/(audioProcessor.DynamicResonator.getFFTSize());//STFTPlot.fftSize);
            float logDf      = std::log(df);
            
            
            g.setFont (12.0f);
            
            for (int idx=0;idx<9;idx++)
            {
                // Frequency
                float ExactXpos = ( FrequencyPosLabelsLog[idx]-PosStart)/(LogStep);
                int xPos =(int) ExactXpos;
                g.setColour (juce::Colour::fromRGBA (ColorText[0],ColorText[1],ColorText[2], ColorText[3]));
                g.drawText(FrequencyLabels[idx], xPos+2, 0, 40, 20, juce::Justification::left);
                
                // Level
                int yPos = (int)((float)height/10.0*idx);
                g.drawText(AmplitudeLabels[idx], 10, yPos, 40, 20, juce::Justification::left);
                g.setColour (juce::Colour::fromRGBA (ColorLines[0],ColorLines[1],ColorLines[2], ColorLines[3]));
                g.drawHorizontalLine(yPos, 0, width);
                
            }
            
            
            g.setColour (juce::Colour::fromRGBA (ColorLines[0],ColorLines[1],ColorLines[2], ColorLines[3]));
            for (int idx=0;idx<27;idx++)
            {
                float ExactXpos = ( FrequencyLinesLog[idx]-PosStart)/(LogStep);
                int xPos =(int) ExactXpos;
                
                g.drawVerticalLine(xPos, 0, height); //width      // height
                
            }
            
            // Add a vertical line where the mouse is
            // Check if within bounds
            if ( (mouseXPosition>0 && mouseXPosition<width) && (mouseYPosition>0 && mouseYPosition<height))
            {
                // Dicker vertical line
                g.setColour (juce::Colour::fromRGBA (ColorCursor[0],ColorCursor[1],ColorCursor[2], 100));
                g.drawVerticalLine(mouseXPosition, 0, height);
                
                // Frequency text
                g.setFont (15.0f);
                float FrequencyMouse = std::exp(  PosStart+ LogStep*mouseXPosition); //
                g.setColour (juce::Colour::fromRGBA (ColorCursor[0],ColorCursor[1],ColorCursor[2], ColorCursor[3]));
                int YmousePlos = std::fmax(15,mouseYPosition-10);
                if(mouseXPosition<width*0.5)
                    g.drawText(std::to_string((int) FrequencyMouse)+" Hz", mouseXPosition+10, YmousePlos, 120, 20, juce::Justification::left);
                else
                    g.drawText(std::to_string((int) FrequencyMouse)+" Hz", mouseXPosition-60, YmousePlos, 120, 20, juce::Justification::left);
                
            }
            
            // Plot fmin and fmax
            
            int ExactXposMin = (int)( (std::log(audioProcessor.Fmin)-PosStart)/(LogStep) );
            int ExactXposMax = (int)( (std::log(audioProcessor.Fmax)-PosStart)/(LogStep) );
            g.setColour (juce::Colour::fromRGBA (ColorCursor[0],ColorCursor[1],ColorCursor[2], 255));
            g.drawVerticalLine(ExactXposMin, 0, height);
            g.drawVerticalLine(ExactXposMax, 0, height);
            
            // Start plotting spectrum
            
            juce::Path path; // Spectrum path
            path.startNewSubPath (juce::Point<float> ((float) juce::jmap (1, 0, scopeSize - 1, 0, width),
                                  juce::jmap (PlotObject.scopeData[0], 0.0f, 1.0f, (float) height, 0.0f) ) );
            for (int i = 2; i < scopeSize; i++)
            {
                float factual          = logIdx[i] + logDf;//i*df;
                // Freq to pixel "function"
                float XPixelValue      =  std::fmin(width, (factual-PosStart)/(LogStep) ) ;
                
                path.lineTo (juce::Point<float> (XPixelValue,
                             juce::jmap (PlotObject.scopeData[i], 0.0f, 1.0f, (float) height, 0.0f) ));
                
                if((width-XPixelValue) <0) // In case frequency above wished max frequency
                    break;
            }
            // Close path completely (Path is always automatically closed, so add low right and low left points at the end so it can render properly)
            path.lineTo (juce::Point<float> ((float) juce::jmap (scopeSize, 0, scopeSize - 1, 0, width),
                         juce::jmap (0.0f, 0.0f, 1.0f, (float) height, 0.0f) ));
            path.lineTo (juce::Point<float> ((float) juce::jmap (0, 0, scopeSize - 1, 0, width),
                         juce::jmap (0.0f, 0.0f, 1.0f, (float) height, 0.0f) ));
            
            path.closeSubPath();
            
            // Draw some glowing effect because "why not?"
            juce::DropShadow glowShadow;
            glowShadow.colour = juce::Colour(0, 255, 255).withAlpha(0.3f);
            glowShadow.radius = 15; // The "spread" of the glow
            glowShadow.offset = { 0, 0 }; // Glows in all directions equally

            glowShadow.drawForPath(g, path);
            
            float lineThickness = 2.0f;
            
            g.setColour (juce::Colour::fromRGBA (ColorSpectrumLine[0],ColorSpectrumLine[1],
                                                      ColorSpectrumLine[2],ColorSpectrumLine[3]));
            g.strokePath (path, juce::PathStrokeType(lineThickness));

            g.setColour (juce::Colour::fromRGBA (ColorSpectrumSurface[0],ColorSpectrumSurface[1],
                                                      ColorSpectrumSurface[2], ColorSpectrumSurface[3]));
            g.fillPath (path);
            
            break;
        }
            
            
    }
    

}

void SpectrumComponent::mouseMove(const juce::MouseEvent &event)
{
    mouseXPosition = event.getPosition().getX();
    mouseYPosition =event.getPosition().getY();
}

void SpectrumComponent::mouseDown(const juce::MouseEvent &event)
{
    int Xpos = event.getPosition().getX();
    //int Ypos = event.getPosition().getY();
    
    
    auto width    = getLocalBounds().getWidth();
    float LogStep    = (PosEnd-PosStart)/width;
    
    float Frequency = std::exp(  PosStart+ LogStep*Xpos);
    
    bool IsFmin = event.mods.isLeftButtonDown();
    audioProcessor.FrequencyLimitsFromPlot(Frequency,IsFmin);
    
}

void SpectrumComponent::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
}

void SpectrumComponent::timerCallback()
{
        repaint();
}

void SpectrumComponent::repaintCallback()
{
        repaint();
}
