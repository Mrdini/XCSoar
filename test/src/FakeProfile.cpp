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

#include "Profile/Profile.hpp"
#include "Profile/ProfileKeys.hpp"

void
Profile::Save(void)
{
}

void
Profile::SetFiles(const TCHAR* override)
{
}

bool
Profile::GetPath(const TCHAR *key, TCHAR *value)
{
  *value = _T('\0');
  return false;
}

bool
Profile::Get(const TCHAR *szRegValue, TCHAR *pPos, size_t dwSize)
{
  return false;
}
bool
Profile::Set(const TCHAR *szRegValue, const TCHAR *Pos)
{
  return false;
}

bool
Profile::Get(const TCHAR *key, int &value)
{
  return false;
}
bool
Profile::Get(const TCHAR *key, short &value)
{
  return false;
}
bool
Profile::Get(const TCHAR *key, bool &value)
{
  return false;
}
bool
Profile::Get(const TCHAR *key, unsigned &value)
{
  return false;
}
bool
Profile::Get(const TCHAR *key, fixed &value)
{
  return false;
}

bool
Profile::Set(const TCHAR *key, bool value)
{
  return false;
}
bool
Profile::Set(const TCHAR *key, int value)
{
  return false;
}
bool
Profile::Set(const TCHAR *key, long value)
{
  return false;
}
bool
Profile::Set(const TCHAR *key, unsigned value)
{
  return false;
}
bool
Profile::Set(const TCHAR *key, fixed value)
{
  return false;
}
