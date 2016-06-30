/*
  ==============================================================================

    TrackerStimulatorCanvas.cpp
    Created: 26 jun 2016
    Author:  alessio

  ==============================================================================
*/


#include "../../../JuceLibraryCode/JuceHeader.h"
#include "TrackerStimulatorCanvas.h"
#include "TrackerStimulator.h"


TrackerStimulatorCanvas::TrackerStimulatorCanvas(TrackerStimulator *TrackerStimulator)
    : processor(TrackerStimulator)
    , m_x(0.5)
    , m_y(0.5)
    , m_width(1.0)
    , m_height(1.0)
    , m_current_cx(0.0)
    , m_current_cy(0.0)
    , m_current_crad(0.0)
    , m_circleToDraw(false)
    , m_newCircle(false)
{
    // Setup buttons
    initButtons();
    // Setup Labels
    initLabels();

    startCallbacks();
    update();
}

TrackerStimulatorCanvas::~TrackerStimulatorCanvas()
{
    TopLevelWindow::getTopLevelWindow(0)->removeKeyListener(this);
}

void TrackerStimulatorCanvas::paint (Graphics& g)
{
    if(m_x != m_x || m_y != m_y || m_width != m_width || m_height != m_height) { // is it nan?
        return;
    }
    // set aspect ratio to cam size
    float aC = m_width / m_height;
    float aS = getWidth() / getHeight();
    int camHeight = (aS > aC) ? getHeight() : getHeight() * (aS / aC);
    int camWidth = (aS < aC) ? getWidth() : getWidth() * (aC / aS);

    g.setColour(Colours::black); // backbackround color
    g.fillRect(0, 0, getWidth(), getHeight());

    //    g.setColour(Colour(0,18,43)); //background color
    //    g.fillRect((getWidth()-camWidth)/2, (getHeight()-camHeight)/2, camWidth, camHeight);

    g.setColour(Colour(0,18,43)); //background color
    // Use Height as scaling -> always square
    g.fillRect(int(0.01*getHeight()), int(0.01*getHeight()), int(0.98*getHeight()), int(0.98*getHeight()));


    if (m_circleToDraw || m_newCircle)
    {
        if (m_newCircle)
        {
            m_current_cx = processor->getCx();
            m_current_cy = processor->getCy();
            m_current_crad = processor->getCradius();
            m_newCircle = false;
        }

        if ((m_current_crad > 0 && m_current_crad <= 1) && (m_current_cx >= 0 && m_current_cx <= 1)
                &&  (m_current_cy >= 0 && m_current_cy <= 1))
        {
            // draw circle
            int x, y, radx, rady;

            x = int(m_current_cx * 0.98*getHeight() + 0.01*getHeight());
            y = int(m_current_cy * 0.98*getHeight() + 0.01*getHeight());
            radx = int(m_current_crad * 0.98*getHeight());
            rady = int(m_current_crad * 0.98*getHeight());
            // center ellipse
            x -= radx;
            y -= rady;

            g.saveState();
            g.reduceClipRegion(int(0.01*getHeight()), int(0.01*getHeight()), int(0.98*getHeight()), int(0.98*getHeight()));
            g.setColour(Colour(200,255,30));
            g.fillEllipse(x, y, 2*radx, 2*rady);
            g.restoreState();
        }

    }


    // Draw a point for the current position
    // If inside circle display in RED

    if ((m_x > 0 && m_x <= 1) && (m_y >= 0 && m_y <= 1))
    {
        int x, y;

        x = int(m_x * 0.98*getHeight() + 0.01*getHeight());
        y = int(m_y * 0.98*getHeight() + 0.01*getHeight());

        if (pow(m_x - m_current_cx,2) + pow(m_y - m_current_cy,2)
                <= m_current_crad*m_current_crad)
            g.setColour(Colour(255,30,30));
        else
            g.setColour(Colour(30, 200, 180));

        g.saveState();
        g.reduceClipRegion(int(0.01*getHeight()), int(0.01*getHeight()), int(0.98*getHeight()), int(0.98*getHeight()));
        g.fillEllipse(x, y, 0.01*getHeight(), 0.01*getHeight());
        g.restoreState();

    }



}

void TrackerStimulatorCanvas::resized()
{
    // Express all positions as proportion of height and width
    // Buttons
    clearButton->setBounds(getWidth() - 0.35*getWidth(), getHeight()-0.1*getHeight(), 0.1*getWidth(),0.05*getHeight());

    drawButton->setBounds(getWidth() - 0.2*getWidth(), 0.05*getHeight(), 0.18*getWidth(),0.07*getHeight());
    uniformButton->setBounds(getWidth() - 0.2*getWidth(), 0.4*getHeight(), 0.09*getWidth(),0.03*getHeight());
    gaussianButton->setBounds(getWidth() - 0.2*getWidth() + 0.08*getWidth(), 0.4*getHeight(), 0.09*getWidth(),0.03*getHeight());
    negFirstButton->setBounds(getWidth() - 0.2*getWidth(), 0.7*getHeight(), 0.09*getWidth(),0.03*getHeight());
    posFirstButton->setBounds(getWidth() - 0.2*getWidth() + 0.08*getWidth(), 0.7*getHeight(), 0.09*getWidth(),0.03*getHeight());

    // Static Labels
    cxLabel->setBounds(getWidth() - 0.2*getWidth(), 0.15*getHeight(), 0.1*getWidth(),0.04*getHeight());
    cyLabel->setBounds(getWidth() - 0.2*getWidth(), 0.2*getHeight(), 0.1*getWidth(),0.04*getHeight());
    cradLabel->setBounds(getWidth() - 0.2*getWidth(), 0.25*getHeight(), 0.1*getWidth(),0.04*getHeight());

    fmaxLabel->setBounds(getWidth() - 0.2*getWidth(), 0.45*getHeight(), 0.1*getWidth(),0.04*getHeight());
    elecLabel->setBounds(getWidth() - 0.2*getWidth(), 0.55*getHeight(), 0.1*getWidth(),0.04*getHeight());

    phaseLabel->setBounds(getWidth() - 0.2*getWidth(), 0.75*getHeight(), 0.1*getWidth(),0.04*getHeight());
    interphaseLabel->setBounds(getWidth() - 0.2*getWidth(), 0.8*getHeight(), 0.1*getWidth(),0.04*getHeight());
    voltageLabel->setBounds(getWidth() - 0.2*getWidth(), 0.85*getHeight(), 0.1*getWidth(),0.04*getHeight());
    repetitionsLabel->setBounds(getWidth() - 0.2*getWidth(), 0.9*getHeight(), 0.1*getWidth(),0.04*getHeight());

    // Edit Labels
    cxEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.15*getHeight(), 0.08*getWidth(),0.04*getHeight());
    cyEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.2*getHeight(), 0.08*getWidth(),0.04*getHeight());
    cradEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.25*getHeight(), 0.08*getWidth(),0.04*getHeight());

    fmaxEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.45*getHeight(), 0.08*getWidth(),0.04*getHeight());
    elecEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.55*getHeight(), 0.08*getWidth(),0.04*getHeight());

    phaseEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.75*getHeight(), 0.08*getWidth(),0.04*getHeight());
    interphaseEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.8*getHeight(), 0.08*getWidth(),0.04*getHeight());
    voltageEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.85*getHeight(), 0.08*getWidth(),0.04*getHeight());
    repetitionsEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.9*getHeight(), 0.08*getWidth(),0.04*getHeight());


    refresh();

}


bool TrackerStimulatorCanvas::keyPressed(const KeyPress &key, Component *originatingComponent)
{
    return false;
}

void TrackerStimulatorCanvas::buttonClicked(Button* button)
{
    if (button == clearButton)
    {
        clear();
    }
    else if (button == drawButton)
    {
        m_circleToDraw = true;
        m_newCircle = true;
        repaint();
    }
    else if (button == uniformButton)
    {
        if (button->getToggleState()==true){
            if (gaussianButton->getToggleState()==true)
                gaussianButton->triggerClick();

            processor->setIsUniform(true);
        }
        else
            if (gaussianButton->getToggleState()==false)
                gaussianButton->triggerClick();
    }
    else if (button == gaussianButton)
    {
        if (button->getToggleState()==true){
            if (uniformButton->getToggleState()==true)
                uniformButton->triggerClick();

            processor->setIsUniform(false);
        }
        else
            if (uniformButton->getToggleState()==false)
                uniformButton->triggerClick();
    }
    else if (button == negFirstButton)
    {
        if (button->getToggleState()==true){
            if (posFirstButton->getToggleState()==true)
                posFirstButton->triggerClick();

            processor->setNegFirst(true);
        }
        else
            if (posFirstButton->getToggleState()==false)
                posFirstButton->triggerClick();
    }
    else if (button == posFirstButton)
    {
        if (button->getToggleState()==true){
            if (negFirstButton->getToggleState()==true)
                negFirstButton->triggerClick();

            processor->setNegFirst(false);
        }
        else
            if (negFirstButton->getToggleState()==false)
                negFirstButton->triggerClick();
    }

}

void TrackerStimulatorCanvas::labelTextChanged(Label *label)
{
    if (label == cxEditLabel)
    {
        Value val = label->getTextValue();
        processor->setCx(float(val.getValue()));
    }
    if (label == cyEditLabel)
    {
        Value val = label->getTextValue();
        processor->setCy(float(val.getValue()));
    }
    if (label == cradEditLabel)
    {
        Value val = label->getTextValue();
        processor->setCradius(float(val.getValue()));
    }
    if (label == fmaxEditLabel)
    {
        Value val = label->getTextValue();
        processor->setStimFreq(float(val.getValue()));
    }
    if (label == elecEditLabel)
    {
        Value val = label->getTextValue();
        processor->setStimElectrode(int(val.getValue()));
    }
    if (label == phaseEditLabel)
    {
        Value val = label->getTextValue();
        processor->setPhaseDuration(int(val.getValue()));
    }
    if (label == interphaseEditLabel)
    {
        Value val = label->getTextValue();
        processor->setInterPhaseInt(int(val.getValue()));
    }
    if (label == voltageEditLabel)
    {
        Value val = label->getTextValue();
        processor->setVoltage(float(val.getValue()));
    }
    if (label == repetitionsEditLabel)
    {
        Value val = label->getTextValue();
        processor->setRepetitions(int(val.getValue()));
    }
}



void TrackerStimulatorCanvas::refreshState()
{
}

void TrackerStimulatorCanvas::update()
{
}

void TrackerStimulatorCanvas::refresh()
{
    if(processor->positionDisplayedIsUpdated()) {
        processor->clearPositionDisplayedUpdated();
        m_x = processor->getX();
        m_y = processor->getY();
        m_width = processor->getWidth();
        m_height = processor->getHeight();
        repaint();
    }
}

void TrackerStimulatorCanvas::beginAnimation()
{
    startCallbacks();
}

void TrackerStimulatorCanvas::endAnimation()
{
    stopCallbacks();
}

void TrackerStimulatorCanvas::initButtons()
{

    clearButton = new UtilityButton("Clear plot", Font("Small Text", 13, Font::plain));
    clearButton->setRadius(3.0f);
    clearButton->addListener(this);
    addAndMakeVisible(clearButton);

    drawButton = new UtilityButton("Draw", Font("Small Text", 13, Font::plain));
    drawButton->setRadius(3.0f);
    drawButton->addListener(this);
    addAndMakeVisible(drawButton);

    uniformButton = new UtilityButton("uni", Font("Small Text", 13, Font::plain));
    uniformButton->setRadius(3.0f);
    uniformButton->addListener(this);
    uniformButton->setClickingTogglesState(true);
    uniformButton->triggerClick();
    addAndMakeVisible(uniformButton);

    gaussianButton = new UtilityButton("gauss", Font("Small Text", 13, Font::plain));
    gaussianButton->setRadius(3.0f);
    gaussianButton->addListener(this);
    gaussianButton->setClickingTogglesState(true);
    addAndMakeVisible(gaussianButton);

    negFirstButton = new UtilityButton("neg", Font("Small Text", 13, Font::plain));
    negFirstButton->setRadius(3.0f);
    negFirstButton->addListener(this);
    negFirstButton->setClickingTogglesState(true);
    negFirstButton->triggerClick();
    addAndMakeVisible(negFirstButton);

    posFirstButton = new UtilityButton("pos", Font("Small Text", 13, Font::plain));
    posFirstButton->setRadius(3.0f);
    posFirstButton->addListener(this);
    posFirstButton->setClickingTogglesState(true);
    addAndMakeVisible(posFirstButton);

}

void TrackerStimulatorCanvas::initLabels()
{
    // Static Labels
    cxLabel = new Label("s_cx", "xpos [%]:");
    cxLabel->setFont(Font(15));
    cxLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(cxLabel);

    cyLabel = new Label("s_cy", "ypos [%]:");
    cyLabel->setFont(Font(15));
    cyLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(cyLabel);

    cradLabel = new Label("s_crad", "radius [%]:");
    cradLabel->setFont(Font(15));
    cradLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(cradLabel);

    fmaxLabel = new Label("s_fmax", "fmax [Hz]:");
    fmaxLabel->setFont(Font(20));
    fmaxLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(fmaxLabel);

    elecLabel = new Label("s_elec", "# electrode:");
    elecLabel->setFont(Font(20));
    elecLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(elecLabel);

    phaseLabel = new Label("s_phase", "phase [us]:");
    phaseLabel->setFont(Font(15));
    phaseLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(phaseLabel);

    interphaseLabel = new Label("s_interphase", "inter [us]:");
    interphaseLabel->setFont(Font(15));
    interphaseLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(interphaseLabel);

    voltageLabel = new Label("s_voltage", "voltage [V]:");
    voltageLabel->setFont(Font(15));
    voltageLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(voltageLabel);

    repetitionsLabel = new Label("s_repetitions", "repetitions:");
    repetitionsLabel->setFont(Font(13));
    repetitionsLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(repetitionsLabel);

    // Edit Labels
    cxEditLabel = new Label("cx", " ");
    cxEditLabel->setFont(Font(15));
    cxEditLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    cxEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    cxEditLabel->setEditable(true);
    cxEditLabel->addListener(this);
    addAndMakeVisible(cxEditLabel);

    cyEditLabel = new Label("cy", " ");
    cyEditLabel->setFont(Font(15));
    cyEditLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    cyEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    cyEditLabel->setEditable(true);
    cyEditLabel->addListener(this);
    addAndMakeVisible(cyEditLabel);

    cradEditLabel = new Label("crad", " ");
    cradEditLabel->setFont(Font(15));
    cradEditLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    cradEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    cradEditLabel->setEditable(true);
    cradEditLabel->addListener(this);
    addAndMakeVisible(cradEditLabel);

    elecEditLabel = new Label("elec", String(0));
    elecEditLabel->setFont(Font(20));
    elecEditLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    elecEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    elecEditLabel->setEditable(true);
    elecEditLabel->addListener(this);
    addAndMakeVisible(elecEditLabel);

    fmaxEditLabel = new Label("fmax", String(DEF_FREQ));
    fmaxEditLabel->setFont(Font(20));
    fmaxEditLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    fmaxEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    fmaxEditLabel->setEditable(true);
    fmaxEditLabel->addListener(this);
    addAndMakeVisible(fmaxEditLabel);

    phaseEditLabel = new Label("phase", String(DEF_PHASE_DURATION));
    phaseEditLabel->setFont(Font(15));
    phaseEditLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    phaseEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    phaseEditLabel->setEditable(true);
    phaseEditLabel->addListener(this);
    addAndMakeVisible(phaseEditLabel);

    interphaseEditLabel = new Label("interphase", String(DEF_INTER_PHASE));
    interphaseEditLabel->setFont(Font(15));
    interphaseEditLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    interphaseEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    interphaseEditLabel->setEditable(true);
    interphaseEditLabel->addListener(this);
    addAndMakeVisible(interphaseEditLabel);

    voltageEditLabel = new Label("voltage", String(DEF_VOLTAGE));
    voltageEditLabel->setFont(Font(15));
    voltageEditLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    voltageEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    voltageEditLabel->setEditable(true);
    voltageEditLabel->addListener(this);
    addAndMakeVisible(voltageEditLabel);

    repetitionsEditLabel = new Label("repetitions", String(DEF_REPETITIONS));
    repetitionsEditLabel->setFont(Font(13));
    repetitionsEditLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    repetitionsEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    repetitionsEditLabel->setEditable(true);
    repetitionsEditLabel->addListener(this);
    addAndMakeVisible(repetitionsEditLabel);


}


void TrackerStimulatorCanvas::clear()
{
    m_circleToDraw = false;
    m_newCircle = false;
    repaint();
}


void TrackerStimulatorCanvas::setParameter(int, float)
{
}

void TrackerStimulatorCanvas::setParameter(int, int, int, float)
{
}
