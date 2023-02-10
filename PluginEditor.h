/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Filmstrip.h"

//==============================================================================
/**
*/
class OneTouchChorusAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    OneTouchChorusAudioProcessorEditor (OneTouchChorusAudioProcessor&);
    ~OneTouchChorusAudioProcessorEditor() override;

    typedef std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
       
    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    OneTouchChorusAudioProcessor& audioProcessor;
    
    juce::Image background, stereoFont, chorusFont;
    
    FilmStripKnob delayTimeSlider;
    
    sliderAttachment delaySliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OneTouchChorusAudioProcessorEditor)
};
