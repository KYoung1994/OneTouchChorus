/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
OneTouchChorusAudioProcessorEditor::OneTouchChorusAudioProcessorEditor (OneTouchChorusAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), background(juce::ImageFileFormat::loadFrom(BinaryData::pinkBackground_png, BinaryData::pinkBackground_pngSize)), stereoFont(juce::ImageFileFormat::loadFrom(BinaryData::stereoOneTouchChorus_png, BinaryData::stereoOneTouchChorus_pngSize)), chorusFont(juce::ImageFileFormat::loadFrom(BinaryData::chorusOneTouchChorus_png, BinaryData::chorusOneTouchChorus_pngSize))
{

    addAndMakeVisible(delayTimeSlider);
    delayTimeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    
    delaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "CHORUS", delayTimeSlider);
    
    setSize (230, 200);
}

OneTouchChorusAudioProcessorEditor::~OneTouchChorusAudioProcessorEditor()
{
}

//==============================================================================
void OneTouchChorusAudioProcessorEditor::paint (juce::Graphics& g)
{
    
    g.drawImage(background, Rectangle<float>(getWidth(), getHeight()));
    g.drawImage(stereoFont, Rectangle<float>(getWidth(), getHeight() * 0.2));
    g.drawImage(chorusFont, Rectangle<float>(0, 164, getWidth(), getHeight() * 0.185));

}

void OneTouchChorusAudioProcessorEditor::resized()
{
    delayTimeSlider.setBoundsRelative(0.241, 0.24, 0.52, 0.68);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
