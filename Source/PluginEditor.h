/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Knob.h"
//==============================================================================
/**
*/
class DelayUnitFDJAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    DelayUnitFDJAudioProcessorEditor(DelayUnitFDJAudioProcessor&);
    ~DelayUnitFDJAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    DelayUnitFDJAudioProcessor& audioProcessor;

    juce::Slider DryWetSlider;
    juce::Slider FeedbackSlider;
    juce::Slider DelayTimeSlider;

    myLookAndFeeKB1 myLookAndFeelKB1;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayUnitFDJAudioProcessorEditor)
};
