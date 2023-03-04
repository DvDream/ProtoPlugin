/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ProtoPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:

    enum
    {
        paramControlHeight = 40,
        paramLabelWidth = 50,
        paramSliderWidth = 200
    };

    ProtoPluginAudioProcessorEditor (ProtoPluginAudioProcessor&);
    ~ProtoPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ProtoPluginAudioProcessor& audioProcessor;
    
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
    juce::Label gainLabel;
    juce::Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;

    juce::ToggleButton invertButton;
    std::unique_ptr<ButtonAttachment> invertAttachment;

    juce::ToggleButton swapChannels;
    std::unique_ptr<ButtonAttachment> swapChannelsAttachment;

    //juce::Slider leftChannelLevelSlider;
    //std::unique_ptr<SliderAttachment> leftChannelLevelAttachment;

    //juce::Slider rightChannelLevelSlider;
    //std::unique_ptr<SliderAttachment> rightChannelLevelAttachment;
    juce::Label leftChannelLabel;
    juce::Label rightChannelLabel;
    juce::Slider balanceChannelsLevelSlider;
    std::unique_ptr<SliderAttachment> balanceChannelsLevelAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProtoPluginAudioProcessorEditor)
};
