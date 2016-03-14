/*
  ==============================================================================

    SpikePositionEditor.cpp
    Created: 11 Mar 2016 11:59:59am
    Author:  alessio

  ==============================================================================
*/

#include "SpikePositionEditor.h"
#include "SpikePositionNode.h"
#include "../../AccessClass.h"
#include "../../UI/EditorViewport.h"

SpikePositionEditor::SpikePositionEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors=true)
    : GenericEditor(parentNode, useDefaultParameterEditors)

{
    desiredWidth = 300;

    SpikePositionNode *processor = (SpikePositionNode *)getProcessor();

    latencyLabel = new Label("Latency", "Latency [s]:");
    latencyLabel->setBounds(10,35,80,25);
    addAndMakeVisible(latencyLabel);

    ipLabel = new Label("IP", "IP address:");
    ipLabel->setBounds(10,65,80,25);
    addAndMakeVisible(ipLabel);

    portLabel = new Label("Port", "Port:");
    portLabel->setBounds(10,95,80,25);
    addAndMakeVisible(portLabel);

    DBG("in editor set default address");
    int defaultLatency = DEFAULT_LATENCY;
    latencyEditLabel = new Label("EditLatency", String(defaultLatency));
    latencyEditLabel->setBounds(100,35,80,25);
    latencyEditLabel->setFont(Font("Default", 15, Font::plain));
    latencyEditLabel->setColour(Label::textColourId, Colours::white);
    latencyEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    latencyEditLabel->setEditable(true);
    latencyEditLabel->addListener(this);
    addAndMakeVisible(latencyEditLabel);
    processor->setLatency(defaultLatency);

    String defaultAddress = "localhost";
    ipEditLabel = new Label("EditAddress", defaultAddress);
    ipEditLabel->setBounds(100,65,80,25);
    ipEditLabel->setFont(Font("Default", 15, Font::plain));
    ipEditLabel->setColour(Label::textColourId, Colours::white);
    ipEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    ipEditLabel->setEditable(true);
    ipEditLabel->addListener(this);
    addAndMakeVisible(ipEditLabel);
    processor->setAddress(defaultAddress);

	int defaultPort = DEFAULT_SERVER_PORT;
    portEditLabel = new Label("Port", String(defaultPort));
    portEditLabel->setBounds(100,95,80,25);
    portEditLabel->setFont(Font("Default", 15, Font::plain));
    portEditLabel->setColour(Label::textColourId, Colours::white);
    portEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    portEditLabel->setEditable(true);
    portEditLabel->addListener(this);
    addAndMakeVisible(portEditLabel);
    processor->setPort(defaultPort);

    // Add buttons

	//Transmit (toggle)
	transmitButton = new UtilityButton("Transmit!", Font("Small Text", 13, Font::plain));
	transmitButton->setRadius(3.0f); // sets the radius of the button's corners
	transmitButton->setBounds(200, 50, 80, 20); // sets the x position, y position, width, and height of the button
	transmitButton->addListener(this); // allows the editor to respond to clicks
	transmitButton->setClickingTogglesState(true); // makes the button toggle its state when clicked
	addAndMakeVisible(transmitButton); // makes the button a child component of the editor and makes it visible

    //Test
    testButton = new UtilityButton("Test!", Font("Small Text", 13, Font::plain));
    testButton->setRadius(3.0f); // sets the radius of the button's corners
    testButton->setBounds(200, 80, 80, 20); // sets the x position, y position, width, and height of the button
    testButton->addListener(this); // allows the editor to respond to clicks
    testButton->setClickingTogglesState(true); // makes the button toggle its state when clicked
    addAndMakeVisible(testButton); // makes the button a child component of the editor and makes it visible

}

SpikePositionEditor::~SpikePositionEditor()
{
    // TODO should we delete all children, check JUCE docs
    // PS: Causes segfault if we do right now
//    deleteAllChildren();
}

void SpikePositionEditor::labelTextChanged(Label *label)
{
    if (label == latencyEditLabel)
    {
        Value val = label->getTextValue();

        SpikePositionNode *p= (SpikePositionNode *)getProcessor();
        p->setAddress(val.getValue());
    }
    if (label == ipEditLabel)
    {
        Value val = label->getTextValue();

        SpikePositionNode *p= (SpikePositionNode *)getProcessor();
        p->setPort(val.getValue());
        
    }
    if (label == portEditLabel)
    {
        Value val = label->getTextValue();

        SpikePositionNode *p= (SpikePositionNode *)getProcessor();
        p->setPort(val.getValue());
        
    }
}

void SpikePositionEditor::buttonEvent(Button *button)
{
    // Test
    if (button == testButton)
    {
        SpikePositionNode *p= (SpikePositionNode *)getProcessor();
        p->connectionTest();
        button->setToggleState(false,true);
    }

    if (button == transmitButton)
    {
        if (button->getToggleState()==true){
            SpikePositionNode *p= (SpikePositionNode *)getProcessor();
            p->startTransmission();
        }
        else {
            SpikePositionNode *p= (SpikePositionNode *)getProcessor();
            p->stopTransmission();
        }
    }
}


void SpikePositionEditor::saveCustomParameters(XmlElement *parentElement)
{
    XmlElement* mainNode = parentElement->createNewChildElement("SpikePositionNODE");
    mainNode->setAttribute("port", portEditLabel->getText());
    mainNode->setAttribute("address", ipEditLabel->getText());
}

void SpikePositionEditor::loadCustomParameters(XmlElement *parametersAsXml)
{
    if (parametersAsXml != nullptr)
    {
        forEachXmlChildElement(*parametersAsXml, mainNode)
        {
            if (mainNode->hasTagName("SpikePositionNODE"))
            {
                portEditLabel->setText(mainNode->getStringAttribute("port"),sendNotification);
                ipEditLabel->setText(mainNode->getStringAttribute("address"),sendNotification);
            }
        }
    }
}

