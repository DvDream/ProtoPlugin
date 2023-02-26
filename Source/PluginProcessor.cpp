/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ProtoPluginAudioProcessor::ProtoPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), apvtsParameters(*this, nullptr, "APVTS", createParameters())
#endif
{
    gainParameter = apvtsParameters.getRawParameterValue("gain");
    invertPhaseParameter = apvtsParameters.getRawParameterValue("invertPhase");
    swapChannelsParameter = apvtsParameters.getRawParameterValue("swapChannels");

}

ProtoPluginAudioProcessor::~ProtoPluginAudioProcessor()
{
}

//==============================================================================
const juce::String ProtoPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ProtoPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ProtoPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ProtoPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ProtoPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ProtoPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ProtoPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ProtoPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ProtoPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void ProtoPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ProtoPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    auto phase = *invertPhaseParameter < 0.5f ? -1.0 : 1.0f;
    // we initialise the previousGain value here [2]
    previousGain = *gainParameter * phase;
}

void ProtoPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ProtoPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void ProtoPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	//auto totalNumInputChannels  = getTotalNumInputChannels();
	//auto totalNumOutputChannels = getTotalNumOutputChannels();

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	//for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
	//    buffer.clear (i, 0, buffer.getNumSamples());

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.

	auto phase = *invertPhaseParameter < 0.5f ? -1.0f : 1.0f;
	bool wantToSwapChannels = *swapChannelsParameter < 0.5f ? false : true;

	auto currentGain = *gainParameter * phase; // We multiply the gain by the pase parameter

	int numChannels = buffer.getNumChannels();
	int numSamples = buffer.getNumSamples();

	if (wantToSwapChannels)
	{
		for (int i = 0; i < numSamples; i++)
		{
			float temp = buffer.getSample(0, i);
			buffer.setSample(0, i, buffer.getSample(1, i));
			buffer.setSample(1, i, temp);
		}
	}

	if (currentGain == previousGain)
	{
		// The AudioSampleBuffer::applyGain() function applies our gain value to all samples across all channels in the buffer.
		buffer.applyGain(0, buffer.getNumSamples(), *gainParameter);
	}
	else
	{
		buffer.applyGain(0, buffer.getNumSamples(), currentGain);
		previousGain = currentGain;
	}
}

//==============================================================================
bool ProtoPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ProtoPluginAudioProcessor::createEditor()
{
    return new ProtoPluginAudioProcessorEditor (*this);
}

//==============================================================================
void ProtoPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = apvtsParameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ProtoPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes)); // converts binary data—created with AudioProcessor::copyXmlToBinary() function—back to XML.

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvtsParameters.state.getType()))
            apvtsParameters.replaceState(juce::ValueTree::fromXml(*xmlState));

}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ProtoPluginAudioProcessor();
}

juce::AudioProcessorValueTreeState::ParameterLayout ProtoPluginAudioProcessor::createParameters()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("gain", // parameterID
        "Gain", // parameter name
        // 0.0f,   // minimum value
        //1.0f,   // maximum value
        juce::NormalisableRange<float>(0.0f, 1.0f),
        0.5f)); // default value
    
    layout.add(std::make_unique<juce::AudioParameterBool>("invertPhase",
        "Invert Phase",
        false));

    layout.add(std::make_unique<juce::AudioParameterBool>("swapChannels",
        "Swap L/R Channels",
        false));

    /*Now we have our parameters setup in our parameters layout and we can pass it to the aptvs constructor*/
    return layout;
}