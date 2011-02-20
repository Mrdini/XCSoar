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

#include "Dialogs/Internal.hpp"
#include "Dialogs/dlgTaskManager.hpp"
#include "Screen/Layout.hpp"
#include "Screen/SingleWindow.hpp"
#include "Task/ProtectedTaskManager.hpp"
#include "Components.hpp"

#include <assert.h>
#include <stdio.h> //debug

static WndForm* wf = NULL;
static WndFrame* wStatus = NULL;
static WndButton* cmdRevert = NULL;
static WndButton* cmdClose = NULL;
static bool* task_modified = NULL;
static bool* goto_calculator_on_exit = NULL;

static void
RefreshStatus()
{
  wStatus->SetText(*task_modified ?
                   _T("Task has been modified") : _T("Task unchanged"));

  cmdRevert->set_visible(*task_modified);
}

void
pnlTaskManagerClose::OnCloseClicked(WndButton &Sender)
{
  (void)Sender;
  dlgTaskManager::OnClose();
}

void
pnlTaskManagerClose::OnCalculatorResumeClicked(WndButton &Sender)
{
  (void)Sender;
  *goto_calculator_on_exit = true;
  dlgTaskManager::OnClose();
}

void
pnlTaskManagerClose::OnRevertClicked(WndButton &Sender)
{
  (void)Sender;
  dlgTaskManager::RevertTask();
  RefreshStatus();
}

void
pnlTaskManagerClose::OnTabReClick()
{
  dlgTaskManager::OnClose();
}

bool
pnlTaskManagerClose::OnTabPreShow(TabBarControl::EventType EventType)
{
  *goto_calculator_on_exit = false;
  WndButton *wb = (WndButton*)wf->FindByName(_T("cmdCalculatorResume"));
  assert(wb);
  TaskManager::TaskMode_t mode = protected_task_manager->get_mode();
  const bool show_calculator_button =
    (CommonInterface::Calculated().flight.Flying &&
                        (mode != TaskManager::MODE_ABORT) &&
                        (mode != TaskManager::MODE_GOTO) &&
                        XCSoarInterface::Calculated().task_stats.task_valid) ?
                            false : true;
  // because we arrived here via the task calculator
  wb->set_visible(show_calculator_button);

  if (!(*task_modified) && EventType == TabBarControl::MouseOrButton) {

    if (!show_calculator_button) {
      dlgTaskManager::OnClose();
      return true;
    }
  }

  RefreshStatus();
  return true;
}

Window*
pnlTaskManagerClose::Load(SingleWindow &parent, TabBarControl* wTabBar,
                          WndForm* _wf, OrderedTask** task,
                          bool* _task_modified, bool* _goto_calculator_on_exit)
{
  assert(wTabBar);

  assert(_wf);
  wf = _wf;

  assert(_task_modified);
  task_modified = _task_modified;

  assert(_goto_calculator_on_exit);
  goto_calculator_on_exit = _goto_calculator_on_exit;

  Window *wTaskManagerClose =
      LoadWindow(dlgTaskManager::CallBackTable, wf, *wTabBar,
                 Layout::landscape ?
                     _T("IDR_XML_TASKMANAGERCLOSE_L") :
                     _T("IDR_XML_TASKMANAGERCLOSE"));
  assert(wTaskManagerClose);

  wStatus = (WndFrame *)wf->FindByName(_T("frmStatus"));
  assert(wStatus);

  cmdRevert = (WndButton *)wf->FindByName(_T("cmdRevert"));
  assert(cmdRevert);

  cmdClose = (WndButton *)wf->FindByName(_T("cmdClose"));
  assert(cmdClose);

  wStatus->SetAlignCenter();
  return wTaskManagerClose;
}