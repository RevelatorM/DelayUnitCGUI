/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayUnitFDJAudioProcessorEditor::DelayUnitFDJAudioProcessorEditor(DelayUnitFDJAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(500, 500);

    auto& params = processor.getParameters();
    juce::AudioParameterFloat* DryWetParameter = (juce::AudioParameterFloat*)params.getUnchecked(0);


    DryWetSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    DryWetSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    DryWetSlider.setRange(DryWetParameter->range.start, DryWetParameter->range.end);
    DryWetSlider.setValue(*DryWetParameter);
    DryWetSlider.setLookAndFeel(&myLookAndFeelKB1);
    addAndMakeVisible(DryWetSlider);

    DryWetSlider.onValueChange = [this, DryWetParameter] {*DryWetParameter = DryWetSlider.getValue(); };
    DryWetSlider.onDragStart = [DryWetParameter] { DryWetParameter->beginChangeGesture(); };
    DryWetSlider.onDragEnd = [DryWetParameter] { DryWetParameter->endChangeGesture(); };
    //======================================================================================================
    juce::AudioParameterFloat* feedbackParameter = (juce::AudioParameterFloat*)params.getUnchecked(1);

    FeedbackSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    FeedbackSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    FeedbackSlider.setRange(feedbackParameter->range.start, feedbackParameter->range.end);
    FeedbackSlider.setValue(*feedbackParameter);
    FeedbackSlider.setLookAndFeel(&myLookAndFeelKB1);
    addAndMakeVisible(FeedbackSlider);

    FeedbackSlider.onValueChange = [this, feedbackParameter] { *feedbackParameter = FeedbackSlider.getValue(); };
    FeedbackSlider.onDragStart = [feedbackParameter] { feedbackParameter->beginChangeGesture(); };
    FeedbackSlider.onDragEnd = [feedbackParameter] { feedbackParameter->endChangeGesture(); };
    //=========================================================================================================
    juce::AudioParameterFloat* DelayTimeParameter = (juce::AudioParameterFloat*)params.getUnchecked(2);

    DelayTimeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    DelayTimeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    DelayTimeSlider.setRange(DelayTimeParameter->range.start, DelayTimeParameter->range.end);
    DelayTimeSlider.setValue(*DelayTimeParameter);
    DelayTimeSlider.setLookAndFeel(&myLookAndFeelKB1);
    addAndMakeVisible(DelayTimeSlider);

    DelayTimeSlider.onValueChange = [this, DelayTimeParameter] { *DelayTimeParameter = DelayTimeSlider.getValue(); };
    DelayTimeSlider.onDragStart = [DelayTimeParameter] { DelayTimeParameter->beginChangeGesture(); };
    DelayTimeSlider.onDragEnd = [DelayTimeParameter] { DelayTimeParameter->endChangeGesture(); };
}

DelayUnitFDJAudioProcessorEditor::~DelayUnitFDJAudioProcessorEditor() //used to turn off added custom sliders
{
    DryWetSlider.setLookAndFeel(nullptr); // turns off DryWetSlider
    FeedbackSlider.setLookAndFeel(nullptr); // turns FeedbackSlider
    DelayTimeSlider.setLookAndFeel(nullptr); // turnsDelayTimeSlider
}

//==============================================================================
void DelayUnitFDJAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromFloatRGBA(172, 151, 118, 0.8));

    //======================beginning of a shadow text part====================================================
    g.setColour(juce::Colours::palevioletred);
    g.setFont(24.0f);
    g.drawFittedText("DelayUnit", getLocalBounds().translated(4, 3), juce::Justification::centredTop, 1);
    //======================end of a shadow text part====================================================
    
    g.setColour(juce::Colours::red);
    g.drawFittedText("DelayUnit", getLocalBounds(), juce::Justification::centredTop, 1);
}

void DelayUnitFDJAudioProcessorEditor::resized() //making all the knobs centrised
{
    auto bounds = getLocalBounds(); // analog to .setBounds(getLocalBounds().withSizeKeepingCentre(150, 150));
    int sliderWidth = 150; 
    int sliderHeight = 150;
    //==================================================
    int spaceBetween = 20; // space between the knobs
    int totalWidth = sliderWidth * 3 + spaceBetween * 2;
    int xOffset = (bounds.getWidth() - totalWidth) / 2;
    int yOffset = (bounds.getHeight() - sliderHeight) / 2;
    //==================================================
    DryWetSlider.setBounds(xOffset, yOffset, sliderWidth, sliderHeight); // creating positions for knobs
    FeedbackSlider.setBounds(xOffset + sliderWidth + spaceBetween, yOffset, sliderWidth, sliderHeight);
    DelayTimeSlider.setBounds(xOffset + 2 * (sliderWidth + spaceBetween), yOffset, sliderWidth, sliderHeight);


}
