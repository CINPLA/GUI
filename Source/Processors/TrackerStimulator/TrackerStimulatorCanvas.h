#ifndef TRACKERSTIMULATORCANVAS_H
#define TRACKERSTIMULATORCANVAS_H

#include "TrackerStimulator.h"
#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../Visualization/Visualizer.h"
#include "../Editors/GenericEditor.h"

class TrackerStimulatorCanvas : public Visualizer,
        public Button::Listener,
        public Label::Listener,
        public KeyListener
//        public ComboBox::Listener,
{
public:
    TrackerStimulatorCanvas(TrackerStimulator* trackerStimulator);
    ~TrackerStimulatorCanvas();

    void paint (Graphics&);
    void resized();
    void clear();
    void initButtons();
    void initLabels();

    TrackerStimulator* getProcessor();

    // KeyListener interface
    virtual bool keyPressed(const KeyPress &key, Component *originatingComponent);

    // Listener interface
    virtual void buttonClicked(Button* button);
    virtual void labelTextChanged(Label *label) override;

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

    float m_current_cx;
    float m_current_cy;
    float m_current_crad;

    bool m_circleToDraw;
    bool m_newCircle;

    ScopedPointer<UtilityButton> clearButton;
    ScopedPointer<UtilityButton> drawButton;
    ScopedPointer<UtilityButton> uniformButton;
    ScopedPointer<UtilityButton> gaussianButton;
    ScopedPointer<UtilityButton> negFirstButton;
    ScopedPointer<UtilityButton> posFirstButton;
    ScopedPointer<UtilityButton> biphasicButton;
    ScopedPointer<UtilityButton> chan1Button;
    ScopedPointer<UtilityButton> chan2Button;
    ScopedPointer<UtilityButton> chan3Button;
    ScopedPointer<UtilityButton> chan4Button;

    // Label with non-editable text
    ScopedPointer<Label> cxLabel;
    ScopedPointer<Label> cyLabel;
    ScopedPointer<Label> cradLabel;
    ScopedPointer<Label> fmaxLabel;
    ScopedPointer<Label> elecLabel;
    ScopedPointer<Label> pulsePalLabel;
    ScopedPointer<Label> pulsePalStatusLabel;
    ScopedPointer<Label> phaseLabel;
    ScopedPointer<Label> interphaseLabel;
    ScopedPointer<Label> voltageLabel;
    ScopedPointer<Label> repetitionsLabel;


    // Labels with editable test
    ScopedPointer<Label> cxEditLabel;
    ScopedPointer<Label> cyEditLabel;
    ScopedPointer<Label> cradEditLabel;
    ScopedPointer<Label> fmaxEditLabel;
    ScopedPointer<Label> elecEditLabel;
    ScopedPointer<Label> phaseEditLabel;
    ScopedPointer<Label> interphaseEditLabel;
    ScopedPointer<Label> voltageEditLabel;
    ScopedPointer<Label> repetitionsEditLabel;

    //std::vector<Position> m_positions;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackerStimulatorCanvas)
};


#endif // TRACKERSTIMULATORCANVAS_H
