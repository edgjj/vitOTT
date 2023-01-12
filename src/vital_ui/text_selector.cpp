/* Copyright 2013-2019 Matt Tytel
 *
 * vital is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * vital is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with vital.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "text_selector.h"

#include "default_look_and_feel.h"
#include "fonts.h"
#include "skin.h"

TextSelector::TextSelector(String name) : SynthSlider(name), long_lookup_(nullptr) { }

void TextSelector::mouseDown(const juce::MouseEvent &e) {
  if (e.mods.isPopupMenu()) {
    SynthSlider::mouseDown(e);
    return;
  }

  const std::string* lookup = string_lookup_;
  if (long_lookup_)
    lookup = long_lookup_;

  PopupItems options;
  for (int i = 0; i <= getMaximum(); ++i)
    options.addItem(i, lookup[i]);

  parent_->showPopupSelector(this, Point<int>(0, getHeight()), options, [=](int value) { setValue(value); });
}

void TextSelector::mouseUp(const MouseEvent& e) {
  if (e.mods.isPopupMenu()) {
    SynthSlider::mouseUp(e);
    return;
  }
}