#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SpectrumComponent.h"
#include "StyleSheet.h"

class DynResoSuppressorEditor : public juce::AudioProcessorEditor
{
public:
    DynResoSuppressorEditor(DynResoSuppressorProcessor&);
    ~DynResoSuppressorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void parentHierarchyChanged() override;

    // Must be declared before Spectrum so it is initialised first
    DynResoSuppressorProcessor& audio_processor;
    SpectrumComponent Spectrum;

private:
    juce::Slider slider_sensibility;
    juce::Slider slider_sharpness;
    juce::Slider slider_gain;
    juce::Slider slider_wet;
    juce::Slider slider_makeup;
    juce::Slider slider_fmin;
    juce::Slider slider_fmax;
    juce::Slider slider_speed;

    juce::ToggleButton btn_diff;
    juce::ToggleButton btn_pass;
    juce::ComboBox     combo_resolution;

    juce::Label label_title;
    juce::Label label_subtitle;
    juce::Label label_sensibility;
    juce::Label label_sharpness;
    juce::Label label_gain;
    juce::Label label_wet;
    juce::Label label_makeup;
    juce::Label label_speed;
    juce::Label label_fmin;
    juce::Label label_fmax;
    juce::Label label_credits;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   att_sensibility;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   att_sharpness;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   att_gain;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   att_wet;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   att_makeup;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   att_fmin;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   att_fmax;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>   att_speed;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   att_diff;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>   att_pass;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> att_resolution;

    CustomLookAndFeel look_and_feel;

    static constexpr int GUI_WIDTH  = 1200;
    static constexpr int GUI_HEIGHT = 400;
    static constexpr int TOP_BAR_H  = 60;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DynResoSuppressorEditor)
};
