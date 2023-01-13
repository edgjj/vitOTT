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

#include "shaders.h"
#include "synth_section.h"
#include "open_gl_background.h"
#include "popup_browser.h"

#include <JuceHeader.h>

class FullInterface : public juce::Component, public juce::OpenGLRenderer {
public:
    FullInterface(SynthSection* section);
    ~FullInterface();

    float getResizingScale() const;

    void popupSelector(Component* source, Point<int> position, const PopupItems& options,
        std::function<void(int)> callback, std::function<void()> cancel);

    void popupDisplay(Component* source, const std::string& text,
        juce::BubbleComponent::BubblePlacement placement, bool primary);

    void hideDisplay(bool primary);
    void repaintChildBackground(SynthSection* child);
    void repaintOpenGlBackground(OpenGlComponent* component);

    // opengl stuff
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;

    void setAllValues(vital::control_map& controls);

    void resized() override;

private:
    int width_ { 0 };
    int resized_width_ { 0 };

    std::unique_ptr<SynthSection> _section;
    juce::OpenGLContext _opengl_context;
    std::unique_ptr<Shaders> _shaders;
    OpenGlWrapper _opengl;

    OpenGlBackground _background;
    juce::Image _background_image;

    bool _opengl_unsupported{ false };

    std::unique_ptr<PopupDisplay> _popup_display_1, _popup_display_2;
    std::unique_ptr<SinglePopupSelector> _popup_selector;

    std::vector<SynthSection*> _section_list;
    /*
    *   param stuff
    */
    /*vital::control_map _vital_control_map;
    std::unordered_map<std::string, ValueBridge*/

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FullInterface);
};