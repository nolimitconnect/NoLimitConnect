/*
 *  Copyright (C) 2005-2018 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#include "CallbackFunction.h"
#if HAVE_ADDONS

namespace XBMCAddon
{
  Callback::~Callback() { XBMC_TRACE; deallocating(); }
}

#endif // HAVE_ADDONS
