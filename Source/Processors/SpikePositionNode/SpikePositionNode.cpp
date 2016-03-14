/*
  ==============================================================================

    SpikePositionNode.cpp
    Created: 11 Mar 2016 11:59:38am
    Author:  alessio

  ==============================================================================
*/

#include "SpikePositionNode.h"
#include "SpikePositionEditor.h"

#include "../../UI/EditorViewport.h"

SpikePositionNode::SpikePositionNode()
    : GenericProcessor("Spike Position Node")
    , m_x(0.0)
    , m_y(0.0)
    , m_height(1.0)
    , m_width(1.0)
    , m_positionIsUpdated(false)
    , m_spikeUpdated(false)
    , m_latency(DEFAULT_LATENCY)
    , m_address("localhost")
	, m_port(DEFAULT_SERVER_PORT)
    , m_timePassed(0.0)
    , m_isStarted(false)
    , m_spikeCount(0)
    , sock("localhost", DEFAULT_PORT)
{
    m_maxSpikeCounts = (int) MAX_BUFFER_LENGTH/ (int) sizeof(spikePositionObj);
	m_previousMillis = Time::currentTimeMillis();
}

SpikePositionNode::~SpikePositionNode()
{
    try {
        sock.disconnect();
    }
    catch (SocketException e) {
        std::cout << e.what();
    }
}

AudioProcessorEditor* SpikePositionNode::createEditor()
{
    editor = new SpikePositionEditor(this, true);
    return editor;
}


int SpikePositionNode::getNumEventChannels()
{
    return 1;
}

// Setters - Getters

void SpikePositionNode::setLatency(int latency)
{
    m_latency = latency;
}

int SpikePositionNode::getLatency()
{
    return m_latency;
}

void SpikePositionNode::setAddress(String address)
{
    m_address = address;
}

String SpikePositionNode::getAddress()
{
    return m_address;
}

void SpikePositionNode::setPort(int port)
{
    m_port = port;
}

int SpikePositionNode::getPort()
{
    return m_port;
}

void SpikePositionNode::process(AudioSampleBuffer& buffer, MidiBuffer& events)
{
    setTimestamp(events,CoreServices::getGlobalTimestamp());
    checkForEvents(events);

    if (m_isStarted){

		lock.enter();

		bool maxSpikeReached = false;

        if(m_spikeUpdated && m_positionIsUpdated) { //At least one event from OScNode has to be received

            // Create spikePos struct
            //spikePositionObj newSpike;

            //m_newSpikePos.source = m_newSpikeObj.source;
			m_newSpikePos.electrodeID = m_newSpikeObj.electrodeID;
			m_newSpikePos.sortedId = m_newSpikeObj.sortedId;

			m_newSpikePos.x = m_x;
			m_newSpikePos.y = m_y;

            // Append to Spike obj vector if not full
            if (m_spikeCount < m_maxSpikeCounts)
            {
				m_spikeBuffer.push_back(m_newSpikePos);
                m_spikeCount++;
                m_spikeUpdated = false;
            }
            else
            {
                // Send anyways, print error message and reset
                sendPacket();
                AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                             String("WARNING: Maximum spike count reached"),
                                             String("Too many spikes have been detected: decrease latency or increase buffer size"),
                                             String("OK. Close"));
                resetTransmission();
				maxSpikeReached = true;
			}

            // Check latency time
			int64 currentTime = Time::currentTimeMillis();
			m_timePassed = (currentTime - m_previousMillis)/1000.0; // in seconds
            
			if (m_timePassed >= m_latency & !maxSpikeReached)
			{
				sendPacket();
				resetTransmission();
                //CoreServices::sendStatusMessage("Not a valid processor type.");

			}
        }
    }

    lock.exit();
}

void SpikePositionNode::handleEvent(int eventType, MidiMessage &event, int samplePosition)
{

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

    if (eventType == SPIKE)
    {
        const uint8_t* dataptr = event.getRawData();
        int bufferSize = event.getRawDataSize();

        if (bufferSize > 0)
        {
            bool isValid = unpackSpike(&m_newSpikeObj, dataptr, bufferSize);

            if (isValid)
            {
                m_spikeUpdated = true;
            }
        }
    }

}

void SpikePositionNode::sendPacket() {
    // Create packet and send it

    // head of the packet: number of spikes counted
    char * pshortint = (char *) &m_spikeCount;
    m_buffer[0] = pshortint[0];
    m_buffer[1] = pshortint[1];

    // copy spikeBuffer as a block
    char *ptr = m_buffer + 2; //pointer to beginning of spikes
    memcpy(ptr, &m_spikeBuffer, m_spikeCount*sizeof(spikePositionObj));

    try {
        sock.sendTo(m_buffer, MAX_BUFFER_LENGTH, m_address.toRawUTF8(), m_port);
    }
    catch (SocketException e)
    {
		std::cout << e.what();
    }
}

void SpikePositionNode::resetTransmission() {

    m_timePassed = 0;
    memset(m_buffer, 0, sizeof(m_buffer));
	m_spikeBuffer.clear();

}


// Sends "Hi from Open-Ephys GUI!" o test UDP connection
void SpikePositionNode::connectionTest(){

    String test("Hi from Open-Ephys GUI!");
    try{
        sock.sendTo(test.toRawUTF8(), test.length(), m_address.toRawUTF8(), m_port);
    }
    catch (SocketException e)
    {
		std::cout << e.what();
    }
}

void SpikePositionNode::startTransmission()
{
    m_isStarted = true;
}

void SpikePositionNode::stopTransmission()
{
    m_isStarted = false;
}

bool SpikePositionNode::isReady()
{
    return true;
}
