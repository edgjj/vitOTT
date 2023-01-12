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

#include "PluginEditor.h"
#include "compressor_section.h"
#include "synth_gui_interface.h"

VitOttAudioProcessorEditor::VitOttAudioProcessorEditor(VitOttAudioProcessor& p)
    : juce::AudioProcessorEditor(&p)
{
    _ui = std::make_unique<SynthGuiInterface>(p, new CompressorSection("vitOTT"));

    addAndMakeVisible(*_ui);
    setSize(600, 250);
}

VitOttAudioProcessorEditor::~VitOttAudioProcessorEditor()
{
}

void VitOttAudioProcessorEditor::resized()
{
    _ui->setBounds(getBounds());
}
