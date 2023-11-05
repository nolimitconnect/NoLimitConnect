/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once
#include "config_components_kodi.h"
#if ENABLE_PERIPHERALS

#include "PeripheralHID.h"

namespace PERIPHERALS
{
  class CPeripheralNyxboard : public CPeripheralHID
  {
  public:
    CPeripheralNyxboard(CPeripherals& manager, const PeripheralScanResult& scanResult, CPeripheralBus* bus);
    ~CPeripheralNyxboard(void) override = default;
    bool LookupSymAndUnicode(XBMC_keysym &keysym, uint8_t *key, char *unicode) override;
  };
}

#endif // ENABLE_PERIPHERALS