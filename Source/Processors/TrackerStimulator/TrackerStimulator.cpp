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
    , m_x(-1.0)
    , m_y(-1.0)
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
    m_stimSD = vector<float>(m_tot_chan, DEF_SD);
    m_stimElectrode = vector<int>(m_tot_chan, 0);

    m_phaseDuration = vector<float>(m_tot_chan, DEF_PHASE_DURATION);
    m_interPhaseInt = vector<float>(m_tot_chan, DEF_INTER_PHASE);
    m_repetitions = vector<int>(m_tot_chan, DEF_REPETITIONS);
    m_voltage = vector<float>(m_tot_chan, DEF_VOLTAGE);
    m_interPulseInt = vector<float>(m_tot_chan, DEF_INTER_PULSE);
    m_trainDuration = vector<float>(m_tot_chan, DEF_TRAINDURATION);

    m_isUniform = vector<int>(m_tot_chan, 1);
    m_isBiphasic = vector<int>(m_tot_chan, 1);
    m_negativeFirst = vector<int>(m_tot_chan, 1);

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
float TrackerStimulator::getStimSD(int chan) const
{
    return m_stimSD[chan];
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
float TrackerStimulator::getPhaseDuration(int chan) const
{
    return m_phaseDuration[chan];
}
float TrackerStimulator::getInterPhaseInt(int chan) const
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
float TrackerStimulator::getInterPulseInt(int chan) const
{
    return m_interPulseInt[chan];
}
float TrackerStimulator::getTrainDuration(int chan) const
{
    return m_trainDuration[chan];
}

uint32_t TrackerStimulator::getPulsePalVersion() const
{
    return m_pulsePalVersion;
}

void TrackerStimulator::setStimFreq(int chan, float stimFreq)
{
    m_stimFreq[chan] = stimFreq;
}
void TrackerStimulator::setStimSD(int chan, float stimSD)
{
    m_stimSD[chan] = stimSD;
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
void TrackerStimulator::setPhaseDuration(int chan, float phaseDuration)
{
    m_phaseDuration[chan] = phaseDuration;
    updatePulsePal();
}
void TrackerStimulator::setInterPhaseInt(int chan, float interPhaseInt)
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
void TrackerStimulator::setInterPulseInt(int chan, float interPulseInt)
{
    m_interPulseInt[chan] = interPulseInt;
    updatePulsePal();
}
void TrackerStimulator::setTrainDuration(int chan, float trainDuration)
{
    m_trainDuration[chan] = trainDuration;
    updatePulsePal();
}

void TrackerStimulator::setChan(int chan)
{
    m_chan = chan;
}

bool TrackerStimulator::checkParameterConsistency(int chan)
{
    if (m_repetitions[chan] > 1)
        if (!m_isBiphasic[chan])
            return !(m_phaseDuration[chan] > m_interPulseInt[chan] || m_interPulseInt[chan] > m_trainDuration[chan]);
        else
            return !((2*m_phaseDuration[chan] + m_interPhaseInt[chan]) > m_interPulseInt[chan]
                     || m_interPulseInt[chan] > m_trainDuration[chan]);
    else
    {
        if (!m_isBiphasic[chan])
        {
            m_interPulseInt[chan] = m_phaseDuration[chan];
            m_trainDuration[chan] = m_phaseDuration[chan];
        }
        else
        {
            m_interPulseInt[chan] = 2*m_phaseDuration[chan] + m_interPhaseInt[chan];
            m_trainDuration[chan] = 2*m_phaseDuration[chan] + m_interPhaseInt[chan];
        }
        return true;
    }
}

void TrackerStimulator::setRepetitionsTrainDuration(int chan, priority whatFirst)
{
    if (whatFirst == REPFIRST)
    {
        if (m_repetitions[chan] > 1)
            if (!m_isBiphasic[chan])
                m_trainDuration[chan] = m_interPulseInt[chan]*m_repetitions[chan] + m_phaseDuration[chan];
            else
                m_trainDuration[chan] = m_interPulseInt[chan]*m_repetitions[chan] + (2*m_phaseDuration[chan]
                        + m_interPhaseInt[chan]);
    }
    else
    {
        if (!m_isBiphasic[chan])
            if (int((m_trainDuration[chan]-m_phaseDuration[chan])/m_interPulseInt[chan]) >= 1)
                m_repetitions[chan] = int((m_trainDuration[chan]-m_phaseDuration[chan])/m_interPulseInt[chan]);
            else
            {
                m_repetitions[chan] = 1;
                m_trainDuration[chan] = m_phaseDuration[chan];
            }
        else
            if (int((m_trainDuration[chan]-(2*m_phaseDuration[chan] + m_interPhaseInt[chan]))
                    /m_interPulseInt[chan]) >= 1)
                m_repetitions[chan] = int((m_trainDuration[chan]-(2*m_phaseDuration[chan] + m_interPhaseInt[chan]))
                                      /m_interPulseInt[chan]);
            else
            {
                m_repetitions[chan] = 1;
                m_trainDuration[chan] = 2*m_phaseDuration[chan] + m_interPhaseInt[chan];
            }
    }
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

            float stim_interval;
            if (m_isUniform[m_chan]) //uniform
                stim_interval = float(1/m_stimFreq[m_chan]);
            else                     //gaussian
            {
                int cirlceIn = isPositionWithinCircles(m_x, m_y);
                float dist = m_circles[cirlceIn].distanceFromCenter(m_x, m_y);

                float freq_gauss = m_stimFreq[m_chan]*std::exp(-pow(dist,2)/(2*pow(m_stimSD[m_chan],2)));
                stim_interval = float(1/freq_gauss);
            }


            if (m_timePassed >= stim_interval)
            {
                // trigger selected channel
                m_pulsePal.triggerChannel(m_chan + 1);
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

		if(!(message[0] != message[0] || message[1] != message[1]) && message[0] != 0 && message[1] != 0)
		{
			m_x = message[0];
			m_y = message[1];
		}
		if(!(message[2] != message[2] || message[3] != message[3]))
		{
			m_width = message[2];
			m_height = message[3];
            m_aspect_ratio = m_width / m_height;
		}
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
    //return m_positionDisplayedIsUpdated;
	return m_positionIsUpdated;
}

void TrackerStimulator::clearPositionDisplayedUpdated()
{
    //m_positionDisplayedIsUpdated = false;
	m_positionIsUpdated = false;
}


bool TrackerStimulator::updatePulsePal()
{
	// check that Pulspal is connected and update param
	if (m_pulsePalVersion != 0)
	{
		int actual_chan = m_chan+1;
		float pulse_duration = 0;
		m_pulsePal.setBiphasic(actual_chan, m_isBiphasic[m_chan]);
		if (m_isBiphasic[m_chan])
		{
			//pulse_duration = float(m_phaseDuration[m_chan])/1000*2 + float(m_interPhaseInt[m_chan])/1000;
			m_pulsePal.setPhase1Duration(actual_chan, float(m_phaseDuration[m_chan])/1000);
			m_pulsePal.setPhase2Duration(actual_chan, float(m_phaseDuration[m_chan])/1000);
			m_pulsePal.setInterPhaseInterval(actual_chan, float(m_interPhaseInt[m_chan])/1000);
			if (m_negativeFirst[m_chan])
			{
				m_pulsePal.setPhase1Voltage(actual_chan, - m_voltage[m_chan]);
				m_pulsePal.setPhase2Voltage(actual_chan, m_voltage[m_chan]);
			}
			else
			{
				m_pulsePal.setPhase1Voltage(actual_chan, m_voltage[m_chan]);
				m_pulsePal.setPhase2Voltage(actual_chan, - m_voltage[m_chan]);
			}
		}
		else
		{
			pulse_duration = float(m_phaseDuration[m_chan])/1000;
			m_pulsePal.setPhase1Duration(actual_chan, float(m_phaseDuration[m_chan])/1000);
			m_pulsePal.setPhase2Duration(actual_chan, 0);
			m_pulsePal.setInterPhaseInterval(actual_chan, 0);
			if (m_negativeFirst[m_chan])
			{
				m_pulsePal.setPhase1Voltage(actual_chan, - m_voltage[m_chan]);
			}
			else
			{
				m_pulsePal.setPhase1Voltage(actual_chan, m_voltage[m_chan]);
			}

		}

        //float train_duration = float(m_interPulseInt[m_chan])/1000 * m_repetitions[m_chan] + float(m_phaseDuration[m_chan])/1000;
        m_pulsePal.setPulseTrainDuration(actual_chan, m_trainDuration[m_chan]);

		if (m_repetitions[m_chan]>1)
		{			
			m_pulsePal.setInterPulseInterval(actual_chan, float(m_interPulseInt[m_chan])/1000);			
		}

		return true;
	}
	else
		return false;
}

bool TrackerStimulator::testStimulation(){

    // check that Pulspal is connected and update param
    if (m_pulsePalVersion > 0)
	{
        m_pulsePal.triggerChannel(m_chan + 1);
		return true;
	}
    else
	{
        CoreServices::sendStatusMessage("PulsePal is not connected!");
		return false;
	}

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

bool TrackerStimulator::saveParametersXml()
{
    //Save
    XmlElement* state = new XmlElement("TRACKERSTIMULATOR");

    // save circles
    XmlElement* circles = new XmlElement("CIRCLES");
    for (int i=0; i<m_circles.size(); i++)
    {
        XmlElement* circ = new XmlElement(String("Circles_")+=String(i));
        circ->setAttribute("id", i);
        circ->setAttribute("xpos", m_circles[i].getX());
        circ->setAttribute("ypos", m_circles[i].getY());
        circ->setAttribute("rad", m_circles[i].getRad());
        circ->setAttribute("on", m_circles[i].getOn());

        circles->addChildElement(circ);
    }
    // save stimulator conf
    XmlElement* channels = new XmlElement("CHANNELS");
    for (int i=0; i<4; i++)
    {
        XmlElement* chan = new XmlElement(String("Chan_")+=String(i+1));
        chan->setAttribute("id", i);
        chan->setAttribute("freq", m_stimFreq[i]);
        chan->setAttribute("elec", m_stimElectrode[i]);
        chan->setAttribute("biphasic", m_isBiphasic[i]);
        chan->setAttribute("negative", m_negativeFirst[i]);
        chan->setAttribute("phase", m_phaseDuration[i]);
        chan->setAttribute("interphase", m_interPhaseInt[i]);
        chan->setAttribute("voltage", m_voltage[i]);
        chan->setAttribute("repetitions", m_repetitions[i]);
        chan->setAttribute("trainduration", m_trainDuration[i]);
        chan->setAttribute("interpulse", m_interPulseInt[i]);

        channels->addChildElement(chan);
    }

    state->addChildElement(circles);
    state->addChildElement(channels);

    if (! state->writeToFile(currentConfigFile, String::empty))
        return false;
    else
        return true;


}

bool TrackerStimulator::loadParametersXml(File fileToLoad)
{
    File currentFile = fileToLoad;

    XmlDocument doc(currentFile);
    XmlElement* xml = doc.getDocumentElement();

    if (xml == 0 || ! xml->hasTagName("TRACKERSTIMULATOR"))
    {
        std::cout << "File not found." << std::endl;
        delete xml;
        return false;
    }
    else
    {

        forEachXmlChildElement(*xml, element)
        {
            if (element->hasTagName("CIRCLES"))
            {
                m_circles.clear();
                forEachXmlChildElement(*element, element2)
                {
                    int id = element2->getIntAttribute("id");
                    double cx = element2->getDoubleAttribute("xpos");
                    double cy = element2->getDoubleAttribute("ypos");
                    double crad = element2->getDoubleAttribute("rad");
                    bool on = element2->getIntAttribute("on");

                    Circle newCircle = Circle((float) cx, (float) cy, (float) crad, on);
                    m_circles.push_back(newCircle);

                }
                break;
            }
            if (element->hasTagName("CHANNELS"))
            {

                forEachXmlChildElement(*element, element2)
                {
                    int id = element2->getIntAttribute("id");
                    double freq = element2->getDoubleAttribute("freq");
                    int elec = element2->getIntAttribute("elec");
                    int biphasic = element2->getIntAttribute("biphasic");
                    int negfirst = element2->getIntAttribute("negative");
                    double phase = element2->getDoubleAttribute("phase");
                    double interphase = element2->getDoubleAttribute("interphase");
                    double voltage = element2->getDoubleAttribute("voltage");
                    int rep = element2->getIntAttribute("repetitions");
                    double interpulse = element2->getDoubleAttribute("interpulse");
                    double trainduration = element2->getDoubleAttribute("trainduration");

                    if (id<4) //pulse pal channels
                    {
                        m_stimFreq[id] = freq;
                        m_stimElectrode[id] = elec;
                        m_isBiphasic[id] = biphasic;
                        m_negativeFirst[id] = negfirst;
                        m_phaseDuration[id] = phase;
                        m_interPhaseInt[id] = interphase;
                        m_voltage[id] = voltage;
                        m_repetitions[id] = rep;
                        m_interPulseInt[id] = interpulse;
                        m_trainDuration[id] = trainduration;
                    }

                }
                break;
            }
        }
        return true;
    }
}

void TrackerStimulator::save()
{
    if (currentConfigFile.exists())
    {
        saveParametersXml();
    }
    else
    {
        FileChooser fc("Choose the file name...",
                       File::getCurrentWorkingDirectory(),
                       "*.xml");

        if (fc.browseForFileToSave(true))
        {
            currentConfigFile = fc.getResult();
            std::cout << currentConfigFile.getFileName() << std::endl;
            saveParametersXml();
        }
        else
        {
            CoreServices::sendStatusMessage("No file chosen!");
        }

    }
}

void TrackerStimulator::saveAs()
{
    FileChooser fc("Choose the file name...",
                   File::getCurrentWorkingDirectory(),
                   "*.xml");

    if (fc.browseForFileToSave(true))
    {
        currentConfigFile = fc.getResult();
        std::cout << currentConfigFile.getFileName() << std::endl;
        saveParametersXml();
    }
    else
    {
        CoreServices::sendStatusMessage("No file chosen!");
    }
}

void TrackerStimulator::load()
{
    FileChooser fc("Choose the file name...",
                   File::getCurrentWorkingDirectory(),
                   "*.xml");

    if (fc.browseForFileToOpen())
    {
        File fileToLoad = fc.getResult();
        std::cout << currentConfigFile.getFileName() << std::endl;
        loadParametersXml(fileToLoad);
    }
    else
    {
        CoreServices::sendStatusMessage("No file chosen!");
    }
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

float Circle::distanceFromCenter(float x, float y){
    return pow(x - m_cx,2) + pow(y - m_cy,2);
}


