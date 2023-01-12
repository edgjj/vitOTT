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

#pragma once

#include "synth_types.h"

#include <climits>
#include <vector>

namespace vital {

  class StatusOutput {
    public:
      static constexpr float kClearValue = INT_MIN;

      StatusOutput(Output* source) : source_(source), value_(0.0f) { }

      force_inline poly_float value() const { return value_; }

      force_inline void update(poly_mask voice_mask) {
        poly_float masked_value = source_->buffer[0] & voice_mask;
        value_ = masked_value + utils::swapVoices(masked_value);
      }

      force_inline void update() {
        value_ = source_->buffer[0];
      }

      force_inline void clear() { value_ = kClearValue; }
      force_inline bool isClearValue(poly_float value) const { return poly_float::equal(value, kClearValue).anyMask(); }
      force_inline bool isClearValue(float value) const { return value == kClearValue; }

    private:
      Output* source_;
      poly_float value_;

      JUCE_LEAK_DETECTOR(StatusOutput)
  };
} // namespace vital

