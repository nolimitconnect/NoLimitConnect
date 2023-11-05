/*
 *  Copyright (C) 2017-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "DiscSettings.h"

#include "Settings.h"
#include "dialogs/GUIDialogKaiToast.h"
#include "lib/Setting.h"
#include "messaging/helpers/DialogOKHelper.h"
#include "utils/Variant.h"
#include "utils/log.h"

#include <string>

#include "config_components_kodi.h"
#if ENABLE_BLURAY
#include <libbluray/src/libbluray/bluray-version.h>
#endif // ENABLE_BLURAY

using namespace KODI::MESSAGING;

CDiscSettings& CDiscSettings::GetInstance()
{
  static CDiscSettings sDiscSettings;
  return sDiscSettings;
}

void CDiscSettings::OnSettingChanged(const std::shared_ptr<const CSetting>& setting)
{
#if ENABLE_BLURAY
#if HAS_OPTICAL_DRIVE
#if (BLURAY_VERSION >= BLURAY_VERSION_CODE(1,0,1))
  if (setting == NULL)
    return;

  const std::string &settingId = setting->GetId();

  if (settingId == CSettings::SETTING_DISC_PLAYBACK)
  {
    int mode = std::static_pointer_cast<const CSettingInt>(setting)->GetValue();
    if (mode == BD_PLAYBACK_DISC_MENU)
    {
      bool bdjWorking = false;
      BLURAY* bd = bd_init();
      const BLURAY_DISC_INFO* info = bd_get_disc_info(bd);

      if (!info->libjvm_detected)
        CLog::Log(LOGDEBUG, "DiscSettings - Could not load the java vm.");
      else if (!info->bdj_handled)
        CLog::Log(LOGDEBUG, "DiscSettings - Could not load the libbluray.jar.");
      else
        bdjWorking = true;

      bd_close(bd);

      if (!bdjWorking)
        HELPERS::ShowOKDialogText(CVariant{ 29803 }, CVariant{ 29804 });
    }
  }
#endif
#endif // HAS_OPTICAL_DRIVE
#endif // ENABLE_BLURAY
}
