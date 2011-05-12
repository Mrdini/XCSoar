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

#include "OS/Clock.hpp"

#if defined(HAVE_POSIX) && !defined(__CYGWIN__)
#include <time.h>
#else /* !HAVE_POSIX */
#include <windows.h>
#endif /* !HAVE_POSIX */
#if defined(HAVE_OSX)
#include <mach/mach_time.h>  

void mach_absolute_difference(uint64_t end, uint64_t start, struct timespec *tp) {  
        uint64_t difference = end - start;  
        static mach_timebase_info_data_t info = {0,0};  
  
        if (info.denom == 0)  
                mach_timebase_info(&info);  
  
        uint64_t elapsednano = difference * (info.numer / info.denom);  
  
        tp->tv_sec = elapsednano * 1e-9;  
        tp->tv_nsec = elapsednano - (tp->tv_sec * 1e9);  
}  
#endif

unsigned
MonotonicClockMS()
{
#if defined(HAVE_POSIX) && !defined(__CYGWIN__) && !defined(HAVE_OSX)
  struct timespec ts;
 clock_gettime(CLOCK_MONOTONIC, &ts);
#elif defined(HAVE_OSX)
  struct timespec ts;
  int end = mach_absolute_time();  
  mach_absolute_difference(end, 0, &ts);
#endif
#if defined(HAVE_POSIX)
return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#else /* !HAVE_POSIX */
  return ::GetTickCount();
#endif /* !HAVE_POSIX */
}
