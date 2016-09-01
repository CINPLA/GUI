/*
  ==============================================================================

    TrackerStimulator.cpp
    Created: 11 Mar 2016 11:59:38am
    Author:  alessio

  ==============================================================================
*/

#include "TrackerStimulator.h"
#include "TrackerStimulatorEditor.h"

#include "../../UI/EditorViewport.h"

TrackerStimulator::TrackerStimulator()
    : GenericProcessor("Tracker Stimulator")
    , m_isOn(false)
    , m_x(0.0)
    , m_y(0.0)
    , m_height(1.0)
    , m_width(1.0)
    , m_positionIsUpdated(false)
    , m_positionDisplayedIsUpdated(false)
    , m_selectedCircle(-1)
    , m_tot_chan(4)
    , m_timePassed(0.0)
    , m_currentTime(0.0)
    , m_chan(0)
    , m_pulsePal()
{
    // Init PulsePal
    m_pulsePal.initialize();
    m_pulsePal.setDefaultParameters();
    m_pulsePal.updateDisplay("GUI Connected","Click for menu");
    m_pulsePalVersion = m_pulsePal.getFirmwareVersion();

    m_circles = vector<Circle>();

    m_stimFreq = vector<float>(m_tot_chan, DEF_FREQ);
    m_stimElectrode = vector<int>(m_tot_chan, 0);

    m_phaseDuration = vector<int>(m_tot_chan, DEF_PHASE_DURATION);
    m_interPhaseInt = vector<int>(m_tot_chan, DEF_INTER_PHASE);
    m_repetitions = vector<int>(m_tot_chan, DEF_REPETITIONS);
    m_voltage = vector<float>(m_tot_chan, DEF_VOLTAGE);
    m_voltage = vector<int>(m_tot_chan, DEF_INTER_PULSE);

    m_isUniform = vector<int>(m_tot_chan, 1);
    m_isBiphasic = vector<int>(m_tot_chan, 1);
    m_negativeFirst = vector<int>(m_tot_chan, 1);
    //int a = 5;
}

TrackerStimulator::~TrackerStimulator()
{

}

AudioProcessorEditor* TrackerStimulator::createEditor()
{
    editor = new TrackerStimulatorEditor(this, true);
    return editor;
}


// Setters - Getters

float TrackerStimulator::getX() const
{
    return m_x;
}
float TrackerStimulator::getY() const
{
    return m_y;
}
float TrackerStimulator::getWidth() const
{
    return m_width;
}
float TrackerStimulator::getHeight() const
{
    return m_height;
}


vector<Circle> TrackerStimulator::getCircles()
{
    return m_circles;
}
void TrackerStimulator::addCircle(Circle c)
{
    m_circles.push_back(c);
}
void TrackerStimulator::editCircle(int ind, float x, float y, float rad, bool on)
{
    m_circles[ind].set(x,y,rad,on);
}
void TrackerStimulator::deleteCircle(int ind)
{
    m_circles.erase(m_circles.begin() + ind);
}
void TrackerStimulator::disableCircles()
{
    for(int i=0; i<m_circles.size(); i++)
        m_circles[i].off();
}

int TrackerStimulator::getSelectedCircle() const
{
    return m_selectedCircle;
}
void TrackerStimulator::setSelectedCircle(int ind)
{
    m_selectedCircle = ind;
}

int TrackerStimulator::getChan() const
{
    return m_chan;
}


float TrackerStimulator::getStimFreq(int chan) const
{
    return m_stimFreq[chan];
}
int TrackerStimulator::getStimElectrode(int chan) const
{
    return m_stimElectrode[chan];
}
bool TrackerStimulator::getIsUniform(int chan) const
{
    if (m_isUniform[chan])
        return true;
    else
        return false;
}
bool TrackerStimulator::getIsBiphasic(int chan) const
{
    if (m_isBiphasic[chan])
        return true;
    else
        return false;
}
bool TrackerStimulator::getNegFirst(int chan) const
{
    if (m_negativeFirst[chan])
        return true;
    else
        return false;
}
int TrackerStimulator::getPhaseDuration(int chan) const
{
    return m_phaseDuration[chan];
}
int TrackerStimulator::getInterPhaseInt(int chan) const
{
    return m_interPhaseInt[chan];
}
float TrackerStimulator::getVoltage(int chan) const
{
    return m_voltage[chan];
}
int TrackerStimulator::getRepetitions(int chan) const
{
    return m_repetitions[chan];
}
int TrackerStimulator::getInterPulseInt(int chan) const
{
    return m_interPulseInt[chan];
}

void TrackerStimulator::setStimFreq(int chan, float stimFreq)
{
    m_stimFreq[chan] = stimFreq;
}
void TrackerStimulator::setStimElectrode(int chan, int stimElectrode)
{
    m_stimElectrode[chan] = stimElectrode;
}
void TrackerStimulator::setIsUniform(int chan, bool isUniform)
{
    if (isUniform)
        m_isUniform[chan] = 1;
    else
        m_isUniform[chan] = 0;
}
void TrackerStimulator::setIsBiphasic(int chan, bool isBiphasic)
{
    if (isBiphasic)
        m_isBiphasic[chan] = 1;
    else
        m_isBiphasic[chan] = 0;
}
void TrackerStimulator::setNegFirst(int chan, bool negFirst)
{
    if (negFirst)
        m_negativeFirst[chan] = 1;
    else
        m_negativeFirst[chan] = 0;

}
void TrackerStimulator::setPhaseDuration(int chan, int phaseDuration)
{
    m_phaseDuration[chan] = phaseDuration;
    updatePulsePal();
}
void TrackerStimulator::setInterPhaseInt(int chan, int interPhaseInt)
{
    m_interPhaseInt[chan] = interPhaseInt;
    updatePulsePal();
}
void TrackerStimulator::setVoltage(int chan, float voltage)
{
    m_voltage[chan] = voltage;
    updatePulsePal();
}
void TrackerStimulator::setRepetitions(int chan, int rep)
{
    m_repetitions[chan] = rep;
    updatePulsePal();
}
void TrackerStimulator::setInterPulseInt(int chan, int interPulseInt)
{
    m_interPulseInt[chan] = interPulseInt;
    updatePulsePal();
}
void TrackerStimulator::setChan(int chan)
{
    m_chan = chan;
}


void TrackerStimulator::process(AudioSampleBuffer& buffer, MidiBuffer& events)
{
    setTimestamp(events,CoreServices::getGlobalTimestamp());
    checkForEvents(events);

    if (m_isOn){

        m_currentTime = Time::currentTimeMillis();
        m_timePassed = float(m_currentTime - m_previousTime)/1000.0; // in seconds

        lock.enter();

        // Check if current position is within stimulation areas
        bool stim = stimulate();

        if (stim)
        {
            // Check if timePassed >= latency
            if (m_timePassed >= float(1/m_stimFreq[m_chan]))
            {
                // trigger selected channel
                m_pulsePal.triggerChannel(m_chan);
                m_previousTime = Time::currentTimeMillis();
                m_timePassed = 0;
            }

        }

        lock.exit();
    }

}

void TrackerStimulator::handleEvent(int eventType, MidiMessage &event, int samplePosition)
{
    // Get current position
    if(eventType == BINARY_MSG) {
        const uint8* rawData = event.getRawData();
        if(event.getRawDataSize() != 6 + sizeof(float)*4) {
            cout << "Position tracker got wrong event size x,y,width,height was expected: " << event.getRawDataSize() << endl;
        }
        const float* message = (float*)(rawData+6);
        m_x = message[0];
        m_y = message[1];
        m_width = message[2];
        m_height = message[3];
        m_positionIsUpdated = true;
    }

}

int TrackerStimulator::isPositionWithinCircles(float x, float y)
{
    int whichCircle = -1;
    for (int i = 0; i < m_circles.size(); i++)
    {
        if (m_circles[i].isPositionIn(x,y))
            whichCircle = i;
    }
    return whichCircle;
}

bool TrackerStimulator::stimulate()
{
    if (isPositionWithinCircles(m_x, m_y) != -1)
        return true;
    else
        return false;
}

bool TrackerStimulator::positionDisplayedIsUpdated() const
{
    return m_positionDisplayedIsUpdated;
}

void TrackerStimulator::clearPositionDisplayedUpdated()
{
    m_positionDisplayedIsUpdated = false;
}


bool TrackerStimulator::updatePulsePal()
{
    // check that Pulspal is connected and update param
    if (m_pulsePalVersion != 0)
    {
        m_pulsePal.setBiphasic(m_chan, m_isBiphasic[m_chan]);
        if (m_negativeFirst[m_chan])
        {
            m_pulsePal.setPhase1Voltage(m_chan, - m_voltage[m_chan]);
            m_pulsePal.setPhase2Voltage(m_chan, m_voltage[m_chan]);
        }
        else
        {
            m_pulsePal.setPhase1Voltage(m_chan, m_voltage[m_chan]);
            m_pulsePal.setPhase2Voltage(m_chan, - m_voltage[m_chan]);
        }

        m_pulsePal.setPhase1Duration(m_chan, float(m_phaseDuration[m_chan])/10e6);
        m_pulsePal.setPhase2Duration(m_chan, float(m_phaseDuration[m_chan])/10e6);
        m_pulsePal.setInterPhaseInterval(m_chan, float(m_interPhaseInt[m_chan])/10e6);

        m_pulsePal.setPulseTrainDuration(m_chan, float(m_interPulseInt[m_chan])/10e6 * m_repetitions[m_chan]);
    }
    else
        CoreServices::sendStatusMessage("PulsePal is not connected!");
}

bool TrackerStimulator::testStimulation(){

    // check that Pulspal is connected and update param
    if (m_pulsePalVersion != 0)
        m_pulsePal.triggerChannel(m_chan);
    else
        CoreServices::sendStatusMessage("PulsePal is not connected!");

} //test from Editor


void TrackerStimulator::startStimulation()
{
    m_isOn = true;

}

void TrackerStimulator::stopStimulation()
{
    m_isOn = false;
}

bool TrackerStimulator::isReady()
{
    return true;
}


// Circle methods

Circle::Circle()
    : m_cx(0),
      m_cy(0),
      m_rad(0)
{}

Circle::Circle(float x, float y, float rad, bool on)
{
    m_cx = x;
    m_cy = y;
    m_rad = rad;
    m_on = on;
}

float Circle::getX()
{
    return m_cx;
}
float Circle::getY()
{
    return m_cy;
}
float Circle::getRad()
{
    return m_rad;
}
bool Circle::getOn()
{
    return m_on;
}
void Circle::setX(float x)
{
    m_cx = x;
}
void Circle::setY(float y)
{
    m_cy = y;
}
void Circle::setRad(float rad)
{
    m_rad = rad;
}
void Circle::set(float x, float y, float rad, bool on)
{
    m_cx = x;
    m_cy = y;
    m_rad = rad;
    m_on = on;
}

bool Circle::on()
{
    m_on = true;
    return m_on;
}
bool Circle::off()
{
    m_on = false;
    return m_on;
}

bool Circle::isPositionIn(float x, float y)
{
    if (pow(x - m_cx,2) + pow(y - m_cy,2)
            <= m_rad*m_rad)
        return true;
    else
        return false;
}


