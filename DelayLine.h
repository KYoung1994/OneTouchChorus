/*
  ==============================================================================

    DelayLine.h
    Created: 8 Feb 2023 10:09:43am
    Author:  Kimberly Young

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//A delay line to process floating points in a signal on a sample by sample basis

class DelayLine
{
public:
    DelayLine()
    {
    }
    
    ~DelayLine()
    {
    }
    
    void prepareDelayLine(double sampleRate, float maxDelayTime, int numChannels) // Function to prepare everything before any processing is done.
    {
        maxDelayInSamples = sampleRate * maxDelayTime / 1000.0; // Calclulate our max delay in samples.
        buffer.setSize(numChannels, maxDelayInSamples + 3);     // Resize the buffer so that it has enough samples for our maximum.
        buffer.clear();                                         // Ensure that buffer is empty.
        
        position.resize(numChannels);                           // Sizing our vectors for the number of channels.
        mixValues.resize(numChannels);
        feedbackValues.resize(numChannels);
        for (int i = 0; i < position.size(); i++) {
            position[i] = 0;                                    // Resetting our positions.
            
            mixValues[i].reset(sampleRate, 0.1);                // Setting the times for our smoothed values.
            feedbackValues[i].reset(sampleRate, 0.1);
        }
        
        this->sampleRate = sampleRate;
    }
    


protected:
    void writeSample(float sample, int channel) // Funtion to store our current sample.
    {
        auto bufferPointer = buffer.getWritePointer(channel);   // Grab a write pointer.
        bufferPointer[position[channel]] = sample;              // Store the sample.
        
        position[channel]++;                                    // Itterate our positon.
        if (position[channel] >= buffer.getNumSamples()) {      // Ensure that our position doesn't go out of bounds of our buffer.
            position[channel] -= buffer.getNumSamples();
        }
    }
    

    
    float readSample(int channel, float delayTime) // Function to read a past sample.
    {
        int thisDelay = position[channel] - sampleRate * delayTime / 1000.0;    // Calculate delay position.
        
        if (thisDelay < 0)                                                      // Ensure that our position doesn't go out of bounds of our buffer.
        {
            thisDelay += buffer.getNumSamples();
        }
        
        auto bufferPointer = buffer.getReadPointer(channel);                    // Grab a read pointer.
        return bufferPointer[thisDelay];                                        // Read the sample.
    }
    
    
    void addToSample(float sample, int channel) // Adds a value to our current sample - useful for feedback etc.
    {
        auto bufferPointer = buffer.getWritePointer(channel);   // Grab a write pointer.
        bufferPointer[position[channel]] += sample;             // Add to stored sample.
    }
    
    juce::AudioBuffer<float> buffer;
    std::vector<int> position;
    int maxDelayInSamples;
    double sampleRate;
    std::vector<juce::SmoothedValue<float>> mixValues, feedbackValues;
};
