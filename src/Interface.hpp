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

#ifndef XCSOAR_INTERFACE_HPP
#define XCSOAR_INTERFACE_HPP

#include "InterfaceBlackboard.hpp"
#include "Compiler.h"

#include <windef.h> /* for HINSTANCE */

class MainWindow;
class StatusMessageList;

/** 
 * Class to hold data/methods accessible by all interface subsystems
 */
class CommonInterface {
  static InterfaceBlackboard blackboard;

protected:
  /**
   * True if movement was detected on a real GPS.
   */
  static bool movement_detected;

public:
  // window.. make this protected TODO so have to subclass to get access
  static StatusMessageList status_messages;
  static MainWindow main_window;

  static bool MovementDetected() {
    return movement_detected;
  }

  // TODO: make this protected
  /**
   * Returns InterfaceBlackboard.Basic (NMEA_INFO) (read-only)
   * @return InterfaceBlackboard.Basic
   */
  gcc_const
  static const NMEA_INFO& Basic() { return blackboard.Basic(); }

  /**
   * Returns InterfaceBlackboard.Calculated (DERIVED_INFO) (read-only)
   * @return InterfaceBlackboard.Calculated
   */
  gcc_const
  static const DERIVED_INFO& Calculated() { return blackboard.Calculated(); }

  /**
   * Returns the InterfaceBlackboard.SettingsComputer (read-only)
   * @return The InterfaceBlackboard.SettingsComputer
   */
  gcc_const
  static const SETTINGS_COMPUTER& SettingsComputer()
  { return blackboard.SettingsComputer(); }

  /**
   * Returns the InterfaceBlackboard.SettingsComputer (read-write)
   * @return The InterfaceBlackboard.SettingsComputer
   */
  gcc_const
  static SETTINGS_COMPUTER& SetSettingsComputer()
  { return blackboard.SetSettingsComputer(); }

  /**
   * Returns the InterfaceBlackboard.SettingsMap (read-only)
   * @return The InterfaceBlackboard.SettingsMap
   */
  gcc_const
  static const SETTINGS_MAP& SettingsMap()
  { return blackboard.SettingsMap(); }

  /**
   * Returns the InterfaceBlackboard.SettingsMap (read-write)
   * @return The InterfaceBlackboard.SettingsMap
   */
  gcc_const
  static SETTINGS_MAP& SetSettingsMap()
  { return blackboard.SetSettingsMap(); }

  static void ReadBlackboardBasic(const NMEA_INFO& nmea_info) {
    blackboard.ReadBlackboardBasic(nmea_info);
  }
  static void ReadBlackboardCalculated(const DERIVED_INFO& derived_info) {
    blackboard.ReadBlackboardCalculated(derived_info);
  }
};

/** 
 * Class to hold data/methods accessible by interface subsystems
 * that can perform actions
 */
class ActionInterface: public CommonInterface {
  friend class ProcessTimer;

public:
  /** timeout in quarter seconds of menu button */
  static unsigned MenuTimeoutMax;

protected:
  static void DisplayModes();
  static void SendSettingsComputer();

  static bool doForceShutdown;

public:
  /**
   * Call this after SETTINGS_MAP has been modified with
   * SetSettingsMap().  It sends the new values to all sub systems,
   * and optionally forces a redraw.
   */
  static void SendSettingsMap(const bool trigger_draw = false);

public:
  // ideally these should be protected
  static void SignalShutdown(bool force);
};

/** 
 * Class to hold data/methods accessible by interface subsystems
 * of main program
 */
class XCSoarInterface: public ActionInterface {
public:
  // settings
  static unsigned debounceTimeout;

public:
  static bool Debounce();

  static bool CheckShutdown();

  static void AfterStartup();
  static void Shutdown();
  static bool Startup(HINSTANCE);

  static void ExchangeBlackboard();

  /**
   * Copy data from and to the DeviceBlackboard.
   */
  static void ExchangeDeviceBlackboard();

private:
  static bool LoadProfile();
};

#endif
