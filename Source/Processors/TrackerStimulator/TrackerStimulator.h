#ifndef TRACKERSTIMULATOR_H
#define TRACKERSTIMULATOR_H

#include "../../../JuceLibraryCode/JuceHeader.h"
#include "../GenericProcessor/GenericProcessor.h"
#include "../Serial/PulsePal.h"

#include <vector>

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

    int getChan() const;

    void setCx(float cx);
    void setCy(float cy);
    void setCradius(float radius);

    float getStimFreq(int chan) const;
    int getStimElectrode(int chan) const;
    bool getIsUniform(int chan) const;

    bool getIsBiphasic(int chan) const;
    bool getNegFirst(int chan) const;
    int getPhaseDuration(int chan) const;
    int getInterPhaseInt(int chan) const;
    float getVoltage(int chan) const;
    int getRepetitions(int chan) const;

    void setStimFreq(int chan, float stimFreq);
    void setStimElectrode(int chan, int stimElectrode);
    void setIsUniform(int chan, bool isUniform);

    void setIsBiphasic(int chan, bool isBiphasic);
    void setNegFirst(int chan, bool negFirst);
    void setPhaseDuration(int chan, int phaseDuration);
    void setInterPhaseInt(int chan, int interPhaseInt);
    void setVoltage(int chan, float voltage);
    void setRepetitions(int chan, int rep);
    void setChan(int chan);

    void clearPositionDisplayedUpdated();
    bool positionDisplayedIsUpdated() const;

    bool isSink(); //get the color correct
    bool isReady();



private:

    CriticalSection lock;

    // OnOff
    bool m_isOn;

    // Time
    float m_timePassed;
    int64 m_previousTime;
    int64 m_currentTime;

    // Current Position
    float m_x;
    float m_y;
    float m_width;
    float m_height;
    bool m_positionIsUpdated;
    bool m_positionDisplayedIsUpdated;

    // Target Circle params (extend to vectors)
    float m_cx;
    float m_cy;
    float m_crad;

    // Stimulation params
    vector<float> m_stimFreq;
    vector<int> m_stimElectrode;
    vector<int> m_isUniform;

    // Pulse params
    vector<int> m_isBiphasic;
    vector<int> m_negativeFirst;
    vector<int> m_phaseDuration; // us
    vector<int> m_interPhaseInt; // us
    vector<int> m_repetitions;
    vector<float> m_voltage; // V
    int m_chan;
    int m_tot_chan;

    // PULSE PAL
    PulsePal m_pulsePal;

    // Stimulate decision
    bool stimulate();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TrackerStimulator)



};

inline bool TrackerStimulator::isSink()
{
    return true;
}

#endif // TRACKERSTIMULATOR_H
