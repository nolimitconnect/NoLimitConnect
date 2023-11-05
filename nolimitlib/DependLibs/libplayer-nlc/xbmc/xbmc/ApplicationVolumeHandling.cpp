/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "config_components_kodi.h"
#include "ApplicationVolumeHandling.h"

#include "ServiceBroker.h"
#include "ApplicationComponents.h"
#include "ApplicationPlayer.h"
#include "cores/AudioEngine/Interfaces/AE.h"
#include "dialogs/GUIDialogVolumeBar.h"
#include "guilib/GUIComponent.h"
#include "guilib/GUIWindowManager.h"
#include "interfaces/AnnouncementManager.h"
#include "peripherals/Peripherals.h"
#include "settings/Settings.h"
#include "settings/lib/Setting.h"
#include "utils/Variant.h"
#include "utils/XMLUtils.h"

#include <libtinyxml/tinyxml.h>

float CApplicationVolumeHandling::GetVolumePercent() const
{
  // converts the hardware volume to a percentage
  return m_volumeLevel * 100.0f;
}

float CApplicationVolumeHandling::GetVolumeRatio() const
{
  return m_volumeLevel;
}

void CApplicationVolumeHandling::SetHardwareVolume(float hardwareVolume)
{
#if !defined(ENABLE_NLC_PLAYER)
  m_volumeLevel = std::clamp(hardwareVolume, VOLUME_MINIMUM, VOLUME_MAXIMUM);

  IAE* ae = CServiceBroker::GetActiveAE();
  if (ae)
    ae->SetVolume(m_volumeLevel);

#endif // !defined(ENABLE_NLC_PLAYER)
}

void CApplicationVolumeHandling::VolumeChanged()
{
#if !defined(ENABLE_NLC_PLAYER)
  CVariant data(CVariant::VariantTypeObject);
  data["volume"] = static_cast<int>(std::lroundf(GetVolumePercent()));
  data["muted"] = m_muted;
  const auto announcementMgr = CServiceBroker::GetAnnouncementManager();
  announcementMgr->Announce(ANNOUNCEMENT::Application, "OnVolumeChanged", data);

  auto& components = CServiceBroker::GetAppComponents();
  const auto appPlayer = components.GetComponent<CApplicationPlayer>();
  // if player has volume control, set it.
  if (appPlayer)
  {
    appPlayer->SetVolume(m_volumeLevel);
    appPlayer->SetMute(m_muted);
  }

#endif // !defined(ENABLE_NLC_PLAYER)
}

void CApplicationVolumeHandling::ShowVolumeBar(const CAction* action)
{
#if !defined(ENABLE_NLC_PLAYER)
  const auto& wm = CServiceBroker::GetGUI()->GetWindowManager();
  auto* volumeBar = wm.GetWindow<CGUIDialogVolumeBar>(WINDOW_DIALOG_VOLUME_BAR);
  if (volumeBar != nullptr && volumeBar->IsVolumeBarEnabled())
  {
    volumeBar->Open();
    if (action)
      volumeBar->OnAction(*action);
  }

#endif // !defined(ENABLE_NLC_PLAYER)
}

bool CApplicationVolumeHandling::IsMuted() const
{
#if !defined(ENABLE_NLC_PLAYER)
  if (CServiceBroker::GetPeripherals().IsMuted())
    return true;
  IAE* ae = CServiceBroker::GetActiveAE();
  if (ae)
    return ae->IsMuted();
  return true;
#else
    return false;
#endif // !defined(ENABLE_NLC_PLAYER)
}

void CApplicationVolumeHandling::ToggleMute(void)
{
#if !defined(ENABLE_NLC_PLAYER)
  if (m_muted)
    UnMute();
  else
    Mute();

#endif // !defined(ENABLE_NLC_PLAYER)
}

void CApplicationVolumeHandling::SetMute(bool mute)
{
#if !defined(ENABLE_NLC_PLAYER)
  if (m_muted != mute)
  {
    ToggleMute();
    m_muted = mute;
  }

#endif // !defined(ENABLE_NLC_PLAYER)
}

void CApplicationVolumeHandling::Mute()
{
#if !defined(ENABLE_NLC_PLAYER)
  if (CServiceBroker::GetPeripherals().Mute())
    return;

  IAE* ae = CServiceBroker::GetActiveAE();
  if (ae)
    ae->SetMute(true);
  m_muted = true;
  VolumeChanged();

#endif // !defined(ENABLE_NLC_PLAYER)
}

void CApplicationVolumeHandling::UnMute()
{
#if !defined(ENABLE_NLC_PLAYER)
  if (CServiceBroker::GetPeripherals().UnMute())
    return;

  IAE* ae = CServiceBroker::GetActiveAE();
  if (ae)
    ae->SetMute(false);
  m_muted = false;
  VolumeChanged();

#endif // !defined(ENABLE_NLC_PLAYER)
}

void CApplicationVolumeHandling::SetVolume(float iValue, bool isPercentage)
{
#if !defined(ENABLE_NLC_PLAYER)
  float hardwareVolume = iValue;

  if (isPercentage)
    hardwareVolume /= 100.0f;

  SetHardwareVolume(hardwareVolume);
  VolumeChanged();

#endif // !defined(ENABLE_NLC_PLAYER)
}

void CApplicationVolumeHandling::CacheReplayGainSettings(const CSettings& settings)
{
#if !defined(ENABLE_NLC_PLAYER)
  // initialize m_replayGainSettings
  m_replayGainSettings.iType = settings.GetInt(CSettings::SETTING_MUSICPLAYER_REPLAYGAINTYPE);
  m_replayGainSettings.iPreAmp = settings.GetInt(CSettings::SETTING_MUSICPLAYER_REPLAYGAINPREAMP);
  m_replayGainSettings.iNoGainPreAmp =
      settings.GetInt(CSettings::SETTING_MUSICPLAYER_REPLAYGAINNOGAINPREAMP);
  m_replayGainSettings.bAvoidClipping =
      settings.GetBool(CSettings::SETTING_MUSICPLAYER_REPLAYGAINAVOIDCLIPPING);

#endif // !defined(ENABLE_NLC_PLAYER)
}

bool CApplicationVolumeHandling::Load(const TiXmlNode* settings)
{
#if !defined(ENABLE_NLC_PLAYER)
  if (!settings)
    return false;

  const TiXmlElement* audioElement = settings->FirstChildElement("audio");
  if (audioElement)
  {
    XMLUtils::GetBoolean(audioElement, "mute", m_muted);
    if (!XMLUtils::GetFloat(audioElement, "fvolumelevel", m_volumeLevel, VOLUME_MINIMUM,
                            VOLUME_MAXIMUM))
      m_volumeLevel = VOLUME_MAXIMUM;
  }

#endif // !defined(ENABLE_NLC_PLAYER)
  return true;
}

bool CApplicationVolumeHandling::Save(TiXmlNode* settings) const
{
#if !defined(ENABLE_NLC_PLAYER)
  if (!settings)
    return false;

  TiXmlElement volumeNode("audio");
  TiXmlNode* audioNode = settings->InsertEndChild(volumeNode);
  if (!audioNode)
    return false;

  XMLUtils::SetBoolean(audioNode, "mute", m_muted);
  XMLUtils::SetFloat(audioNode, "fvolumelevel", m_volumeLevel);

  return true;
#else
    return false;
#endif // !defined(ENABLE_NLC_PLAYER)
}

bool CApplicationVolumeHandling::OnSettingChanged(const CSetting& setting)
{
#if !defined(ENABLE_NLC_PLAYER)
  const std::string& settingId = setting.GetId();

  if (StringUtils::EqualsNoCase(settingId, CSettings::SETTING_MUSICPLAYER_REPLAYGAINTYPE))
    m_replayGainSettings.iType = static_cast<const CSettingInt&>(setting).GetValue();
  else if (StringUtils::EqualsNoCase(settingId, CSettings::SETTING_MUSICPLAYER_REPLAYGAINPREAMP))
    m_replayGainSettings.iPreAmp = static_cast<const CSettingInt&>(setting).GetValue();
  else if (StringUtils::EqualsNoCase(settingId,
                                     CSettings::SETTING_MUSICPLAYER_REPLAYGAINNOGAINPREAMP))
    m_replayGainSettings.iNoGainPreAmp = static_cast<const CSettingInt&>(setting).GetValue();
  else if (StringUtils::EqualsNoCase(settingId,
                                     CSettings::SETTING_MUSICPLAYER_REPLAYGAINAVOIDCLIPPING))
    m_replayGainSettings.bAvoidClipping = static_cast<const CSettingBool&>(setting).GetValue();
  else
    return false;

  return true;
#else
    return false;
#endif // !defined(ENABLE_NLC_PLAYER)
}
