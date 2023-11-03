/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "config_components_kodi.h"
#if ENABLE_PERIPHERALS

#include "PeripheralBluetooth.h"

using namespace PERIPHERALS;

CPeripheralBluetooth::CPeripheralBluetooth(CPeripherals& manager, const PeripheralScanResult& scanResult, CPeripheralBus* bus) :
  CPeripheral(manager, scanResult, bus)
{
  m_features.push_back(FEATURE_BLUETOOTH);
}

#endif // ENABLE_PERIPHERALS
