#ifndef TRACKERSTIMULATOR_H
#define TRACKERSTIMULATOR_H

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../GenericProcessor/GenericProcessor.h"
#include "../Serial/PulsePal.h"

#define DEF_PHASE_DURATION 100
#define DEF_INTER_PHASE 50
#define DEF_REPETITIONS 1
#define DEF_VOLTAGE 5
#define DEF_FREQ 2


class TrackerStimulator : public GenericProcessor
{

public:
    TrackerStimulator();
    ~TrackerStimulator();

    AudioProcessorEditor* createEditor();

    virtual void process(AudioSampleBuffer& buffer, MidiBuffer& events);
    virtual void handleEvent(int eventType, MidiMessage &event, int samplePosition);

    // Pulse Pal
    bool updatePulsePal();
    bool testStimulation(); //test from Editor
    void startStimulation();
    void stopStimulation();

    // Setter-Getters
    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;

    float getCx() const;
    float getCy() const;
    float getCradius() const;

    void setCx(float cx);
    void setCy(float cy);
    void setCradius(float radius);

    void setStimFreq(float stimFreq);
    void setStimElectrode(int stimElectrode);
    void setIsUniform(bool isUniform);

    void setIsBiphasic(bool isBiphasic);
    void setNegFirst(bool negFirst);
    void setPhaseDuration(int phaseDuration);
    void setInterPhaseInt(int interPhaseInt);
    void setVoltage(float voltage);
    void setRepetitions(int rep);

    void clearPositionDisplayedUpdated();
    bool positionDisplayedIsUpdated() const;

    bool isSink(); //get the color correct
    bool isReady();



private:
    // OnOff
    bool m_isOn;

    // Current Position
    float m_x;
    float m_y;
    float m_width;
    float m_height;
    bool m_positionIsUpdated;
    bool m_positionDisplayedIsUpdated;

    // Target Circle params
    float m_cx;
    float m_cy;
    float m_crad;

    // Stimulation params
    float m_stimFreq;
    int m_stimElectrode;
    bool m_isUniform;

    // Pulse params
    bool m_isBiphasic;
    bool m_negativeFirst;
    int m_phaseDuration; // us
    int m_interPhaseInt; // us
    int m_repetitions;
    float m_voltage; // V

    // PULSE PAL
    PulsePal m_pulsePal;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackerStimulator)



};

inline bool TrackerStimulator::isSink()
{
    return true;
}

#endif // TRACKERSTIMULATOR_H
