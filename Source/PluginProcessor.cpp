/* Copyright 2021 Yegor Suslin
 *
 * vitOTT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * vitOTT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with vitOTT.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PluginProcessor.h"
#include "vital_dsp/utilities/smooth_value.h"
//==============================================================================
VitOttAudioProcessor::VitOttAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), 
    parameters(*this, nullptr, juce::Identifier("vitOTT"), {
            std::make_unique<juce::AudioParameterFloat>("in_gain", "In Gain", -60.0f, 30.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>("out_gain", "Out Gain", -60.0f, 30.0f, 0.0f),
            std::make_unique<juce::AudioParameterFloat>("mix", "Mix", 0, 1.f, 1.0f),
            std::make_unique<juce::AudioParameterFloat>("depth", "Depth", 0, 1.f, 1.0f),
            std::make_unique<juce::AudioParameterFloat>("upward", "Upward", 0, 2.f, 1.0f),
            std::make_unique<juce::AudioParameterFloat>("downward", "Downward", 0, 2.f, 1.0f),
            std::make_unique<juce::AudioParameterFloat>("att_time", "Attack", 0, 1.f, 0.5f),
            std::make_unique<juce::AudioParameterFloat>("rel_time", "Release", 0, 1.f, 0.5f),
            std::make_unique<juce::AudioParameterFloat>("lgain", "Low. Gain", -40, 40, 16.3f),
            std::make_unique<juce::AudioParameterFloat>("mgain", "Mid. Gain", -40, 40, 11.7f),
            std::make_unique<juce::AudioParameterFloat>("hgain", "High. Gain", -40, 40, 16.3f),
            std::make_unique<juce::AudioParameterFloat>("low_cross_freq", "Low/Mid Freq", 20.f, 18000.f, 120.f),
            std::make_unique<juce::AudioParameterFloat>("high_cross_freq", "Mid/High Freq", 20.f, 18000.f, 2500.f),
                           })
#endif
{
    comp = std::make_unique<vital::MultibandCompressor>();
    sig_in = std::make_unique<vital::Output>();
    comp->plug(sig_in.get(), vital::MultibandCompressor::kAudio);

    initVals();
    for (int i = 0; i < vals.size(); i++)
    {
        comp->plug(vals[i], i + 1);
    }

}

VitOttAudioProcessor::~VitOttAudioProcessor()
{
    for (int i = 0; i < vals.size(); i++)
    {
        delete vals[i];
    }
}

void VitOttAudioProcessor::initVals()
{
    for (int i = 0; i < vals.size(); i++)
    {
        vals[i] = new vital::SmoothValue(0);
    }
    setStaticParams();
    updParams();
    
}

void VitOttAudioProcessor::setStaticParams()
{
    vals[vital::MultibandCompressor::kLowLowerThreshold - 1]->set(comp_basic_vals::kLowLowerThreshold);
    vals[vital::MultibandCompressor::kLowUpperThreshold - 1]->set(comp_basic_vals::kLowUpperThreshold);

    vals[vital::MultibandCompressor::kBandLowerThreshold - 1]->set(comp_basic_vals::kBandLowerThreshold);
    vals[vital::MultibandCompressor::kBandUpperThreshold - 1]->set(comp_basic_vals::kBandUpperThreshold);

    vals[vital::MultibandCompressor::kHighLowerThreshold - 1]->set(comp_basic_vals::kHighLowerThreshold);
    vals[vital::MultibandCompressor::kHighUpperThreshold - 1]->set(comp_basic_vals::kHighUpperThreshold);

    vals[vital::MultibandCompressor::kEnabledBands - 1]->set(vital::MultibandCompressor::kMultiband);
}

void VitOttAudioProcessor::updParams()
{

    in_gain = parameters.getRawParameterValue("in_gain")->load();
    out_gain = parameters.getRawParameterValue("out_gain")->load();

    double  depth = parameters.getRawParameterValue("depth")->load(),
        upward = parameters.getRawParameterValue("upward")->load(),
        downward = parameters.getRawParameterValue("downward")->load(),
        lgain = parameters.getRawParameterValue("lgain")->load(),
        mgain = parameters.getRawParameterValue("mgain")->load(),
        hgain = parameters.getRawParameterValue("hgain")->load(),
        att_time = parameters.getRawParameterValue("att_time")->load(),
        rel_time = parameters.getRawParameterValue("rel_time")->load(),
        mix = parameters.getRawParameterValue("mix")->load();

    
    vals[vital::MultibandCompressor::kLowLowerRatio - 1]->set(comp_basic_vals::kLowLowerRatio * depth * downward);
    vals[vital::MultibandCompressor::kLowUpperRatio - 1]->set(comp_basic_vals::kLowUpperRatio * depth * upward);

    vals[vital::MultibandCompressor::kBandLowerRatio - 1]->set(comp_basic_vals::kBandLowerRatio * depth * downward);
    vals[vital::MultibandCompressor::kBandUpperRatio - 1]->set(comp_basic_vals::kBandUpperRatio * depth * upward);

    vals[vital::MultibandCompressor::kHighLowerRatio - 1]->set(comp_basic_vals::kHighLowerRatio * depth * downward);
    vals[vital::MultibandCompressor::kHighUpperRatio - 1]->set(comp_basic_vals::kHighUpperRatio * depth * upward);


    vals[vital::MultibandCompressor::kAttack - 1]->set(att_time);
    vals[vital::MultibandCompressor::kRelease - 1]->set(rel_time);

    vals[vital::MultibandCompressor::kLowOutputGain - 1]->set(lgain);
    vals[vital::MultibandCompressor::kBandOutputGain - 1]->set(mgain);
    vals[vital::MultibandCompressor::kHighOutputGain - 1]->set(hgain);

    vals[vital::MultibandCompressor::kMix - 1]->set(mix);
}


//==============================================================================
const juce::String VitOttAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VitOttAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool VitOttAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool VitOttAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double VitOttAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VitOttAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VitOttAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VitOttAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String VitOttAudioProcessor::getProgramName (int index)
{
    return {};
}

void VitOttAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void VitOttAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    comp->reset(vital::constants::kFullMask);
    comp->setSampleRate(sampleRate);
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void VitOttAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VitOttAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void VitOttAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    int total_samples = buffer.getNumSamples();

    if (total_samples == 0)
        return;
    
    updParams();
    // transform buffer to L-R-L-R aligned simd buffer

    for (int sample_offset = 0; sample_offset < total_samples;) {
        int num_samples = std::min<int>(total_samples - sample_offset, vital::kMaxBufferSize);

        for (auto& i : vals)
            i->process(num_samples);
        
        readAudio(sig_in->buffer, &buffer, totalNumInputChannels, num_samples, sample_offset);

        comp->process(num_samples);

        writeAudio (comp->output(vital::MultibandCompressor::kAudioOut)->buffer, &buffer, totalNumOutputChannels, num_samples, sample_offset);

        sample_offset += num_samples;
    }
     
}
void VitOttAudioProcessor::readAudio(vital::poly_float* comp_buf, juce::AudioSampleBuffer* buffer, int channels, int samples, int offset) {

    vital::mono_float* comp_output = (vital::mono_float*)comp_buf;
    double mag = vital::utils::dbToMagnitude(in_gain);
    for (int channel = 0; channel < channels; ++channel)
    {
        const float* channel_data = buffer->getReadPointer(channel, offset);

        for (int i = 0; i < samples; ++i)
        {
            comp_output[vital::poly_float::kSize * i + channel] = channel_data[i] * mag;
            VITAL_ASSERT(std::isfinite(channel_data[i]));
        }
    }

}

void VitOttAudioProcessor::writeAudio(vital::poly_float* comp_buf, juce::AudioSampleBuffer* buffer, int channels, int samples, int offset) {

    const vital::mono_float* comp_output = (const vital::mono_float*)comp_buf;
    double mag = vital::utils::dbToMagnitude(out_gain);
    for (int channel = 0; channel < channels; ++channel) 
    {
        float* channel_data = buffer->getWritePointer(channel, offset);

        for (int i = 0; i < samples; ++i) 
        {
            channel_data[i] = comp_output[vital::poly_float::kSize * i + channel] * mag;
            VITAL_ASSERT(std::isfinite(channel_data[i]));
        }
    }

}


//==============================================================================
bool VitOttAudioProcessor::hasEditor() const
{
    return false; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VitOttAudioProcessor::createEditor()
{
    return nullptr;
}

//==============================================================================
void VitOttAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void VitOttAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VitOttAudioProcessor();
}
