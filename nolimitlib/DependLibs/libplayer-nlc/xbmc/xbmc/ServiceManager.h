/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once
#include "config_components_kodi.h"

#include <memory>
#include "platform/Platform.h"

class CAppParamParser;

namespace ADDON
{
class CAddonMgr;
class CBinaryAddonManager;
class CBinaryAddonCache;
class CVFSAddonCache;
class CServiceAddonManager;
class CRepositoryUpdater;
}
#if ENABLE_PVR
namespace PVR
{
class CPVRManager;
}
#endif // ENABLE_PVR

namespace PLAYLIST
{
  class CPlayListPlayer;
}

class CContextMenuManager;
#ifdef HAS_PYTHON
class XBPython;
#endif
#if defined(HAS_FILESYSTEM_SMB)
namespace WSDiscovery
{
class IWSDiscovery;
}
#endif
class CDataCacheCore;
class CFavouritesService;
class CNetworkBase;
class CWinSystemBase;
class CPowerManager;
class CWeatherManager;

namespace KODI
{
namespace ADDONS
{
class CExtsMimeSupportList;
}

#if ENABLE_GAMES
namespace GAME
{
  class CControllerManager;
  class CGameServices;
} // namespace GAME

namespace RETRO
{
  class CGUIGameRenderManager;
}
#endif // ENABLE_GAMES

} // namespace KODI

namespace MEDIA_DETECT
{
class CDetectDVDMedia;
}

#if ENABLE_PERIPHERALS
namespace PERIPHERALS
{
  class CPeripherals;
}
#endif // ENABLE_PERIPHERALS

class CInputManager;
class CFileExtensionProvider;
class CPlayerCoreFactory;
class CDatabaseManager;
class CProfileManager;
class CEventLog;
class CMediaManager;

class CServiceManager
{
public:
  CServiceManager();
  ~CServiceManager();

  bool InitForTesting();
  bool InitStageOne();
  bool InitStageTwo(const std::string& profilesUserDataFolder);
  bool InitStageThree(const std::shared_ptr<CProfileManager>& profileManager);
  void DeinitTesting();
  void DeinitStageThree();
  void DeinitStageTwo();
  void DeinitStageOne();

  bool HasPlayerFactory() { return m_playerCoreFactory != nullptr; }

  ADDON::CAddonMgr& GetAddonMgr();
  ADDON::CBinaryAddonManager& GetBinaryAddonManager();
  ADDON::CBinaryAddonCache& GetBinaryAddonCache();
  KODI::ADDONS::CExtsMimeSupportList& GetExtsMimeSupportList();
  ADDON::CVFSAddonCache& GetVFSAddonCache();
  ADDON::CServiceAddonManager& GetServiceAddons();
  ADDON::CRepositoryUpdater& GetRepositoryUpdater();
  CNetworkBase& GetNetwork();
#ifdef HAS_PYTHON
  XBPython& GetXBPython();
#endif
#if defined(HAS_FILESYSTEM_SMB)
  WSDiscovery::IWSDiscovery& GetWSDiscovery();
#endif
#if ENABLE_PVR
  PVR::CPVRManager& GetPVRManager();
#endif // ENABLE_PVR
  CContextMenuManager& GetContextMenuManager();
  CDataCacheCore& GetDataCacheCore();
  /**\brief Get the platform object. This is save to be called after Init1() was called
   */
  CPlatform& GetPlatform();
#if ENABLE_GAMES
  KODI::GAME::CControllerManager& GetGameControllerManager();
  KODI::GAME::CGameServices& GetGameServices();
  KODI::RETRO::CGUIGameRenderManager& GetGameRenderManager();
  PERIPHERALS::CPeripherals& GetPeripherals();
#endif // ENABLE_GAMES

  PLAYLIST::CPlayListPlayer& GetPlaylistPlayer();
  int init_level = 0;

  CFavouritesService& GetFavouritesService();
  CInputManager &GetInputManager();
  CFileExtensionProvider &GetFileExtensionProvider();

  CPowerManager &GetPowerManager();

  CWeatherManager &GetWeatherManager();

  CPlayerCoreFactory &GetPlayerCoreFactory();

  CDatabaseManager &GetDatabaseManager();

  CMediaManager& GetMediaManager();

#if !defined(TARGET_WINDOWS) && defined(HAS_OPTICAL_DRIVE)
  MEDIA_DETECT::CDetectDVDMedia& GetDetectDVDMedia();
#endif

  int GetInitLevel() {return init_level; }

protected:
  struct delete_dataCacheCore
  {
    void operator()(CDataCacheCore *p) const;
  };

  struct delete_contextMenuManager
  {
    void operator()(CContextMenuManager *p) const;
  };

  struct delete_favouritesService
  {
    void operator()(CFavouritesService *p) const;
  };

  std::unique_ptr<ADDON::CAddonMgr> m_addonMgr;
  std::unique_ptr<ADDON::CBinaryAddonManager> m_binaryAddonManager;
  std::unique_ptr<ADDON::CBinaryAddonCache> m_binaryAddonCache;
  std::unique_ptr<KODI::ADDONS::CExtsMimeSupportList> m_extsMimeSupportList;
  std::unique_ptr<ADDON::CVFSAddonCache> m_vfsAddonCache;
  std::unique_ptr<ADDON::CServiceAddonManager> m_serviceAddons;
  std::unique_ptr<ADDON::CRepositoryUpdater> m_repositoryUpdater;
#if defined(HAS_FILESYSTEM_SMB)
  std::unique_ptr<WSDiscovery::IWSDiscovery> m_WSDiscovery;
#endif
#ifdef HAS_PYTHON
  std::unique_ptr<XBPython> m_XBPython;
#endif
#if ENABLE_PVR
  std::unique_ptr<PVR::CPVRManager> m_PVRManager;
#endif // ENABLE_PVR
  std::unique_ptr<CContextMenuManager, delete_contextMenuManager> m_contextMenuManager;
  std::unique_ptr<CDataCacheCore, delete_dataCacheCore> m_dataCacheCore;
  std::unique_ptr<CPlatform> m_Platform;
  std::unique_ptr<PLAYLIST::CPlayListPlayer> m_playlistPlayer;
#if ENABLE_GAMES
  std::unique_ptr<KODI::GAME::CControllerManager> m_gameControllerManager;
  std::unique_ptr<KODI::GAME::CGameServices> m_gameServices;
  std::unique_ptr<KODI::RETRO::CGUIGameRenderManager> m_gameRenderManager;
  std::unique_ptr<PERIPHERALS::CPeripherals> m_peripherals;
#endif // ENABLE_GAMES

  std::unique_ptr<CFavouritesService, delete_favouritesService> m_favouritesService;
  std::unique_ptr<CInputManager> m_inputManager;
  std::unique_ptr<CFileExtensionProvider> m_fileExtensionProvider;
  std::unique_ptr<CNetworkBase> m_network;
  std::unique_ptr<CPowerManager> m_powerManager;
  std::unique_ptr<CWeatherManager> m_weatherManager;
  std::unique_ptr<CPlayerCoreFactory> m_playerCoreFactory;
  std::unique_ptr<CDatabaseManager> m_databaseManager;
  std::unique_ptr<CMediaManager> m_mediaManager;
#if !defined(TARGET_WINDOWS) && defined(HAS_OPTICAL_DRIVE)
  std::unique_ptr<MEDIA_DETECT::CDetectDVDMedia> m_DetectDVDType;
#endif
};


