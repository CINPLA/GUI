#ifndef TRACKERSTIMULATOREDITOR_H
#define TRACKERSTIMULATOREDITOR_H


#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../Editors/GenericEditor.h"
#include "../Editors/VisualizerEditor.h"


class TrackerStimulatorEditor : public VisualizerEditor
{
public:
    TrackerStimulatorEditor(GenericProcessor* parentNode, bool useDefaultParameterEditors);
    virtual ~TrackerStimulatorEditor();

    // Listener Interface
//    void buttonEvent(Button* button);
    void buttonCallback(Button* button);

    Visualizer* createNewCanvas();

private:
    // Transmit button
    ScopedPointer<TextButton> stimulateButton;

    // Transmit button
    ScopedPointer<UtilityButton> testPatternButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackerStimulatorEditor)
};


#endif // TRACKERSTIMULATOREDITOR_H
