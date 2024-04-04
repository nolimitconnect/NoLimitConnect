/*
 *  Copyright (C) 2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "config_components_kodi.h"
#if HAVE_ADDONS

namespace PVR
{
class IPVRComponent
{
public:
  virtual ~IPVRComponent() = default;
};
} // namespace PVR

#endif // HAVE_ADDONS
