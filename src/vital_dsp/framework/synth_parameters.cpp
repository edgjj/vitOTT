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

#include "synth_parameters.h"

#include "compressor.h"
#include "synth_constants.h"
#include "synth_strings.h"
#include "utils.h"

#include <cfloat>

namespace vital {

  bool compareValueDetails(const ValueDetails* a, const ValueDetails* b) {
    if (a->version_added != b->version_added)
      return a->version_added < b->version_added;
    
    return a->name.compare(b->name) < 0;
  }

  using namespace constants;
  static const std::string kIdDelimiter = "_";
  static const std::string kEnvIdPrefix = "env";
  static const std::string kLfoIdPrefix = "lfo";
  static const std::string kRandomIdPrefix = "random";
  static const std::string kOscIdPrefix = "osc";
  static const std::string kFilterIdPrefix = "filter";
  static const std::string kModulationIdPrefix = "modulation";
  static const std::string kNameDelimiter = " ";
  static const std::string kEnvNamePrefix = "Envelope";
  static const std::string kLfoNamePrefix = "LFO";
  static const std::string kRandomNamePrefix = "Random LFO";
  static const std::string kOscNamePrefix = "Oscillator";
  static const std::string kFilterNamePrefix = "Filter";
  static const std::string kModulationNamePrefix = "Modulation";

  const ValueDetails ValueDetailsLookup::parameter_list[] = {
    { "bypass", 0x000702, 0.0, 1.0, 0.0, 0.0, 60.0,
      ValueDetails::kIndexed, false, "", "Bypass", nullptr },
    { "compressor_on", 0x000000, 0.0, 1.0, 0.0, 0.0, 1.0,
      ValueDetails::kIndexed, false, "", "Compressor Switch", strings::kOffOnNames },
    { "compressor_low_upper_threshold", 0x000000, -80.0, 0.0, -28.0, 0.0, 1.0,
      ValueDetails::kLinear, false, " dB", "Low Upper Threshold", nullptr },
    { "compressor_band_upper_threshold", 0x000000, -80.0, 0.0, -25.0, 0.0, 1.0,
      ValueDetails::kLinear, false, " dB", "Band Upper Threshold", nullptr },
    { "compressor_high_upper_threshold", 0x000000, -80.0, 0.0, -30.0, 0.0, 1.0,
      ValueDetails::kLinear, false, " dB", "High Upper Threshold", nullptr },
    { "compressor_low_lower_threshold", 0x000000, -80.0, 0.0, -35.0, 0.0, 1.0,
      ValueDetails::kLinear, false, " dB", "Low Lower Threshold", nullptr },
    { "compressor_band_lower_threshold", 0x000000, -80.0, 0.0, -36.0, 0.0, 1.0,
      ValueDetails::kLinear, false, " dB", "Band Lower Threshold", nullptr },
    { "compressor_high_lower_threshold", 0x000000, -80.0, 0.0, -35.0, 0.0, 1.0,
      ValueDetails::kLinear, false, " dB", "High Lower Threshold", nullptr },
    { "compressor_low_upper_ratio", 0x000000, 0.0, 1.0, 0.9, 0.0, 1.0,
      ValueDetails::kLinear, false, "", "Low Upper Ratio", nullptr },
    { "compressor_band_upper_ratio", 0x000000, 0.0, 1.0, 0.857, 0.0, 1.0,
      ValueDetails::kLinear, false, "", "Band Upper Ratio", nullptr },
    { "compressor_high_upper_ratio", 0x000000, 0.0, 1.0, 1.0, 0.0, 1.0,
      ValueDetails::kLinear, false, "", "High Upper Ratio", nullptr },
    { "compressor_low_lower_ratio", 0x000000, -1.0, 1.0, 0.8, 0.0, 1.0,
      ValueDetails::kLinear, false, "", "Low Lower Ratio", nullptr },
    { "compressor_band_lower_ratio", 0x000000, -1.0, 1.0, 0.8, 0.0, 1.0,
      ValueDetails::kLinear, false, "", "Band Lower Ratio", nullptr },
    { "compressor_high_lower_ratio", 0x000000, -1.0, 1.0, 0.8, 0.0, 1.0,
      ValueDetails::kLinear, false, "", "High Lower Ratio", nullptr },
    { "compressor_low_gain", 0x000000, -30.0, 30.0, 16.3, 0.0, 1.0,
      ValueDetails::kLinear, false, " dB", "Compressor Low Gain", nullptr },
    { "compressor_band_gain", 0x000000, -30.0, 30.0, 11.7, 0.0, 1.0,
      ValueDetails::kLinear, false, " dB", "Compressor Band Gain", nullptr },
    { "compressor_high_gain", 0x000000, -30.0, 30.0, 16.3, 0.0, 1.0,
      ValueDetails::kLinear, false, " dB", "Compressor Low/Mid Freq", nullptr },
    { "compressor_low_cross_freq", 0x000000, 20.f, 18000.f, 120.f, 0.0, 1.0,
      ValueDetails::kLinear, false, " Hz", "Compressor High Gain", nullptr },
    { "compressor_high_cross_freq", 0x000000, 20.f, 18000.f, 2500.f, 0.0, 1.0,
      ValueDetails::kLinear, false, " Hz", "Compressor Mid/High Freq", nullptr },
    { "compressor_attack", 0x000000, 0.0, 1.0, 0.5, 0.0, 100.0,
      ValueDetails::kLinear, false, "%", "Compressor Attack", nullptr },
    { "compressor_release", 0x000000, 0.0, 1.0, 0.5, 0.0, 100.0,
      ValueDetails::kLinear, false, "%", "Compressor Release", nullptr },
    { "compressor_enabled_bands", 0x000000, 0.0, vital::MultibandCompressor::kNumBandOptions - 1, 0.0, 0.0, 1.0,
      ValueDetails::kIndexed, false, "", "Compressor Enabled Bands", strings::kCompressorBandNames },
    { "compressor_mix", 0x000602, 0.0, 1.0, 1.0, 0.0, 1.0,
      ValueDetails::kLinear, false, "", "Compressor Mix", nullptr },
    { "compressor_low_band_unused", 0x000000, 0.0, 1.0, 1.0, 0.0, 1.0,
      ValueDetails::kIndexed, false, "", "Compressor Unused", nullptr },
  };

  ValueDetailsLookup::ValueDetailsLookup() {
    static constexpr int kNumOscillatorsOld = 2;
    static constexpr int kNewOscillatorVersion = 0x000500;
    static constexpr int kOldMaxModulations = 32;
    static constexpr int kNewModulationVersion = 0x000601;

    int num_parameters = sizeof(parameter_list) / sizeof(ValueDetails);
    for (int i = 0; i < num_parameters; ++i) {
      details_lookup_[parameter_list[i].name] = parameter_list[i];
      details_list_.push_back(&parameter_list[i]);

      VITAL_ASSERT(parameter_list[i].default_value <= parameter_list[i].max);
      VITAL_ASSERT(parameter_list[i].default_value >= parameter_list[i].min);
    }

    std::sort(details_list_.begin(), details_list_.end(), compareValueDetails);
  }

  void ValueDetailsLookup::addParameterGroup(const ValueDetails* list, int num_parameters, int index,
                                             std::string id_prefix, std::string name_prefix, int version) {
    std::string string_num = std::to_string(index + 1);
    addParameterGroup(list, num_parameters, string_num, id_prefix, name_prefix, version);
  }

  void ValueDetailsLookup::addParameterGroup(const ValueDetails* list, int num_parameters, std::string id,
                                             std::string id_prefix, std::string name_prefix, int version) {
    std::string id_start = id_prefix + kIdDelimiter + id + kIdDelimiter;
    std::string name_start = name_prefix + kNameDelimiter + id + kNameDelimiter;

    for (int i = 0; i < num_parameters; ++i) {
      ValueDetails details = list[i];
      if (version > details.version_added)
        details.version_added = version;

      details.name = id_start + details.name;
      details.local_description = details.display_name;
      details.display_name = name_start + details.display_name;
      details_lookup_[details.name] = details;
      details_list_.push_back(&details_lookup_[details.name]);
    }
  }

  ValueDetailsLookup Parameters::lookup_;

} // namespace vital
