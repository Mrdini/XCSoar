/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2010 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "Form/TabBar.hpp"
#include "Screen/PaintWindow.hpp"
#include "Screen/Fonts.hpp"
#include "Screen/Layout.hpp"
#include "Screen/Key.h"
#include "Screen/Icon.hpp"

#include <assert.h>


TabBarControl::TabBarControl(ContainerWindow &_parent,
                int x, int y, unsigned _width, unsigned _height,
                const WindowStyle style):
                TabbedControl(_parent, 0, 0, _parent.get_width(), _parent.get_height(), style),
                theTabDisplay(NULL),
                TabBarHeight(_height),
                TabBarWidth(_width),
                TabLineHeight(Layout::landscape ?
                    (Layout::Scale(TabLineHeightInitUnscaled) * 0.75) :
                    Layout::Scale(TabLineHeightInitUnscaled))
{
  theTabDisplay = new TabDisplay(*this, TabBarWidth, TabBarHeight);
}

bool
TabBarControl::GetButtonIsButtonOnly(unsigned i)
{

  assert(i < buttons.size());

  if (i >= buttons.size())
    return false;

  return buttons[i]->IsButtonOnly;
}

const TCHAR*
TabBarControl::GetButtonCaption(unsigned i)
{
  assert(i < buttons.size());

  if (i >= buttons.size())
    return _T("");

  return buttons[i]->Caption;
}

const MaskedIcon *
TabBarControl::GetButtonIcon(unsigned i)
{
  assert(i < buttons.size());

  if (i >= buttons.size())
    return NULL;

  return buttons[i]->bmp;
}

unsigned
TabBarControl::AddClient(Window *w, const TCHAR* Caption,
    bool IsButtonOnly, MaskedIcon *bmp,
    PreHideNotifyCallback_t PreHideFunction,
    PreShowNotifyCallback_t PreShowFunction,
    PostShowNotifyCallback_t PostShowFunction)
{
  if (GetCurrentPage() != buttons.size())
    w->hide();

  TabbedControl::AddClient(w);
  const RECT rc = get_client_rect();
  if (Layout::landscape)
    w->move(rc.left , rc.top, rc.right - rc.left , rc.bottom - rc.top);
  else
    w->move(rc.left, rc.top + TabBarHeight, rc.right - rc.left, rc.bottom - rc.top - TabBarHeight);

  OneTabButton *b = new OneTabButton(Caption, IsButtonOnly, bmp,
      PreHideFunction, PreShowFunction, PostShowFunction);

  buttons.append(b);

  return buttons.size() - 1;
}

void
TabBarControl::SetCurrentPage(unsigned i, unsigned EventType)
{
  bool Continue = true;
  assert(i < buttons.size());

  if (buttons[GetCurrentPage()]->PreHideFunction) {
    if (!buttons[GetCurrentPage()]->PreHideFunction())
        Continue = false;
  }

  if (Continue) {
    if (buttons[i]->PreShowFunction) {
      Continue = buttons[i]->PreShowFunction(EventType);
    }
  }

  if (Continue) {
    TabbedControl::SetCurrentPage(i);
    if (buttons[i]->PostShowFunction) {
      buttons[i]->PostShowFunction();
    }
  }
  theTabDisplay->trigger_invalidate();
}

void
TabBarControl::NextPage(unsigned EventType)
{
  if (buttons.size() < 2)
    return;

  assert(GetCurrentPage() < buttons.size());

  SetCurrentPage((GetCurrentPage() + 1) % buttons.size(), EventType);
}

void
TabBarControl::PreviousPage(unsigned EventType)
{
  if (buttons.size() < 2)
    return;

  assert(GetCurrentPage() < buttons.size());

  SetCurrentPage((GetCurrentPage() + buttons.size() - 1) % buttons.size(), EventType);
}

const RECT
TabBarControl::GetButtonSize(unsigned i)
{
  const unsigned margin = 1;

  bool partialTab = false;
  if ( (Layout::landscape && TabBarHeight < get_height()) ||
      (!Layout::landscape && TabBarWidth < get_width()) )
    partialTab = true;

  const unsigned finalmargin = partialTab ? TabLineHeight - 3 * margin : margin;

  const unsigned but_width = Layout::landscape ? (TabBarHeight - finalmargin) / buttons.size() - margin
       : (TabBarWidth - finalmargin) / buttons.size() - margin;
  RECT rc;

  if (Layout::landscape) {
    rc.left = 0;
    rc.right = TabBarWidth - TabLineHeight;

    rc.top = margin + (margin + but_width) * i;
    rc.bottom = rc.top + but_width;
    if (!partialTab && (i == buttons.size() - 1))
      rc.bottom = TabBarHeight - margin - 1;

  } else {
  rc.top = 0;
  rc.bottom = rc.top + TabBarHeight - TabLineHeight;

    rc.left = margin + (margin + but_width) * i;
    rc.right = rc.left + but_width;
    if (!partialTab && (i == buttons.size() - 1))
      rc.right = TabBarWidth - margin - 1;
  }

  return rc;
}

// TabDisplay Functions
TabDisplay::TabDisplay(TabBarControl& _theTabBar,
    unsigned width, unsigned height) :
  PaintWindow(),
  theTabBar(_theTabBar),
  dragging(false),
  downindex(-1)
{
  WindowStyle mystyle;
  mystyle.tab_stop();
  set(theTabBar, 0, 0, width, height, mystyle);
}

void
TabDisplay::on_paint(Canvas &canvas)
{
  canvas.clear(Color::BLACK);
  canvas.select(Fonts::MapBold);
  const unsigned CaptionStyle = DT_EXPANDTABS | DT_CENTER | DT_NOCLIP
      | DT_WORDBREAK;

  for (unsigned i = 0; i < theTabBar.GetTabCount(); i++) {
    if (i == theTabBar.GetCurrentPage()) {
      canvas.set_text_color(Color::WHITE);
      canvas.set_background_color(Color::BLACK);

    } else if (((int)i == downindex) && (dragoffbutton == false)) {
      canvas.set_text_color(Color::BLACK);
      canvas.set_background_color(Color::YELLOW);

    } else {
      canvas.set_text_color(Color::BLACK);
      canvas.set_background_color(Color::WHITE);
    }
    const RECT &rc = theTabBar.GetButtonSize(i);

    RECT rcText = rc;
    RECT rcTextFinal = rc;
    const unsigned buttonheight = rc.bottom - rc.top;
    const int textwidth = canvas.text_width(theTabBar.GetButtonCaption(i));
    const int textheight = canvas.text_height(theTabBar.GetButtonCaption(i));
    unsigned textheightoffset = 0;

    if (textwidth >= rc.right - rc.left) // assume 2 lines
      textheightoffset = max(0, (int)(buttonheight - textheight * 2) / 2);
    else
      textheightoffset = max(0, (int)(buttonheight - textheight) / 2);

    rcTextFinal.top += textheightoffset;

    //button-only formatting
    if (theTabBar.GetButtonIsButtonOnly(i)
        && (int)i != downindex) {
      canvas.draw_button(rc, false);
      canvas.background_transparent();
    } else {
      canvas.fill_rectangle(rc, canvas.get_background_color());
    }
    if (theTabBar.GetButtonIcon(i) != NULL) {
      const MaskedIcon *bmp = theTabBar.GetButtonIcon(i);
      const int offsetx = (rc.right - rc.left - bmp->get_size().cx) / 2;
      const int offsety = (rc.bottom - rc.top - bmp->get_size().cy) / 2;

      bmp->draw(canvas, rc.left + offsetx, rc.top + offsety);
    } else {
      canvas.formatted_text(&rcTextFinal, theTabBar.GetButtonCaption(i),
          CaptionStyle);
    }
  }
  if (has_focus()) {
    RECT rcFocus;
    rcFocus.top = rcFocus.left = 0;
    rcFocus.right = canvas.get_width();
    rcFocus.bottom = canvas.get_height();
    canvas.draw_focus(rcFocus);
  }
  this->show();
}

bool
TabDisplay::on_killfocus()
{
  invalidate();
  PaintWindow::on_killfocus();

  return true;
}

bool
TabDisplay::on_setfocus()
{
  invalidate();
  PaintWindow::on_setfocus();
  return true;
}

bool
TabDisplay::on_key_check(unsigned key_code) const
{
  switch (key_code) {

  case VK_RETURN:
    return false; // ToDo
    break;

  case VK_LEFT:
    if (Layout::landscape)
      return false;
    else
      return (theTabBar.GetCurrentPage() > 0);

  case VK_RIGHT:
    if (Layout::landscape)
      return false;
    else
      return theTabBar.GetCurrentPage() < theTabBar.GetTabCount() - 1;

  case VK_DOWN:
    if (Layout::landscape)
      return theTabBar.GetCurrentPage() < theTabBar.GetTabCount() - 1;
    else
      return false;

  case VK_UP:
    if (Layout::landscape)
      return (theTabBar.GetCurrentPage() > 0);
    else
      return false;

  default:
    return false;
  }
}


bool
TabDisplay::on_key_down(unsigned key_code)
{
  switch (key_code) {
#ifdef GNAV
  // JMW added this to make data entry easier
  case VK_F4:
#endif
  case VK_RETURN: //ToDo: support Return
    break;

  case VK_DOWN:
  case VK_RIGHT:
    theTabBar.NextPage(1);
    return true;

  case VK_UP:
  case VK_LEFT:
    theTabBar.PreviousPage(1);
    return true;
  }

  return PaintWindow::on_key_down(key_code);
}

bool
TabDisplay::on_mouse_down(int x, int y)
{
  drag_end();

  POINT Pos;
  Pos.x = x;
  Pos.y = y;

  // If possible -> Give focus to the Control
  bool had_focus = has_focus();
  if (!had_focus)
    set_focus();
  for (unsigned i = 0; i < theTabBar.GetTabCount(); i++) {
    const RECT rc = theTabBar.GetButtonSize(i);
    if (PtInRect(&rc, Pos)
        && i != theTabBar.GetCurrentPage()) {
      dragging = true;
      downindex = i;
      set_capture();
      invalidate();
      return true;
    }
  }
  return PaintWindow::on_mouse_down(x, y);
}

bool
TabDisplay::on_mouse_up(int x, int y)
{
  POINT Pos;
  Pos.x = x;
  Pos.y = y;

  if (dragging) {
    drag_end();
    for (unsigned i = 0; i < theTabBar.GetTabCount(); i++) {
      const RECT rc = theTabBar.GetButtonSize(i);
      if (PtInRect(&rc, Pos)) {
        if ((int)i == downindex) {
          theTabBar.SetCurrentPage(i, 0);
          break;
        }
      }
    }
    if (downindex > -1)
      invalidate();
    downindex = -1;
    return true;
  } else {
    return PaintWindow::on_mouse_up(x, y);
  }
}

bool
TabDisplay::on_mouse_move(int x, int y, unsigned keys)
{
  if (downindex == -1)
    return false;

  const RECT rc = theTabBar.GetButtonSize(downindex);
  POINT Pos;
  Pos.x = x;
  Pos.y = y;

  const bool tmp = !PtInRect(&rc, Pos);
  if (dragoffbutton != tmp) {
    dragoffbutton = tmp;
    invalidate(rc);
  }
  return true;
}

void
TabDisplay::drag_end()
{
  if (dragging) {
    dragging = false;
    dragoffbutton = false;
    release_capture();
  }
}