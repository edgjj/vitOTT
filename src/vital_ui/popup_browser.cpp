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

#include "popup_browser.h"

#include "skin.h"
#include "fonts.h"
#include "paths.h"
#include "open_gl_component.h"
#include "synth_section.h"

namespace {
  template<class Comparator>
  void sortSelectionArray(Array<File>& selection_array) {
    Comparator comparator;
    selection_array.sort(comparator, true);
  }

  const std::string kAddFolderName = "Add Folder";
  const std::string kStoreUrl = "";
  constexpr int kMaxRootFiles = 8000;

  bool isAcceptableRoot(const File& file) {
    std::list<File> folders;
    folders.push_back(file);
    int num_files = 0;

    while (!folders.empty()) {
      File current_file = folders.back();
      folders.pop_back();

      num_files += current_file.getNumberOfChildFiles(File::findFiles);
      if (num_files > kMaxRootFiles)
        return false;

      Array<File> sub_folders = current_file.findChildFiles(File::findDirectories, false);

      for (const File& folder : sub_folders)
        folders.push_back(folder);

    }
    return true;
  }

  void showAddRootWarning() {
    String error = String("Folder has too many files to add to browser. Max: ") + String(kMaxRootFiles);
    NativeMessageBox::showMessageBoxAsync(AlertWindow::WarningIcon, "Error Adding Folder", error);
  }
}

PopupDisplay::PopupDisplay() : SynthSection("Popup Display"), text_("Popup Text", ""),
                               body_(Shaders::kRoundedRectangleFragment),
                               border_(Shaders::kRoundedRectangleBorderFragment) {
  addOpenGlComponent(&body_);
  addOpenGlComponent(&border_);
  addOpenGlComponent(&text_);

  text_.setJustification(Justification::centred);
  text_.setFontType(PlainTextComponent::kLight);

  setSkinOverride(Skin::kPopupBrowser);
}

void PopupDisplay::resized() {
  Rectangle<int> bounds = getLocalBounds();
  int rounding = findValue(Skin::kBodyRounding);

  body_.setBounds(bounds);
  body_.setRounding(rounding);
  body_.setColor(findColour(Skin::kBody, true));

  border_.setBounds(bounds);
  border_.setRounding(rounding);
  border_.setThickness(1.0f, true);
  border_.setColor(findColour(Skin::kBorder, true));

  text_.setBounds(bounds);
  text_.setColor(findColour(Skin::kBodyText, true));
}

void PopupDisplay::setContent(const std::string& text, Rectangle<int> bounds,
                              BubbleComponent::BubblePlacement placement) {
  static constexpr int kHeight = 24;

  int height = kHeight * size_ratio_;
  int mult = getPixelMultiple();
  Font font = Fonts::instance()->proportional_light().withPointHeight(height * 0.5f * mult);
  int padding = height / 4;
  int buffer = padding * 2 + 2;
  int width = (font.getStringWidth(text) / getPixelMultiple()) + buffer;

  int middle_x = bounds.getX() + bounds.getWidth() / 2;
  int middle_y = bounds.getY() + bounds.getHeight() / 2;

  if (placement == BubbleComponent::above)
    setBounds(middle_x - width / 2, bounds.getY() - height, width, height);
  else if (placement == BubbleComponent::below)
    setBounds(middle_x - width / 2, bounds.getBottom(), width, height);
  else if (placement == BubbleComponent::left)
    setBounds(bounds.getX() - width, middle_y - height / 2, width, height);
  else if (placement == BubbleComponent::right)
    setBounds(bounds.getRight(), middle_y - height / 2, width, height);

  text_.setText(text);
  text_.setTextSize(height * 0.5f);
}

PopupList::PopupList() : SynthSection("Popup List"),
                         selected_(-1), hovered_(-1), show_selected_(false), view_position_(0.0f),
                         highlight_(Shaders::kColorFragment), hover_(Shaders::kColorFragment) {
  highlight_.setTargetComponent(this);
  highlight_.setAdditive(true);

  hover_.setTargetComponent(this);
  hover_.setAdditive(true);

  scroll_bar_ = std::make_unique<OpenGlScrollBar>();
  addAndMakeVisible(scroll_bar_.get());
  addOpenGlComponent(scroll_bar_->getGlComponent());
  scroll_bar_->addListener(this);
}

void PopupList::resized() {
  Colour lighten = findColour(Skin::kLightenScreen, true);
  scroll_bar_->setColor(lighten);

  if (getScrollableRange() > getHeight()) {
    int scroll_bar_width = kScrollBarWidth * getSizeRatio();
    int scroll_bar_height = getHeight();
    scroll_bar_->setVisible(true);
    scroll_bar_->setBounds(getWidth() - scroll_bar_width, 0, scroll_bar_width, scroll_bar_height);
    setScrollBarRange();
  }
  else
    scroll_bar_->setVisible(false);

  redoImage();
}

void PopupList::setSelections(PopupItems selections) {
  selections_ = std::move(selections);
  selected_ = std::min(selected_, selections_.size() - 1);
  hovered_ = std::min(hovered_, selections_.size() - 1);
  for (int i = 0; i < selections_.size(); ++i) {
    if (selections_.items[i].selected)
      selected_ = i;
  }
  resized();
}

int PopupList::getRowFromPosition(float mouse_position) {
  int index = floorf((mouse_position + getViewPosition()) / getRowHeight());
  if (index < selections_.size() && index >= 0 && selections_.items[index].id < 0)
    return -1;
  return index;
}

int PopupList::getBrowseWidth() {
  static constexpr int kMinWidth = 150;

  Font font = getFont();
  int max_width = kMinWidth * size_ratio_;
  int buffer = getTextPadding() * 2 + 2;
  for (int i = 0; i < selections_.size(); ++i)
    max_width = std::max(max_width, font.getStringWidth(selections_.items[i].name) / getPixelMultiple() + buffer);

  return max_width;
}

void PopupList::mouseMove(const MouseEvent& e) {
  int row = getRowFromPosition(e.position.y);
  if (row >= selections_.size() || row < 0)
    row = -1;
  hovered_ = row;
}

void PopupList::mouseDrag(const MouseEvent& e) {
  int row = getRowFromPosition(e.position.y);
  if (e.position.x < 0 || e.position.x > getWidth() || row >= selections_.size() || row < 0)
    row = -1;
  hovered_ = row;
}

void PopupList::mouseExit(const MouseEvent& e) {
  hovered_ = -1;
}

int PopupList::getSelection(const MouseEvent& e) {
  float click_y_position = e.position.y;
  int row = getRowFromPosition(click_y_position);
  if (row < selections_.size() && row >= 0)
    return row;

  return -1;
}

void PopupList::mouseUp(const MouseEvent& e) {
  if (e.position.x < 0 || e.position.x > getWidth())
    return;

  select(getSelection(e));
}

void PopupList::mouseDoubleClick(const MouseEvent& e) {
  int selection = getSelection(e);
  if (selection != selected_ || selection < 0)
    return;

  for (Listener* listener : listeners_)
    listener->doubleClickedSelected(this, selections_.items[selection].id, selection);
}

void PopupList::select(int selection) {
  if (selection < 0 || selection >= selections_.size())
    return;

  selected_ = selection;
  for (int i = 0; i < selections_.size(); ++i)
    selections_.items[i].selected = false;
  selections_.items[selected_].selected = true;

  for (Listener* listener : listeners_)
    listener->newSelection(this, selections_.items[selection].id, selection);
}

void PopupList::initOpenGlComponents(OpenGlWrapper& open_gl) {
  rows_.init(open_gl);
  rows_.setColor(Colours::white);

  highlight_.init(open_gl);
  hover_.init(open_gl);
  SynthSection::initOpenGlComponents(open_gl);
}

void PopupList::redoImage() {
  if (getWidth() <= 0 || getHeight() <= 0)
    return;

  int mult = getPixelMultiple();
  int row_height = getRowHeight() * mult;
  int image_width = getWidth() * mult;

  Colour text_color = findColour(Skin::kTextComponentText, true);
  Colour lighten = findColour(Skin::kLightenScreen, true);
  int image_height = std::max(row_height * selections_.size(), getHeight());
  Image rows_image(Image::ARGB, image_width, image_height, true);
  Graphics g(rows_image);
  g.setColour(text_color);
  g.setFont(getFont());

  int padding = getTextPadding();
  int width = (getWidth() - 2 * padding) * mult;
  for (int i = 0; i < selections_.size(); ++i) {
    if (selections_.items[i].id < 0) {
      g.setColour(lighten);
      int y = row_height * (i + 0.5f);
      g.drawRect(padding, y, width, 1);
    }
    else {
      g.setColour(text_color);
      String name = selections_.items[i].name;
      g.drawText(name, padding, row_height * i, width, row_height, Justification::centredLeft, true);
    }
  }
  rows_.setOwnImage(rows_image);
}

void PopupList::moveQuadToRow(OpenGlQuad& quad, int row) {
  int row_height = getRowHeight();
  float view_height = getHeight();
  float open_gl_row_height = 2.0f * row_height / view_height;
  float offset = row * open_gl_row_height - 2.0f * getViewPosition() / view_height;

  float y = 1.0f - offset;
  quad.setQuad(0, -1.0f, y - open_gl_row_height, 2.0f, open_gl_row_height);
}

void PopupList::renderOpenGlComponents(OpenGlWrapper& open_gl, bool animate) {
  Rectangle<int> view_bounds(getLocalBounds());
  OpenGlComponent::setViewPort(this, view_bounds, open_gl);

  float image_width = vital::utils::nextPowerOfTwo(getWidth());
  float image_height = vital::utils::nextPowerOfTwo(rows_.getImageHeight());
  float width_ratio = image_width / getWidth();
  float height_ratio = image_height / (getPixelMultiple() * getHeight());
  float y_offset = 2.0f * getViewPosition() / getHeight();

  rows_.setTopLeft(-1.0f, 1.0f + y_offset);
  rows_.setTopRight(2.0f * width_ratio - 1.0f, 1.0f + y_offset);
  rows_.setBottomLeft(-1.0f, 1.0f + y_offset - 2.0f * height_ratio);
  rows_.setBottomRight(2.0f * width_ratio - 1.0f, 1.0f + y_offset - 2.0f * height_ratio);
  rows_.drawImage(open_gl);

  if (hovered_ >= 0) {
    moveQuadToRow(hover_, hovered_);
    if (show_selected_)
      hover_.setColor(findColour(Skin::kLightenScreen, true));
    else
      hover_.setColor(findColour(Skin::kWidgetPrimary1, true).darker(0.8f));
    hover_.render(open_gl, animate);
  }
  if (selected_ >= 0 && show_selected_) {
    moveQuadToRow(highlight_, selected_);
    highlight_.setColor(findColour(Skin::kWidgetPrimary1, true).darker(0.8f));
    highlight_.render(open_gl, animate);
  }

  SynthSection::renderOpenGlComponents(open_gl, animate);
}

void PopupList::destroyOpenGlComponents(OpenGlWrapper& open_gl) {
  rows_.destroy(open_gl);

  highlight_.destroy(open_gl);
  hover_.destroy(open_gl);
  SynthSection::destroyOpenGlComponents(open_gl);
}

void PopupList::resetScrollPosition() {
  view_position_ = 0;
  setScrollBarRange();
}

void PopupList::mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& wheel) {
  view_position_ -= wheel.deltaY * kScrollSensitivity;
  view_position_ = std::max(0.0f, view_position_);
  float scaled_height = getHeight();
  int scrollable_range = getScrollableRange();
  view_position_ = std::min(view_position_, 1.0f * scrollable_range - scaled_height);
  setScrollBarRange();
}

void PopupList::scrollBarMoved(ScrollBar* scroll_bar, double range_start) {
  view_position_ = range_start;
}

void PopupList::setScrollBarRange() {
  static constexpr float kScrollStepRatio = 0.05f;

  float scaled_height = getHeight();
  scroll_bar_->setRangeLimits(0.0f, getScrollableRange());
  scroll_bar_->setCurrentRange(getViewPosition(), scaled_height, dontSendNotification);
  scroll_bar_->setSingleStepSize(scroll_bar_->getHeight() * kScrollStepRatio);
  scroll_bar_->cancelPendingUpdate();
}

int PopupList::getScrollableRange() {
  int row_height = getRowHeight();
  int selections_height = row_height * static_cast<int>(selections_.size());
  return std::max(selections_height, getHeight());
}

SinglePopupSelector::SinglePopupSelector() : SynthSection("Popup Selector"),
                                             body_(Shaders::kRoundedRectangleFragment),
                                             border_(Shaders::kRoundedRectangleBorderFragment) {
  callback_ = nullptr;
  cancel_ = nullptr;
  addOpenGlComponent(&body_);
  addOpenGlComponent(&border_);

  popup_list_ = std::make_unique<PopupList>();
  popup_list_->addListener(this);
  addSubSection(popup_list_.get());
  popup_list_->setAlwaysOnTop(true);
  popup_list_->setWantsKeyboardFocus(false);

  setSkinOverride(Skin::kPopupBrowser);
}

void SinglePopupSelector::resized() {
  SynthSection::resized();

  Rectangle<int> bounds = getLocalBounds();
  int rounding = findValue(Skin::kBodyRounding);
  popup_list_->setBounds(1, rounding, getWidth() - 2, getHeight() - 2 * rounding);

  body_.setBounds(bounds);
  body_.setRounding(findValue(Skin::kBodyRounding));
  body_.setColor(findColour(Skin::kBody, true));

  border_.setBounds(bounds);
  border_.setRounding(findValue(Skin::kBodyRounding));
  border_.setThickness(1.0f, true);
  border_.setColor(findColour(Skin::kBorder, true));
}

void SinglePopupSelector::setPosition(Point<int> position, Rectangle<int> bounds) {
  int rounding = findValue(Skin::kBodyRounding);
  int width = popup_list_->getBrowseWidth();
  int height = popup_list_->getBrowseHeight() + 2 * rounding;
  int x = position.x;
  int y = position.y;
  if (x + width > bounds.getRight())
    x -= width;
  if (y + height > bounds.getBottom())
    y = bounds.getBottom() - height;
  setBounds(x, y, width, height);
}

DualPopupSelector::DualPopupSelector() : SynthSection("Dual Popup Selector"),
                                         body_(Shaders::kRoundedRectangleFragment),
                                         border_(Shaders::kRoundedRectangleBorderFragment),
                                         divider_(Shaders::kColorFragment) {
  callback_ = nullptr;

  addOpenGlComponent(&body_);
  addOpenGlComponent(&border_);
  addOpenGlComponent(&divider_);

  left_list_ = std::make_unique<PopupList>();
  left_list_->addListener(this);
  addSubSection(left_list_.get());
  left_list_->setAlwaysOnTop(true);
  left_list_->setWantsKeyboardFocus(false);
  left_list_->showSelected(true);

  right_list_ = std::make_unique<PopupList>();
  right_list_->addListener(this);
  addSubSection(right_list_.get());
  right_list_->setAlwaysOnTop(true);
  right_list_->setWantsKeyboardFocus(false);
  right_list_->showSelected(true);

  setSkinOverride(Skin::kPopupBrowser);
}

void DualPopupSelector::resized() {
  SynthSection::resized();

  Rectangle<int> bounds = getLocalBounds();
  int rounding = findValue(Skin::kBodyRounding);
  int height = getHeight() - 2 * rounding;
  left_list_->setBounds(1, rounding, getWidth() / 2 - 2, height);
  int right_x = left_list_->getRight() + 1;
  right_list_->setBounds(right_x, rounding, getWidth() - right_x - 1, height);

  body_.setBounds(bounds);
  body_.setRounding(findValue(Skin::kBodyRounding));
  body_.setColor(findColour(Skin::kBody, true));

  border_.setBounds(bounds);
  border_.setRounding(findValue(Skin::kBodyRounding));
  border_.setThickness(1.0f, true);

  divider_.setBounds(getWidth() / 2 - 1, 1, 1, getHeight() - 2);

  Colour border = findColour(Skin::kBorder, true);
  border_.setColor(border);
  divider_.setColor(border);
}

void DualPopupSelector::setPosition(Point<int> position, int width, Rectangle<int> bounds) {
  int rounding = findValue(Skin::kBodyRounding);
  int height = left_list_->getBrowseHeight() + 2 * rounding;
  int x = position.x;
  int y = position.y;
  if (x + width > bounds.getRight())
    x -= width;
  if (y + height > bounds.getBottom())
    y = bounds.getBottom() - height;
  setBounds(x, y, width, height);
}

void DualPopupSelector::newSelection(PopupList* list, int id, int index) {
  if (list == left_list_.get()) {
    PopupItems right_items = left_list_->getSelectionItems(index);
    if (right_items.size() == 0) {
      callback_(id);
      right_list_->setSelections(right_items);
      return;
    }

    int right_selection = right_list_->getSelected();
    if (right_selection < 0 || right_selection >= right_items.size() ||
      right_list_->getSelectionItems(right_selection).name != right_items.items[right_selection].name) {
      right_selection = 0;
    }

    right_list_->setSelections(right_items);
    right_list_->select(right_selection);
  }
  else
    callback_(id);
}