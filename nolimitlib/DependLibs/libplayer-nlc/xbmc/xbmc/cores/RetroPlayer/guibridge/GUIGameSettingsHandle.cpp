/*
 *      Copyright (C) 2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "config_components_kodi.h"
#if ENABLE_GAMES

#include "GUIGameSettingsHandle.h"
#include "GUIGameRenderManager.h"

using namespace KODI;
using namespace RETRO;

CGUIGameSettingsHandle::CGUIGameSettingsHandle(CGUIGameRenderManager &renderManager) :
  m_renderManager(renderManager)
{
}

CGUIGameSettingsHandle::~CGUIGameSettingsHandle()
{
  m_renderManager.UnregisterHandle(this);
}

std::string CGUIGameSettingsHandle::GameClientID()
{
  return m_renderManager.GameClientID();
}

#endif // ENABLE_GAMES
