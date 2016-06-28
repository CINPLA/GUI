#ifndef TRACKERSTIMULATORCANVAS_H
#define TRACKERSTIMULATORCANVAS_H

#include "TrackerStimulator.h"
#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../Visualization/Visualizer.h"
#include "../Editors/GenericEditor.h"

class TrackerStimulatorCanvas : public Visualizer,
        public ComboBox::Listener,
        public Button::Listener,
        public KeyListener
{
public:
    TrackerStimulatorCanvas(TrackerStimulator* trackerStimulator);
    ~TrackerStimulatorCanvas();

    void paint (Graphics&);
    void resized();
    void clear();
    // KeyListener interface
    virtual bool keyPressed(const KeyPress &key, Component *originatingComponent);

    // Listener interface
    virtual void buttonClicked(Button* button);

    // Listener interface
    virtual void comboBoxChanged(ComboBox *comboBoxThatHasChanged);

    // Visualizer interface
    virtual void refreshState();
    virtual void update();
    virtual void refresh();
    virtual void beginAnimation();
    virtual void endAnimation();
    virtual void setParameter(int, float);
    virtual void setParameter(int, int, int, float);

private:
    TrackerStimulator* processor;
    float m_x;
    float m_y;
    float m_width;
    float m_height;

    ScopedPointer<UtilityButton> clearButton;

    //std::vector<Position> m_positions;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackerStimulatorCanvas)
};


#endif // TRACKERSTIMULATORCANVAS_H
