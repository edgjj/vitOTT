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
    // we do this for resolving override-related things
    skin.setComponentColors(_section.get(), _section->getSkinOverride());
    _section_list.push_back(_section.get());

    addAndMakeVisible(*_section);

    // various popup stuff
    _popup_display_1 = std::make_unique<PopupDisplay>();
    addChildComponent(*_popup_display_1);
    _popup_display_1->setVisible(false);
    _popup_display_1->setAlwaysOnTop(true);
    _popup_display_1->setWantsKeyboardFocus(false);
    _section_list.push_back(_popup_display_1.get());

    _popup_display_2 = std::make_unique<PopupDisplay>();
    addChildComponent(*_popup_display_2);
    _popup_display_2->setVisible(false);
    _popup_display_2->setAlwaysOnTop(true);
    _popup_display_2->setWantsKeyboardFocus(false);
    _section_list.push_back(_popup_display_2.get());

    _popup_selector = std::make_unique<SinglePopupSelector>();
    addChildComponent(*_popup_selector);
    _popup_selector->setVisible(false);
    _popup_selector->setAlwaysOnTop(true);
    _popup_selector->setWantsKeyboardFocus(true);
    _section_list.push_back(_popup_selector.get());

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
    auto display_scale_ = 1.0f;

    _popup_selector->setCallback(callback);
    _popup_selector->setCancelCallback(cancel);
    _popup_selector->showSelections(options);
    Rectangle<int> bounds(0, 0, std::ceil(display_scale_ * getWidth()), std::ceil(display_scale_ * getHeight()));
    _popup_selector->setPosition(getLocalPoint(source, position), bounds);
    _popup_selector->setVisible(true);
}

void FullInterface::popupDisplay(Component* source, const std::string& text, BubbleComponent::BubblePlacement placement, bool primary)
{
    auto display_scale_ = 1.0f;

    PopupDisplay* display = primary ? _popup_display_1.get() : _popup_display_2.get();
    display->setContent(text, getLocalArea(source, source->getLocalBounds()), BubbleComponent::BubblePlacement::left); // left placement for testing needs
    display->setVisible(true);
}

void FullInterface::hideDisplay(bool primary)
{
    PopupDisplay* display = primary ? _popup_display_1.get() : _popup_display_2.get();
    if (display)
        display->setVisible(false);
}

void FullInterface::repaintChildBackground(SynthSection* child)
{
    if (!_background_image.isValid())
        return;

    _background.lock();
    Graphics g(_background_image);
    child->paintBackground(g);
    _background.updateBackgroundImage(_background_image);
    _background.unlock();
}

void FullInterface::repaintOpenGlBackground(OpenGlComponent* component)
{
    if (!_background_image.isValid())
        return;

    _background.lock();
    Graphics g(_background_image);
    component->paintBackground(g);
    _background.updateBackgroundImage(_background_image);
    _background.unlock();
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

    for (auto& v : _section_list) {
        v->initOpenGlComponents(_opengl);
        v->repaintBackground();
    }
}

void FullInterface::renderOpenGL()
{
    if (_opengl_unsupported)
        return;

    _background.render(_opengl);
    for (auto& v : _section_list) {
        v->renderOpenGlComponents(_opengl, true);
    }
}

void FullInterface::openGLContextClosing()
{
    if (_opengl_unsupported)
        return;

    _background.destroy(_opengl);
    for (auto& v : _section_list) {
        v->destroyOpenGlComponents(_opengl);
    }

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
    auto display_scale_ = 1.0f;

    int width = std::ceil(display_scale_ * getWidth());
    int height = std::ceil(display_scale_ * getHeight());

    _background.lock();
    _background_image = Image(Image::RGB, width, height, true);
    Graphics g(_background_image);
    for (auto& v : _section_list) {
        v->repaintBackground();
    }

    _background.updateBackgroundImage(_background_image);
    _background.unlock();
}

