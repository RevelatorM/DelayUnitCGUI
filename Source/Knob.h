/*
  ==============================================================================

    Knob.h
    Created: 7 Mar 2025 1:08:20pm
    Author:  Defus

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class myLookAndFeeKB1 : public juce::LookAndFeel_V4
{
public:
    myLookAndFeeKB1();

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;

private:
    juce::Image img1;

};
