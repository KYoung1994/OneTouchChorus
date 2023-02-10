/*
  ==============================================================================

    Filmstrip.h
    Created: 28 Jun 2022 9:03:33pm
    Author:  Kimberly Young

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

using namespace juce;

//==============================================================================
// //inheriting from the slider class to draw the filmstrip to create an animated slider

class FilmStripKnob : public Slider
{
public:
	FilmStripKnob()
        : Slider(juce::String(0)),
    filmStrip(juce::ImageFileFormat::loadFrom(BinaryData::silverKnob_png, BinaryData::silverKnob_pngSize)),
    
    //Jupiter_8_Slider_Rev_4_png
		numFrames_(31),
		isHorizontal_(false)
	{
		setTextBoxStyle(NoTextBox, 0, 0, 0);
		setSliderStyle(RotaryVerticalDrag);
		frameHeight = filmStrip.getHeight() / numFrames_;
		frameWidth = filmStrip.getWidth();
		setRange(0.0f, 1.0f, 0.001f);

        getProperties().set(Identifier("index"), &index);
	}

	void paint(Graphics& g) override
	{
			int value = (int)((getValue() - getMinimum()) / (getMaximum() - getMinimum()) * (numFrames_ - 1));
			if(isHorizontal_) 
			{
				g.drawImage(filmStrip, 0, 0, getWidth(), getHeight(),
					value * frameWidth, 0, frameWidth, frameHeight);
			} 
			else 
			{
				g.drawImage(filmStrip, 0, 0, getWidth(), getHeight()*0.8,
					0, value * frameHeight, frameWidth, frameHeight);
			}
	}

private:
	Image filmStrip;
	const int numFrames_;
	const bool isHorizontal_;
	int frameWidth, frameHeight;
};
