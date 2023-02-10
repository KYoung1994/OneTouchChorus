/*
  ==============================================================================

    Warp.h
    Created: 23 Mar 2022 9:43:14am
    Author:  Kimberly Young

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SineGenerator.h"
#include "VariableDelayLine.h"

//A class using a SineGenerator to modulate a delay, changing the pitch to emulate a doppler effect, this is useful to make chorus and vibrato.

class Vibrato {
public:
    Vibrato()
    {
        
    }
    
    ~Vibrato()
    {
        
    }
    
    void prepare(juce::dsp::ProcessSpec spec)
    {
        //parparing delay object with a fixed deley time that sine wave objects will be added to to create warp effect
        delay.prepareVariableDelayLine(spec.sampleRate, 1000.0, spec.numChannels);
        
        //preparing sine wave objects that will be multiplied together to create a modulated sine wave
        sine.prepare(spec.sampleRate, spec.numChannels);
  
        
    }
    
    
    void setParams(float newFrequency, float newDepth, float newMix)
    {
        //this is called in the processBuffBlock function and sets the parameters that will be called into other functions in delay and sine objects
        frequency = newFrequency;
        depth = newDepth;
        
        mix = juce::jlimit(0.0f, 1.0f, newMix);
        
    }
    
    
    float processSample (float input , int channel)
    {
            
        auto thisSineSample = sine.getValue(frequency * 2, channel);
        auto thisDelayTime = thisSineSample * depth;
        thisSineSample += 1.0;
        thisSineSample  /= 2.0;
                input = delay.processSample(input, channel, thisDelayTime + 1, 0.0, mix);
            
        return input;
    }
    
    void processBlock (juce::AudioBuffer<float>& buffer)
       {
           for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
           {
               auto* channelData = buffer.getWritePointer (channel);
               
               for (int sample = 0; sample < buffer.getNumSamples(); sample++)
               {
                   auto thisSineSample = sine.getValue(frequency * 2, channel);
                   thisSineSample += 1.0;
                   thisSineSample  /= 2.0;
                   auto thisDelayTime = thisSineSample * depth;
                   channelData[sample] = delay.processSample(channelData[sample], channel, thisDelayTime + 1, 0.0, mix);
               }
               
           }
       }
    
private:
    VariableDelayLine delay;
    SineGenerator sine;
    float frequency, depth, mix;

};
