#include "synth_base.h"

SynthBase::SynthBase(juce::AudioProcessor* parent)
    : _parent(parent)
{
}

void SynthBase::beginChangeGesture(const std::string& s)
{
    JUCE_BREAK_IN_DEBUGGER
}

void SynthBase::endChangeGesture(const std::string& s)
{
    JUCE_BREAK_IN_DEBUGGER
}

const vital::StatusOutput* SynthBase::getStatusOutput(const std::string& name) const
{   
    if (_status_outputs.count(name) != 0) {
        return _status_outputs.at(name).get();
    }

    return nullptr;
}

bool SynthBase::isMidiMapped(const std::string& name) const
{
    JUCE_BREAK_IN_DEBUGGER
    return false;
}

void SynthBase::armMidiLearn(const std::string& name)
{
    JUCE_BREAK_IN_DEBUGGER
}

void SynthBase::clearMidiLearn(const std::string& name)
{
    JUCE_BREAK_IN_DEBUGGER
}

juce::CriticalSection& SynthBase::getCriticalSection()
{
    return _critical_section;
}

void SynthBase::valueChangedInternal(const std::string& name, double value)
{
    JUCE_BREAK_IN_DEBUGGER
}

void SynthBase::createStatusOutput(std::string name, vital::Output* source)
{
	_status_outputs[std::move(name)] = std::make_unique<vital::StatusOutput>(source);
}
