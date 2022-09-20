/*
  ==============================================================================

     Coursework 2 for Advanced Audio Processing - Martynas Kazlauskas

     Explanation of the processes of the code are included inside the comments
     
     contains implementation of the actual sound processing 
     or the sound generation in case of a generator or instrument. 
     The actual class inside that file has a generic name formed of your PluginName you supplied. 
     It inherits juce::AudioProcessor to function correctly.

  ==============================================================================
*/


/*includes all of the headers of the source code*/
#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginParameter.h"

//==============================================================================


/*main audio processor class constructor (All classes need at least one constructor)*/
PingPongDelayAudioProcessor::PingPongDelayAudioProcessor():
#ifndef JucePlugin_PreferredChannelConfigurations
    AudioProcessor (BusesProperties()
                    #if ! JucePlugin_IsMidiEffect
                     #if ! JucePlugin_IsSynth

        /*Setting up the AudioChannels to stereo for input and output*/
                      .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     #endif
                      .withOutput ("Output", AudioChannelSet::stereo(), true)
                    #endif
                   ),
#endif
    //parameters for the sliders of the plugin. (">additional naming attribute to the float number<",minVlaue, maxValue, ScalingValue)
    parameters (*this)
    , paramBalance (parameters, "Balance input", "", 0.0f, 1.0f, 0.25f)
    , paramDelayTime (parameters, "Delay time", "s", 0.0f, 5.0f, 0.1f)
    , paramFeedback (parameters, "Feedback", "", 0.0f, 0.9f, 0.7f)
    , paramMix (parameters, "Mix", "", 0.0f, 1.0f, 0.1f)
{
    parameters.apvts.state = ValueTree (Identifier (getName().removeCharacters ("- ")));

    //ValueTrue - enables to save previous settings/state of the application.
}


/*~PingPongDelayAudioProcessor.The "~" symbol indicates that is the class destructor.
It gets called when user destroys (reaching end of scope, or calling delete to a pointer to) 
the instance of the object.*/

PingPongDelayAudioProcessor::~PingPongDelayAudioProcessor()
{
}

//==============================================================================

void PingPongDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    /*Setting up the initial playback without distortion or sound artifacts. Smoothing the playback time using the formula "1e-3" */
    const double smoothTime = 1e-3;
    paramBalance.reset (sampleRate, smoothTime);
    paramDelayTime.reset (sampleRate, smoothTime);
    paramFeedback.reset (sampleRate, smoothTime);
    paramMix.reset (sampleRate, smoothTime);

    //======================================

    /*Setting up max delay time using the int value of the overall sample delay time 
    and multiplying by float based sample rate.Normalizing to value of 1 in each scenario to have a stable buffer sample playback.*/
    float maxDelayTime = paramDelayTime.maxValue;
    delayBufferSamples = (int)(maxDelayTime * (float)sampleRate) + 1;
    if (delayBufferSamples < 1)
        delayBufferSamples = 1;

    delayBufferChannels = getTotalNumInputChannels();
    delayBuffer.setSize (delayBufferChannels, delayBufferSamples);
    delayBuffer.clear();

    delayWritePosition = 0;
}

void PingPongDelayAudioProcessor::releaseResources()
{
}
/*When this method is called, the buffer contains a number of channels which is at least as great as 
the maximum number of input and output channels that this processor is using. It will be filled with the processor's input data 
and should be replaced with the processor's output.*/
void PingPongDelayAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    /*Controlling the total of */
    const int numInputChannels = getTotalNumInputChannels();
    const int numOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    //======================================

    float currentBalance = paramBalance.getNextValue();
    float currentDelayTime = paramDelayTime.getTargetValue() * (float)getSampleRate();
    float currentFeedback = paramFeedback.getNextValue();
    float currentMix = paramMix.getNextValue();

    int localWritePosition = delayWritePosition;

    float* channelDataL = buffer.getWritePointer (0);
    float* channelDataR = buffer.getWritePointer (1);
    float* delayDataL = delayBuffer.getWritePointer (0);
    float* delayDataR = delayBuffer.getWritePointer (1);
    //numSmaples = in the original data (sound playing)
    for (int sample = 0; sample < numSamples; ++sample) //increase another sample +1 to continue the loop. Stops until the samples end.
    {
        const float inL = (1.0f - currentBalance) * channelDataL[sample]; //currentBalance - 
        const float inR = currentBalance * channelDataR[sample]; //read sample data and enables to modify with a parameter/slider
        float outL = 0.0f;
        float outR = 0.0f;
        //Initialize as zero to sound distortion, this is stereo initial code
        
        float readPosition =
            fmodf ((float)localWritePosition - currentDelayTime + (float)delayBufferSamples, delayBufferSamples);
        int localReadPosition = floorf (readPosition); //actually to act at current settings.
        
        // Ping-Pong = send the delayed signal of the L channel and send to the R, vice versa. Feedback should lowered before returning. 
       
        if (localReadPosition != localWritePosition) //security measure against if delay would be 0. 

			/*Each delay line may be driven by a separate input, or only one input can be used. The output of each delay line, rather than feeding back to itself, attaches to the input of the opposite delay line. In its two-channel
            configuration, ping-pong delay produces a sound that bounces between left and right channels in a stereo track.*/
        {
            float fraction = readPosition - (float)localReadPosition;
            float delayed1L = delayDataL[(localReadPosition + 0)];
            float delayed1R = delayDataR[(localReadPosition + 0)];
            float delayed2L = delayDataL[(localReadPosition + 1) % delayBufferSamples];
            float delayed2R = delayDataR[(localReadPosition + 1) % delayBufferSamples];
            outL = delayed1L + fraction * (delayed2L - delayed1L);
            outR = delayed1R + fraction * (delayed2R - delayed1R);

            channelDataL[sample] = inL + currentMix * (outL - inL);
            channelDataR[sample] = inR + currentMix * (outR - inR);
            delayDataL[localWritePosition] = inL + outR * currentFeedback;
            delayDataR[localWritePosition] = inR + outL * currentFeedback;
        }

        if (++localWritePosition >= delayBufferSamples)
            localWritePosition -= delayBufferSamples;
    }

    delayWritePosition = localWritePosition;

    //======================================

    for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
        buffer.clear (channel, 0, numSamples);

}

//==============================================================================






//==============================================================================

/*MemoryBlock is a class to hold a resizable block of raw data*/
void PingPongDelayAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    //records all the parameter data into a raw data file. This converts it into xml format from there to binary to get raw parameter data.
    auto state = parameters.apvts.copyState();
    std::unique_ptr<XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PingPongDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.apvts.state.getType()))
            parameters.apvts.replaceState (ValueTree::fromXml (*xmlState));
}

//==============================================================================

AudioProcessorEditor* PingPongDelayAudioProcessor::createEditor()
{
    return new PingPongDelayAudioProcessorEditor (*this);
}

bool PingPongDelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

//==============================================================================

#ifndef JucePlugin_PreferredChannelConfigurations
bool PingPongDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

//==============================================================================


//Generates the over plugin name throughout the source code and the DAW environment. 
const String PingPongDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}
//enables capability of midi within the DAW
bool PingPongDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PingPongDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PingPongDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PingPongDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

//==============================================================================

int PingPongDelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PingPongDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PingPongDelayAudioProcessor::setCurrentProgram (int index)
{
}

const String PingPongDelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void PingPongDelayAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================

// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PingPongDelayAudioProcessor();
}

//==============================================================================
