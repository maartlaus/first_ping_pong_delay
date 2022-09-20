/*
  ==============================================================================

	 Coursework 2 for Advanced Audio Processing - Martynas Kazlauskas

     This is a header where the main plugin parameter functions and classes are called 
     They are interconnected with ValueTreeState attribute which allows adjusted parameter to be saved into the internal memory.


  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
using Parameter = AudioProcessorValueTreeState::Parameter;

/*Value TreeState is a powerful tree structure that can be used to hold free-form data, and which enables and handles its own undo and redo behaviour.
A ValueTree can contain a list of named properties as var objects, and also holds any number of sub-trees.*/
//==============================================================================

class PluginParametersManager
{
public:
    PluginParametersManager (AudioProcessor& p) : apvts (p, nullptr)
    {
    }

    /*Classes of main adjustable parameter elements:*/
    AudioProcessorValueTreeState apvts; //Enables the plugin to handle internal memory settings + (undo/redo) functionality
    StringArray parameterTypes; //A created StringArray class to store different parameters of the plugin numerical data for: mix, balance, feedback,etc.
    Array<StringArray> comboBoxItemLists; //array of string for the functionality naming (text based data)
};

//==============================================================================


/*Plugin Parameter class which has a 'public' status. */
class PluginParameter
    : public LinearSmoothedValue<float> //includes float based linear parameter settings which makes the editing of the VST plugin parameters smoother, more sensitive.
    , public AudioProcessorValueTreeState::Listener // Receives callbacks when a Value objects from the ValueTreeState changes.
{
 
protected: // protected is used to access to class members in the member-list up to the next access specifier ( public or private ) or the end of the class definition
    PluginParameter (PluginParametersManager& parametersManager,
                     const std::function<float (float)> callback = nullptr)
        : parametersManager (parametersManager)
        , callback (callback)
    {
    }

    /*public class for implementing properly functioning value updating of the whole data sets of parameters */

public: 
    void updateValue (float value)
    {
        if (callback != nullptr)
            setCurrentAndTargetValue (callback (value));
        else
            setCurrentAndTargetValue (value);
    }
    // Overwriting the newly modified settings/parameters using 'parameterID' and thus creating a new value.

    void parameterChanged (const String& parameterID, float newValue) override
    {
        updateValue (newValue);
    }

    PluginParametersManager& parametersManager;
    std::function<float (float)> callback;
    String paramID;
};

//==============================================================================

class PluginParameterSlider : public PluginParameter
{

    /*Plugin parameter slider class which implements the unique setting categories and its main functionality rules ('min value/max value/default value) 
    of the specific modifiers/parameters*/
protected:
    PluginParameterSlider (PluginParametersManager& parametersManager,
                           const String& paramName,
                           const String& labelText,
                           const float minValue,
                           const float maxValue,
                           const float defaultValue,
                           const std::function<float (float)> callback,
                           const bool logarithmic)
        : PluginParameter (parametersManager, callback)
        , paramName (paramName)
        , labelText (labelText)
        , minValue (minValue)
        , maxValue (maxValue)
        , defaultValue (defaultValue)
    {

        //creating a parameter ID and creating proper naming conventions for ease of use and indication in the source code.
        paramID = paramName.removeCharacters (" ").toLowerCase();
        parametersManager.parameterTypes.add ("Slider");
        //indicates that the float type range of values for the parameters will be scaled logarithmically 
        NormalisableRange<float> range (minValue, maxValue);
        if (logarithmic)
            range.setSkewForCentre (sqrt (minValue * maxValue));

        /* Creates an ability to create a separate/unique parameters and add additional ones if necessary */
        parametersManager.apvts.createAndAddParameter (std::make_unique<Parameter>
            (paramID, paramName, labelText, range, defaultValue,
             [](float value){ return String (value, 2); },
             [](const String& text){ return text.getFloatValue(); })
        );

        parametersManager.apvts.addParameterListener (paramID, this);
        updateValue (defaultValue);
    }
    //main public values of the parameters. 
public:
    const String& paramName;
    const String& labelText;
    const float minValue;
    const float maxValue;
    const float defaultValue;
};

//======================================

/*Slider functionality class*/

class PluginParameterLinSlider : public PluginParameterSlider
{
public: /*this adds a functionality to assign sliders to a specific value of a parameter with the all the necessary min/max/default values */
    PluginParameterLinSlider (PluginParametersManager& parametersManager,
                              const String& paramName,
                              const String& labelText,
                              const float minValue,
                              const float maxValue,
                              const float defaultValue,
                              const std::function<float (float)> callback = nullptr)
        : PluginParameterSlider (parametersManager,
                                 paramName,
                                 labelText,
                                 minValue,
                                 maxValue,
                                 defaultValue,
                                 callback,
                                 false)
    {
    }
};

//======================================

class PluginParameterLogSlider : public PluginParameterSlider
{
    /* This enables logarithmically+float based slider configuration to be used within the VST plugin main parameters with its min/max/default values. */
public: 
    PluginParameterLogSlider (PluginParametersManager& parametersManager,
                              const String& paramName,
                              const String& labelText,
                              const float minValue,
                              const float maxValue,
                              const float defaultValue,
                              const std::function<float (float)> callback = nullptr)
        : PluginParameterSlider (parametersManager,
                                 paramName,
                                 labelText,
                                 minValue,
                                 maxValue,
                                 defaultValue,
                                 callback,
                                 true)
    {
    }
};

//==============================================================================

class PluginParameterToggle : public PluginParameter
{

    //Enables the functionality of parameter toggling on/off (bool = true or false) associated with parameterID.
public:
    PluginParameterToggle (PluginParametersManager& parametersManager,
                           const String& paramName,
                           const bool defaultState = false,
                           const std::function<float (float)> callback = nullptr)
        : PluginParameter (parametersManager, callback)
        , paramName (paramName)
        , defaultState (defaultState)
    {
        paramID = paramName.removeCharacters (" ").toLowerCase();
        parametersManager.parameterTypes.add ("ToggleButton");

        const StringArray toggleStates = {"False", "True"};
        NormalisableRange<float> range (0.0f, 1.0f, 1.0f);

        parametersManager.apvts.createAndAddParameter (std::make_unique<Parameter>
            (paramID, paramName, "", range, (float)defaultState,
             [toggleStates](float value){ return toggleStates[(int)value]; },
             [toggleStates](const String& text){ return toggleStates.indexOf (text); })
        );

        parametersManager.apvts.addParameterListener (paramID, this);
        updateValue ((float)defaultState);
    }

    const String& paramName;
    const bool defaultState;
};

//==============================================================================

class PluginParameterComboBox : public PluginParameter
{

    /*Enables the functionality and visibility of parameter ComboBoxes. Which are integrated in the VST's UI experience  */
public:

    //
    PluginParameterComboBox (PluginParametersManager& parametersManager,
                             const String& paramName,
                             const StringArray items,
                             const int defaultChoice = 0,
                             const std::function<float (const float)> callback = nullptr)
        : PluginParameter (parametersManager, callback)
        , paramName (paramName)
        , items (items)
        , defaultChoice (defaultChoice)
    {
        /*Parameter names are modified to default to lower cases*/
        paramID = paramName.removeCharacters (" ").toLowerCase();
        parametersManager.parameterTypes.add ("ComboBox");
        //the range of settings are from -1.0 to 1.0 in floating-point (4 bytes in size)
        parametersManager.comboBoxItemLists.add (items);
        NormalisableRange<float> range (0.0f, (float)items.size() - 1.0f, 1.0f);

        parametersManager.apvts.createAndAddParameter (std::make_unique<Parameter>
            (paramID, paramName, "", range, (float)defaultChoice,
             [items](float value){ return items[(int)value]; },
             [items](const String& text){ return items.indexOf (text); })
        );

        parametersManager.apvts.addParameterListener (paramID, this);
        updateValue ((float)defaultChoice);
    }

    const String& paramName;
    const StringArray items;
    const int defaultChoice;
};

//==============================================================================
