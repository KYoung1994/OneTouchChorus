/*
  ==============================================================================

    SineGenerator.h
    Created: 20 Mar 2022 12:55:01pm
    Author:  Kimberly Young

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// A class to create and sinewave output on a sample by sample basis
//getValue should be called in a processBlock when samples are iterating in a loop

class SineGenerator {
public:
    SineGenerator()
    {
        
    }
    
    ~SineGenerator()
    {
        
    }
    
    void prepare(double sampleRate, int channels)
    {
        //preparing the sine wave class
        this->sampleRate = sampleRate;
        currentAngle.resize(channels);
        angleDelta.resize(channels);
        frequency.resize(channels);
        
        //resets each number in the vector to correct initial vanue
        for (int i = 0; i < channels; i++)
        {
            currentAngle[i] = 0.0;
            angleDelta[i] = 0.0;
            frequency[i] = 1.0;
            updateAngleDelta(i);
        }
    }
    
    double getValue(double sampleFrequency, int channel)
    {
        
        // Checking if the frequency has changed to see if a new angle delta is needed
        if (sampleFrequency != frequency[channel])
        {
            frequency[channel] = sampleFrequency;
            updateAngleDelta(channel);
        }
        
        // Adding angle to sample
        currentAngle[channel] += angleDelta[channel];
        
        if (currentAngle[channel] > juce::MathConstants<double>::twoPi)
        {
            currentAngle[channel] -= juce::MathConstants<double>::twoPi;
        }
        
        // "sin" section of equation.
        return std::sin(currentAngle[channel]);
    }
    
private:
    void updateAngleDelta(int channel)
    // Calculating angle to add for each sample
    {
        const double cyclesPerSample = frequency[channel] / sampleRate;
        // Converting time frequency into sample frequency
        angleDelta[channel] = cyclesPerSample * juce::MathConstants<double>::twoPi;
        // "2 * pi * frequency" section of equation
    }
    
    double sampleRate;
    std::vector<double> currentAngle, angleDelta, frequency;
};
