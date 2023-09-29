/*
 *      Copyright (C) 2005-2015 Team XBMC
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "config_kodi.h"
#ifdef HAVE_QT_GUI
#if defined(TARGET_OS_WINDOWS)
# include <libglew/include/GL/glew.h>
#endif

#include "Texture.h"
#include "TextureQt.h"

#include "ServiceBroker.h"
#include "rendering/RenderSystem.h"
#include "utils/log.h"
#include "utils/GLUtils.h"
#include "guilib/TextureManager.h"
#include "settings/AdvancedSettings.h"
#ifdef TARGET_POSIX
#include "platform/linux/XMemUtils.h"
#endif

#include "../../../../../nolimitgui/AppInterface/INlc.h"


 /************************************************************************/
 /*    CTextureQt                                                       */
 /************************************************************************/
CTextureQt::CTextureQt( unsigned int width, unsigned int height, XB_FMT format )
    : CTextureBase( width, height, format )
    , m_INlc( INlc::getINlc() )
{
    unsigned int major, minor;
    CServiceBroker::GetRenderSystem()->GetRenderVersion( major, minor );
    if( major >= 3 )
        m_isOglVersion3orNewer = true;
}

CTextureQt::~CTextureQt()
{
    DestroyTextureObject();
}

void CTextureQt::CreateTextureObject()
{
    m_INlc.createTextureObject( this );
    //glGenTextures( 1, ( GLuint* )&m_texture );
}

void CTextureQt::DestroyTextureObject()
{
    m_INlc.destroyTextureObject( this );
    if( m_texture )
        CServiceBroker::GetGUI()->GetTextureManager().ReleaseHwTexture( m_texture );
}

void CTextureQt::LoadToGPU()
{
    m_INlc.loadToGPU( this );
}

void CTextureQt::BindToUnit( unsigned int unit )
{
   m_INlc.bindToUnit( this, unit );
}

#endif // HAS_GL
