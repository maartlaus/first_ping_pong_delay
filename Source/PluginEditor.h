/*
  ==============================================================================

     Coursework 2 for Advanced Audio Processing - Martynas Kazlauskas

	 Plugin Editor.h:

	 contains Class definitions ( class X { ... }; )
     
     implementation of the actual sound processing
	 or the sound generation in case of a generator or instrument.
	 The actual class inside that file has a generic name formed of your PluginName you supplied.
	 It inherits juce::AudioProcessor to function correctly.

     .h files, or header files, are used to list the publicly accessible instance variables and methods in the class declaration.
  ==============================================================================
*/

#pragma once


// library inclusion from integrated juce library code database and from a plugin processor header 
// in order to activate the functionalities and connect the source files.
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================

/*Main class of the plugin with public access. */

class PingPongDelayAudioProcessorEditor : public AudioProcessorEditor
{
public:
    //==============================================================================

    PingPongDelayAudioProcessorEditor (PingPongDelayAudioProcessor&);
    ~PingPongDelayAudioProcessorEditor();

    //==============================================================================

    //includes the ability to modify (override) the visual aspects of the plugin (using Graphics class which is a pointer) 
    //and the window size.

    void paint(Graphics&) override;
    void resized() override;


private:
    //==============================================================================

    //Private plugin class has an Access modifiers. 

	/*Private data can be accessed only by other members of the class. Public data can be accessed by other parts of the program
Protected needed only when inheritance is involved. Access-specifier can be changed in any order, default is private.*/

    PingPongDelayAudioProcessor& processor;

    //the main plugin window parameters and characteristics.

    enum {
        editorWidth = 1000,
        editorMargin = 20,
        editorPadding = 20,

        sliderTextEntryBoxWidth = 100,
        sliderTextEntryBoxHeight = 50,
        sliderHeight = 40,
        buttonHeight = 60,
        comboBoxHeight = 25,
        labelWidth = 100,
    };

    //======================================
    /* A call of integrated JUCE library for the arrays of interaction for sliders, toggles and drop-down list text string feature */
    OwnedArray<Slider> sliders;
    OwnedArray<ToggleButton> toggles;
    OwnedArray<ComboBox> comboBoxes;

    // A component that displays a text string for the meaning of the slider, toggle values

    OwnedArray<Label> labels;
    //component serves as a base-class for all of JUCE interfaces
    Array<Component*> components;

    /*AudioProcessorValueTreeState class contains AudioProcessor's entire functional state. It contains its own internal class
    of parameter object that is linked to values within the ValueTree. It is possible to add additional properties to it as necessary.
    */ 

    typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    typedef AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

    /*OwnedArray - it is a JUCE based class for holding objects. In this case - pointers to objects. 
    Creating new variables for later edit within the file structure. */

    OwnedArray<SliderAttachment> sliderAttachments;
    OwnedArray<ButtonAttachment> buttonAttachments;
    OwnedArray<ComboBoxAttachment> comboBoxAttachments;

    //==============================================================================

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PingPongDelayAudioProcessorEditor)
};

//==============================================================================
