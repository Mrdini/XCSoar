/* Copyright_License {

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
#ifndef TASK_VISITOR_HPP
#define TASK_VISITOR_HPP

class TaskInterface;
class OrderedTask;
class AbortTask;
class GotoTask;

/**
 * Generic visitor to achieve double-dispatch of an AbstractTask
 * (e.g. the active task in the TaskManager)
 */
class TaskVisitor {
public:
  virtual void Visit(const OrderedTask &t) = 0;
  virtual void Visit(const AbortTask &t) = 0;
  virtual void Visit(const GotoTask &t) = 0;

  void Visit(const TaskInterface &t);
};

#endif
