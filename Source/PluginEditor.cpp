#include "PluginProcessor.h"
#include "PluginEditor.h"

DynResoSuppressorEditor::DynResoSuppressorEditor(DynResoSuppressorProcessor& p)
    : AudioProcessorEditor(&p), audio_processor(p), Spectrum(p)  // order matches declaration
{
    setSize(GUI_WIDTH, GUI_HEIGHT);
    setLookAndFeel(&look_and_feel);

    // --- Title bar ---
    label_title.setText("Dy-Reso-Supress", juce::dontSendNotification);
    label_title.setFont(look_and_feel.getHackFont(26.0f).boldened());
    label_title.setColour(juce::Label::textColourId, juce::Colours::white);

    label_subtitle.setText("DYNAMIC RESONANCE SUPPRESSOR", juce::dontSendNotification);
    label_subtitle.setFont(look_and_feel.getHackFont(11.0f));
    label_subtitle.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.35f));

    // --- Rotary knobs ---
    auto make_rotary = [](juce::Slider& s) {
        s.setSliderStyle(juce::Slider::Rotary);
        s.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    };

    make_rotary(slider_sensibility);
    make_rotary(slider_sharpness);
    make_rotary(slider_speed);
    make_rotary(slider_wet);
    make_rotary(slider_makeup);

    slider_gain.setSliderStyle(juce::Slider::Rotary);
    slider_gain.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);

    // --- Linear sliders ---
    slider_fmin.setSliderStyle(juce::Slider::LinearHorizontal);
    slider_fmin.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    slider_fmax.setSliderStyle(juce::Slider::LinearHorizontal);
    slider_fmax.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);

    // --- Labels ---
    auto make_label = [](juce::Label& l, const juce::String& text) {
        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centredTop);
        l.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.4f));
    };

    make_label(label_sensibility, "SENSITIVITY");
    make_label(label_sharpness,   "SHARPNESS");
    make_label(label_gain,        "RESONANCE GAIN");
    make_label(label_wet,         "WET");
    make_label(label_makeup,      "MAKEUP");
    make_label(label_speed,       "SPEED");
    make_label(label_fmin,        "START FREQ");
    make_label(label_fmax,        "END FREQ");

    label_credits.setText("V 0.3  Thiago Lobato", juce::dontSendNotification);
    label_credits.setColour(juce::Label::textColourId, juce::Colours::white.withAlpha(0.25f));
    label_credits.setJustificationType(juce::Justification::centredLeft);

    // --- Buttons ---
    btn_diff.setButtonText("DIFF");
    btn_pass.setButtonText("PASS");

    // --- Combo ---
    combo_resolution.addItemList({"FFT 1024", "FFT 2048", "FFT 4096", "FFT 8192", "FFT 16384"}, 1);

    // --- Visibility ---
    addAndMakeVisible(label_title);
    addAndMakeVisible(label_subtitle);
    addAndMakeVisible(btn_diff);
    addAndMakeVisible(btn_pass);
    addAndMakeVisible(combo_resolution);
    addAndMakeVisible(Spectrum);
    addAndMakeVisible(slider_gain);    addAndMakeVisible(label_gain);
    addAndMakeVisible(slider_sensibility); addAndMakeVisible(label_sensibility);
    addAndMakeVisible(slider_sharpness);   addAndMakeVisible(label_sharpness);
    addAndMakeVisible(slider_speed);       addAndMakeVisible(label_speed);
    addAndMakeVisible(slider_makeup);      addAndMakeVisible(label_makeup);
    addAndMakeVisible(slider_wet);         addAndMakeVisible(label_wet);
    addAndMakeVisible(slider_fmin);        addAndMakeVisible(label_fmin);
    addAndMakeVisible(slider_fmax);        addAndMakeVisible(label_fmax);
    addAndMakeVisible(label_credits);

    // --- Attachments ---
    att_sensibility = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audio_processor.apvts, "prominence", slider_sensibility);
    att_sharpness   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audio_processor.apvts, "width",      slider_sharpness);
    att_gain        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audio_processor.apvts, "gain",       slider_gain);
    att_wet         = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audio_processor.apvts, "wet",        slider_wet);
    att_makeup      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audio_processor.apvts, "makeup",     slider_makeup);
    att_speed       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audio_processor.apvts, "speed",      slider_speed);
    att_fmin        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audio_processor.apvts, "fmin",       slider_fmin);
    att_fmax        = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audio_processor.apvts, "fmax",       slider_fmax);
    att_diff        = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audio_processor.apvts, "onlyDiff",   btn_diff);
    att_pass        = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audio_processor.apvts, "passThrough",btn_pass);
    att_resolution  = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audio_processor.apvts, "fftsize", combo_resolution);
}

DynResoSuppressorEditor::~DynResoSuppressorEditor()
{
    setLookAndFeel(nullptr);
}

void DynResoSuppressorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromRGB(15, 15, 16));

    // Top bar background
    g.setColour(juce::Colour::fromRGB(22, 22, 23));
    g.fillRect(0, 0, getWidth(), TOP_BAR_H);

    // Top bar bottom border
    g.setColour(juce::Colours::white.withAlpha(0.05f));
    g.drawHorizontalLine(TOP_BAR_H, 0.0f, (float)getWidth());
}

void DynResoSuppressorEditor::resized()
{
    auto full = getLocalBounds();

    // --- Top bar ---
    auto top_bar = full.removeFromTop(TOP_BAR_H);

    // Title block on left
    auto title_block = top_bar.removeFromLeft(350).reduced(12, 0);
    label_title.setBounds(title_block.removeFromTop(34).withTrimmedTop(10));
    label_subtitle.setBounds(title_block.removeFromTop(18));

    // Controls on right: [DIFF] [PASS] [FFT combo]
    auto controls = top_bar.removeFromRight(300).reduced(0, 14);
    combo_resolution.setBounds(controls.removeFromRight(130).reduced(4, 0));
    btn_pass.setBounds(controls.removeFromRight(60).reduced(4, 0));
    btn_diff.setBounds(controls.removeFromRight(60).reduced(4, 0));

    // --- Left panel ---
    auto panel = full.removeFromLeft(350);

    // --- Spectrum (takes the rest) ---
    Spectrum.setBounds(full);

    auto place_rotary = [](juce::Rectangle<int> r, juce::Slider& s, juce::Label& l) {
        l.setBounds(r.removeFromBottom(24));
        s.setBounds(r);
    };

    // Row 1 — Resonance Gain (big, centred)
    auto row1 = panel.removeFromTop((int)(panel.getHeight() * 0.35f));
    {
        int knob_w = juce::jmin(row1.getWidth(), row1.getHeight() - 10);
        auto gain_bounds = row1.withSizeKeepingCentre(knob_w, row1.getHeight()).reduced(8);
        place_rotary(gain_bounds, slider_gain, label_gain);
    }

    // Row 2 — Sensitivity & Sharpness
    auto row2 = panel.removeFromTop((int)(panel.getHeight() * 0.28f));
    {
        int w = row2.getWidth() / 2;
        place_rotary(row2.removeFromLeft(w).reduced(16, 6), slider_sensibility, label_sensibility);
        place_rotary(row2.reduced(16, 6), slider_sharpness, label_sharpness);
    }

    // Row 3 — Makeup & Wet
    auto row3 = panel.removeFromTop((int)(panel.getHeight() * 0.38f));
    {
        int w = row3.getWidth() / 2;
        place_rotary(row3.removeFromLeft(w).reduced(16, 6), slider_makeup, label_makeup);
        place_rotary(row3.reduced(16, 6), slider_wet, label_wet);
    }

    // Row 4 — Speed (Centered)
    auto row4 = panel.removeFromTop(60);
    {
        int w = row4.getWidth() / 2;
        auto speed_bounds = row4.withSizeKeepingCentre(w, row4.getHeight()).reduced(0, 6);
        place_rotary(speed_bounds, slider_speed, label_speed);
    }

    // Row 5 — Fmin / Fmax sliders at the bottom
    auto row5 = panel;
    {
        auto fmin_row = row5.removeFromTop(row5.getHeight() / 2).reduced(10, 0);
        label_fmin.setBounds(fmin_row.removeFromTop(12));
        slider_fmin.setBounds(fmin_row);

        auto fmax_row = row5.reduced(10, 0);
        label_fmax.setBounds(fmax_row.removeFromTop(12));
        slider_fmax.setBounds(fmax_row);
    }
}

void DynResoSuppressorEditor::parentHierarchyChanged()
{
    if (auto* peer = getPeer())
    {
        peer->setCurrentRenderingEngine(0); // 0 corresponds to the software renderer
    }
}
