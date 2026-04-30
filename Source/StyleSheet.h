#pragma once
#include <JuceHeader.h>
#include <BinaryData.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        hack_typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::HackRegular_ttf,
            BinaryData::HackRegular_ttfSize
        );

        setColour(juce::Slider::thumbColourId,               juce::Colours::cyan);
        setColour(juce::Slider::rotarySliderFillColourId,    juce::Colours::cyan);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white.withAlpha(0.1f));

        setColour(juce::ComboBox::backgroundColourId,               juce::Colours::black.withAlpha(0.4f));
        setColour(juce::ComboBox::outlineColourId,                  juce::Colours::white.withAlpha(0.15f));
        setColour(juce::ComboBox::textColourId,                     juce::Colours::white);
        setColour(juce::ComboBox::arrowColourId,                    juce::Colours::cyan);
        setColour(juce::PopupMenu::backgroundColourId,              juce::Colours::black.withAlpha(0.85f));
        setColour(juce::PopupMenu::textColourId,                    juce::Colours::white);
        setColour(juce::PopupMenu::highlightedBackgroundColourId,   juce::Colours::cyan.withAlpha(0.25f));
        setColour(juce::PopupMenu::highlightedTextColourId,         juce::Colours::white);

        setColour(juce::TextButton::buttonColourId,   juce::Colour::fromRGB(45, 45, 45));
        setColour(juce::TextButton::buttonOnColourId, juce::Colours::cyan.withAlpha(0.85f));
        setColour(juce::TextButton::textColourOffId,  juce::Colours::white.withAlpha(0.7f));
        setColour(juce::TextButton::textColourOnId,   juce::Colour::fromRGB(10, 10, 10));

        // Slider text box styling
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::cyan);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxHighlightColourId, juce::Colours::cyan.withAlpha(0.3f));
    }

    juce::Font getHackFont(float height = 15.0f) const
    {
        return juce::Font(juce::FontOptions().withTypeface(hack_typeface).withHeight(height));
    }

    juce::Font getLabelFont(juce::Label&) override
    {
        return getHackFont(13.0f);
    }

    juce::Font getComboBoxFont(juce::ComboBox&) override
    {
        return getHackFont(12.0f);
    }

    juce::Font getTextButtonFont(juce::TextButton&, int) override
    {
        return getHackFont(12.0f);
    }

    juce::Font getPopupMenuFont() override
    {
        return getHackFont(13.0f);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider&) override
    {
        auto radius  = (float)juce::jmin(width / 2, height / 2) - 4.0f;
        auto centreX = (float)x + (float)width  * 0.5f;
        auto centreY = (float)y + (float)height * 0.5f;
        auto angle   = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        g.setColour(findColour(juce::Slider::rotarySliderOutlineColourId));
        juce::Path bg_arc;
        bg_arc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.strokePath(bg_arc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
        juce::Path val_arc;
        val_arc.addCentredArc(centreX, centreY, radius, radius, 0.0f, rotaryStartAngle, angle, true);
        g.strokePath(val_arc, juce::PathStrokeType(3.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        const float thumb_r = 4.0f;
        juce::Point<float> thumb(centreX + radius * std::cos(angle - juce::MathConstants<float>::halfPi),
                                 centreY + radius * std::sin(angle - juce::MathConstants<float>::halfPi));
        g.setColour(juce::Colours::white);
        g.fillEllipse(thumb.x - thumb_r, thumb.y - thumb_r, thumb_r * 2.0f, thumb_r * 2.0f);
    }

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& btn,
                          bool, bool) override
    {
        auto bounds = btn.getLocalBounds().toFloat().reduced(1.0f);
        const bool on = btn.getToggleState();

        g.setColour(on ? findColour(juce::TextButton::buttonOnColourId)
                       : findColour(juce::TextButton::buttonColourId));
        g.fillRoundedRectangle(bounds, 4.0f);

        g.setColour(juce::Colours::white.withAlpha(0.15f));
        g.drawRoundedRectangle(bounds.reduced(0.5f), 4.0f, 1.0f);

        g.setColour(on ? findColour(juce::TextButton::textColourOnId)
                       : findColour(juce::TextButton::textColourOffId));
        g.setFont(getHackFont(11.5f));
        g.drawText(btn.getButtonText(), btn.getLocalBounds(), juce::Justification::centred, false);
    }

    void drawComboBox(juce::Graphics& g, int width, int height, bool,
                      int, int, int, int, juce::ComboBox& box) override
    {
        auto bounds  = juce::Rectangle<int>(0, 0, width, height).toFloat();
        auto bg      = box.findColour(juce::ComboBox::backgroundColourId);
        auto outline = box.findColour(juce::ComboBox::outlineColourId);
        auto arrow   = box.findColour(juce::ComboBox::arrowColourId);

        g.setColour(bg);
        g.fillRoundedRectangle(bounds, 6.0f);

        g.setColour(outline);
        g.drawRoundedRectangle(bounds.reduced(0.5f), 6.0f, 1.0f);

        auto arrow_area = bounds.removeFromRight(18.0f).reduced(4.0f);
        juce::Path p;
        p.startNewSubPath(arrow_area.getCentreX() - 4.0f, arrow_area.getCentreY() - 2.0f);
        p.lineTo        (arrow_area.getCentreX() + 4.0f, arrow_area.getCentreY() - 2.0f);
        p.lineTo        (arrow_area.getCentreX(),         arrow_area.getCentreY() + 3.5f);
        p.closeSubPath();

        g.setColour(arrow);
        g.fillPath(p);
    }

    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override
    {
        label.setBounds(box.getLocalBounds().reduced(8, 2));
        label.setFont(getHackFont(12.0f));
    }

private:
    juce::Typeface::Ptr hack_typeface;
};
