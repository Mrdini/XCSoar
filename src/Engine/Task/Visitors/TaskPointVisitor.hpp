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
#ifndef TASKPOINTVISITOR_HPP
#define TASKPOINTVISITOR_HPP

class TaskPoint;
class UnorderedTaskPoint;
class StartPoint;
class ASTPoint;
class AATPoint;
class FinishPoint;

/**
 * Generic visitor for const task points (for double-dispatch)
 */
class TaskPointConstVisitor {
protected:
  virtual void Visit(const UnorderedTaskPoint &p) = 0;
  virtual void Visit(const StartPoint &p) = 0;
  virtual void Visit(const ASTPoint &p) = 0;
  virtual void Visit(const AATPoint &p) = 0;
  virtual void Visit(const FinishPoint &p) = 0;

public:
  void Visit(const TaskPoint &tp);
};

/**
 * Generic visitor for task points (for double-dispatch)
 */
class TaskPointVisitor {
protected:
  virtual void Visit(UnorderedTaskPoint &p) = 0;
  virtual void Visit(StartPoint &p) = 0;
  virtual void Visit(ASTPoint &p) = 0;
  virtual void Visit(AATPoint &p) = 0;
  virtual void Visit(FinishPoint &p) = 0;

public:
  void Visit(TaskPoint &tp);
};


#endif
