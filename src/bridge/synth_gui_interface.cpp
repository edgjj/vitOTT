// clang-format off
#include "synth_gui_interface.h"
// clang-format on

// hold section component
SynthGuiInterface::SynthGuiInterface(SynthBase& base, SynthSection* section)
    : _fake_base(base)
{

    /*
        this thing keeps next hierarchy:

        SynthGuiInterface->FullInterface->SynthSection
    */
    _gui = std::make_unique<FullInterface>(section);
    addAndMakeVisible(*_gui);
}

SynthBase* SynthGuiInterface::getSynth()
{
    return &_fake_base;
}

void SynthGuiInterface::resized()
{
    _gui->setBounds(getBounds());
}
