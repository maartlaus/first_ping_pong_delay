/*
  ==============================================================================

     Coursework 2 for Advanced Audio Processing - Martynas Kazlauskas

     this header file enables the main classes for audio processing capabilities an functionalities to be used throughout the whole source code.

  ==============================================================================
*/

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginParameter.h"

//==============================================================================

class PingPongDelayAudioProcessor : public AudioProcessor
{
public:
    //==============================================================================

    PingPongDelayAudioProcessor();

/*~PingPongDelayAudioProcessor.The "~" symbol indicates that is the class destructor.
It gets called when user destroys (reaching end of scope, or calling delete to a pointer to)
the instance of the object.*/

    ~PingPongDelayAudioProcessor();

    //==============================================================================

    /*Enables the ability for the plugin to listen incoming audio samples and process them within the internal memory and releasing resources when necessary*/
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================






    //==============================================================================

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    //==============================================================================

    const juce::String getName() const override; //added juce library call on entrance

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    //==============================================================================

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================






    //==============================================================================
    
    /*int values of delay parameters (samples,channels,sound position) for the plugin*/


    AudioSampleBuffer delayBuffer;
    int delayBufferSamples;
    int delayBufferChannels;
    int delayWritePosition;

    //======================================


    //Enables the slider control of the mentioned main parameters such as "Balance, DelayTime, Feedback, Dry/Wet mix"
    PluginParametersManager parameters;

    PluginParameterLinSlider paramBalance;
    PluginParameterLinSlider paramDelayTime;
    PluginParameterLinSlider paramFeedback;
    PluginParameterLinSlider paramMix;

private:
    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PingPongDelayAudioProcessor)
};
