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

#include "MapWindowLabels.hpp"
#include "MapWindow.hpp"

#include <stdlib.h>
#include <assert.h>

static int _cdecl
MapWaypointLabelListCompare(const void *elem1, const void *elem2)
{
  const WayPointLabelList::Label &e1 = *(const WayPointLabelList::Label *)elem1;
  const WayPointLabelList::Label &e2 = *(const WayPointLabelList::Label *)elem2;

  if (e1.inTask && !e2.inTask)
    return -1;

  if (!e1.inTask && e2.inTask)
    return 1;

  if (e1.isAirport && !e2.isAirport)
    return -1;

  if (!e1.isAirport && e2.isAirport)
    return 1;

  if (e1.isLandable && !e2.isLandable)
    return -1;

  if (!e1.isLandable && e2.isLandable)
    return 1;

  if (e1.isWatchedWaypoint && !e2.isWatchedWaypoint)
    return -1;

  if (!e1.isWatchedWaypoint && e2.isWatchedWaypoint)
    return 1;

  if (e1.AltArivalAGL > e2.AltArivalAGL)
    return -1;

  if (e1.AltArivalAGL < e2.AltArivalAGL)
    return 1;

  return 0;
}

void
WayPointLabelList::Add(const TCHAR *Name, int X, int Y, TextInBoxMode_t Mode,
                       int AltArivalAGL, bool inTask,
                       bool isLandable, bool isAirport, bool isWatchedWaypoint)
{
  if ((X < - WPCIRCLESIZE)
      || (X > (int)width + (WPCIRCLESIZE * 3))
      || (Y < - WPCIRCLESIZE)
      || (Y > (int)height + WPCIRCLESIZE))
    return;

  if (num_labels >= sizeof(labels) / sizeof(labels[0]))
    return;

  Label *E = &labels[num_labels++];

  _tcscpy(E->Name, Name);
  E->Pos.x = X;
  E->Pos.y = Y;
  E->Mode = Mode;
  E->AltArivalAGL = AltArivalAGL;
  E->inTask = inTask;
  E->isLandable = isLandable;
  E->isAirport  = isAirport;
  E->isWatchedWaypoint = isWatchedWaypoint;
}

void
WayPointLabelList::Sort()
{
  qsort(&labels, num_labels, sizeof(labels[0]),
        MapWaypointLabelListCompare);
}
