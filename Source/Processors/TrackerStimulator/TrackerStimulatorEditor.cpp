/*
  ==============================================================================

    TrackerStimulatorEditor.cpp
    Created: 28 Jun 2016 11:59:59am
    Author:  alessio

  ==============================================================================
*/

#include "TrackerStimulatorEditor.h"
#include "TrackerStimulator.h"
#include "TrackerStimulatorCanvas.h"
#include "../../AccessClass.h"
#include "../../UI/EditorViewport.h"

TrackerStimulatorEditor::TrackerStimulatorEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors=true)
    : VisualizerEditor(parentNode, useDefaultParameterEditors)

{
    desiredWidth = 200;


    // Add buttons

    // Stimulate (toggle)
    stimulateButton = new UtilityButton("ON", Font("Small Text", 13, Font::plain));
    stimulateButton->setRadius(3.0f); // sets the radius of the button's corners
    stimulateButton->setBounds(60, 40, 80, 30); // sets the x position, y position, width, and height of the button
    stimulateButton->addListener(this); // allows the editor to respond to clicks
    stimulateButton->setClickingTogglesState(true); // makes the button toggle its state when clicked
    addAndMakeVisible(stimulateButton); // makes the button a child component of the editor and makes it visible

    //testPattern
    testPatternButton = new UtilityButton("Test", Font("Small Text", 13, Font::plain));
    testPatternButton->setRadius(3.0f); // sets the radius of the button's corners
    testPatternButton->setBounds(70, 80, 60, 20); // sets the x position, y position, width, and height of the button
    testPatternButton->addListener(this); // allows the editor to respond to clicks
    testPatternButton->setClickingTogglesState(false); // makes the button toggle its state when clicked
    addAndMakeVisible(testPatternButton); // makes the button a child component of the editor and makes it visible

}

TrackerStimulatorEditor::~TrackerStimulatorEditor()
{
    //deleteAllChildren();
}

Visualizer* TrackerStimulatorEditor::createNewCanvas()
{
    TrackerStimulator* processor = (TrackerStimulator*) getProcessor();
    return new TrackerStimulatorCanvas(processor);
}


void TrackerStimulatorEditor::buttonCallback(Button* button)
{
    int gId = button->getRadioGroupId();

    if (gId > 0)
    {
        if (canvas != 0)
        {
            canvas->setParameter(gId-1, button->getName().getFloatValue());
        }

    }

    // testPattern
    if (button == testPatternButton)
    {
        TrackerStimulator *p= (TrackerStimulator *)getProcessor();

        if (!button->isDown())
            p->testStimulation();
    }

    if (button == stimulateButton)
    {
        if (button->getToggleState()==true){
            TrackerStimulator *p= (TrackerStimulator *)getProcessor();
            p->startStimulation();
        }
        else {
            TrackerStimulator *p= (TrackerStimulator *)getProcessor();
            p->stopStimulation();
        }
    }

}

