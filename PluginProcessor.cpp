/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
OneTouchChorusAudioProcessor::OneTouchChorusAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
,apvts(*this, nullptr, "Parameters", createParameters())
#endif
{
   chorusParameter = apvts.getRawParameterValue("CHORUS");
}

OneTouchChorusAudioProcessor::~OneTouchChorusAudioProcessor()
{
}

//==============================================================================
const juce::String OneTouchChorusAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OneTouchChorusAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool OneTouchChorusAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool OneTouchChorusAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double OneTouchChorusAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OneTouchChorusAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int OneTouchChorusAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OneTouchChorusAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String OneTouchChorusAudioProcessor::getProgramName (int index)
{
    return {};
}

void OneTouchChorusAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void OneTouchChorusAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{

    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();
    chorusLineLeft.prepare(spec);
    chorusLineRight.prepare(spec);
    
    smoothedChorusValues.resize(getTotalNumOutputChannels());
    for (int i = 0; i < smoothedChorusValues.size(); i++) {
        smoothedChorusValues[i].reset(sampleRate, 0.0001);
    }
}

void OneTouchChorusAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OneTouchChorusAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void OneTouchChorusAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto pbChorus = chorusParameter->load();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
        
        for ( int sample = 0; sample < buffer.getNumSamples(); sample++){
            
            smoothedChorusValues[channel].setTargetValue(pbChorus);
            pbChorus = smoothedChorusValues[channel].getNextValue();
            
            chorusLineLeft.setParams(5, pbChorus, pbChorus/100);
            chorusLineRight.setParams(10, pbChorus, pbChorus/100);
            
            if(channel == 0){
            channelData[sample] = chorusLineLeft.processSample(channelData[sample], channel);
            }
            else
                channelData[sample] = chorusLineRight.processSample(channelData[sample], channel);
                
        }
       
    }
}

//==============================================================================
bool OneTouchChorusAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* OneTouchChorusAudioProcessor::createEditor()
{
    return new OneTouchChorusAudioProcessorEditor (*this);
}

//==============================================================================
void OneTouchChorusAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{

    auto stat = apvts.copyState();
               std::unique_ptr<juce::XmlElement> xml (stat.createXml());
        
               copyXmlToBinary(*xml, destData);
}

void OneTouchChorusAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
                 if (xmlState.get() != nullptr) {
                     if (xmlState->hasTagName(apvts.state.getType())) {
                         apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
                     }
                 }
  
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OneTouchChorusAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout OneTouchChorusAudioProcessor::createParameters(){
    
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> vParams;
    
    vParams.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID("CHORUS",1), "Chorus", juce::NormalisableRange<float> {0.00f, 50.0f}, 0.1f));
       
    
return{vParams.begin(), vParams.end()};
}
