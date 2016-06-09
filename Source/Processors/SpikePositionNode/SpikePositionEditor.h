/*
  ==============================================================================

    SpikePositionEditor.h
    Created: 11 Mar 2016 11:59:59am
    Author:  alessio

  ==============================================================================
*/

#ifndef SPIKEPOSITIONEDITOR_H_INCLUDED
#define SPIKEPOSITIONEDITOR_H_INCLUDED

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../Editors/GenericEditor.h"

#define DEFAULT_PORT 27021
#define DEFAULT_SERVER_PORT 9090
#define DEFAULT_LATENCY 2

class SpikePositionEditor : public GenericEditor, public Label::Listener
{
public:
    SpikePositionEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);
    virtual ~SpikePositionEditor();

private:
    // Label with non-editable text
    ScopedPointer<Label> latencyLabel;
    ScopedPointer<Label> ipLabel;
    ScopedPointer<Label> portLabel;


    // Labels with editable test
    ScopedPointer<Label> latencyEditLabel;
    ScopedPointer<Label> ipEditLabel;
    ScopedPointer<Label> portEditLabel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpikePositionEditor);

    // Test button
    ScopedPointer<UtilityButton> testButton;
    // Transmit button
    ScopedPointer<UtilityButton> transmitButton;


    // Listener interface
public:
    virtual void labelTextChanged(Label *labelThatHasChanged) override;
	virtual void buttonEvent(Button *button) override;
	

    // GenericEditor interface
public:
    void saveCustomParameters(XmlElement *parentElement) override;
    void loadCustomParameters(XmlElement *parametersAsXml) override;
};

#endif  // SPIKEPOSITIONEDITOR_H_INCLUDED
