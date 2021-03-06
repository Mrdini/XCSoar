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

#include "DataField/Enum.hpp"
#include "DataField/ComboList.hpp"
#include "Form/Edit.hpp"
#include "Form/Util.hpp"
#include "Form/Frame.hpp"
#include "Dialogs/Dialogs.h"
#include "Profile/ProfileKeys.hpp"
#include "Profile/Profile.hpp"
#include "Interface.hpp"
#include "AirspaceConfigPanel.hpp"
#include "Language/Language.hpp"

static WndForm* wf = NULL;


void
AirspaceConfigPanel::OnAirspaceColoursClicked(gcc_unused WndButton &button)
{
  dlgAirspaceShowModal(true);
}


void
AirspaceConfigPanel::OnAirspaceModeClicked(gcc_unused WndButton &button)
{
  dlgAirspaceShowModal(false);
}


void
AirspaceConfigPanel::Init(WndForm *_wf)
{
  assert(_wf != NULL);
  wf = _wf;
  WndProperty *wp;

  const AirspaceComputerSettings &computer =
    CommonInterface::SettingsComputer().airspace;
  const AirspaceRendererSettings &renderer =
    CommonInterface::SettingsMap().airspace;

  wp = (WndProperty*)wf->FindByName(_T("prpAirspaceDisplay"));
  if (wp) {
    DataFieldEnum* dfe;
    dfe = (DataFieldEnum*)wp->GetDataField();
    dfe->addEnumText(_("All on"));
    dfe->addEnumText(_("Clip"));
    dfe->addEnumText(_("Auto"));
    dfe->addEnumText(_("All below"));
    dfe->Set(renderer.altitude_mode);
    wp->RefreshDisplay();
  }

  LoadFormProperty(*wf, _T("prpClipAltitude"), ugAltitude,
                   renderer.clip_altitude);

  LoadFormProperty(*wf, _T("prpAltWarningMargin"), ugAltitude,
                   computer.warnings.AltWarningMargin);

  LoadFormProperty(*wf, _T("prpAirspaceWarnings"), computer.enable_warnings);
  LoadFormProperty(*wf, _T("prpWarningTime"), computer.warnings.WarningTime);
  LoadFormProperty(*wf, _T("prpAcknowledgementTime"),
                   computer.warnings.AcknowledgementTime);

  LoadFormProperty(*wf, _T("prpAirspaceOutline"), renderer.black_outline);

  wp = (WndProperty *)wf->FindByName(_T("prpAirspaceFillMode"));
  {
#ifdef ENABLE_OPENGL
    wp->hide();
    wf->RemoveExpert(wp);  // prevent unhiding with expert-switch
#else
    DataFieldEnum &dfe = *(DataFieldEnum *)wp->GetDataField();
    dfe.addEnumText(_("Default"), AirspaceRendererSettings::AS_FILL_DEFAULT);
    dfe.addEnumText(_("Fill all"), AirspaceRendererSettings::AS_FILL_ALL);
    dfe.addEnumText(_("Fill padding"),
                    AirspaceRendererSettings::AS_FILL_PADDING);
    dfe.Set(renderer.fill_mode);
    wp->RefreshDisplay();
#endif
  }

#if !defined(ENABLE_OPENGL) && defined(HAVE_ALPHA_BLEND)
  if (AlphaBlendAvailable())
    LoadFormProperty(*wf, _T("prpAirspaceTransparency"),
                     renderer.transparency);
  else
#endif
  {
    wp = (WndProperty *)wf->FindByName(_T("prpAirspaceTransparency"));
    wp->hide();
    wf->RemoveExpert(wp);  // prevent unhiding with expert-switch
  }
}


bool
AirspaceConfigPanel::Save(bool &requirerestart)
{
  bool changed = false;

  AirspaceComputerSettings &computer =
    CommonInterface::SetSettingsComputer().airspace;
  AirspaceRendererSettings &renderer =
    CommonInterface::SetSettingsMap().airspace;

  short tmp = renderer.altitude_mode;
  changed |= SaveFormProperty(*wf, _T("prpAirspaceDisplay"),
                              szProfileAltMode, tmp);
  renderer.altitude_mode = (AirspaceDisplayMode_t)tmp;

  changed |= SaveFormProperty(*wf, _T("prpClipAltitude"), ugAltitude,
                              renderer.clip_altitude,
                              szProfileClipAlt);

  changed |= SaveFormProperty(*wf, _T("prpAltWarningMargin"),
                              ugAltitude, computer.warnings.AltWarningMargin,
                              szProfileAltMargin);

  changed |= SaveFormProperty(*wf, _T("prpAirspaceWarnings"),
                              szProfileAirspaceWarning,
                              computer.enable_warnings);

  if (SaveFormProperty(*wf, _T("prpWarningTime"),
                       szProfileWarningTime,
                       computer.warnings.WarningTime)) {
    changed = true;
    requirerestart = true;
  }

  if (SaveFormProperty(*wf, _T("prpAcknowledgementTime"),
                       szProfileAcknowledgementTime,
                       computer.warnings.AcknowledgementTime)) {
    changed = true;
    requirerestart = true;
  }

  changed |= SaveFormProperty(*wf, _T("prpAirspaceOutline"),
                              szProfileAirspaceBlackOutline,
                              renderer.black_outline);

#ifndef ENABLE_OPENGL
  tmp = renderer.fill_mode;
  changed |= SaveFormProperty(*wf, _T("prpAirspaceFillMode"),
                              szProfileAirspaceFillMode, tmp);
  renderer.fill_mode = (enum AirspaceRendererSettings::AirspaceFillMode)tmp;

#ifdef HAVE_ALPHA_BLEND
  if (AlphaBlendAvailable())
    changed |= SaveFormProperty(*wf, _T("prpAirspaceTransparency"),
                                szProfileAirspaceTransparency,
                                renderer.transparency);
#endif
#endif /* !OpenGL */

  return changed;
}
