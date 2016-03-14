/*
  ==============================================================================

    SpikePositionNode.h
    Created: 11 Mar 2016 11:59:38am
    Author:  alessio

  ==============================================================================
*/

#ifndef SPIKEPOSITIONNODE_H_INCLUDED
#define SPIKEPOSITIONNODE_H_INCLUDED

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../GenericProcessor/GenericProcessor.h"
//#include "SpikePositionServer.h"

#include "../Visualization/SpikeObject.h" // use to handle event spikes

#include "../../Network/PracticalSocket.h"

#include <stdio.h>

/* MAX_BUFFER_LENGTH spikePositionObj size (+2bytes short int length)
 *
 * 1024: 46
 * 2048: 93
 * 4096: 186
 * 8192: 372
 *
 * **TODO***MAKE adjustable buffer length and error message if too many spikes detected in ;atency time ****
 */
#define MAX_BUFFER_LENGTH 4096



typedef struct {

    float x;
    float y;

    int64_t     spikeTiming;
    uint16_t    electrodeID; // unique electrode ID (regardless electrode position in the array)
    uint16_t    sortedId;   // sorted unit ID (or 0 if unsorted)

    //uint16_t    source; // used internally, the index of the electrode in the electrode array

} spikePositionObj; // 20 bytes


class SpikePositionNode : public GenericProcessor
{
public:

    SpikePositionNode();
    ~SpikePositionNode();

    AudioProcessorEditor* createEditor();

    bool isSink();
    bool isReady();


    virtual void process(AudioSampleBuffer&, MidiBuffer&);
    virtual void handleEvent(int eventType, MidiMessage &event, int samplePosition);

    int getNumEventChannels();

    void startTransmission();
    void stopTransmission();

	void connectionTest();


    // Setters-Getters
    void setLatency(int latency);
    int getLatency();
    void setAddress(String address);
    String getAddress();
    void setPort(int port);
    int getPort();

private:

    CriticalSection lock;

    std::vector<spikePositionObj> m_spikeBuffer;
    short int m_spikeCount;
    short int m_maxSpikeCounts;
    char m_buffer[MAX_BUFFER_LENGTH];

    UDPSocket sock;

    float m_x;
    float m_y;
    float m_height;
    float m_width;

	spikePositionObj m_newSpikePos;

    bool m_positionIsUpdated;
    bool m_spikeUpdated;
    bool m_isStarted;

    int m_latency;
    String m_address;
    int m_port;

    float m_timePassed;
	int64 m_previousMillis;

	SpikeObject m_newSpikeObj;


    //private methods
    void sendPacket();
    void resetTransmission();


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpikePositionNode);

};

inline bool SpikePositionNode::isSink()
{
	return true;
}

#endif  // SPIKEPOSITIONNODE_H_INCLUDED
