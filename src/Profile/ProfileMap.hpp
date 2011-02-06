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

#ifndef XCSOAR_PROFILE_MAP_HPP
#define XCSOAR_PROFILE_MAP_HPP

#include "Math/fixed.hpp"
#include "Util/tstring.hpp"

#include <cstdio>
#include <map>

class ProfileWriter;

namespace ProfileMap 
{
  typedef std::map<tstring, tstring> map_t;

  static map_t map;

  /**
   * Reads a value from the profile map
   * @param szRegValue Name of the value that should be read
   * @param pPos Pointer to the output buffer
   * @param dwSize Maximum size of the output buffer
   */
  bool Get(const TCHAR *szRegValue, TCHAR *pPos, size_t dwSize);
  /**
   * Writes a value to the profile map
   * @param szRegValue Name of the value that should be written
   * @param Pos Value that should be written
   */
  bool Set(const TCHAR *szRegValue, const TCHAR *Pos);

  static inline bool Get(const TCHAR *key, int &value)
  {
    // Try to read the profile map
    TCHAR str[50];
    if (!Get(key, str, 50))
      return false;

    // Parse the string for a number
    TCHAR *endptr;
    int tmp = _tcstol(str, &endptr, 0);
    if (endptr == str)
      return false;

    // Save parsed value to output parameter value and return success
    value = tmp;
    return true;
  }

  static inline bool Get(const TCHAR *key, short &value)
  {
    // Try to read the profile map
    TCHAR str[50];
    if (!Get(key, str, 50))
      return false;

    // Parse the string for a number
    TCHAR *endptr;
    short tmp = _tcstol(str, &endptr, 0);
    if (endptr == str)
      return false;

    // Save parsed value to output parameter value and return success
    value = tmp;
    return true;
  }

  static inline bool Get(const TCHAR *key, bool &value)
  {
    // Try to read the profile map
    TCHAR str[5];
    if (!Get(key, str, 5))
      return false;

    // Save value to output parameter value and return success
    value = (str[0] != '0');
    return true;
  }

  static inline bool Get(const TCHAR *key, unsigned &value)
  {
    // Try to read the profile map
    TCHAR str[50];
    if (!Get(key, str, 50))
      return false;

    // Parse the string for a unsigned number
    TCHAR *endptr;
    unsigned tmp = _tcstoul(str, &endptr, 0);
    if (endptr == str)
      return false;

    // Save parsed value to output parameter value and return success
    value = tmp;
    return true;
  }

  static inline bool Get(const TCHAR *key, fixed &value)
  {
    // Try to read the profile map
    TCHAR str[50];
    if (!Get(key, str, 50))
      return false;

    // Parse the string for a floating point number
    TCHAR *endptr;
    double tmp = _tcstod(str, &endptr);
    if (endptr == str)
      return false;

    // Save parsed value to output parameter value and return success
    value = fixed(tmp);
    return true;
  }

  static inline bool Set(const TCHAR *key, bool value)
  {
    return Set(key, value ? _T("1") : _T("0"));
  }

  static inline bool Set(const TCHAR *key, int value)
  {
    TCHAR tmp[50];
    _sntprintf(tmp, 50, _T("%d"), value);
    return Set(key, tmp);
  }

  static inline bool Set(const TCHAR *key, long value)
  {
    TCHAR tmp[50];
    _sntprintf(tmp, 50, _T("%ld"), value);
    return Set(key, tmp);
  }

  static inline bool Set(const TCHAR *key, unsigned value)
  {
    TCHAR tmp[50];
    _sntprintf(tmp, 50, _T("%u"), value);
    return Set(key, tmp);
  }

  static inline bool Set(const TCHAR *key, fixed value)
  {
    TCHAR tmp[50];
    _sntprintf(tmp, 50, _T("%f"), (double)value);
    return Set(key, tmp);
  }

  void Export(ProfileWriter &writer);
}

#endif
