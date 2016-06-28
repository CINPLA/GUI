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

/**

  Used to change port, address, latency of transmission, to test connection, and to start/stop trasmission

  @see SpikePositionEditor

*/

class SpikePositionEditor : public GenericEditor, public Label::Listener
{
public:
    SpikePositionEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);
    virtual ~SpikePositionEditor();

    // Listener interface
    virtual void labelTextChanged(Label *labelThatHasChanged) override;
    virtual void buttonEvent(Button *button) override;


    // GenericEditor interface
    void saveCustomParameters(XmlElement *parentElement) override;
    void loadCustomParameters(XmlElement *parametersAsXml) override;

private:
    // Label with non-editable text
    ScopedPointer<Label> latencyLabel;
    ScopedPointer<Label> ipLabel;
    ScopedPointer<Label> portLabel;


    // Labels with editable test
    ScopedPointer<Label> latencyEditLabel;
    ScopedPointer<Label> ipEditLabel;
    ScopedPointer<Label> portEditLabel;

    // Test button
    ScopedPointer<UtilityButton> testButton;
    // Transmit button
    ScopedPointer<UtilityButton> transmitButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpikePositionEditor)



};

#endif  // SPIKEPOSITIONEDITOR_H_INCLUDED
