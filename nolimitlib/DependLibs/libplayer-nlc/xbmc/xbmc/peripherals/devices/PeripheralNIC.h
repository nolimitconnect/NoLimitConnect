/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once
#include "config_kodi.h"
#if ENABLE_PERIPHERALS

#include "Peripheral.h"

namespace PERIPHERALS
{
  class CPeripheralNIC : public CPeripheral
  {
  public:
    CPeripheralNIC(CPeripherals& manager, const PeripheralScanResult& scanResult, CPeripheralBus* bus);
    ~CPeripheralNIC(void) override = default;
  };
}

#endif // ENABLE_PERIPHERALS