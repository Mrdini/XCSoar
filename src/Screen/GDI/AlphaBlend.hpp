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

#ifndef XCSOAR_SCREEN_GDI_ALPHA_BLEND_HPP
#define XCSOAR_SCREEN_GDI_ALPHA_BLEND_HPP

#include <windows.h>

#ifdef _WIN32_WCE /* embedded Windows? */

#if _WIN32_WCE >= 0x500
#define HAVE_ALPHA_BLEND
#define HAVE_DYNAMIC_ALPHA_BLEND

typedef BOOL (*AlphaBlend_t)(HDC hdcDest,
                             int xoriginDest, int yoriginDest,
                             int wDest, int hDest,
                             HDC hdcSrc,
                             int xoriginSrc, int yoriginSrc,
                             int wSrc, int hSrc,
                             BLENDFUNCTION ftn);

extern AlphaBlend_t AlphaBlend;

void
AlphaBlendInit();

void
AlphaBlendDeinit();

static inline bool
AlphaBlendAvailable()
{
  return AlphaBlend != NULL;
}

#endif /* WM5 */

#elif defined(_WIN32_WINDOWS) /* desktop Windows? */

#if _WIN32_WINDOWS >= 0x500
/* need Windows 2000 for alpha blending */
#define HAVE_ALPHA_BLEND
#define HAVE_BUILTIN_ALPHA_BLEND

static inline bool
AlphaBlendAvailable()
{
  return true;
}

#endif

#endif

#endif