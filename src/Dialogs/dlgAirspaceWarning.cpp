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

#include "Dialogs/Internal.hpp"
#include "Units.hpp"
#include "Components.hpp"
#include "Screen/Layout.hpp"
#include "Screen/Key.h"
#include "Airspace/AirspaceWarning.hpp"
#include "Airspace/ProtectedAirspaceWarningManager.hpp"
#include "Airspace/AirspaceWarningManager.hpp"
#include "Engine/Airspace/AbstractAirspace.hpp"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

static WndForm *wf = NULL;
static const AbstractAirspace* airspace = NULL;  // Current action airspace

static void
Hide()
{
  wf->hide();
  wf->SetModalResult(mrOK);
}

/** ack inside */
static void
Ack()
{
  if (airspace != NULL)
    airspace_warnings->acknowledge_inside(*airspace, true);
}

static void
OnAckClicked(WndButton &Sender)
{
  (void)Sender;
  Ack();
}

/** ack warn */
static void
Ack1()
{
  if (airspace != NULL)
    airspace_warnings->acknowledge_warning(*airspace, true);
}

static void
OnAck1Clicked(WndButton &Sender)
{
  (void)Sender;
  Ack1();
}

/** ack day */
static void
Ack2()
{
  if (airspace != NULL)
    airspace_warnings->acknowledge_day(*airspace, true);
}

static void
OnAck2Clicked(WndButton &Sender)
{
  (void)Sender;
  Ack2();
}

static void
OnCloseClicked(WndButton &Sender)
{
  (void)Sender;
  Hide();
}

static bool
OnKeyDown(WndForm &Sender, unsigned key_code)
{
  switch (key_code){
    case VK_ESCAPE:
      Hide();
    return true;

#ifdef GNAV
    case VK_APP1:
    case '6':
      Ack();
    return true;

    case VK_APP2:
    case '7':
      Ack1();
    return true;

    case VK_APP3:
    case '8':
      Ack2();
    return true;

    case VK_APP4:
    case '9':
      Enable();
    return true;
#endif

  default:
    return false;
  }
}

static void
update()
{
  ProtectedAirspaceWarningManager::Lease lease(*airspace_warnings);
  int index = lease->get_warning_index(*airspace);
  if (index < 0)
    Hide();

  const AirspaceWarning *warning = lease->get_warning(index);

  TCHAR sTmp[100];
  _stprintf(sTmp, _T("%d secs dist %d m"),
            (int)warning->get_solution().elapsed_time,
            (int)warning->get_solution().distance);

  wf->SetCaption(sTmp);
}

static void
OnTimer(WndForm &Sender)
{
  update();
}

bool
dlgAirspaceWarningVisible()
{
  return (wf != NULL);
}

static CallBackTableEntry CallBackTable[] = {
  DeclareCallBackEntry(OnAckClicked),
  DeclareCallBackEntry(OnAck1Clicked),
  DeclareCallBackEntry(OnAck2Clicked),
  DeclareCallBackEntry(OnCloseClicked),
  DeclareCallBackEntry(NULL)
};

void
dlgAirspaceWarningShowModal(SingleWindow &parent, const AbstractAirspace &as)
{
  assert(airspace_warnings != NULL);

  airspace = &as;

  wf = LoadDialog(CallBackTable, parent, _T("IDR_XML_AIRSPACEWARNING_L"));
  assert(wf != NULL);

  wf->SetKeyDownNotify(OnKeyDown);
  wf->SetTimerNotify(OnTimer);

  update();

  wf->ShowModal();

  delete wf;

  // Needed for dlgAirspaceWarningVisible()
  wf = NULL;
}

void
dlgAirspaceWarningShowAll(SingleWindow &parent)
{
  ProtectedAirspaceWarningManager::Lease lease(*airspace_warnings);
  for (unsigned i = 0; i < lease->size(); ++i) {
    const AirspaceWarning *warning = lease->get_warning(i);
    if (warning != NULL && warning->get_ack_expired())
      dlgAirspaceWarningShowModal(parent, warning->get_airspace());
  }
}
