/*
  ==============================================================================

    StyleSheet.h
    Created: 20 Dec 2025 11:22:49am
    Author:  Thiago Lobato

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::cyan); // The accent color
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::cyan);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white.withAlpha(0.1f));
        
        setColour(juce::ComboBox::backgroundColourId, juce::Colours::black.withAlpha(0.4f));
        setColour(juce::ComboBox::outlineColourId, juce::Colours::white.withAlpha(0.15f));
        setColour(juce::ComboBox::textColourId, juce::Colours::white);
        setColour(juce::ComboBox::arrowColourId, juce::Colours::cyan);
        setColour(juce::PopupMenu::backgroundColourId, juce::Colours::black.withAlpha(0.85f));
        setColour(juce::PopupMenu::textColourId, juce::Colours::white);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::cyan.withAlpha(0.25f));
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::white);
        

    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // 1. Draw the background track (dark grey arc)
        g.setColour (findColour(juce::Slider::rotarySliderOutlineColourId));
        juce::Path backgroundArc;
        backgroundArc.addCentredArc (centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.strokePath (backgroundArc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // 2. Draw the value track (colored arc)
        g.setColour (findColour(juce::Slider::rotarySliderFillColourId));
        juce::Path valueArc;
        valueArc.addCentredArc (centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        g.strokePath (valueArc, juce::PathStrokeType (3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // 3. Draw a dot
        
        auto thumbRadius = 4.0f;
        juce::Point<float> thumbPoint (centreX + radius * std::cos (angle - juce::MathConstants<float>::halfPi),
                                       centreY + radius * std::sin (angle - juce::MathConstants<float>::halfPi));
        g.setColour(juce::Colours::white);
        g.fillEllipse(thumbPoint.x - thumbRadius, thumbPoint.y - thumbRadius, thumbRadius * 2, thumbRadius * 2);
        
    }
    
    void drawComboBox (juce::Graphics& g, int width, int height, bool,
      int, int, int, int, juce::ComboBox& box) override
    {
        auto bounds = juce::Rectangle<int>(0, 0, width, height).toFloat();
        auto bg     = box.findColour(juce::ComboBox::backgroundColourId);
        auto outline= box.findColour(juce::ComboBox::outlineColourId);
        auto arrow  = box.findColour(juce::ComboBox::arrowColourId);

        g.setColour(bg);
        g.fillRoundedRectangle(bounds, 6.0f);

        g.setColour(outline);
        g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);

        auto arrowArea = bounds.removeFromRight(18.0f).reduced(4.0f);
        juce::Path path;
        path.startNewSubPath (arrowArea.getCentreX() - 4.0f, arrowArea.getCentreY() - 2.0f);
        path.lineTo        (arrowArea.getCentreX() + 4.0f, arrowArea.getCentreY() - 2.0f);
        path.lineTo        (arrowArea.getCentreX(),        arrowArea.getCentreY() + 3.5f);
        path.closeSubPath();

        g.setColour(arrow);
        g.fillPath(path);
    }

    void positionComboBoxText (juce::ComboBox& box, juce::Label& label) override
    {
        label.setBounds (box.getLocalBounds().reduced(8, 2));
        label.setFont   (juce::Font (label.getFont().getHeight(), juce::Font::FontStyleFlags::plain));
    }
};
