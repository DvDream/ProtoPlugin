/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ProtoPluginAudioProcessorEditor::ProtoPluginAudioProcessorEditor (ProtoPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    gainLabel.setText("Gain", juce::dontSendNotification);
    addAndMakeVisible(gainLabel);

    addAndMakeVisible(gainSlider);
    gainAttachment.reset(new SliderAttachment(audioProcessor.apvtsParameters, "gain", gainSlider));

    addAndMakeVisible(leftChannelLevelSlider);
    leftChannelLevelAttachment.reset(new SliderAttachment(audioProcessor.apvtsParameters, "leftChannelLevel", leftChannelLevelSlider));
    
    addAndMakeVisible(rightChannelLevelSlider);
    rightChannelLevelAttachment.reset(new SliderAttachment(audioProcessor.apvtsParameters, "rightChannelLevel", rightChannelLevelSlider));

    addAndMakeVisible(invertButton);
    invertButton.setButtonText("Invert Phase");
    invertAttachment.reset(new ButtonAttachment(audioProcessor.apvtsParameters, "invertPhase", invertButton));

    addAndMakeVisible(swapChannels);
    swapChannels.setButtonText("Swap L/R Channels");
    swapChannelsAttachment.reset(new ButtonAttachment(audioProcessor.apvtsParameters, "swapChannels", swapChannels));

    setSize(paramSliderWidth + paramLabelWidth, juce::jmax(200, paramControlHeight * 2));
}

ProtoPluginAudioProcessorEditor::~ProtoPluginAudioProcessorEditor()
{
}

//==============================================================================
void ProtoPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void ProtoPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto r = getLocalBounds();

    auto gainRect = r.removeFromTop(paramControlHeight);
    gainLabel.setBounds(gainRect.removeFromLeft(paramLabelWidth));
    gainSlider.setBounds(gainRect);

    leftChannelLevelSlider.setBounds(r.removeFromTop(paramControlHeight));

    rightChannelLevelSlider.setBounds(r.removeFromTop(paramControlHeight));

    invertButton.setBounds(r.removeFromTop(paramControlHeight));

    swapChannels.setBounds(r.removeFromTop(paramControlHeight));

}
