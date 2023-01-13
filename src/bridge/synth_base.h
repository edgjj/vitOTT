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

#include "synth_module.h"
#include <JuceHeader.h>

#include <map>
#include <string>

class SynthBase {
public:

    SynthBase(juce::AudioProcessor* parent, juce::AudioProcessorValueTreeState& vts);
    ~SynthBase();

    void beginChangeGesture(const std::string& s);
    void endChangeGesture(const std::string& s);

    const vital::StatusOutput* getStatusOutput(const std::string& name) const;

    bool isMidiMapped(const std::string& name) const;
    void armMidiLearn(const std::string& name);
    void clearMidiLearn(const std::string& name);
    juce::CriticalSection& getCriticalSection();

    void valueChangedInternal(const std::string& name, double value);
    vital::control_map& getControls();

protected:
    void createControlMap();
    void createStatusOutput(std::string name, vital::Output* source);
    void updateStatusOutputs();

private:
    std::map<std::string, std::unique_ptr<vital::StatusOutput>> _status_outputs;
    juce::CriticalSection _critical_section;

    juce::AudioProcessor* _parent;
    juce::AudioProcessorValueTreeState& _parent_vts;

    vital::control_map _controls;

    JUCE_LEAK_DETECTOR(SynthBase)
};