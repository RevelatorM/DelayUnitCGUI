/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayUnitFDJAudioProcessor::DelayUnitFDJAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{

    addParameter(DryWetParameter = new juce::AudioParameterFloat("drywet", "Dry Wet", 0.0, 1.0, 0.1));
    addParameter(feedbackParameter = new juce::AudioParameterFloat("feedback", "Feedback", 0, 1.00, 0.1));
    addParameter(DelayTimeParameter = new juce::AudioParameterFloat("delaytime", "Delay Time", 0.01, MAX_DELAY_TIME, 0.35));

    DelayTimeSmoothed = 0;

    mCircularBufferLeft = nullptr;
    mCircularBufferRight = nullptr;

    CircularBufferWriteHead = 0;
    CircularBufferLength = 0;

    DelayTimeInSamples = 0;
    DelayReadHead = 0;

    FeedBackLeft = 0;
    FeedBackRight = 0;


}

DelayUnitFDJAudioProcessor::~DelayUnitFDJAudioProcessor()
{
    if (mCircularBufferLeft != nullptr) {
        delete[] mCircularBufferLeft;
        mCircularBufferLeft = nullptr;
    }

    if (mCircularBufferRight != nullptr) {
        delete[] mCircularBufferRight;
        mCircularBufferRight = nullptr;
    }
}

//==============================================================================
const juce::String DelayUnitFDJAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DelayUnitFDJAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool DelayUnitFDJAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool DelayUnitFDJAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double DelayUnitFDJAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DelayUnitFDJAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DelayUnitFDJAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DelayUnitFDJAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String DelayUnitFDJAudioProcessor::getProgramName(int index)
{
    return {};
}

void DelayUnitFDJAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void DelayUnitFDJAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    DelayTimeInSamples = sampleRate * *DelayTimeParameter;

    CircularBufferLength = sampleRate * MAX_DELAY_TIME;

    if (mCircularBufferLeft == nullptr) { //checking if the Circular buffer in the left channel has something
        mCircularBufferLeft = new float[CircularBufferLength];
    }

    juce::zeromem(mCircularBufferLeft, CircularBufferLength * sizeof(float));

    if (mCircularBufferRight == nullptr) { //checking if the Circular buffer in the right channel has something
        mCircularBufferRight = new float[CircularBufferLength];
    }

    juce::zeromem(mCircularBufferRight, CircularBufferLength * sizeof(float));

    int CircularBufferWriteHead = 0;

    if (CircularBufferWriteHead >= CircularBufferLength) {
        CircularBufferWriteHead = 0;
    }

    DelayTimeSmoothed = *DelayTimeParameter;
}

void DelayUnitFDJAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DelayUnitFDJAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void DelayUnitFDJAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    //===================================================================
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
    //===================================================================


    float* leftChannel = buffer.getWritePointer(0);
    float* rightChannel = buffer.getWritePointer(1);

    //===================================================================
    for (int i = 0; i < buffer.getNumSamples(); i++) {

        DelayTimeSmoothed = DelayTimeSmoothed - 0.001 * (DelayTimeSmoothed - *DelayTimeParameter);
        DelayTimeInSamples = getSampleRate() * DelayTimeSmoothed;

        mCircularBufferLeft[CircularBufferWriteHead] = leftChannel[i] + FeedBackLeft;
        mCircularBufferRight[CircularBufferWriteHead] = rightChannel[i] + FeedBackRight;

        DelayReadHead = CircularBufferWriteHead - DelayTimeInSamples;

        if (DelayReadHead < 0) {
            DelayReadHead += CircularBufferLength;
        }

        int readHead_x = (int)DelayReadHead;
        int readHead_x1 = readHead_x + 1;

        float readHeadFloat = DelayReadHead - readHead_x;

        if (readHead_x1 >= CircularBufferLength) {
            readHead_x1 -= CircularBufferLength;
        }


        float delay_sample_left = lin_interp(mCircularBufferLeft[readHead_x], mCircularBufferLeft[readHead_x1], readHeadFloat);
        float delay_sample_right = lin_interp(mCircularBufferRight[readHead_x], mCircularBufferRight[readHead_x1], readHeadFloat);

        FeedBackLeft = delay_sample_left * 0.8;
        FeedBackRight = delay_sample_right * 0.8;

        buffer.setSample(0, i, buffer.getSample(0, i) * (1 - *DryWetParameter) + delay_sample_left * *DryWetParameter);
        buffer.setSample(1, i, buffer.getSample(1, i) * (1 - *DryWetParameter) + delay_sample_right * *DryWetParameter);

        CircularBufferWriteHead++;

        if (CircularBufferWriteHead >= CircularBufferLength) {
            CircularBufferWriteHead = 0;
        }
    }
    //===================================================================

}

//==============================================================================
bool DelayUnitFDJAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* DelayUnitFDJAudioProcessor::createEditor()
{
    return new DelayUnitFDJAudioProcessorEditor(*this);
}

//==============================================================================
void DelayUnitFDJAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DelayUnitFDJAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..

float DelayUnitFDJAudioProcessor::lin_interp(float sample_x, float sample_x1, float inPhase) { //linear interpolation
    return (1 - inPhase) * sample_x + inPhase * sample_x1; //(1-t)*sample1+t*sample2;t = time between two samples
}


juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DelayUnitFDJAudioProcessor();
}

