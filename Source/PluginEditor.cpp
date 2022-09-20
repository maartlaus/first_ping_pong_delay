/*
  ==============================================================================

     Coursework 2 for Advanced Audio Processing - Martynas Kazlauskas

     Contains implementation of the main fnctionalities (like sliders, text boxes, initialising reactive parameters) 
     and the appearance overall systems of the VST's UI
     
    

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

/*Initialize the main audio processor class*/
PingPongDelayAudioProcessorEditor::PingPongDelayAudioProcessorEditor (PingPongDelayAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    //This is where audio data array of the plugin parameters are called in.
    const Array<AudioProcessorParameter*> parameters = processor.getParameters();
    int comboBoxCounter = 0;
    // Integrating the associating parameter scrolling and ability to modify proportionally with the parameter data. . 
    int editorHeight = 2 * editorMargin;
    for (int i = 0; i < parameters.size(); ++i) {
        if (const AudioProcessorParameterWithID* parameter =
                dynamic_cast<AudioProcessorParameterWithID*> (parameters[i])) {

            if (processor.parameters.parameterTypes[i] == "Slider") {
                Slider* aSlider;
                sliders.add (aSlider = new Slider());
                aSlider->setTextValueSuffix (parameter->label);
                aSlider->setTextBoxStyle (Slider::TextBoxBelow,
                                          false,
                                          sliderTextEntryBoxWidth,
                                          sliderTextEntryBoxHeight);

                SliderAttachment* aSliderAttachment;
                sliderAttachments.add (aSliderAttachment =
                    new SliderAttachment (processor.parameters.apvts, parameter->paramID, *aSlider));

                components.add (aSlider);

                //making the overall editor height and slider height proportional.
                editorHeight += sliderHeight;
            }

            //======================================

            else if (processor.parameters.parameterTypes[i] == "ToggleButton") {
                ToggleButton* aButton;
                toggles.add (aButton = new ToggleButton());
                aButton->setToggleState (parameter->getDefaultValue(), dontSendNotification);

                ButtonAttachment* aButtonAttachment;
                buttonAttachments.add (aButtonAttachment =
                    new ButtonAttachment (processor.parameters.apvts, parameter->paramID, *aButton));

                components.add (aButton);
                editorHeight += buttonHeight;
            }

            //======================================

            else if (processor.parameters.parameterTypes[i] == "ComboBox") {
                ComboBox* aComboBox;
                comboBoxes.add (aComboBox = new ComboBox());
                aComboBox->setEditableText (false);
                aComboBox->setJustificationType (Justification::bottomLeft);
                aComboBox->addItemList (processor.parameters.comboBoxItemLists[comboBoxCounter++], 1);

                ComboBoxAttachment* aComboBoxAttachment;
                comboBoxAttachments.add (aComboBoxAttachment =
                    new ComboBoxAttachment (processor.parameters.apvts, parameter->paramID, *aComboBox));

                components.add (aComboBox);
                editorHeight += comboBoxHeight;
            }

            //======================================

            Label* aLabel;
            labels.add (aLabel = new Label (parameter->name, parameter->name));
            aLabel->attachToComponent (components.getLast(), true);
            addAndMakeVisible (aLabel);

            components.getLast()->setName (parameter->name);
            components.getLast()->setComponentID (parameter->paramID);
            addAndMakeVisible (components.getLast());
        }
    }

    //======================================

    editorHeight += components.size() * editorPadding;
    setSize (editorWidth, editorHeight);
}

PingPongDelayAudioProcessorEditor::~PingPongDelayAudioProcessorEditor()
{
}

//==============================================================================

void PingPongDelayAudioProcessorEditor::paint (Graphics& g)
{

    	//introducing a background for the overall background of the vst plugin
    	g.fillAll(juce::Colours::black);

	//introducing a background image for the VST plugin
	Image background = ImageCache::getFromMemory(BinaryData::VST_Image_Back_Small_png, BinaryData::VST_Image_Back_Small_pngSize);
	g.drawImageAt(background, 0, 0);;
	
	//enables to edit using JUCE internal graphic functions on top of the uploaded images
	background.duplicateIfShared();
    
    	//Draw a simple line 
	g.setColour(juce::Colours::hotpink);
	juce::Line<float> line(juce::Point<float>(15, 15),
		juce::Point<float>(15, 240));

	g.drawLine(line, 3.0f);

    // Additional square paint element
	g.setColour(juce::Colours::hotpink);
	juce::Rectangle<int> area(30, 20, 85, 40); // [area1 "Balance Input"]
    g.fillRect(area);

	juce::Rectangle<int> area2(30, 80, 85, 40); // [area2 "Delay Time"]
	g.fillRect(area2);

	juce::Rectangle<int> area3(30, 140, 85, 40); // [area3 "Feedback"]
	g.fillRect(area3);

	juce::Rectangle<int> area4(30, 200, 85, 40); // [area4 "Mix"]
	g.fillRect(area4);


    //UI text
    g.drawText("Advanced Audio Processing Coursework by Martynas Kazlauskas", 350, 250, 400, 20, juce::Justification::centred, true);

}

void PingPongDelayAudioProcessorEditor::resized()
{
    Rectangle<int> r = getLocalBounds().reduced (editorMargin);
    r = r.removeFromRight (r.getWidth() - labelWidth);

    for (int i = 0; i < components.size(); ++i) {
        if (Slider* aSlider = dynamic_cast<Slider*> (components[i]))
            components[i]->setBounds (r.removeFromTop (sliderHeight));

        if (ToggleButton* aButton = dynamic_cast<ToggleButton*> (components[i]))
            components[i]->setBounds (r.removeFromTop (buttonHeight));

        if (ComboBox* aComboBox = dynamic_cast<ComboBox*> (components[i]))
            components[i]->setBounds (r.removeFromTop (comboBoxHeight));

        r = r.removeFromBottom (r.getHeight() - editorPadding);
    }
}

//==============================================================================
