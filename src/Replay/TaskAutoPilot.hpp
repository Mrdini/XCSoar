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
#ifndef TASK_AUTOPILOT_HPP
#define TASK_AUTOPILOT_HPP

#include "Math/fixed.hpp"
#include "Navigation/Aircraft.hpp"
#include "Math/Angle.hpp"
#include "Util/Filter.hpp"
#include <vector>

struct AutopilotParameters {
  AutopilotParameters():
    target_noise(0.1),
    sink_factor(1.0),
    climb_factor(1.0),
    start_alt(1500.0),
    enable_bestcruisetrack(false),
    goto_target(false)
    {
      realistic();
    };

  fixed bearing_noise;
  fixed target_noise;
  fixed turn_speed;
  fixed sink_factor;
  fixed climb_factor;
  fixed start_alt;
  bool enable_bestcruisetrack;
  bool goto_target;

  void ideal();
  void realistic();
};

class AbstractAutoPilot {
public:
  AbstractAutoPilot() {};

  void set_default_location(const GeoPoint& default_location) {
    location_start = default_location;
    location_previous = default_location;
    location_previous.Latitude-= Angle::degrees(fixed(1.0));
  }

  GeoPoint location_start;
  GeoPoint location_previous;
  Angle heading;

protected:
  virtual void on_manual_advance() {};
  virtual void on_mode_change() {};
  virtual void on_close() {};
};

class GlidePolar;
class ElementStat;

class TaskAccessor {
public:
  virtual bool is_ordered() const = 0;
  virtual bool is_empty() const = 0;
  virtual bool is_finished() const = 0;
  virtual bool is_started() const = 0;
  virtual GeoPoint random_oz_point(unsigned index, const fixed noise) const = 0;
  virtual unsigned size() const = 0;
  virtual GeoPoint getActiveTaskPointLocation() const = 0;
  virtual bool has_entered(unsigned index) const = 0;
  virtual const ElementStat leg_stats() const = 0;
  virtual fixed target_height() const = 0;
  virtual fixed distance_to_final() const = 0;
  virtual fixed remaining_alt_difference() const = 0;
  virtual GlidePolar get_glide_polar() const =0;
  virtual void setActiveTaskPoint(unsigned index) = 0;
  virtual unsigned getActiveTaskPointIndex() const = 0;
};


class TaskAutoPilot: public AbstractAutoPilot {
public:
  enum AcState {
    Climb = 0,
    Cruise,
    FinalGlide
  };

  TaskAutoPilot(const AutopilotParameters &_parms);

  virtual void Start(const TaskAccessor& task);
  virtual void Stop();
  virtual void update_mode(const TaskAccessor& task,
                           const AIRCRAFT_STATE& state);

  virtual void update_state(const TaskAccessor& task,
                            AIRCRAFT_STATE& state, const fixed timestep=fixed_one);

  bool update_autopilot(TaskAccessor& task,
                        const AIRCRAFT_STATE& state,
                        const AIRCRAFT_STATE& state_last);

  GeoPoint target(const TaskAccessor& task) const;
  void set_speed_factor(fixed f) {
    speed_factor = f;
  }

protected:
  AcState acstate;
  unsigned awp;

private:
  const AutopilotParameters &parms;
  Filter heading_filt;
  fixed climb_rate;
  fixed speed_factor;
  bool short_flight;
  GeoPoint w[2];

  bool do_advance(TaskAccessor& task);
  void advance_if_required(TaskAccessor& task);
  bool has_finished(TaskAccessor& task);
  void get_awp(TaskAccessor& task);

  bool current_has_target(const TaskAccessor& task) const;

  virtual GeoPoint get_start_location(const TaskAccessor& task,
                                      bool previous=false);
  void update_cruise_bearing(const TaskAccessor& task,
                             const AIRCRAFT_STATE& state,
                             const fixed timestep);
  fixed target_height(const TaskAccessor& task) const;
  Angle heading_deviation();
  bool update_computer(TaskAccessor& task, const AIRCRAFT_STATE& state);
  bool far_from_target(const TaskAccessor& task,
                       const AIRCRAFT_STATE& state);
};

#endif

