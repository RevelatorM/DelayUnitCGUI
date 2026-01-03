/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#define MAX_DELAY_TIME 2

//==============================================================================
/**
*/
class DelayUnitFDJAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    DelayUnitFDJAudioProcessor();
    ~DelayUnitFDJAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    float lin_interp(float sample_x, float sample_x1, float inPhase);//declaring linear interpolation

private:

    float DelayTimeSmoothed;

    juce::AudioParameterFloat* DryWetParameter;
    juce::AudioParameterFloat* feedbackParameter;
    juce::AudioParameterFloat* DelayTimeParameter;


    float FeedBackLeft;
    float FeedBackRight;

    float DelayTimeInSamples;
    float DelayReadHead;
    int CircularBufferLength;
    int CircularBufferWriteHead;

    float* mCircularBufferLeft;
    float* mCircularBufferRight;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayUnitFDJAudioProcessor)
};
