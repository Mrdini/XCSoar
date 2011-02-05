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

#include "Profile/Profile.hpp"
#include "Profile/Writer.hpp"
#include "LogFile.hpp"
#include "Asset.hpp"
#include "LocalPath.hpp"
#include "StringUtil.hpp"
#include "IO/FileLineReader.hpp"
#include "IO/TextWriter.hpp"

#include <string.h>
#include <windef.h> /* for MAX_PATH */

#define XCSPROFILE "xcsoar-registry.prf"

TCHAR startProfileFile[MAX_PATH];

namespace Profile {
  ProfileMap p;
}

void
Profile::Load()
{
  LogStartUp(_T("Loading profiles"));
  LoadFile(startProfileFile, p);
}

void
Profile::LoadFile(const TCHAR *szFile, ProfileMap &map)
{
  if (string_is_empty(szFile))
    return;

  FileLineReader reader(szFile);
  if (reader.error())
    return;

  LogStartUp(_T("Loading profile from %s"), szFile);

  TCHAR *line;
  while ((line = reader.read()) != NULL) {
    if (string_is_empty(line) || *line == _T('#'))
      continue;

    TCHAR *p = _tcschr(line, _T('='));
    if (p == line || p == NULL)
      continue;

    *p = _T('\0');
    TCHAR *value = p + 1;

#ifdef PROFILE_KEY_PREFIX
    TCHAR key[sizeof(PROFILE_KEY_PREFIX) + _tcslen(line)];
    _tcscpy(key, PROFILE_KEY_PREFIX);
    _tcscat(key, line);
#else
    const TCHAR *key = line;
#endif

    if (*value == _T('"')) {
      ++value;
      p = _tcschr(value, _T('"'));
      if (p == NULL)
        continue;

      *p = _T('\0');

      map.Set(key, value);
    } else {
      long l = _tcstol(value, &p, 10);
      if (p > value)
        map.Set(key, l);
    }
  }
}

void
Profile::Save()
{
  LogStartUp(_T("Saving profiles"));
  SaveFile(startProfileFile, p);
}

void
Profile::SaveFile(const TCHAR *szFile, ProfileMap &map)
{
  if (string_is_empty(szFile))
    return;

  // Try to open the file for writing
  TextWriter writer(szFile);
  // ... on error -> return
  if (writer.error())
    return;

  ProfileWriter profile_writer(writer);

  LogStartUp(_T("Saving profile to %s"), szFile);
  map.Export(profile_writer);
}


void
Profile::SetFiles(const TCHAR* override)
{
  // Set the profile file to load at startup
  if (!string_is_empty(override))
    _tcsncpy(startProfileFile, override, MAX_PATH - 1);
  else
    LocalPath(startProfileFile,
              is_altair() ? _T("config/")_T(XCSPROFILE) : _T(XCSPROFILE));
}

bool
Profile::GetPath(const TCHAR *key, TCHAR *value)
{
  if (!Get(key, value, MAX_PATH) || string_is_empty(value))
    return false;

  ExpandLocalPath(value);
  return true;
}

bool
Profile::Get(const TCHAR *szRegValue, TCHAR *pPos, size_t dwSize)
{
  return p.Get(szRegValue, pPos, dwSize);
}
bool
Profile::Set(const TCHAR *szRegValue, const TCHAR *Pos)
{
  return p.Set(szRegValue, Pos);
}

bool
Profile::Get(const TCHAR *key, int &value)
{
  return p.Get(key, value);
}
bool
Profile::Get(const TCHAR *key, short &value)
{
  return p.Get(key, value);
}
bool
Profile::Get(const TCHAR *key, bool &value)
{
  return p.Get(key, value);
}
bool
Profile::Get(const TCHAR *key, unsigned &value)
{
  return p.Get(key, value);
}
bool
Profile::Get(const TCHAR *key, fixed &value)
{
  return p.Get(key, value);
}

bool
Profile::Set(const TCHAR *key, bool value)
{
  return p.Set(key, value);
}
bool
Profile::Set(const TCHAR *key, int value)
{
  return p.Set(key, value);
}
bool
Profile::Set(const TCHAR *key, long value)
{
  return p.Set(key, value);
}
bool
Profile::Set(const TCHAR *key, unsigned value)
{
  return p.Set(key, value);
}
bool
Profile::Set(const TCHAR *key, fixed value)
{
  return p.Set(key, value);
}

void
Profile::Export(ProfileWriter &writer)
{
  p.Export(writer);
}
