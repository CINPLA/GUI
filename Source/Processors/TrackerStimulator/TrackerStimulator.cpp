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
    , m_cx(0.0)
    , m_cy(0.0)
    , m_crad(0.0)
    , m_tot_chan(4)
    , m_timePassed(0.0)
    , m_currentTime(0.0)
    , m_chan(0)
    , m_pulsePal()
{
    // Init PulsePal
//    m_pulsePal.initialize();
//    m_pulsePal.setDefaultParameters();
//    m_pulsePal.updateDisplay("GUI Connected","Click for menu");

    m_stimFreq = vector<float>(m_tot_chan, DEF_FREQ);
    m_stimElectrode = vector<int>(m_tot_chan, 0);

    m_phaseDuration = vector<int>(m_tot_chan, DEF_PHASE_DURATION);
    m_interPhaseInt = vector<int>(m_tot_chan, DEF_INTER_PHASE);
    m_repetitions = vector<int>(m_tot_chan, DEF_REPETITIONS);
    m_voltage = vector<float>(m_tot_chan, DEF_VOLTAGE);

    m_isUniform = vector<int>(m_tot_chan, 1);
    m_isBiphasic = vector<int>(m_tot_chan, 1);
    m_negativeFirst = vector<int>(m_tot_chan, 1);
    int a = 5;
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
float TrackerStimulator::getCx() const
{
    return m_cx;
}
float TrackerStimulator::getCy() const
{
    return m_cy;
}
float TrackerStimulator::getCradius() const
{
    return m_crad;
}
int TrackerStimulator::getChan() const
{
    return m_chan;
}

void TrackerStimulator::setCx(float cx)
{
    m_cx = cx;
}
void TrackerStimulator::setCy(float cy)
{
    m_cy = cy;
}
void TrackerStimulator::setCradius(float crad)
{
    m_crad = crad;
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
}
void TrackerStimulator::setInterPhaseInt(int chan, int interPhaseInt)
{
    m_interPhaseInt[chan] = interPhaseInt;
}
void TrackerStimulator::setVoltage(int chan, float voltage)
{
    m_voltage[chan] = voltage;
}
void TrackerStimulator::setRepetitions(int chan, int rep)
{
    m_repetitions[chan] = rep;
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

bool TrackerStimulator::stimulate()
{
    // make decision
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
    m_pulsePal.setBiphasic(m_chan, m_isBiphasic[m_chan]);
    m_pulsePal.setPhase1Voltage(m_chan, m_voltage[m_chan]);
    m_pulsePal.setPhase2Voltage(m_chan, m_voltage[m_chan]);
    m_pulsePal.setPhase1Duration(m_chan, float(m_phaseDuration[m_chan])/1000.0);
    m_pulsePal.setPhase2Duration(m_chan, float(m_phaseDuration[m_chan])/1000.0);
    m_pulsePal.setInterPhaseInterval(m_chan, float(m_interPhaseInt[m_chan])/1000.0);

}
bool TrackerStimulator::testStimulation(){} //test from Editor


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
