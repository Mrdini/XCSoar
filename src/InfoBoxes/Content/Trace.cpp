/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2011 The XCSoar Project
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

#include "InfoBoxes/Content/Trace.hpp"
#include "InfoBoxes/InfoBoxWindow.hpp"
#include "InfoBoxes/InfoBoxManager.hpp"
#include "FlightStatisticsRenderer.hpp"
#include "TraceHistoryRenderer.hpp"
#include "UnitsFormatter.hpp"
#include "Units.hpp"
#include "Components.hpp"
#include "Interface.hpp"
#include "DeviceBlackboard.hpp"
#include "Screen/Layout.hpp"
#include "Protection.hpp"
#include "MainWindow.hpp"
#include "GlideComputer.hpp"
#include "Dialogs/dlgInfoBoxAccess.hpp"

void
InfoBoxContentSpark::do_paint(InfoBoxWindow &infobox, Canvas &canvas,
                              const TraceVariableHistory& var)
{
  RECT rc = infobox.get_value_rect();
  rc.top += Layout::FastScale(2);

  if (var.empty())
    return;

  TraceHistoryRenderer::RenderVario(canvas, rc, var,
                                    CommonInterface::Calculated().glide_polar_task.get_mc());
}

void
InfoBoxContentVarioSpark::on_custom_paint(InfoBoxWindow &infobox, Canvas &canvas)
{
  do_paint(infobox, canvas, CommonInterface::Calculated().trace_history.BruttoVario);
}

void
InfoBoxContentNettoVarioSpark::on_custom_paint(InfoBoxWindow &infobox, Canvas &canvas)
{
  do_paint(infobox, canvas, CommonInterface::Calculated().trace_history.NettoVario);
}

void
InfoBoxContentCirclingAverageSpark::on_custom_paint(InfoBoxWindow &infobox, Canvas &canvas)
{
  do_paint(infobox, canvas, CommonInterface::Calculated().trace_history.CirclingAverage);
}

void
InfoBoxContentSpark::label_vspeed(InfoBoxWindow &infobox,
                                  const TraceVariableHistory& var)
{
  if (var.empty())
    return;

  TCHAR sTmp[32];
  Units::FormatUserVSpeed(var.last(), sTmp,
                          sizeof(sTmp) / sizeof(sTmp[0]));
  infobox.SetComment(sTmp);

  infobox.SetValue(_T(""));
  infobox.invalidate();
}

void
InfoBoxContentVarioSpark::Update(InfoBoxWindow &infobox)
{
  label_vspeed(infobox, CommonInterface::Calculated().trace_history.BruttoVario);
}

void
InfoBoxContentNettoVarioSpark::Update(InfoBoxWindow &infobox)
{
  label_vspeed(infobox, CommonInterface::Calculated().trace_history.NettoVario);
}

void
InfoBoxContentCirclingAverageSpark::Update(InfoBoxWindow &infobox)
{
  label_vspeed(infobox, CommonInterface::Calculated().trace_history.CirclingAverage);
}


void
InfoBoxContentBarogram::Update(InfoBoxWindow &infobox)
{
  const NMEA_INFO &basic = CommonInterface::Basic();
  TCHAR sTmp[32];

  Units::FormatUserAltitude(basic.NavAltitude, sTmp,
                            sizeof(sTmp) / sizeof(sTmp[0]));
  infobox.SetComment(sTmp);

  infobox.SetValue(_T(""));
  infobox.invalidate();
}

void
InfoBoxContentBarogram::on_custom_paint(InfoBoxWindow &infobox, Canvas &canvas)
{
  RECT rc = infobox.get_value_rect();
  rc.top += Layout::FastScale(2);

  FlightStatisticsRenderer fs(glide_computer->GetFlightStats());
  fs.RenderBarographSpark(canvas, rc, XCSoarInterface::Basic(),
                          XCSoarInterface::Calculated(), protected_task_manager);
}

bool
InfoBoxContentBarogram::HandleKey(const InfoBoxKeyCodes keycode)
{
  switch (keycode) {
  case ibkEnter:
    dlgAnalysisShowModal(XCSoarInterface::main_window, 0);
    return true;

  case ibkUp:
  case ibkDown:
  case ibkLeft:
  case ibkRight:
    break;
  }
  return false;
}