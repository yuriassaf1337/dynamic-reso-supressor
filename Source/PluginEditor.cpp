/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DynResoSuppressorEditor::DynResoSuppressorEditor (DynResoSuppressorProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),Spectrum(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    //setSize (1200, 400);
    setSize (GUI_WIDTH, GUI_HEIGHT);
    
    // Apply our custom style to the whole editor
    setLookAndFeel(&LookAndFeel);
    
    // add objects
    sliderSensibility.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderSensibility.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    labelSensibility.setText("Sensitivity", juce::NotificationType::dontSendNotification);
    labelSensibility.setJustificationType(juce::Justification::centredTop);
    
    sliderSharpness.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderSharpness.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    labelSharpness.setText("Sharpness", juce::NotificationType::dontSendNotification);
    labelSharpness.setJustificationType(juce::Justification::centredTop);
    
    
    sliderGain.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderGain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    labelGain.setText("Resonance Gain dB", juce::NotificationType::dontSendNotification);
    labelGain.setJustificationType(juce::Justification::centredTop);
    
    sliderWet.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderWet.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    labelWet.setText("Wet", juce::NotificationType::dontSendNotification);
    labelWet.setJustificationType(juce::Justification::centredTop);
    
    
    sliderMakeUp.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderMakeUp.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    labelMakeUp.setText("MakeUp", juce::NotificationType::dontSendNotification);
    labelMakeUp.setJustificationType(juce::Justification::centredTop);
    
    sliderFmin.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    sliderFmin.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    
    labelsliderFmin.setText("Frequency to start filters", juce::NotificationType::dontSendNotification);
    labelsliderFmin.setJustificationType(juce::Justification::centredTop);
    
    sliderFmax.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    sliderFmax.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    labelsliderFmax.setText("Frequency to stop filters", juce::NotificationType::dontSendNotification);
    labelsliderFmax.setJustificationType(juce::Justification::centredTop);
    
    
    sliderSpeed.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderSpeed.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    labelsliderSpeed.setText("Speed", juce::NotificationType::dontSendNotification);
    labelsliderSpeed.setJustificationType(juce::Justification::centredTop);
    
    
    ComboBoxResolution.addItemList(juce::StringArray {"1024","2048","4096","8192","16384"}, 1);
    labelResolution.setText("FFT size (resolution)",juce::NotificationType::dontSendNotification);
    labelResolution.setJustificationType(juce::Justification::centredTop);
    
    
    ButtonPassThrough.setButtonText("Passthrough");

    
    // Button labels
    ButtonHearDiff.setButtonText("Difference");
    
    labelsliderFmin.setText("Start Frequency", juce::NotificationType::dontSendNotification);
    labelsliderFmax.setText("End Frequency", juce::NotificationType::dontSendNotification);
    
    labelCredits.setText("DyERS V 0.3 by Thiago Lobato", juce::NotificationType::dontSendNotification);
    labelCredits.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(labelCredits);
    // Make it visible
    addAndMakeVisible(Spectrum);
    addAndMakeVisible(sliderSensibility); addAndMakeVisible(labelSensibility);
    addAndMakeVisible(sliderSharpness); addAndMakeVisible(labelSharpness);
    addAndMakeVisible(sliderGain); addAndMakeVisible(labelGain);
    addAndMakeVisible(sliderWet); addAndMakeVisible(labelWet);
    addAndMakeVisible(sliderMakeUp); addAndMakeVisible(labelMakeUp);
    addAndMakeVisible(ButtonHearDiff); addAndMakeVisible(labelHearDiff);
    addAndMakeVisible(sliderSpeed); addAndMakeVisible(labelsliderSpeed);
    addAndMakeVisible(ComboBoxResolution); addAndMakeVisible(labelResolution);
    addAndMakeVisible(ButtonPassThrough); addAndMakeVisible(labelPassThrough);

    // Add atachments
    sliderSensibilityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"prominence",sliderSensibility);
    sliderSharpnessAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"width",sliderSharpness);
    sliderGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"gain",sliderGain);
    sliderWetAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"wet",sliderWet);
    sliderMakeUpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"makeup",sliderMakeUp);
    HearDiffAttachment    = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts,"onlyDiff",ButtonHearDiff);
    PassThroughAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts,"passThrough",ButtonPassThrough);
    sliderMakeUpAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"makeup",sliderMakeUp);
    sliderFminAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"fmin",sliderFmin);
    sliderFmaxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"fmax",sliderFmax);
    sliderSpeedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts,"speed",sliderSpeed);
    ResolutionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts,"fftsize",ComboBoxResolution);
        
}

DynResoSuppressorEditor::~DynResoSuppressorEditor()
{
    setLookAndFeel(nullptr); // Important! Detach before destruction
}

//==============================================================================
void DynResoSuppressorEditor::paint (juce::Graphics& g)
{
    // Component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillAll (juce::Colour::fromRGB(30, 30, 30));
}

void DynResoSuppressorEditor::resized()
{
    
    auto area = getLocalBounds();

    // 1. Place Spectrum on the Right
    Spectrum.setBounds(area.removeFromRight((int)(area.getWidth() * 0.6)));

    // 2. Helper lambda
    auto placeControl = [this](juce::Rectangle<int> r, juce::Slider& slider, juce::Label& label)
    {
        label.setBounds(r.removeFromBottom(20));
        slider.setBounds(r);
    };

    // 3. Setup FlexBox
    juce::FlexBox mainColumn;
    mainColumn.flexDirection = juce::FlexBox::Direction::column;
    
    // --- ROW 1 ---
    juce::FlexItem topRowItem;
    topRowItem.flexGrow = 2.0f;
    mainColumn.items.add(topRowItem.withMargin(5));

    // --- ROW 2 ---
    juce::FlexItem midRowItem;
    midRowItem.flexGrow = 1.5f;
    mainColumn.items.add(midRowItem.withMargin(5));

    // --- ROW 3 ---
    juce::FlexItem botRowItem;
    botRowItem.flexGrow = 1.25f;
    mainColumn.items.add(botRowItem.withMargin(5));

    // 4. Perform Layout
    // Note: area is currently int, but performLayout handles the conversion internally
    mainColumn.performLayout(area);
    
    // === ROW 1 LOGIC ===
    // FIX: Convert 'currentBounds' (float) to 'int' using .toNearestInt()
    auto topRowBounds = mainColumn.items[0].currentBounds.toNearestInt().reduced(5);
    
    auto utilsBounds = topRowBounds.removeFromRight(topRowBounds.getWidth() / 3);
    auto gainBounds  = topRowBounds;
    
    placeControl(gainBounds, sliderGain, labelGain);

    // Utilities
    int utilHeight = utilsBounds.getHeight() / 3;
    
    auto resRow = utilsBounds.removeFromTop(utilHeight);
    labelResolution.setBounds(resRow.removeFromBottom(25));
    ComboBoxResolution.setBounds(resRow);
    
    auto hearRow = utilsBounds.removeFromTop(utilHeight);
    labelHearDiff.setBounds(hearRow.removeFromRight(50));
    ButtonHearDiff.setBounds(hearRow);
    
    labelPassThrough.setBounds(utilsBounds.removeFromRight(50));
    ButtonPassThrough.setBounds(utilsBounds);

    // === ROW 2 LOGIC ===
    auto midRowBounds = mainColumn.items[1].currentBounds.toNearestInt().reduced(5);
    
    int sliderWidth = midRowBounds.getWidth() / 3;
    
    placeControl(midRowBounds.removeFromLeft(sliderWidth).reduced(5), sliderSensibility, labelSensibility);
    placeControl(midRowBounds.removeFromLeft(sliderWidth).reduced(5), sliderSharpness, labelSharpness);
    placeControl(midRowBounds.reduced(5), sliderSpeed, labelsliderSpeed);

    // === ROW 3 LOGIC ===
    auto botRowBounds = mainColumn.items[2].currentBounds.toNearestInt().reduced(5);
    
    auto outputSection = botRowBounds.removeFromRight(200);
    int outKnobWidth = outputSection.getWidth() / 2;
    
    placeControl(outputSection.removeFromLeft(outKnobWidth).reduced(5), sliderMakeUp, labelMakeUp);
    placeControl(outputSection.reduced(5), sliderWet, labelWet);

    labelCredits.setBounds(botRowBounds);
    labelCredits.setJustificationType(juce::Justification::centredLeft);

}
