/*
  ==============================================================================

    Knob.cpp
    Created: 7 Mar 2025 1:08:20pm
    Author:  Defus

  ==============================================================================
*/

#include "Knob.h"
//==============================================================================
myLookAndFeeKB1::myLookAndFeeKB1()
{
    img1 = juce::ImageCache::getFromMemory(BinaryData::knob11_png, BinaryData::knob11_pngSize);
}
//==============================================================================
void myLookAndFeeKB1::drawRotarySlider(juce::Graphics& g,
    int x, int y, int width, int height, float sliderPos,
    float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider)
{

    if (img1.isValid())
    {
        const double rotation = (slider.getValue()
            - slider.getMinimum())
            / (slider.getMaximum()
                - slider.getMinimum());

        const int frames = img1.getHeight() / img1.getWidth();
        const int frameId = (int)ceil(rotation * ((double)frames - 1.0));
        const float radius = juce::jmin(width / 2.0f, height / 2.0f);
        const float centerX = x + width * 0.5f;
        const float centerY = y + height * 0.5f;
        const float rx = centerX - radius - 1.0f;
        const float ry = centerY - radius;

        g.drawImage(img1,
            (int)rx,
            (int)ry,
            2 * (int)radius,
            2 * (int)radius,
            0,
            frameId * img1.getWidth(),
            img1.getWidth(),
            img1.getWidth());
    }
   //adding amount visualisation
    g.setColour(juce::Colours::white); //setting colour
    g.setFont(14.0f); // setting font
    g.drawFittedText(juce::String(slider.getValue(), 2), x, y + height - 8, width, 8,juce::Justification::centred, 1);
}
