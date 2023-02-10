/*
  ==============================================================================

    VariableDelayLine.h
    Created: 7 Feb 2023 1:36:44pm
    Author:  Kimberly Young

  ==============================================================================
*/


#pragma once
#include "DelayLine.h"

//A class inheriting from the DelayLine base class incorperating LaGrange interpolation. This is useful for analogue style delay signals.


class VariableDelayLine : public DelayLine {
public:
    VariableDelayLine()
    {
        
    }
    
    ~VariableDelayLine()
    {
        
    }
    
    void prepareVariableDelayLine(double sampleRate, float maxDelayTime, int numChannels) // Function to prepare everything before any processing is done.
    {
        prepareDelayLine(sampleRate, maxDelayTime, numChannels);
        delayValues.resize(numChannels);
        for (int i = 0; i < delayValues.size(); i++) {
            delayValues[i].reset(sampleRate, 1.0);
        }
    }
    
    float processSample(float sample, int channel, float delayTime, float feedback, float mix) // Parent function which calls our child functions. Used to neaten up other classes.
    {
        mixValues[channel].setTargetValue(mix);             // Set the parameter target values for smoothing.
        feedbackValues[channel].setTargetValue(feedback);
        delayValues[channel].setTargetValue(delayTime);
        
        mix = mixValues[channel].getNextValue();                    // Getting the current smoothed values for parameters.
        feedback = feedbackValues[channel].getNextValue();
        delayTime = delayValues[channel].getNextValue();
        
        float output = readFractionalSample(channel, delayTime);    // Call child functions.
        writeSample(sample + output * feedback, channel);
        return output * mix + sample * (1 - mix);
    }


    
protected:
    float readFractionalSample(int channel, float delayTime) // Function to read a past sample.
    {
        auto samplesToDelay = sampleRate * delayTime / 1000.0 + 1;
        int lowerSample = position[channel] - sampleRate * delayTime / 1000.0 - 1;  // Truncated position of our delay (forget any decimal) - whole number sample below desired value.
        
        float variablePosition = (position[channel] - sampleRate * delayTime / 1000.0) - lowerSample;
        if (variablePosition < 0)
        {
            variablePosition += 1;
        }
        
        if (lowerSample < 0)                                                    // Ensure that our position doesn't go out of bounds of our buffer.
        {
            lowerSample += buffer.getNumSamples();
        }
        
        int extraLowSample = lowerSample - 1;                                   // Lower sample -1 for 2 samples below.
        if (extraLowSample < 0)                                                 // Ensure that our position doesn't go out of bounds of our buffer.
        {
            extraLowSample += buffer.getNumSamples();
        }
        
        int upperSample = lowerSample + 1;                                      // +1 to our lower sample. This means it is the sample directly above.
        if (upperSample >= buffer.getNumSamples())                              // Ensure that our position doesn't go out of bounds of our buffer.
        {
            upperSample -= buffer.getNumSamples();
        }
        
        int extraHighSample = upperSample + 1;                                  // Upper sample +1 for 2 samples above.
        if (extraHighSample >= buffer.getNumSamples())                          // Ensure that our position doesn't go out of bounds of our buffer.
        {
            extraHighSample -= buffer.getNumSamples();
        }
        
        auto bufferPointer = buffer.getReadPointer(channel);                    // Grab a read pointer.
        int x1 = 1;
        double y1 = bufferPointer[extraLowSample];
        int x2 = 2;
        double y2 = bufferPointer[lowerSample];
        int x3 = 3;
        double y3 = bufferPointer[upperSample];
        int x4 = 4;
        double y4 = bufferPointer[extraHighSample];
        
        float result = 0.0;
        if (samplesToDelay > 2.0)
        {
            result = interpolate(x1, y1, x2, y2, x3, y3, x4, y4, variablePosition + x2);
        } else if (samplesToDelay > 1.0)
        {
            result = interpolate(x1, y1, x2, y2, x3, y3, variablePosition + x2);
        } else
        {
            result = interpolate(x1, y1, x2, y2, variablePosition + x2);
        }
        return result;
    }
    
    static double interpolate(int x1, double y1, int x2, double y2, double xPos)
    {
        double firstResult =    (xPos - x2) / (x1 - x2) * y1;
        double secondResult =   (xPos - x1) / (x2 - x1) * y2;
        double result = firstResult + secondResult;
        
        return result;
    }
    
        static double interpolate(int x1, double y1, int x2, double y2, int x3, double y3, double xPos)
        {
            double firstResult =    (((xPos - x2) * (xPos - x3)) / ((x1 - x2) * (x1 - x3))) * y1;
            double secondResult =   (((xPos - x1) * (xPos - x3)) / ((x2 - x1) * (x2 - x3))) * y2;
            double thirdResult =    (((xPos - x1) * (xPos - x2)) / ((x3 - x1) * (x3 - x2))) * y3;
            double result = firstResult + secondResult + thirdResult;
            
            return result;
        }
        
        static double interpolate(int x1, double y1, int x2, double y2, int x3, double y3, int x4, double y4, double xPos)
        {
            double firstResult =    (((xPos - x2) * ( xPos - x3 ) * (xPos - x4)) / ((x1 - x2) * (x1 - x3) * (x1 - x4))) * y1;
            double secondResult =   (((xPos - x1) * ( xPos - x3 ) * (xPos - x4)) / ((x2 - x1) * (x2 - x3) * (x2 - x4))) * y2;
            double thirdResult =    (((xPos - x1) * ( xPos - x2 ) * (xPos - x4)) / ((x3 - x1) * (x3 - x2) * (x3 - x4))) * y3;
            double fourthResult =   (((xPos - x1) * ( xPos - x2 ) * (xPos - x3)) / ((x4 - x1) * (x4 - x2) * (x4 - x3))) * y4;
            double result = firstResult + secondResult + thirdResult + fourthResult;
            
            return result;
        }
    
    std::vector<juce::SmoothedValue<float>> delayValues;
    
};
