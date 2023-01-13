#include "synth_base.h"

SynthBase::SynthBase(juce::AudioProcessor* parent, juce::AudioProcessorValueTreeState& vts)
    : _parent(parent)
    , _parent_vts(vts)
{
}

SynthBase::~SynthBase()
{
    for (auto& [k, v] : _controls) {
        delete v;
    }
}

void SynthBase::beginChangeGesture(const std::string& s)
{
    _parent_vts.getParameter(s)->beginChangeGesture();
}

void SynthBase::endChangeGesture(const std::string& s)
{
    _parent_vts.getParameter(s)->endChangeGesture();
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
    auto* param = _parent_vts.getParameter(name);
    auto normalized = param->convertTo0to1(value);
    param->setValueNotifyingHost(normalized);
}

vital::control_map& SynthBase::getControls()
{
    return _controls;
}

void SynthBase::updateControls()
{
    if (_controls.empty()) {
        for (const auto& v : _parent_vts.state) {
            _controls[v.getProperty("id").toString().toStdString()] = new vital::Value((double)v.getProperty("value"));
        }
    }
    else {
        for (const auto& v : _parent_vts.state) {
            _controls[v.getProperty("id").toString().toStdString()]->set((double)v.getProperty("value"));
        }
    }
}

void SynthBase::createStatusOutput(std::string name, vital::Output* source)
{
	_status_outputs[std::move(name)] = std::make_unique<vital::StatusOutput>(source);
}

void SynthBase::updateStatusOutputs()
{
    for (auto& [k, v] : _status_outputs) {
        v->update();
    }
}
