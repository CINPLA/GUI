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


TrackerStimulatorCanvas::TrackerStimulatorCanvas(TrackerStimulator* trackerStimulator)
    : processor(trackerStimulator)
    , m_x(0.5)
    , m_y(0.5)
    , m_width(1.0)
    , m_height(1.0)
    , m_updateCircle(false)
    , m_onoff(false)
{
    // Setup buttons
    initButtons();
    // Setup Labels
    initLabels();

    m_ax = new DisplayAxes(trackerStimulator, this);

    startCallbacks();
    update();
}

TrackerStimulatorCanvas::~TrackerStimulatorCanvas()
{
    TopLevelWindow::getTopLevelWindow(0)->removeKeyListener(this);
}

void TrackerStimulatorCanvas::paint (Graphics& g)
{
    if(m_x != m_x || m_y != m_y || m_width != m_width || m_height != m_height)
    { // is it nan?
        return;
    }

    // set aspect ratio to cam size
//    float aC = m_width / m_height;
//    float aS = getWidth() / getHeight();
//    int camHeight = (aS > aC) ? getHeight() : getHeight() * (aS / aC);
//    int camWidth = (aS < aC) ? getWidth() : getWidth() * (aC / aS);

    g.setColour(Colours::black); // backbackround color
    g.fillRect(0, 0, getWidth(), getHeight());

    //    g.setColour(Colour(0,18,43)); //background color
    //    g.fillRect((getWidth()-camWidth)/2, (getHeight()-camHeight)/2, camWidth, camHeight);

    // on-off LED
    if (m_onoff)
        g.setColour(Colour(200,255,30));
    else
        g.setColour(Colour(Colours::grey));
    g.fillEllipse(getWidth() - 0.065*getWidth(), 0.25*getHeight(), 0.03*getWidth(), 0.03*getHeight());


    //m_ax->paint(Graphics g);
}

void TrackerStimulatorCanvas::resized()
{
    m_ax->setBounds(int(0.01*getHeight()), int(0.01*getHeight()), int(0.98*getHeight()), int(0.98*getHeight()));
    addAndMakeVisible(m_ax);

    // Express all positions as proportion of height and width
    // Buttons
    clearButton->setBounds(getWidth() - 0.35*getWidth(), getHeight()-0.1*getHeight(), 0.1*getWidth(),0.05*getHeight());

    newButton->setBounds(getWidth() - 0.2*getWidth(), 0.05*getHeight(), 0.06*getWidth(),0.04*getHeight());
    editButton->setBounds(getWidth() - 0.14*getWidth(), 0.05*getHeight(), 0.06*getWidth(),0.04*getHeight());
    delButton->setBounds(getWidth() - 0.08*getWidth(), 0.05*getHeight(), 0.06*getWidth(),0.04*getHeight());

    onButton->setBounds(getWidth() - 0.065*getWidth(), 0.21*getHeight(), 0.03*getWidth(),0.03*getHeight());

    for (int i = 0; i<MAX_CIRCLES; i++)
    {
        circlesButton[i]->setBounds(getWidth() - 0.2*getWidth()+i*(0.18/MAX_CIRCLES)*getWidth(), 0.32*getHeight(),
                                    (0.18/MAX_CIRCLES)*getWidth(),0.03*getHeight());
        circlesButton[i]->setVisible(false);
    }

    uniformButton->setBounds(getWidth() - 0.2*getWidth(), 0.4*getHeight(), 0.09*getWidth(),0.03*getHeight());
    gaussianButton->setBounds(getWidth() - 0.2*getWidth() + 0.09*getWidth(), 0.4*getHeight(), 0.09*getWidth(),0.03*getHeight());

    biphasicButton->setBounds(getWidth() - 0.2*getWidth(), 0.6*getHeight(), 0.18*getWidth(),0.03*getHeight());
    negFirstButton->setBounds(getWidth() - 0.2*getWidth(), 0.65*getHeight(), 0.09*getWidth(),0.03*getHeight());
    posFirstButton->setBounds(getWidth() - 0.2*getWidth() + 0.09*getWidth(), 0.65*getHeight(), 0.09*getWidth(),0.03*getHeight());
    chan1Button->setBounds(getWidth() - 0.2*getWidth(), 0.7*getHeight(), 0.045*getWidth(),0.03*getHeight());
    chan2Button->setBounds(getWidth() - 0.2*getWidth() + 0.045*getWidth(), 0.7*getHeight(), 0.045*getWidth(),0.03*getHeight());
    chan3Button->setBounds(getWidth() - 0.2*getWidth() + 0.09 *getWidth(), 0.7*getHeight(), 0.045*getWidth(),0.03*getHeight());
    chan4Button->setBounds(getWidth() - 0.2*getWidth() + 0.135*getWidth(), 0.7*getHeight(), 0.045*getWidth(),0.03*getHeight());



    // Static Labels
    cxLabel->setBounds(getWidth() - 0.2*getWidth(), 0.1*getHeight(), 0.06*getWidth(),0.04*getHeight());
    cyLabel->setBounds(getWidth() - 0.2*getWidth(), 0.15*getHeight(), 0.06*getWidth(),0.04*getHeight());
    cradLabel->setBounds(getWidth() - 0.2*getWidth(), 0.2*getHeight(), 0.06*getWidth(),0.04*getHeight());
//    onLabel->setBounds(getWidth() - 0.065*getWidth(), 0.16*getHeight(), 0.03*getWidth(),0.03*getHeight());

    fmaxLabel->setBounds(getWidth() - 0.2*getWidth(), 0.45*getHeight(), 0.1*getWidth(),0.04*getHeight());
    elecLabel->setBounds(getWidth() - 0.2*getWidth(), 0.5*getHeight(), 0.1*getWidth(),0.04*getHeight());

    phaseLabel->setBounds(getWidth() - 0.2*getWidth(), 0.75*getHeight(), 0.1*getWidth(),0.04*getHeight());
    interphaseLabel->setBounds(getWidth() - 0.2*getWidth(), 0.8*getHeight(), 0.1*getWidth(),0.04*getHeight());
    voltageLabel->setBounds(getWidth() - 0.2*getWidth(), 0.85*getHeight(), 0.1*getWidth(),0.04*getHeight());
    repetitionsLabel->setBounds(getWidth() - 0.2*getWidth(), 0.9*getHeight(), 0.1*getWidth(),0.04*getHeight());
    interpulseLabel->setBounds(getWidth() - 0.2*getWidth(), 0.95*getHeight(), 0.1*getWidth(),0.04*getHeight());

    // Edit Labels
    cxEditLabel->setBounds(getWidth() - 0.14*getWidth(), 0.1*getHeight(), 0.06*getWidth(),0.04*getHeight());
    cyEditLabel->setBounds(getWidth() - 0.14*getWidth(), 0.15*getHeight(), 0.06*getWidth(),0.04*getHeight());
    cradEditLabel->setBounds(getWidth() - 0.14*getWidth(), 0.2*getHeight(), 0.06*getWidth(),0.04*getHeight());

    fmaxEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.45*getHeight(), 0.08*getWidth(),0.04*getHeight());
    elecEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.5*getHeight(), 0.08*getWidth(),0.04*getHeight());

    phaseEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.75*getHeight(), 0.08*getWidth(),0.04*getHeight());
    interphaseEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.8*getHeight(), 0.08*getWidth(),0.04*getHeight());
    voltageEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.85*getHeight(), 0.08*getWidth(),0.04*getHeight());
    repetitionsEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.9*getHeight(), 0.08*getWidth(),0.04*getHeight());
    interpulseEditLabel->setBounds(getWidth() - 0.1*getWidth(), 0.95*getHeight(), 0.08*getWidth(),0.04*getHeight());


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
    else if (button == newButton)
    {
        m_updateCircle = true;

        // Create new circle
        Value x = cxEditLabel->getTextValue();
        Value y = cyEditLabel->getTextValue();
        Value rad = cradEditLabel->getTextValue();
        processor->addCircle(Circle(float(x.getValue()), float(y.getValue()), float(rad.getValue()), m_onoff));
        processor->setSelectedCircle(processor->getCircles().size()-1);
//        int length = processor->getCircles().size()-1;
        circlesButton[processor->getSelectedCircle()]->setVisible(true);

        // toggle current circle button (untoggles all the others)
        if (circlesButton[processor->getSelectedCircle()]->getToggleState()==false)
                circlesButton[processor->getSelectedCircle()]->triggerClick();


    }
    else if (button == editButton)
    {
        m_updateCircle = true;

        Value x = cxEditLabel->getTextValue();
        Value y = cyEditLabel->getTextValue();
        Value rad = cradEditLabel->getTextValue();
        processor->editCircle(processor->getSelectedCircle(),x.getValue(),y.getValue(),rad.getValue(),m_onoff);
    }
    else if (button == delButton)
    {
        m_updateCircle = true;

        processor->deleteCircle(processor->getSelectedCircle());
        // make visible only the remaining labels
        for (int i = 0; i<MAX_CIRCLES; i++)
        {
            if (i<processor->getCircles().size())
                circlesButton[i]->setVisible(true);
            else
                circlesButton[i]->setVisible(false);
        }

        // Blank labels and untoggle all circle buttons
        // blank labels
        processor->setSelectedCircle(-1);
        cxEditLabel->setText(String(""), dontSendNotification);
        cyEditLabel->setText(String(""), dontSendNotification);
        cradEditLabel->setText(String(""), dontSendNotification);
        m_onoff = false;

        for (int i = 0; i<MAX_CIRCLES; i++)
                circlesButton[i]->setToggleState(false, true);

    }
    else if (button == onButton)
    {
        m_onoff = !m_onoff;
        // make changes immediately if circle already exist
        if (processor->getSelectedCircle() != -1)
            editButton->triggerClick();
    }
    else if (button == uniformButton)
    {
        if (button->getToggleState()==true){
            if (gaussianButton->getToggleState()==true)
                gaussianButton->triggerClick();

            processor->setIsUniform(processor->getChan(), true);
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

            processor->setIsUniform(processor->getChan(), false);
        }
        else
            if (uniformButton->getToggleState()==false)
                uniformButton->triggerClick();

    }
    else if (button == biphasicButton)
    {
        if (button->getToggleState()==true)
            processor->setIsBiphasic(processor->getChan(), true);
        else
            processor->setIsBiphasic(processor->getChan(), false);
    }
    else if (button == negFirstButton)
    {
        if (button->getToggleState()==true){
            if (posFirstButton->getToggleState()==true)
                posFirstButton->triggerClick();

            processor->setNegFirst(processor->getChan(), true);
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

            processor->setNegFirst(processor->getChan(), false);
        }
        else
            if (negFirstButton->getToggleState()==false)
                negFirstButton->triggerClick();
    }
    else if (button == chan1Button)
    {
        if (button->getToggleState()==true){
            if (chan2Button->getToggleState()==true)
                chan2Button->triggerClick();
            if (chan3Button->getToggleState()==true)
                chan3Button->triggerClick();
            if (chan4Button->getToggleState()==true)
                chan4Button->triggerClick();

            processor->setChan(0);

            // retrieve and set label values
            fmaxEditLabel->setText(String(processor->getStimFreq(0)), dontSendNotification);
            elecEditLabel->setText(String(processor->getStimElectrode(0)), dontSendNotification);
            phaseEditLabel->setText(String(processor->getPhaseDuration(0)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(0)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(0)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(0)), dontSendNotification);

            // toggle buttons
            if (processor->getIsUniform(0) == true){
                if (uniformButton->getToggleState()==false)
                    uniformButton->triggerClick();
            } else {
                if (uniformButton->getToggleState()==true)
                    uniformButton->triggerClick();
            }

            if (processor->getIsBiphasic(0) == true) {
                if (biphasicButton->getToggleState()==false)
                    biphasicButton->triggerClick();
            } else {
                if (biphasicButton->getToggleState()==true)
                    biphasicButton->triggerClick();
            }

            if (processor->getNegFirst(0) == true) {
                if (negFirstButton->getToggleState()==false)
                    negFirstButton->triggerClick();
            } else {
                if (negFirstButton->getToggleState()==true)
                    negFirstButton->triggerClick();
            }


        }
        else
            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
                chan1Button->triggerClick();
    }
    else if (button == chan2Button)
    {
        if (button->getToggleState()==true){
            if (chan1Button->getToggleState()==true)
                chan1Button->triggerClick();
            if (chan3Button->getToggleState()==true)
                chan3Button->triggerClick();
            if (chan4Button->getToggleState()==true)
                chan4Button->triggerClick();

            processor->setChan(1);

            // retrieve and set label values
            fmaxEditLabel->setText(String(processor->getStimFreq(1)), dontSendNotification);
            elecEditLabel->setText(String(processor->getStimElectrode(1)), dontSendNotification);
            phaseEditLabel->setText(String(processor->getPhaseDuration(1)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(1)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(1)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(1)), dontSendNotification);

            // toggle buttons
            if (processor->getIsUniform(1) == true){
                if (uniformButton->getToggleState()==false)
                    uniformButton->triggerClick();
            } else {
                if (uniformButton->getToggleState()==true)
                    uniformButton->triggerClick();
            }

            if (processor->getIsBiphasic(1) == true) {
                if (biphasicButton->getToggleState()==false)
                    biphasicButton->triggerClick();
            } else {
                if (biphasicButton->getToggleState()==true)
                    biphasicButton->triggerClick();
            }

            if (processor->getNegFirst(1) == true) {
                if (negFirstButton->getToggleState()==false)
                    negFirstButton->triggerClick();
            } else {
                if (negFirstButton->getToggleState()==true)
                    negFirstButton->triggerClick();
            }


        }
        else
            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
                chan2Button->triggerClick();

    }
    else if (button == chan3Button)
    {
        if (button->getToggleState()==true){
            if (chan2Button->getToggleState()==true)
                chan2Button->triggerClick();
            if (chan1Button->getToggleState()==true)
                chan1Button->triggerClick();
            if (chan4Button->getToggleState()==true)
                chan4Button->triggerClick();

            processor->setChan(2);

            // retrieve and set label values
            fmaxEditLabel->setText(String(processor->getStimFreq(2)), dontSendNotification);
            elecEditLabel->setText(String(processor->getStimElectrode(2)), dontSendNotification);
            phaseEditLabel->setText(String(processor->getPhaseDuration(2)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(2)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(2)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(2)), dontSendNotification);

            // toggle buttons
            if (processor->getIsUniform(2) == true){
                if (uniformButton->getToggleState()==false)
                    uniformButton->triggerClick();
            } else {
                if (uniformButton->getToggleState()==true)
                    uniformButton->triggerClick();
            }

            if (processor->getIsBiphasic(2) == true) {
                if (biphasicButton->getToggleState()==false)
                    biphasicButton->triggerClick();
            } else {
                if (biphasicButton->getToggleState()==true)
                    biphasicButton->triggerClick();
            }

            if (processor->getNegFirst(2) == true) {
                if (negFirstButton->getToggleState()==false)
                    negFirstButton->triggerClick();
            } else {
                if (negFirstButton->getToggleState()==true)
                    negFirstButton->triggerClick();
            }
        }
        else
            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
                chan3Button->triggerClick();

    }
    else if (button == chan4Button)
    {
        if (button->getToggleState()==true){
            if (chan2Button->getToggleState()==true)
                chan2Button->triggerClick();
            if (chan3Button->getToggleState()==true)
                chan3Button->triggerClick();
            if (chan1Button->getToggleState()==true)
                chan1Button->triggerClick();

            processor->setChan(3);

            // retrieve and set label values
            fmaxEditLabel->setText(String(processor->getStimFreq(3)), dontSendNotification);
            elecEditLabel->setText(String(processor->getStimElectrode(3)), dontSendNotification);
            phaseEditLabel->setText(String(processor->getPhaseDuration(3)), dontSendNotification);
            interphaseEditLabel->setText(String(processor->getInterPhaseInt(3)), dontSendNotification);
            voltageEditLabel->setText(String(processor->getVoltage(3)), dontSendNotification);
            repetitionsEditLabel->setText(String(processor->getRepetitions(3)), dontSendNotification);

            // toggle buttons
            if (processor->getIsUniform(3) == true){
                if (uniformButton->getToggleState()==false)
                    uniformButton->triggerClick();
            } else {
                if (uniformButton->getToggleState()==true)
                    uniformButton->triggerClick();
            }

            if (processor->getIsBiphasic(3) == true) {
                if (biphasicButton->getToggleState()==false)
                    biphasicButton->triggerClick();
            } else {
                if (biphasicButton->getToggleState()==true)
                    biphasicButton->triggerClick();
            }

            if (processor->getNegFirst(3) == true) {
                if (negFirstButton->getToggleState()==false)
                    negFirstButton->triggerClick();
            } else {
                if (negFirstButton->getToggleState()==true)
                    negFirstButton->triggerClick();
            }
        }
        else
            if (chan1Button->getToggleState()==false && chan2Button->getToggleState() == false
                    && chan3Button->getToggleState()==false && chan4Button->getToggleState() == false)
                chan4Button->triggerClick();

    }
    else
    {
        // check if one of circle button has been clicked
        bool someToggled = false;
        for (int i = 0; i<MAX_CIRCLES; i++)
        {
            if (button == circlesButton[i] && circlesButton[i]->isVisible() )
            {
                // toggle button and untoggle all the others + update
                if (button->getToggleState()==true)
                {


                    for (int j = 0; j<MAX_CIRCLES; j++)
                        if (i!=j && circlesButton[j]->getToggleState()==true)
                        {
                            //                        circlesButton[j]->triggerClick();
                            circlesButton[j]->setToggleState(false, true);

                        }
                    processor->setSelectedCircle(i);
                    someToggled = true;
                    // retrieve labels and on button values
                    cxEditLabel->setText(String(processor->getCircles()[processor->getSelectedCircle()].getX()), dontSendNotification);
                    cyEditLabel->setText(String(processor->getCircles()[processor->getSelectedCircle()].getY()), dontSendNotification);
                    cradEditLabel->setText(String(processor->getCircles()[processor->getSelectedCircle()].getRad()), dontSendNotification);
                    m_onoff = processor->getCircles()[processor->getSelectedCircle()].getOn();
                }


            }
        }
        if (!someToggled)
        {
            // blank labels
            processor->setSelectedCircle(-1);
            cxEditLabel->setText(String(""), dontSendNotification);
            cyEditLabel->setText(String(""), dontSendNotification);
            cradEditLabel->setText(String(""), dontSendNotification);
            m_onoff = false;

        }

    }
     repaint();

}

void TrackerStimulatorCanvas::labelTextChanged(Label *label)
{
    // Instance a new circle only when the add new button is clicked
    if (label == cxEditLabel)
    {
        Value val = label->getTextValue();
        if (!(float(val.getValue())>=0 && float(val.getValue())<=1))
        {
            CoreServices::sendStatusMessage("Select value must be within 0 and 1!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == cyEditLabel)
    {
        Value val = label->getTextValue();
        if (!(float(val.getValue())>=0 && (float(val.getValue())<=1)))
        {
            CoreServices::sendStatusMessage("Select value must be within 0 and 1!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == cradEditLabel)
    {
        Value val = label->getTextValue();
        if (!(float(val.getValue())>=0 && (float(val.getValue())<=1)))
        {
            CoreServices::sendStatusMessage("Select value must be within 0 and 1!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == fmaxEditLabel)
    {
        Value val = label->getTextValue();
        if ((float(val.getValue())>=0 && float(val.getValue())<=1))
            processor->setStimFreq(processor->getChan(), float(val.getValue()));
        else
        {
            CoreServices::sendStatusMessage("Selected values cannot be negative!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == elecEditLabel)
    {
        Value val = label->getTextValue();
        if (int(val.getValue())>-1)
            processor->setStimElectrode(processor->getChan(), int(val.getValue()));
        else
        {
            CoreServices::sendStatusMessage("Selected values cannot be negative!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == phaseEditLabel)
    {
        // 100 - 3600*10e6 (3600 s)
        Value val = label->getTextValue();
        int value = int(round(float(val.getValue())/100) * 100); //only multiple of 100us
        if ((int(val.getValue())>=0 && int(val.getValue())<=3600*10e6))
        {
            processor->setPhaseDuration(processor->getChan(), value);
            label->setText(String(value), dontSendNotification);
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values must be within 0 and 3600*10e6 with 100 step!");
            label->setText("", dontSendNotification);

        }
    }
    if (label == interphaseEditLabel)
    {
        Value val = label->getTextValue();
        int value = int(round(float(val.getValue())/100) * 100); //only multiple of 100us
        if ((int(val.getValue())>=0 && int(val.getValue())<=3600*10e6))
        {
            processor->setInterPhaseInt(processor->getChan(), value);
            label->setText(String(value), dontSendNotification);
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values must be within 0 and 3600*10e6 with 100 step!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == voltageEditLabel)
    {
        Value val = label->getTextValue();
        if (float(val.getValue())>=0 && float(val.getValue())<=10)
        {
//            float value = float(int(float(val.getValue())*100) / 5 * 5)/100; //only multiple of 100us
            processor->setVoltage(processor->getChan(), float(val.getValue()));
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values must be within 0 and 10 with 0.05 step!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == repetitionsEditLabel)
    {
        Value val = label->getTextValue();
        if (int(val.getValue())>1)
            processor->setRepetitions(processor->getChan(), int(val.getValue()));
        else
        {
            CoreServices::sendStatusMessage("Selected values cannot be less than 1!");
            label->setText("", dontSendNotification);
        }
    }
    if (label == interpulseEditLabel)
    {
        Value val = label->getTextValue();
        int value = int(round(float(val.getValue())/100) * 100); //only multiple of 100us
        if ((int(val.getValue())>=0 && int(val.getValue())<=3600*10e6))
        {
            processor->setInterPulseInt(processor->getChan(), value);
            label->setText(String(value), dontSendNotification);
        }
        else
        {
            CoreServices::sendStatusMessage("Selected values must be within 0 and 3600*10e6 with 100 step!");
            label->setText("", dontSendNotification);
        }
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

    newButton = new UtilityButton("New", Font("Small Text", 13, Font::plain));
    newButton->setRadius(3.0f);
    newButton->addListener(this);
    addAndMakeVisible(newButton);

    editButton = new UtilityButton("Edit", Font("Small Text", 13, Font::plain));
    editButton->setRadius(3.0f);
    editButton->addListener(this);
    editButton->setColour(TextButton::buttonColourId, Colour(255, 255, 255));
    addAndMakeVisible(editButton);

    delButton = new UtilityButton("Del", Font("Small Text", 13, Font::plain));
    delButton->setRadius(3.0f);
    delButton->addListener(this);
    addAndMakeVisible(delButton);

    onButton = new UtilityButton("On", Font("Small Text", 13, Font::plain));
    onButton->setRadius(3.0f);
    onButton->addListener(this);
    addAndMakeVisible(onButton);


    // Create invisible circle toggle button
    for (int i = 0; i<MAX_CIRCLES; i++)
    {
        newcircButton = new UtilityButton(to_string(i+1), Font("Small Text", 13, Font::plain));
        newcircButton->setRadius(5.0f);
        newcircButton->addListener(this);
        newcircButton->setClickingTogglesState(true);
        //newcircButton->triggerClick();
        circlesButton[i] = newcircButton;
        addAndMakeVisible(circlesButton[i]);
    }

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

    biphasicButton = new UtilityButton("biphasic", Font("Small Text", 13, Font::plain));
    biphasicButton->setRadius(3.0f);
    biphasicButton->addListener(this);
    biphasicButton->setClickingTogglesState(true);
    biphasicButton->triggerClick();
    addAndMakeVisible(biphasicButton);

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

    chan1Button = new UtilityButton("1", Font("Small Text", 13, Font::plain));
    chan1Button->setRadius(3.0f);
    chan1Button->addListener(this);
    chan1Button->setClickingTogglesState(true);
    chan1Button->triggerClick();
    addAndMakeVisible(chan1Button);

    chan2Button = new UtilityButton("2", Font("Small Text", 13, Font::plain));
    chan2Button->setRadius(3.0f);
    chan2Button->addListener(this);
    chan2Button->setClickingTogglesState(true);
    addAndMakeVisible(chan2Button);

    chan3Button = new UtilityButton("3", Font("Small Text", 13, Font::plain));
    chan3Button->setRadius(3.0f);
    chan3Button->addListener(this);
    chan3Button->setClickingTogglesState(true);
    addAndMakeVisible(chan3Button);

    chan4Button = new UtilityButton("4", Font("Small Text", 13, Font::plain));
    chan4Button->setRadius(3.0f);
    chan4Button->addListener(this);
    chan4Button->setClickingTogglesState(true);
    addAndMakeVisible(chan4Button);

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

//    onLabel = new Label("S_ON", "radius [%]:");
//    onLabel->setFont(Font(15));
//    onLabel->setColour(Label::textColourId, Colour(200, 255, 0));
//    addAndMakeVisible(onLabel);

    fmaxLabel = new Label("s_fmax", "fmax [Hz]:");
    fmaxLabel->setFont(Font(20));
    fmaxLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(fmaxLabel);

    elecLabel = new Label("s_elec", "# electrode:");
    elecLabel->setFont(Font(20));
    elecLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(elecLabel);

    pulsePalLabel = new Label("s_pp", "Pulse Pal:");
    pulsePalLabel->setFont(Font(20));
    pulsePalLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(pulsePalLabel);

    pulsePalStatusLabel = new Label("s_pp", "Pulse Pal:");
    pulsePalStatusLabel->setFont(Font(20));
    pulsePalStatusLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(pulsePalStatusLabel);

    phaseLabel = new Label("s_phase", "phase [us]:");
    phaseLabel->setFont(Font(15));
    phaseLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(phaseLabel);

    interphaseLabel = new Label("s_interphase", "interphase [us]:");
    interphaseLabel->setFont(Font(15));
    interphaseLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(interphaseLabel);

    voltageLabel = new Label("s_voltage", "voltage [V]:");
    voltageLabel->setFont(Font(15));
    voltageLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(voltageLabel);

    interpulseLabel = new Label("s_interpulse", "interpulse [us]:");
    interpulseLabel->setFont(Font(15));
    interpulseLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(interpulseLabel);

    repetitionsLabel = new Label("s_repetitions", "repetitions:");
    repetitionsLabel->setFont(Font(15));
    repetitionsLabel->setColour(Label::textColourId, Colour(200, 255, 0));
    addAndMakeVisible(repetitionsLabel);

    // Edit Labels
    cxEditLabel = new Label("cx", " ");
    cxEditLabel->setFont(Font(15));
    cxEditLabel->setColour(Label::textColourId, Colour(200, 200, 0));
    cxEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    cxEditLabel->setEditable(true);
    cxEditLabel->addListener(this);
    addAndMakeVisible(cxEditLabel);

    cyEditLabel = new Label("cy", " ");
    cyEditLabel->setFont(Font(15));
    cyEditLabel->setColour(Label::textColourId, Colour(200, 200, 0));
    cyEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    cyEditLabel->setEditable(true);
    cyEditLabel->addListener(this);
    addAndMakeVisible(cyEditLabel);

    cradEditLabel = new Label("crad", " ");
    cradEditLabel->setFont(Font(15));
    cradEditLabel->setColour(Label::textColourId, Colour(200, 200, 0));
    cradEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    cradEditLabel->setEditable(true);
    cradEditLabel->addListener(this);
    addAndMakeVisible(cradEditLabel);

    elecEditLabel = new Label("elec", String(0));
    elecEditLabel->setFont(Font(20));
    elecEditLabel->setColour(Label::textColourId, Colour(200, 200, 0));
    elecEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    elecEditLabel->setEditable(true);
    elecEditLabel->addListener(this);
    addAndMakeVisible(elecEditLabel);

    fmaxEditLabel = new Label("fmax", String(DEF_FREQ));
    fmaxEditLabel->setFont(Font(20));
    fmaxEditLabel->setColour(Label::textColourId, Colour(200, 200, 0));
    fmaxEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    fmaxEditLabel->setEditable(true);
    fmaxEditLabel->addListener(this);
    addAndMakeVisible(fmaxEditLabel);

    phaseEditLabel = new Label("phase", String(DEF_PHASE_DURATION));
    phaseEditLabel->setFont(Font(15));
    phaseEditLabel->setColour(Label::textColourId, Colour(200, 200, 0));
    phaseEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    phaseEditLabel->setEditable(true);
    phaseEditLabel->addListener(this);
    addAndMakeVisible(phaseEditLabel);

    interphaseEditLabel = new Label("interphase", String(DEF_INTER_PHASE));
    interphaseEditLabel->setFont(Font(15));
    interphaseEditLabel->setColour(Label::textColourId, Colour(200, 200, 0));
    interphaseEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    interphaseEditLabel->setEditable(true);
    interphaseEditLabel->addListener(this);
    addAndMakeVisible(interphaseEditLabel);

    voltageEditLabel = new Label("voltage", String(DEF_VOLTAGE));
    voltageEditLabel->setFont(Font(15));
    voltageEditLabel->setColour(Label::textColourId, Colour(200, 200, 0));
    voltageEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    voltageEditLabel->setEditable(true);
    voltageEditLabel->addListener(this);
    addAndMakeVisible(voltageEditLabel);

    repetitionsEditLabel = new Label("repetitions", String(DEF_REPETITIONS));
    repetitionsEditLabel->setFont(Font(15));
    repetitionsEditLabel->setColour(Label::textColourId, Colour(200, 200, 0));
    repetitionsEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    repetitionsEditLabel->setEditable(true);
    repetitionsEditLabel->addListener(this);
    addAndMakeVisible(repetitionsEditLabel);

    interpulseEditLabel = new Label("interpulse", String(DEF_INTER_PULSE));
    interpulseEditLabel->setFont(Font(15));
    interpulseEditLabel->setColour(Label::textColourId, Colour(200, 200, 0));
    interpulseEditLabel->setColour(Label::backgroundColourId, Colours::grey);
    interpulseEditLabel->setEditable(true);
    interpulseEditLabel->addListener(this);
    addAndMakeVisible(interpulseEditLabel);


}


void TrackerStimulatorCanvas::clear()
{
    // set all circles to off
    processor->disableCircles();

    repaint();
}

bool TrackerStimulatorCanvas::getUpdateCircle(){
    return m_updateCircle;
}

void TrackerStimulatorCanvas::setOnButton()
{
    m_onoff = true;
}


void TrackerStimulatorCanvas::setParameter(int, float)
{
}

void TrackerStimulatorCanvas::setParameter(int, int, int, float)
{
}


// DisplayAxes methods

DisplayAxes::DisplayAxes(TrackerStimulator* trackerStimulator, TrackerStimulatorCanvas* trackerStimulatorCanvas)
    : processor(trackerStimulator)
    , canvas(trackerStimulatorCanvas)
    , selectedCircleColour(Colour(255,155,30))
    , unselectedCircleColour(Colour(200,255,30))
    , backgroundColour(Colour(0,18,43))
    , outOfCirclesColour(Colour(30, 200, 180))
    , inOfCirclesColour(Colour(255,30,30))
    , m_creatingNewCircle(false)
    , m_movingCircle(false)
    , m_mayBeMoving(false)
    , m_firstPaint(true)

{
    // *** Add: 1) delete circle with delete button
    //          2) double fast click -> change size

    xlims[0] = getBounds().getRight() - getWidth();
    xlims[1] = getBounds().getRight();
    ylims[0] = getBounds().getBottom() - getHeight();
    ylims[1] = getBounds().getBottom();

}

DisplayAxes::~DisplayAxes(){}

void DisplayAxes::paint(Graphics& g){

    xlims[0] = getBounds().getRight() - getWidth();
    xlims[1] = getBounds().getRight();
    ylims[0] = getBounds().getBottom() - getHeight();
    ylims[1] = getBounds().getBottom();

    g.setColour(backgroundColour); //background color
    g.fillAll();

    if (canvas->getUpdateCircle())
    {

        for (int i = 0; i < processor->getCircles().size(); i++)
        {

            // draw circle if it is ON
            if (processor->getCircles()[i].getOn())
            {
                float cur_x, cur_y, cur_rad;
                int x, y, radx, rady;

                cur_x = processor->getCircles()[i].getX();
                cur_y = processor->getCircles()[i].getY();
                cur_rad = processor->getCircles()[i].getRad();


                x = int(cur_x * getWidth() + xlims[0]);
                y = int(cur_y * getHeight() + ylims[0]);

                radx = int(cur_rad * getWidth());
                rady = int(cur_rad * getHeight());
                // center ellipse
                x -= radx;
                y -= rady;

//                g.saveState();
//                g.reduceClipRegion(getBounds());
                if (i==processor->getSelectedCircle())
                {
                    // if circle is being moved, draw moving circle
                    if (!m_movingCircle)
                    {
                        g.setColour(selectedCircleColour);
                        g.fillEllipse(x, y, 2*radx, 2*rady);
                    }
                }
                else
                {
                    g.setColour(unselectedCircleColour);
                    g.fillEllipse(x, y, 2*radx, 2*rady);
                }

//                g.restoreState();
            }
        }

    }

    float pos_x = processor->getX();
    float pos_y = processor->getY();

    // Draw a point for the current position
    // If inside circle display in RED

    if ((pos_x >= 0 && pos_x <= 1) && (pos_y >= 0 && pos_y <= 1))
    {
        int x, y;

        x = int(pos_x * getWidth() + xlims[0]);
        y = int(pos_y * getHeight() + ylims[0]);

        int circleIn = processor->isPositionWithinCircles(pos_x, pos_y);

        if (circleIn != -1 && processor->getCircles()[circleIn].getOn())
            g.setColour(inOfCirclesColour);
        else
            g.setColour(outOfCirclesColour);

        //g.saveState();
        //g.reduceClipRegion(getBounds());
        g.fillEllipse(x, y, 0.01*getHeight(), 0.01*getHeight());
        //g.restoreState();

    }

    if (m_creatingNewCircle)
    {
        // draw circle increasing in size
        int x, y, radx, rady;

        x = int(m_newX * getWidth() + xlims[0]);
        y = int(m_newY * getHeight() + ylims[0]);
        radx = int(m_tempRad * getWidth());
        rady = int(m_tempRad * getHeight());
        // center ellipse
        x -= radx;
        y -= rady;

        g.setColour(unselectedCircleColour);
        g.fillEllipse(x, y, 2*radx, 2*rady);

    }

    if (m_movingCircle)
    {
        // draw moving circle
        int x, y, radx, rady;

        x = int(m_newX * getWidth() + xlims[0]);
        y = int(m_newY * getHeight() + ylims[0]);
        m_tempRad = processor->getCircles()[processor->getSelectedCircle()].getRad();
        radx = int(m_tempRad * getWidth());
        rady = int(m_tempRad * getHeight());
        // center ellipse
        x -= radx;
        y -= rady;

        g.setColour(selectedCircleColour);
        g.fillEllipse(x, y, 2*radx, 2*rady);
    }

//    if (m_doubleClick)
//    {
//        // draw moving circle
//        int x, y, radx, rady;

//        x = int(m_newX * getWidth() + xlims[0]);
//        y = int(m_newY * getHeight() + ylims[0]);
//        m_tempRad = processor->getCircles()[processor->getSelectedCircle()].getRad();
//        radx = int(m_tempRad * getWidth());
//        rady = int(m_tempRad * getHeight());
//        // center ellipse
//        x -= radx;
//        y -= rady;

//        g.setColour(selectedCircleColour);
//        g.fillEllipse(x, y, 2*radx, 2*rady);
//    }

}

void DisplayAxes::clear(){}

void DisplayAxes::mouseMove(const MouseEvent& event){
    if (m_creatingNewCircle)
    {
        // compute m_tempRad
        m_tempRad = sqrt((pow(float(event.x)/float(getWidth())-m_newX, 2) +
                        pow(float(event.y)/float(getHeight())-m_newY, 2)));
        repaint();
    }
//    if (m_doubleClick)
//    {
//        // compute m_tempRad
//        m_tempRad = sqrt((pow(float(event.x)/float(getWidth())-m_newX, 2) +
//                        pow(float(event.y)/float(getHeight())-m_newY, 2)));
//        m_newX = processor->getCircles()[processor->getSelectedCircle()].getX();
//        m_newY = processor->getCircles()[processor->getSelectedCircle()].getY();
//        repaint();
//    }
}

void DisplayAxes::mouseEnter(const MouseEvent& event){
    setMouseCursor(MouseCursor::CrosshairCursor);
}

void DisplayAxes::mouseExit(const MouseEvent& event){
    m_movingCircle = false;
    m_creatingNewCircle = false;
    m_mayBeMoving = false;
}

void DisplayAxes::mouseDown(const MouseEvent& event)
{
    if (!event.mods.isRightButtonDown())
    {
        // check previous click time
        int64 current = Time::currentTimeMillis();

        int circleIn = processor->isPositionWithinCircles(float(event.x)/float(getWidth()),
                                                          float(event.y)/float(getHeight()));
        // If on a circle -> select circle!
        if (circleIn != -1)
        {
            if (canvas->circlesButton[circleIn]->getToggleState() != true)
                canvas->circlesButton[circleIn]->triggerClick();
            m_creatingNewCircle = false;
            m_newX = float(event.x)/float(getWidth());
            m_newY = float(event.y)/float(getHeight());
            m_mayBeMoving = true;

//            if (current-click_time <= 300)
//                m_doubleClick = true;
//            else
//            {
//                m_mayBeMoving = true;
//                m_doubleClick = false;
//                click_time = Time::currentTimeMillis();
//            }
        }
        else if (m_creatingNewCircle)
        {
            m_creatingNewCircle = false;
            m_newRad = sqrt((pow(float(event.x)/float(getWidth())-m_newX, 2) +
                            pow(float(event.y)/float(getHeight())-m_newY, 2)));
            canvas->cradEditLabel->setText(String(m_newRad), dontSendNotification);
            setMouseCursor(MouseCursor::CrosshairCursor);

            // Add new circle
            canvas->setOnButton();
            canvas->newButton->triggerClick();

        }
        else  // Else -> create new circle (set center and drag radius)
        {
            m_creatingNewCircle = true;
            m_newX = float(event.x)/float(getWidth());
            m_newY = float(event.y)/float(getHeight());
            setMouseCursor(MouseCursor::DraggingHandCursor);
            canvas->cxEditLabel->setText(String(m_newX), dontSendNotification);
            canvas->cyEditLabel->setText(String(m_newY), dontSendNotification);
        }
    }
}

void DisplayAxes::mouseUp(const MouseEvent& event){
    if (m_movingCircle)
    {
        // Change to new center and edit circle
        m_newX = float(event.x)/float(getWidth());
        m_newY = float(event.y)/float(getHeight());
        canvas->cxEditLabel->setText(String(m_newX),dontSendNotification);
        canvas->cyEditLabel->setText(String(m_newY),dontSendNotification);

        canvas->editButton->triggerClick();
        m_movingCircle = false;
    }
}

void DisplayAxes::mouseDrag(const MouseEvent& event){
    if (m_mayBeMoving)
    {
        // if dragging is grater than 0.05 -> start moving circle
        // compute m_tempRad
        float cx, cy;
        cx = processor->getCircles()[processor->getSelectedCircle()].getX();
        cy = processor->getCircles()[processor->getSelectedCircle()].getY();
        m_tempRad = sqrt((pow(float(event.x)/float(getWidth())-cx, 2) +
                        pow(float(event.y)/float(getHeight())-cy, 2)));

        if (m_tempRad > 0.02)
        {
            m_movingCircle = true;
            m_mayBeMoving = false;
        }
    }
    if (m_movingCircle)
    {
        m_newX = float(event.x)/float(getWidth());
        m_newY = float(event.y)/float(getHeight());

        // Check boundaries
        if (!(m_newX <= 1 && m_newX >= 0) || !(m_newY <= 1 && m_newY >= 0))
            m_movingCircle = false;
        repaint();
    }
}






