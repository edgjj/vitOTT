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
 * along with vital.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <JuceHeader.h>
#include <array>
#include "vital_dsp/compressor.h"
#include "vital_dsp/framework/value.h"


namespace comp_basic_vals
{
    constexpr float kLowLowerThreshold = -35.0;
    constexpr float kLowUpperThreshold = -28.0;
    constexpr float kLowLowerRatio = 0.8;
    constexpr float kLowUpperRatio = 0.9;

    constexpr float kBandLowerThreshold = -36.0;
    constexpr float kBandUpperThreshold = -25.0;
    constexpr float kBandLowerRatio = 0.8;
    constexpr float kBandUpperRatio = 0.857;

    constexpr float kHighLowerThreshold = -35.0;
    constexpr float kHighUpperThreshold = -30.0;
    constexpr float kHighLowerRatio = 0.8;
    constexpr float kHighUpperRatio = 1.0;

    constexpr uint8_t kEnabledBands = vital::MultibandCompressor::kMultiband;

}

//==============================================================================
/**
*/
class VitOttAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    VitOttAudioProcessor();
    ~VitOttAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    void writeAudio(vital::poly_float* comp_buf, juce::AudioSampleBuffer* buffer, int channels, int samples, int offset);
    void readAudio(vital::poly_float* comp_buf, juce::AudioSampleBuffer* buffer, int channels, int samples, int offset);


    void setStaticParams();
    void initVals();
    void updParams();

private:
    //==============================================================================

    juce::AudioProcessorValueTreeState parameters;
    std::unique_ptr<vital::MultibandCompressor> comp;
    std::unique_ptr<vital::Output> sig_in;

    std::array <vital::Value*, 19> vals = {};

    double in_gain = 1.0f, out_gain = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VitOttAudioProcessor)
};
