// clang-format off
#include "full_interface.h"
#include "synth_gui_interface.h"
#include "synth_base.h"

#include "default_look_and_feel.h"
#include "skin.h"
// clang-format on

FullInterface::FullInterface(SynthSection* section)
    : _section(section)
    , _opengl(_opengl_context)
{
    Skin skin;
    skin.copyValuesToLookAndFeel(DefaultLookAndFeel::instance());

    setLookAndFeel(DefaultLookAndFeel::instance());
    _section->setLookAndFeel(DefaultLookAndFeel::instance());

    // required for looking up various UI-related values; check value_lookup_ && findValue
    _section->setSkinValues(skin, true);  

    addAndMakeVisible(*_section);

    // OpenGL stuff
    _opengl_context.setContinuousRepainting(true);
    _opengl_context.setOpenGLVersionRequired(OpenGLContext::openGL4_3); // was 3_2
    _opengl_context.setSwapInterval(0);
    _opengl_context.setRenderer(this);
    _opengl_context.setComponentPaintingEnabled(false);
    _opengl_context.attachTo(*this);

    // animate stuff
    _section->animate(true);
    _section->setAlwaysOnTop(true);
}

FullInterface::~FullInterface()
{
    _opengl_context.detach();
    _opengl_context.setRenderer(nullptr);
}

float FullInterface::getResizingScale() const
{
    // careful: -inf 'd cause GL_INVALID_VALUE (neg width/height) on things like glViewport
    return 1.0f; //width_ * 1.0f / resized_width_;
}

void FullInterface::popupSelector(Component* source, Point<int> position, const PopupItems& options, std::function<void(int)> callback, std::function<void()> cancel)
{
    JUCE_BREAK_IN_DEBUGGER
}

void FullInterface::popupDisplay(Component* source, const std::string& text, BubbleComponent::BubblePlacement placement, bool primary)
{
    JUCE_BREAK_IN_DEBUGGER
}

void FullInterface::hideDisplay(bool primary)
{
    JUCE_BREAK_IN_DEBUGGER
}

void FullInterface::repaintChildBackground(SynthSection* child)
{
    //if (!_background_image.isValid())
    //    return;

    //_background.lock();
    //Graphics g(_background_image);
    //paintChildBackground(g, child);
    //_background.updateBackgroundImage(_background_image);
    //_background.unlock();
}

void FullInterface::repaintOpenGlBackground(OpenGlComponent* component)
{
    //if (!_background_image.isValid())
    //    return;

    //_background.lock();
    //Graphics g(_background_image);
    //paintOpenGlBackground(g, component);
    //_background.updateBackgroundImage(_background_image);
    //_background.unlock();
}

void FullInterface::newOpenGLContextCreated()
{
    constexpr double kMinOpenGlVersion = 1.4;

    double version_supported = OpenGLShaderProgram::getLanguageVersion();
    _opengl_unsupported = version_supported < kMinOpenGlVersion;

    if (_opengl_unsupported) {
        NativeMessageBox::showMessageBoxAsync(AlertWindow::WarningIcon, "Unsupported OpenGL Version",
            String("Plugin requires OpenGL version: ") + String(kMinOpenGlVersion) +
            String("\nSupported version: ") + String(version_supported));
        return;
    }

    _shaders = std::make_unique<Shaders>(_opengl_context);
    _opengl.shaders = _shaders.get();
    _opengl.display_scale = 1.0f;
    // last_render_scale_ = display_scale_;

    _background.init(_opengl);
    _section->initOpenGlComponents(_opengl);
}

void FullInterface::renderOpenGL()
{
    if (_opengl_unsupported)
        return;

    _background.render(_opengl);
    _section->renderOpenGlComponents(_opengl, true);
}

void FullInterface::openGLContextClosing()
{
    if (_opengl_unsupported)
        return;

    _background.destroy(_opengl);
    _section->destroyOpenGlComponents(_opengl);

    _opengl.shaders = nullptr;
    _shaders = nullptr;
}

void FullInterface::setAllValues(vital::control_map& controls)
{
    _section->setAllValues(controls);
}

void FullInterface::resized()
{
    _section->setBounds(getBounds());
}

