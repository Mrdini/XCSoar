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
#include <windef.h>
#include "Screen/Graphics.hpp"
#include "Screen/Point.hpp"
#include "Screen/UnitSymbol.hpp"
#include "Screen/Layout.hpp"
#include "Screen/Ramp.hpp"
#include "Screen/Bitmap.hpp"
#include "Screen/Icon.hpp"
#include "Screen/Brush.hpp"
#include "Screen/Color.hpp"
#include "Screen/Pen.hpp"
#include "Screen/Canvas.hpp"
#include "Math/Screen.hpp"
#include "Appearance.hpp"
#include "SettingsMap.hpp"
#include "resource.h"
#include "Asset.hpp"
#include "LogFile.hpp"

Pen Graphics::hAirspacePassivePen;
Pen Graphics::hAirspacePens[AIRSPACECLASSCOUNT];

#ifndef ENABLE_SDL
Brush Graphics::hAirspaceBrushes[NUMAIRSPACEBRUSHES];
Bitmap Graphics::hAirspaceBitmap[NUMAIRSPACEBRUSHES];
#endif

#ifdef HAVE_ALPHA_BLEND
Brush Graphics::solid_airspace_brushes[NUMAIRSPACECOLORS];
#endif

Pen Graphics::hpSnail[NUMSNAILCOLORS];
Pen Graphics::hpSnailVario[NUMSNAILCOLORS];

#ifndef ENABLE_SDL
Bitmap Graphics::hAboveTerrainBitmap;
Brush Graphics::hAboveTerrainBrush;
#endif

MaskedIcon Graphics::hAirspaceInterceptBitmap;
MaskedIcon Graphics::hTerrainWarning;
MaskedIcon Graphics::hFLARMTraffic;
MaskedIcon Graphics::hLogger, Graphics::hLoggerOff;
MaskedIcon Graphics::hCruise, Graphics::hClimb,
           Graphics::hFinalGlide, Graphics::hAbort;
MaskedIcon Graphics::hGPSStatus1, Graphics::hGPSStatus2;

Pen Graphics::hpAircraft;
Pen Graphics::hpAircraftBorder;
Pen Graphics::hpWind;
Pen Graphics::hpBearing;
Pen Graphics::hpBestCruiseTrack;
Pen Graphics::hpCompass;
Pen Graphics::hpThermalBand;
Pen Graphics::hpThermalBandGlider;
Pen Graphics::hpFinalGlideAbove;
Pen Graphics::hpFinalGlideBelow;
Pen Graphics::hpFinalGlideBelowLandable;
Pen Graphics::hpMapScale;
Pen Graphics::hpTerrainLine;
Pen Graphics::hpTerrainLineBg;
Pen Graphics::hpTrackBearingLine;

Brush Graphics::hbCompass;
Brush Graphics::hbThermalBand;
Brush Graphics::hbBestCruiseTrack;
Brush Graphics::hbFinalGlideBelow;
Brush Graphics::hbFinalGlideBelowLandable;
Brush Graphics::hbFinalGlideAbove;
Brush Graphics::hbWind;

MaskedIcon Graphics::SmallIcon, Graphics::TurnPointIcon;
MaskedIcon Graphics::MountainTopIcon, Graphics::BridgeIcon, Graphics::TunnelIcon;
MaskedIcon Graphics::TowerIcon, Graphics::PowerPlantIcon;
MaskedIcon Graphics::AirportReachableIcon, Graphics::AirportUnreachableIcon;
MaskedIcon Graphics::FieldReachableIcon, Graphics::FieldUnreachableIcon;
MaskedIcon Graphics::hBmpThermalSource;
MaskedIcon Graphics::hBmpTarget;
MaskedIcon Graphics::hBmpTeammatePosition;

MaskedIcon Graphics::hBmpMapScaleLeft;
MaskedIcon Graphics::hBmpMapScaleRight;

Bitmap Graphics::hBmpTabTask;
Bitmap Graphics::hBmpTabFolder;
Bitmap Graphics::hBmpTabSettings;
Bitmap Graphics::hBmpTabCalculator;

// used for flarm
Brush Graphics::AlarmBrush;
Brush Graphics::WarningBrush;
Brush Graphics::TrafficBrush;

// used for landable rendering
Brush Graphics::hbGreen;
Brush Graphics::hbWhite;
Brush Graphics::hbMagenta;
Brush Graphics::hbOrange;
Brush Graphics::hbLightGray;
Brush Graphics::hbNotReachableTerrain;

// airspace brushes/colours
const Color
Graphics::GetAirspaceColour(const int i)
{
  return Colours[i];
}

#ifndef ENABLE_SDL
const Brush &
Graphics::GetAirspaceBrush(const int i)
{
  return hAirspaceBrushes[i];
}
#endif

const Color
Graphics::GetAirspaceColourByClass(const int i, const SETTINGS_MAP &settings)
{
  return GetAirspaceColour(settings.iAirspaceColour[i]);
}

#ifndef ENABLE_SDL
const Brush &
Graphics::GetAirspaceBrushByClass(const int i, const SETTINGS_MAP &settings)
{
  return GetAirspaceBrush(settings.iAirspaceBrush[i]);
}
#endif

const Color Graphics::inv_redColor = Color(0xff, 0x70, 0x70);
const Color Graphics::inv_blueColor = Color(0x90, 0x90, 0xff);
const Color Graphics::inv_yellowColor = Color::YELLOW;
const Color Graphics::inv_greenColor = Color::GREEN;
const Color Graphics::inv_magentaColor = Color::MAGENTA;

const Color Graphics::TaskColor = Color(0, 120, 0);

const Color Graphics::Colours[] = {
  Color::RED,
  Color::GREEN,
  Color::BLUE,
  Color::YELLOW,
  Color::MAGENTA,
  Color::CYAN,
  Color(0x7F, 0x00, 0x00),
  Color(0x00, 0x7F, 0x00),
  Color(0x00, 0x00, 0x7F),
  Color(0x7F, 0x7F, 0x00),
  Color(0x7F, 0x00, 0x7F),
  Color(0x00, 0x7F, 0x7F),
  Color::WHITE,
  Color(0xC0, 0xC0, 0xC0),
  Color(0x7F, 0x7F, 0x7F),
  Color::BLACK,
};

void
Graphics::Initialise()
{
  /// @todo enhancement: support red/green color blind pilots with adjusted colour scheme

  LogStartUp(_T("Initialise graphics"));

  LoadUnitSymbols();

  AlarmBrush.set(Color::RED);
  WarningBrush.set(Color(0xFF, 0xA2, 0x00));
  TrafficBrush.set(Color::GREEN);

  hFLARMTraffic.load_big(IDB_FLARMTRAFFIC, IDB_FLARMTRAFFIC_HD);
  hTerrainWarning.load_big(IDB_TERRAINWARNING, IDB_TERRAINWARNING_HD);
  hGPSStatus1.load_big(IDB_GPSSTATUS1, IDB_GPSSTATUS1_HD, false);
  hGPSStatus2.load_big(IDB_GPSSTATUS2, IDB_GPSSTATUS2_HD, false);
  hLogger.load_big(IDB_LOGGER, IDB_LOGGER_HD);
  hLoggerOff.load_big(IDB_LOGGEROFF, IDB_LOGGEROFF_HD);
  hBmpTeammatePosition.load_big(IDB_TEAMMATE_POS, IDB_TEAMMATE_POS_HD);

  hCruise.load_big(IDB_CRUISE, IDB_CRUISE_HD, false);
  hClimb.load_big(IDB_CLIMB, IDB_CLIMB_HD, false);
  hFinalGlide.load_big(IDB_FINALGLIDE, IDB_FINALGLIDE_HD, false);
  hAbort.load_big(IDB_ABORT, IDB_ABORT_HD, false);

  // airspace brushes and colors
#ifndef ENABLE_SDL
  hAirspaceBitmap[0].load(IDB_AIRSPACE0);
  hAirspaceBitmap[1].load(IDB_AIRSPACE1);
  hAirspaceBitmap[2].load(IDB_AIRSPACE2);
  hAirspaceBitmap[3].load(IDB_AIRSPACE3);
  hAirspaceBitmap[4].load(IDB_AIRSPACE4);
  hAirspaceBitmap[5].load(IDB_AIRSPACE5);
  hAirspaceBitmap[6].load(IDB_AIRSPACE6);
  hAirspaceBitmap[7].load(IDB_AIRSPACE7);
#endif

  hAirspaceInterceptBitmap.load_big(IDB_AIRSPACEI, IDB_AIRSPACEI_HD);

#ifndef ENABLE_SDL
  hAboveTerrainBitmap.load(IDB_ABOVETERRAIN);

  for (int i = 0; i < NUMAIRSPACEBRUSHES; i++)
    hAirspaceBrushes[i].set(hAirspaceBitmap[i]);

  hAboveTerrainBrush.set(hAboveTerrainBitmap);
#endif

#ifdef HAVE_ALPHA_BLEND
  if (AlphaBlendAvailable())
    for (unsigned i = 0; i < NUMAIRSPACECOLORS; ++i)
      solid_airspace_brushes[i].set(Colours[i]);
#endif

  hbWind.set(Color::GRAY);

  hBmpMapScaleLeft.load_big(IDB_MAPSCALE_LEFT, 0, false);
  hBmpMapScaleRight.load_big(IDB_MAPSCALE_RIGHT, 0, false);

  hBmpTabTask.load((Layout::scale > 1) ? IDB_TASK_HD : IDB_TASK);
  hBmpTabFolder.load((Layout::scale > 1) ? IDB_FOLDER_HD : IDB_FOLDER);
  hBmpTabSettings.load((Layout::scale > 1) ? IDB_SETTINGS_HD : IDB_SETTINGS);
  hBmpTabCalculator.load((Layout::scale > 1) ? IDB_CALCULATOR_HD : IDB_CALCULATOR);

  hBmpThermalSource.load_big(IDB_THERMALSOURCE, IDB_THERMALSOURCE_HD);
  hBmpTarget.load_big(IDB_TARGET, IDB_TARGET_HD);

  hbCompass.set(Color(207, 207, 207));

  hbThermalBand.set(Color(0x80, 0x80, 0xFF));
  hbBestCruiseTrack.set(Color::BLUE);
  hbFinalGlideBelow.set(Color::RED);
  hbFinalGlideBelowLandable.set(Color(0xFF, 180, 0x00));
  hbFinalGlideAbove.set(Color::GREEN);

  hpWind.set(Layout::Scale(2), Color::BLACK);

  hpBearing.set(Layout::Scale(2), Color::BLACK);
  hpBestCruiseTrack.set(Layout::Scale(1), Color::BLUE);
  hpCompass.set(Layout::Scale(1), Color(109, 109, 109));
  hpThermalBand.set(Layout::Scale(2), Color(0x40, 0x40, 0xFF));
  hpThermalBandGlider.set(Layout::Scale(2), Color(0x00, 0x00, 0x30));

  hpFinalGlideBelow.set(Layout::Scale(1), Color(0xFF, 0xA0, 0xA0));
  hpFinalGlideBelowLandable.set(Layout::Scale(1), Color(255, 196, 0));

  hpFinalGlideAbove.set(Layout::Scale(1), Color(0xA0, 0xFF, 0xA0));

  hpMapScale.set(Layout::Scale(1), Color::BLACK);
  hpTerrainLine.set(Pen::DASH, Layout::Scale(2), Color(0x30, 0x30, 0x30));
  hpTerrainLineBg.set(Layout::Scale(2), Color::WHITE);

  SmallIcon.load_big(IDB_SMALL, IDB_SMALL_HD);
  TurnPointIcon.load_big(IDB_TURNPOINT, IDB_TURNPOINT_HD);
  MountainTopIcon.load_big(IDB_MOUNTAIN_TOP, IDB_MOUNTAIN_TOP_HD);
  BridgeIcon.load_big(IDB_BRIDGE, IDB_BRIDGE_HD);
  TunnelIcon.load_big(IDB_TUNNEL, IDB_TUNNEL_HD);
  TowerIcon.load_big(IDB_TOWER, IDB_TOWER_HD);
  PowerPlantIcon.load_big(IDB_POWER_PLANT, IDB_POWER_PLANT_HD);

  hpAircraft.set(Layout::Scale(3), Color::WHITE);
  hpAircraftBorder.set(Layout::Scale(1), Color::BLACK);

    // used for landable rendering
  hbGreen.set(Color::GREEN);
  hbWhite.set(Color::WHITE);
  hbMagenta.set(Color::MAGENTA);
  hbOrange.set(Color::ORANGE);
  hbLightGray.set(Color::LIGHT_GRAY);
  hbNotReachableTerrain.set(Color(224, 64, 64));

  hpTrackBearingLine.set(3, Color::GRAY);
}

void
Graphics::InitialiseConfigured(const SETTINGS_MAP &settings_map)
{
  InitSnailTrail(settings_map);
  InitLandableIcons();
  InitAirspacePens(settings_map);
}

void
Graphics::InitSnailTrail(const SETTINGS_MAP &settings_map)
{
  const ColorRamp snail_colors_vario[] = {
    {0,   0xff, 0x3e, 0x00},
    {100, 0x8f, 0x8f, 0x8f},
    {200, 0x00, 0xff, 0x3e}
  };

  const ColorRamp snail_colors_vario2[] = {
    {0,   0x00, 0x00, 0xff},
    {99,  0x00, 0xff, 0xff},
    {100, 0xff, 0xff, 0x00},
    {200, 0xff, 0x00, 0x00}
  };

  const ColorRamp snail_colors_alt[] = {
    {0,   0xff, 0x00, 0x00},
    {50,  0xff, 0xff, 0x00},
    {100, 0x00, 0xff, 0x00},
    {150, 0x00, 0xff, 0xff},
    {200, 0x00, 0x00, 0xff},
  };

  int iwidth;
  int minwidth = Layout::Scale(2);

  for (int i = 0; i < NUMSNAILCOLORS; i++) {
    short ih = i * 200 / (NUMSNAILCOLORS - 1);
    Color color = (settings_map.SnailType == stAltitude) ?
                  ColorRampLookup(ih, snail_colors_alt, 5) :
                  (settings_map.SnailType == stSeeYouVario) ?
                  ColorRampLookup(ih, snail_colors_vario2, 4) :
                  ColorRampLookup(ih, snail_colors_vario, 3);

    if (i < NUMSNAILCOLORS / 2 ||
        !settings_map.SnailScaling)
      iwidth = minwidth;
    else
      iwidth = max(minwidth, (i - NUMSNAILCOLORS / 2) *
                             Layout::Scale(16) / NUMSNAILCOLORS);

    hpSnail[i].set(minwidth, color);
    hpSnailVario[i].set(iwidth, color);
  }
}

void
Graphics::InitLandableIcons()
{
  if (Appearance.IndLandable == wpLandableWinPilot) {
    AirportReachableIcon.load_big(IDB_REACHABLE, IDB_REACHABLE_HD);
    AirportUnreachableIcon.load_big(IDB_LANDABLE, IDB_LANDABLE_HD);
    FieldReachableIcon.load_big(IDB_REACHABLE, IDB_REACHABLE_HD);
    FieldUnreachableIcon.load_big(IDB_LANDABLE, IDB_LANDABLE_HD);
  } else if (Appearance.IndLandable == wpLandableAltA) {
    AirportReachableIcon.load_big(IDB_AIRPORT_REACHABLE,
                                  IDB_AIRPORT_REACHABLE_HD);
    AirportUnreachableIcon.load_big(IDB_AIRPORT_UNREACHABLE,
                                    IDB_AIRPORT_UNREACHABLE_HD);
    FieldReachableIcon.load_big(IDB_OUTFIELD_REACHABLE,
                                IDB_OUTFIELD_REACHABLE_HD);
    FieldUnreachableIcon.load_big(IDB_OUTFIELD_UNREACHABLE,
                                  IDB_OUTFIELD_UNREACHABLE_HD);
  } else if (Appearance.IndLandable == wpLandableAltB) {
    AirportReachableIcon.load_big(IDB_AIRPORT_REACHABLE,
                                  IDB_AIRPORT_REACHABLE_HD);
    AirportUnreachableIcon.load_big(IDB_AIRPORT_UNREACHABLE2,
                                    IDB_AIRPORT_UNREACHABLE2_HD);
    FieldReachableIcon.load_big(IDB_OUTFIELD_REACHABLE,
                                IDB_OUTFIELD_REACHABLE_HD);
    FieldUnreachableIcon.load_big(IDB_OUTFIELD_UNREACHABLE2,
                                  IDB_OUTFIELD_UNREACHABLE2_HD);
  }
}

void
Graphics::InitAirspacePens(const SETTINGS_MAP &settings_map)
{
  hAirspacePassivePen.set(Layout::Scale(2), Colours[14]);
  for (int i = 0; i < AIRSPACECLASSCOUNT; i++)
    hAirspacePens[i].set(Layout::Scale(2),
                         GetAirspaceColourByClass(i, settings_map));
}

void
Graphics::Deinitialise()
{
  DeinitialiseUnitSymbols();

  AlarmBrush.reset();
  WarningBrush.reset();
  TrafficBrush.reset();

  hAirspaceInterceptBitmap.reset();
  hTerrainWarning.reset();
  hFLARMTraffic.reset();
  hLogger.reset();
  hLoggerOff.reset();
  hCruise.reset();
  hClimb.reset();
  hFinalGlide.reset();
  hAbort.reset();
  hGPSStatus1.reset();
  hGPSStatus2.reset();

#ifndef ENABLE_SDL
  for (unsigned i = 0; i < NUMAIRSPACEBRUSHES; i++) {
    hAirspaceBrushes[i].reset();
    hAirspaceBitmap[i].reset();
  }

  hAboveTerrainBrush.reset();
  hAboveTerrainBitmap.reset();
#endif

#ifdef HAVE_ALPHA_BLEND
  if (AlphaBlendAvailable())
    for (unsigned i = 0; i < NUMAIRSPACECOLORS; ++i)
      solid_airspace_brushes[i].reset();
#endif

  hAirspacePassivePen.reset();
  for (unsigned i = 0; i < AIRSPACECLASSCOUNT; i++)
    hAirspacePens[i].reset();

  SmallIcon.reset();
  TurnPointIcon.reset();
  MountainTopIcon.reset();
  BridgeIcon.reset();
  TunnelIcon.reset();
  TowerIcon.reset();
  PowerPlantIcon.reset();
  AirportReachableIcon.reset();
  AirportUnreachableIcon.reset();
  FieldReachableIcon.reset();
  FieldUnreachableIcon.reset();
  hBmpThermalSource.reset();
  hBmpTarget.reset();
  hBmpTeammatePosition.reset();

  hBmpMapScaleLeft.reset();
  hBmpMapScaleRight.reset();

  hBmpTabTask.reset();
  hBmpTabFolder.reset();
  hBmpTabSettings.reset();
  hBmpTabCalculator.reset();

  hbGreen.reset();
  hbWhite.reset();
  hbMagenta.reset();
  hbOrange.reset();
  hbLightGray.reset();
  hbNotReachableTerrain.reset();

  hpTrackBearingLine.reset();

  for (unsigned i = 0; i < NUMSNAILCOLORS; i++) {
    hpSnail[i].reset();
    hpSnailVario[i].reset();
  }
}

void
Graphics::DrawAircraft(Canvas &canvas, 
                       const Angle angle,
                       const RasterPoint aircraft_pos)
{
  RasterPoint Aircraft[] = {
    {1, -5},
    {1, 0},
    {14, 0},
    {14, 1},
    {1, 1},
    {1, 8},
    {4, 8},
    {4, 9},
    {-3, 9},
    {-3, 8},
    {0, 8},
    {0, 1},
    {-13, 1},
    {-13, 0},
    {0, 0},
    {0, -5},
  };

  int n = sizeof(Aircraft) / sizeof(Aircraft[0]);

  PolygonRotateShift(Aircraft, n, aircraft_pos.x - 1, aircraft_pos.y, angle);

  canvas.select(Graphics::hpAircraft);
  canvas.polygon(Aircraft, n);

  canvas.black_brush();

  canvas.select(Graphics::hpAircraftBorder);
  canvas.polygon(Aircraft, n);
}
