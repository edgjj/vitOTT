/* Copyright 2023 Yegor Suslin
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

#pragma once

#include "PluginProcessor.h"
#include <JuceHeader.h>
#include <memory>

class SynthGuiInterface;

class VitOttAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    VitOttAudioProcessorEditor(VitOttAudioProcessor&);
    ~VitOttAudioProcessorEditor() override;

    void resized() override;

private:
    std::unique_ptr<SynthGuiInterface> _ui;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VitOttAudioProcessorEditor)
};