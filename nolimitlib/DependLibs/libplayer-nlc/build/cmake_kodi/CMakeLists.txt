cmake_minimum_required(VERSION 3.12.2)

project(libkodi)

set(CMAKE_CONFIGURATION_TYPES "Release"
    CACHE STRING "Configuration types" FORCE)

add_library(libkodi STATIC
  ../../xbmc/addons/Addon.h
  ../../xbmc/addons/Addon2.cpp
  ../../xbmc/addons/AddonBuilder.cpp
  ../../xbmc/addons/AddonBuilder.h
  ../../xbmc/addons/AddonDatabase.cpp
  ../../xbmc/addons/AddonDatabase.h
  ../../xbmc/addons/AddonEvents.h
  ../../xbmc/addons/AddonInfo.cpp
  ../../xbmc/addons/AddonInfo.h
  ../../xbmc/addons/AddonInstaller.cpp
  ../../xbmc/addons/AddonInstaller.h
  ../../xbmc/addons/AddonManager.cpp
  ../../xbmc/addons/AddonManager.h
  ../../xbmc/addons/AddonProvider.h
  ../../xbmc/addons/AddonStatusHandler.cpp
  ../../xbmc/addons/AddonStatusHandler.h
  ../../xbmc/addons/AddonSystemSettings.cpp
  ../../xbmc/addons/AddonSystemSettings.h
  ../../xbmc/addons/AddonVersion.cpp
  ../../xbmc/addons/AddonVersion.h
  ../../xbmc/addons/AudioDecoder.h
  ../../xbmc/addons/AudioDecoder2.cpp
  ../../xbmc/addons/AudioEncoder.cpp
  ../../xbmc/addons/AudioEncoder.h
  ../../xbmc/addons/BinaryAddonCache.cpp
  ../../xbmc/addons/BinaryAddonCache.h
  ../../xbmc/addons/binary-addons/AddonDll.cpp
  ../../xbmc/addons/binary-addons/AddonDll.h
  ../../xbmc/addons/binary-addons/AddonInstanceHandler.cpp
  ../../xbmc/addons/binary-addons/AddonInstanceHandler.h
  ../../xbmc/addons/binary-addons/BinaryAddonBase.cpp
  ../../xbmc/addons/binary-addons/BinaryAddonBase.h
  ../../xbmc/addons/binary-addons/BinaryAddonExtensions.cpp
  ../../xbmc/addons/binary-addons/BinaryAddonExtensions.h
  ../../xbmc/addons/binary-addons/BinaryAddonManager.cpp
  ../../xbmc/addons/binary-addons/BinaryAddonManager.h
  ../../xbmc/addons/binary-addons/BinaryAddonType.cpp
  ../../xbmc/addons/binary-addons/BinaryAddonType.h
  ../../xbmc/addons/binary-addons/DllAddon.h
  ../../xbmc/addons/ContextMenuAddon.cpp
  ../../xbmc/addons/ContextMenuAddon.h
  ../../xbmc/addons/ContextMenus.h
  ../../xbmc/addons/ContextMenus2.cpp
  ../../xbmc/addons/DllLibCPluff.cpp
  ../../xbmc/addons/DllLibCPluff.h
  ../../xbmc/addons/FilesystemInstaller.cpp
  ../../xbmc/addons/FilesystemInstaller.h
  ../../xbmc/addons/FontResource.cpp
  ../../xbmc/addons/FontResource.h
  ../../xbmc/addons/GameResource.cpp
  ../../xbmc/addons/GameResource.h
  ../../xbmc/addons/GUIDialogAddonInfo.cpp
  ../../xbmc/addons/GUIDialogAddonInfo.h
  ../../xbmc/addons/GUIViewStateAddonBrowser.cpp
  ../../xbmc/addons/GUIViewStateAddonBrowser.h
  ../../xbmc/addons/GUIWindowAddonBrowser.cpp
  ../../xbmc/addons/GUIWindowAddonBrowser.h
  ../../xbmc/addons/IAddon.h
  ../../xbmc/addons/ImageDecoder.cpp
  ../../xbmc/addons/ImageDecoder.h
  ../../xbmc/addons/ImageResource.cpp
  ../../xbmc/addons/ImageResource.h
  ../../xbmc/addons/interfaces/Addon/AddonCallbacksAddon.cpp
  ../../xbmc/addons/interfaces/Addon/AddonCallbacksAddon.h
  ../../xbmc/addons/interfaces/AddonInterfaces.cpp
  ../../xbmc/addons/interfaces/AddonInterfaces.h
  ../../xbmc/addons/interfaces/Filesystem.cpp
  ../../xbmc/addons/interfaces/Filesystem.h
  ../../xbmc/addons/interfaces/General.cpp
  ../../xbmc/addons/interfaces/General.h
  ../../xbmc/addons/interfaces/GUI/AddonCallbacksGUI.cpp
  ../../xbmc/addons/interfaces/GUI/AddonCallbacksGUI.h
  ../../xbmc/addons/interfaces/GUI/AddonGUIRenderingControl.cpp
  ../../xbmc/addons/interfaces/GUI/AddonGUIRenderingControl.h
  ../../xbmc/addons/interfaces/GUI/AddonGUIWindow.cpp
  ../../xbmc/addons/interfaces/GUI/AddonGUIWindow.h
  ../../xbmc/addons/interfaces/GUI/controls/Button.cpp
  ../../xbmc/addons/interfaces/GUI/controls/Button.h
  ../../xbmc/addons/interfaces/GUI/controls/Edit.cpp
  ../../xbmc/addons/interfaces/GUI/controls/Edit.h
  ../../xbmc/addons/interfaces/GUI/controls/FadeLabel.cpp
  ../../xbmc/addons/interfaces/GUI/controls/FadeLabel.h
  ../../xbmc/addons/interfaces/GUI/controls/Image.cpp
  ../../xbmc/addons/interfaces/GUI/controls/Image.h
  ../../xbmc/addons/interfaces/GUI/controls/Label.cpp
  ../../xbmc/addons/interfaces/GUI/controls/Label.h
  ../../xbmc/addons/interfaces/GUI/controls/Progress.h
  ../../xbmc/addons/interfaces/GUI/controls/Progress3.cpp
  ../../xbmc/addons/interfaces/GUI/controls/RadioButton.cpp
  ../../xbmc/addons/interfaces/GUI/controls/RadioButton.h
  ../../xbmc/addons/interfaces/GUI/controls/Rendering.cpp
  ../../xbmc/addons/interfaces/GUI/controls/Rendering.h
  ../../xbmc/addons/interfaces/GUI/controls/SettingsSlider.cpp
  ../../xbmc/addons/interfaces/GUI/controls/SettingsSlider.h
  ../../xbmc/addons/interfaces/GUI/controls/Slider.cpp
  ../../xbmc/addons/interfaces/GUI/controls/Slider.h
  ../../xbmc/addons/interfaces/GUI/controls/Spin.cpp
  ../../xbmc/addons/interfaces/GUI/controls/Spin.h
  ../../xbmc/addons/interfaces/GUI/controls/TextBox.cpp
  ../../xbmc/addons/interfaces/GUI/controls/TextBox.h
  ../../xbmc/addons/interfaces/GUI/dialogs/ContextMenu.cpp
  ../../xbmc/addons/interfaces/GUI/dialogs/ContextMenu.h
  ../../xbmc/addons/interfaces/GUI/dialogs/ExtendedProgressBar.cpp
  ../../xbmc/addons/interfaces/GUI/dialogs/ExtendedProgressBar.h
  ../../xbmc/addons/interfaces/GUI/dialogs/FileBrowser.cpp
  ../../xbmc/addons/interfaces/GUI/dialogs/FileBrowser.h
  ../../xbmc/addons/interfaces/GUI/dialogs/Keyboard.h
  ../../xbmc/addons/interfaces/GUI/dialogs/Keyboard2.cpp
  ../../xbmc/addons/interfaces/GUI/dialogs/Numeric.cpp
  ../../xbmc/addons/interfaces/GUI/dialogs/Numeric.h
  ../../xbmc/addons/interfaces/GUI/dialogs/OK.cpp
  ../../xbmc/addons/interfaces/GUI/dialogs/OK.h
  ../../xbmc/addons/interfaces/GUI/dialogs/Progress.h
  ../../xbmc/addons/interfaces/GUI/dialogs/Progress2.cpp
  ../../xbmc/addons/interfaces/GUI/dialogs/Select.cpp
  ../../xbmc/addons/interfaces/GUI/dialogs/Select.h
  ../../xbmc/addons/interfaces/GUI/dialogs/TextViewer.cpp
  ../../xbmc/addons/interfaces/GUI/dialogs/TextViewer.h
  ../../xbmc/addons/interfaces/GUI/dialogs/YesNo.cpp
  ../../xbmc/addons/interfaces/GUI/dialogs/YesNo.h
  ../../xbmc/addons/interfaces/GUI/General.h
  ../../xbmc/addons/interfaces/GUI/General2.cpp
  ../../xbmc/addons/interfaces/GUI/ListItem.h
  ../../xbmc/addons/interfaces/GUI/ListItem2.cpp
  ../../xbmc/addons/interfaces/GUI/Window.h
  ../../xbmc/addons/interfaces/GUI/Window2.cpp
  ../../xbmc/addons/interfaces/Network.h
  ../../xbmc/addons/interfaces/Network2.cpp
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/AddonBase.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/addon-instance/AudioDecoder.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/addon-instance/AudioDSP.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/addon-instance/AudioEncoder.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/addon-instance/ImageDecoder.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/addon-instance/Inputstream.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/addon-instance/Peripheral.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/addon-instance/PeripheralUtils.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/addon-instance/Screensaver.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/addon-instance/VFS.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/addon-instance/VideoCodec.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/addon-instance/Visualization.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/AudioEngine.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/Filesystem.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/General.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/Button.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/Edit.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/FadeLabel.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/Image.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/Label.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/Progress.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/RadioButton.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/Rendering.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/SettingsSlider.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/Slider.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/Spin.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/controls/TextBox.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/definitions.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/dialogs/ContextMenu.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/dialogs/ExtendedProgress.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/dialogs/FileBrowser.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/dialogs/Keyboard.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/dialogs/Numeric.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/dialogs/OK.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/dialogs/Progress.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/dialogs/Select.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/dialogs/TextViewer.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/dialogs/YesNo.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/General.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/ListItem.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/gui/Window.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/kodi_game_dll.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/kodi_game_types.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/kodi_vfs_types.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/libKODI_game.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/libKODI_guilib.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/libXBMC_addon.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/libXBMC_pvr.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/Network.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/StreamCodec.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/StreamCrypto.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/tools/DllHelper.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/versions.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/xbmc_addon_dll.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/xbmc_addon_types.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/xbmc_epg_types.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/xbmc_pvr_dll.h
  ../../xbmc/addons/kodi-addon-dev-kit/include/kodi/xbmc_pvr_types.h
  ../../xbmc/addons/LanguageResource.cpp
  ../../xbmc/addons/LanguageResource.h
  ../../xbmc/addons/PluginSource.cpp
  ../../xbmc/addons/PluginSource.h
  ../../xbmc/addons/PVRClient.cpp
  ../../xbmc/addons/PVRClient.h
  ../../xbmc/addons/PVRClientMenuHooks.cpp
  ../../xbmc/addons/PVRClientMenuHooks.h
  ../../xbmc/addons/Repository.cpp
  ../../xbmc/addons/Repository.h
  ../../xbmc/addons/RepositoryUpdater.cpp
  ../../xbmc/addons/RepositoryUpdater.h
  ../../xbmc/addons/Resource.h
  ../../xbmc/addons/Scraper.cpp
  ../../xbmc/addons/Scraper.h
  ../../xbmc/addons/ScreenSaver.cpp
  ../../xbmc/addons/ScreenSaver.h
  ../../xbmc/addons/Service.cpp
  ../../xbmc/addons/Service.h
  ../../xbmc/addons/settings/AddonSettings.cpp
  ../../xbmc/addons/settings/AddonSettings.h
  ../../xbmc/addons/settings/GUIDialogAddonSettings.cpp
  ../../xbmc/addons/settings/GUIDialogAddonSettings.h
  ../../xbmc/addons/settings/SettingUrlEncodedString.cpp
  ../../xbmc/addons/settings/SettingUrlEncodedString.h
  ../../xbmc/addons/Skin.cpp
  ../../xbmc/addons/Skin.h
  ../../xbmc/addons/UISoundsResource.cpp
  ../../xbmc/addons/UISoundsResource.h
  ../../xbmc/addons/VFSEntry.cpp
  ../../xbmc/addons/VFSEntry.h
  ../../xbmc/addons/Visualization.cpp
  ../../xbmc/addons/Visualization.h
  ../../xbmc/addons/Webinterface.cpp
  ../../xbmc/addons/Webinterface.h
  ../../xbmc/AppInboundProtocol.cpp
  ../../xbmc/AppInboundProtocol.h
  ../../xbmc/Application.cpp
  ../../xbmc/Application.h
  ../../xbmc/ApplicationEvents.cpp
  ../../xbmc/ApplicationPlayer.cpp
  ../../xbmc/ApplicationPlayer.h
  ../../xbmc/ApplicationStackHelper.cpp
  ../../xbmc/ApplicationStackHelper.h
  ../../xbmc/AppParamParser.cpp
  ../../xbmc/AppParamParser.h
  ../../xbmc/Autorun.cpp
  ../../xbmc/Autorun.h
  ../../xbmc/AutoSwitch.cpp
  ../../xbmc/AutoSwitch.h
  ../../xbmc/BackgroundInfoLoader.cpp
  ../../xbmc/BackgroundInfoLoader.h
  ../../xbmc/cdrip/CDDARipJob.cpp
  ../../xbmc/cdrip/CDDARipJob.h
  ../../xbmc/cdrip/CDDARipper.cpp
  ../../xbmc/cdrip/CDDARipper.h
  ../../xbmc/cdrip/Encoder.cpp
  ../../xbmc/cdrip/Encoder.h
  ../../xbmc/cdrip/EncoderFFmpeg.cpp
  ../../xbmc/cdrip/EncoderFFmpeg.h
  ../../xbmc/cdrip/IEncoder.h
  ../../xbmc/commons/Buffer.h
  ../../xbmc/commons/Exception.cpp
  ../../xbmc/commons/Exception.h
  ../../xbmc/commons/ilog.cpp
  ../../xbmc/commons/ilog.h
  ../../xbmc/CompileInfo.cpp
  ../../xbmc/CompileInfo.h
  ../../xbmc/ContextMenuItem.cpp
  ../../xbmc/ContextMenuItem.h
  ../../xbmc/ContextMenuManager.cpp
  ../../xbmc/ContextMenuManager.h
  ../../xbmc/ContextMenus.cpp
  ../../xbmc/ContextMenus.h
  ../../xbmc/contrib/kissfft/_kiss_fft_guts.h
  ../../xbmc/contrib/kissfft/kiss_fft.c
  ../../xbmc/contrib/kissfft/kiss_fft.h
  ../../xbmc/contrib/kissfft/kiss_fftr.c
  ../../xbmc/contrib/kissfft/kiss_fftr.h
  ../../xbmc/cores/AudioEngine/AEResampleFactory.cpp
  ../../xbmc/cores/AudioEngine/AEResampleFactory.h
  ../../xbmc/cores/AudioEngine/AESinkFactory.cpp
  ../../xbmc/cores/AudioEngine/AESinkFactory.h
  ../../xbmc/cores/AudioEngine/Encoders/AEEncoderFFmpeg.cpp
  ../../xbmc/cores/AudioEngine/Encoders/AEEncoderFFmpeg.h
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAE.cpp
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAE.h
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEBuffer.cpp
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEBuffer.h
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEFilter.cpp
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEFilter.h
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEResampleFFMPEG.cpp
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEResampleFFMPEG.h
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAESettings.cpp
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAESettings.h
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAESink.cpp
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAESink.h
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAESound.cpp
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAESound.h
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEStream.cpp
  ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEStream.h
  ../../xbmc/cores/AudioEngine/Interfaces/AE.h
  ../../xbmc/cores/AudioEngine/Interfaces/AEEncoder.h
  ../../xbmc/cores/AudioEngine/Interfaces/AEResample.h
  ../../xbmc/cores/AudioEngine/Interfaces/AESink.h
  ../../xbmc/cores/AudioEngine/Interfaces/AESound.h
  ../../xbmc/cores/AudioEngine/Interfaces/AEStream.h
  ../../xbmc/cores/AudioEngine/Interfaces/IAudioCallback.h
  ../../xbmc/cores/AudioEngine/Interfaces/ThreadedAE.h
  ../../xbmc/cores/AudioEngine/Sinks/AESinkDirectSound.cpp
  ../../xbmc/cores/AudioEngine/Sinks/AESinkDirectSound.h
  ../../xbmc/cores/AudioEngine/Sinks/AESinkQt.cpp
  ../../xbmc/cores/AudioEngine/Sinks/AESinkQt.h
  ../../xbmc/cores/AudioEngine/Sinks/AESinkWASAPI.cpp
  ../../xbmc/cores/AudioEngine/Sinks/AESinkWASAPI.h
  ../../xbmc/cores/AudioEngine/Sinks/windows/AESinkFactoryWin.cpp
  ../../xbmc/cores/AudioEngine/Sinks/windows/AESinkFactoryWin.h
  ../../xbmc/cores/AudioEngine/Sinks/windows/AESinkFactoryWin32.cpp
  ../../xbmc/cores/AudioEngine/Utils/AEAudioFormat.h
  ../../xbmc/cores/AudioEngine/Utils/AEBitstreamPacker.cpp
  ../../xbmc/cores/AudioEngine/Utils/AEBitstreamPacker.h
  ../../xbmc/cores/AudioEngine/Utils/AEChannelData.h
  ../../xbmc/cores/AudioEngine/Utils/AEChannelInfo.cpp
  ../../xbmc/cores/AudioEngine/Utils/AEChannelInfo.h
  ../../xbmc/cores/AudioEngine/Utils/AEDeviceInfo.cpp
  ../../xbmc/cores/AudioEngine/Utils/AEDeviceInfo.h
  ../../xbmc/cores/AudioEngine/Utils/AELimiter.cpp
  ../../xbmc/cores/AudioEngine/Utils/AELimiter.h
  ../../xbmc/cores/AudioEngine/Utils/AEPackIEC61937.cpp
  ../../xbmc/cores/AudioEngine/Utils/AEPackIEC61937.h
  ../../xbmc/cores/AudioEngine/Utils/AERingBuffer.h
  ../../xbmc/cores/AudioEngine/Utils/AEStreamData.h
  ../../xbmc/cores/AudioEngine/Utils/AEStreamInfo.cpp
  ../../xbmc/cores/AudioEngine/Utils/AEStreamInfo.h
  ../../xbmc/cores/AudioEngine/Utils/AEUtil.cpp
  ../../xbmc/cores/AudioEngine/Utils/AEUtil.h
  ../../xbmc/cores/DataCacheCore.cpp
  ../../xbmc/cores/DataCacheCore.h
  ../../xbmc/cores/DllLoader/coff.cpp
  ../../xbmc/cores/DllLoader/coff.h
  ../../xbmc/cores/DllLoader/coffldr.h
  ../../xbmc/cores/DllLoader/dll.cpp
  ../../xbmc/cores/DllLoader/dll.h
  ../../xbmc/cores/DllLoader/dll_tracker.cpp
  ../../xbmc/cores/DllLoader/dll_tracker.h
  ../../xbmc/cores/DllLoader/dll_tracker_file.cpp
  ../../xbmc/cores/DllLoader/dll_tracker_file.h
  ../../xbmc/cores/DllLoader/dll_tracker_library.cpp
  ../../xbmc/cores/DllLoader/dll_tracker_library.h
  ../../xbmc/cores/DllLoader/dll_util.cpp
  ../../xbmc/cores/DllLoader/dll_util.h
  ../../xbmc/cores/DllLoader/DllLoader.cpp
  ../../xbmc/cores/DllLoader/DllLoader.h
  ../../xbmc/cores/DllLoader/DllLoaderContainer.cpp
  ../../xbmc/cores/DllLoader/DllLoaderContainer.h
  ../../xbmc/cores/DllLoader/exports/emu_dummy.cpp
  ../../xbmc/cores/DllLoader/exports/emu_dummy.h
  ../../xbmc/cores/DllLoader/exports/emu_msvcrt.cpp
  ../../xbmc/cores/DllLoader/exports/emu_msvcrt.h
  ../../xbmc/cores/DllLoader/exports/util/EmuFileWrapper.cpp
  ../../xbmc/cores/DllLoader/exports/util/EmuFileWrapper.h
  ../../xbmc/cores/DllLoader/LibraryLoader.cpp
  ../../xbmc/cores/DllLoader/LibraryLoader.h
  ../../xbmc/cores/DllLoader/Win32DllLoader.cpp
  ../../xbmc/cores/DllLoader/Win32DllLoader.h
  ../../xbmc/cores/ExternalPlayer/ExternalPlayer.cpp
  ../../xbmc/cores/ExternalPlayer/ExternalPlayer.h
  ../../xbmc/cores/FFmpeg.h
  ../../xbmc/cores/IPlayer.h
  ../../xbmc/cores/IPlayerCallback.h
  ../../xbmc/cores/paplayer/AudioDecoder.cpp
  ../../xbmc/cores/paplayer/AudioDecoder.h
  ../../xbmc/cores/paplayer/CachingCodec.h
  ../../xbmc/cores/paplayer/CodecFactory.cpp
  ../../xbmc/cores/paplayer/CodecFactory.h
  ../../xbmc/cores/paplayer/ICodec.h
  ../../xbmc/cores/paplayer/PAPlayer.cpp
  ../../xbmc/cores/paplayer/PAPlayer.h
  ../../xbmc/cores/paplayer/VideoPlayerCodec.cpp
  ../../xbmc/cores/paplayer/VideoPlayerCodec.h
  ../../xbmc/cores/playercorefactory/PlayerCoreConfig.h
  ../../xbmc/cores/playercorefactory/PlayerCoreFactory.cpp
  ../../xbmc/cores/playercorefactory/PlayerCoreFactory.h
  ../../xbmc/cores/playercorefactory/PlayerSelectionRule.cpp
  ../../xbmc/cores/playercorefactory/PlayerSelectionRule.h
  ../../xbmc/cores/RetroPlayer/audio/AudioTranslator.cpp
  ../../xbmc/cores/RetroPlayer/audio/AudioTranslator.h
  ../../xbmc/cores/RetroPlayer/buffers/BaseRenderBuffer.cpp
  ../../xbmc/cores/RetroPlayer/buffers/BaseRenderBuffer.h
  ../../xbmc/cores/RetroPlayer/buffers/BaseRenderBufferPool.cpp
  ../../xbmc/cores/RetroPlayer/buffers/BaseRenderBufferPool.h
  ../../xbmc/cores/RetroPlayer/buffers/IRenderBuffer.h
  ../../xbmc/cores/RetroPlayer/buffers/IRenderBufferPool.h
  ../../xbmc/cores/RetroPlayer/buffers/RenderBufferManager.cpp
  ../../xbmc/cores/RetroPlayer/buffers/RenderBufferManager.h
  ../../xbmc/cores/RetroPlayer/buffers/video/RenderBufferGuiTexture.cpp
  ../../xbmc/cores/RetroPlayer/buffers/video/RenderBufferGuiTexture.h
  ../../xbmc/cores/RetroPlayer/buffers/video/RenderBufferSysMem.cpp
  ../../xbmc/cores/RetroPlayer/buffers/video/RenderBufferSysMem.h
  ../../xbmc/cores/RetroPlayer/guibridge/GUIGameRenderManager.cpp
  ../../xbmc/cores/RetroPlayer/guibridge/GUIGameRenderManager.h
  ../../xbmc/cores/RetroPlayer/guibridge/GUIGameSettings.cpp
  ../../xbmc/cores/RetroPlayer/guibridge/GUIGameSettings.h
  ../../xbmc/cores/RetroPlayer/guibridge/GUIGameSettingsHandle.cpp
  ../../xbmc/cores/RetroPlayer/guibridge/GUIGameSettingsHandle.h
  ../../xbmc/cores/RetroPlayer/guibridge/GUIGameVideoHandle.cpp
  ../../xbmc/cores/RetroPlayer/guibridge/GUIGameVideoHandle.h
  ../../xbmc/cores/RetroPlayer/guibridge/GUIRenderHandle.cpp
  ../../xbmc/cores/RetroPlayer/guibridge/GUIRenderHandle.h
  ../../xbmc/cores/RetroPlayer/guibridge/GUIRenderTarget.cpp
  ../../xbmc/cores/RetroPlayer/guibridge/GUIRenderTarget.h
  ../../xbmc/cores/RetroPlayer/guibridge/GUIRenderTargetFactory.cpp
  ../../xbmc/cores/RetroPlayer/guibridge/GUIRenderTargetFactory.h
  ../../xbmc/cores/RetroPlayer/guibridge/IGameCallback.h
  ../../xbmc/cores/RetroPlayer/guibridge/IGUIRenderSettings.h
  ../../xbmc/cores/RetroPlayer/guibridge/IRenderCallback.h
  ../../xbmc/cores/RetroPlayer/guicontrols/GUIGameControl.cpp
  ../../xbmc/cores/RetroPlayer/guicontrols/GUIGameControl.h
  ../../xbmc/cores/RetroPlayer/guicontrols/GUIRenderSettings.cpp
  ../../xbmc/cores/RetroPlayer/guicontrols/GUIRenderSettings.h
  ../../xbmc/cores/RetroPlayer/guiplayback/GUIPlaybackControl.cpp
  ../../xbmc/cores/RetroPlayer/guiplayback/GUIPlaybackControl.h
  ../../xbmc/cores/RetroPlayer/guiwindows/GameWindowFullScreen.cpp
  ../../xbmc/cores/RetroPlayer/guiwindows/GameWindowFullScreen.h
  ../../xbmc/cores/RetroPlayer/guiwindows/GameWindowFullScreenText.cpp
  ../../xbmc/cores/RetroPlayer/guiwindows/GameWindowFullScreenText.h
  ../../xbmc/cores/RetroPlayer/playback/GameLoop.cpp
  ../../xbmc/cores/RetroPlayer/playback/GameLoop.h
  ../../xbmc/cores/RetroPlayer/playback/IPlayback.h
  ../../xbmc/cores/RetroPlayer/playback/IPlaybackControl.h
  ../../xbmc/cores/RetroPlayer/playback/RealtimePlayback.h
  ../../xbmc/cores/RetroPlayer/playback/ReversiblePlayback.cpp
  ../../xbmc/cores/RetroPlayer/playback/ReversiblePlayback.h
  ../../xbmc/cores/RetroPlayer/process/qt/RPProcessInfoQt.cpp
  ../../xbmc/cores/RetroPlayer/process/qt/RPProcessInfoQt.h
  ../../xbmc/cores/RetroPlayer/process/RPProcessInfo.cpp
  ../../xbmc/cores/RetroPlayer/process/RPProcessInfo.h
  ../../xbmc/cores/RetroPlayer/process/windows/RPProcessInfoWin.cpp
  ../../xbmc/cores/RetroPlayer/process/windows/RPProcessInfoWin.h
  ../../xbmc/cores/RetroPlayer/rendering/IRenderManager.h
  ../../xbmc/cores/RetroPlayer/rendering/RenderContext.cpp
  ../../xbmc/cores/RetroPlayer/rendering/RenderContext.h
  ../../xbmc/cores/RetroPlayer/rendering/RenderSettings.cpp
  ../../xbmc/cores/RetroPlayer/rendering/RenderSettings.h
  ../../xbmc/cores/RetroPlayer/rendering/RenderTranslator.cpp
  ../../xbmc/cores/RetroPlayer/rendering/RenderTranslator.h
  ../../xbmc/cores/RetroPlayer/rendering/RenderUtils.cpp
  ../../xbmc/cores/RetroPlayer/rendering/RenderUtils.h
  ../../xbmc/cores/RetroPlayer/rendering/RenderVideoSettings.cpp
  ../../xbmc/cores/RetroPlayer/rendering/RenderVideoSettings.h
  ../../xbmc/cores/RetroPlayer/rendering/RPRenderManager.cpp
  ../../xbmc/cores/RetroPlayer/rendering/RPRenderManager.h
  ../../xbmc/cores/RetroPlayer/rendering/VideoRenderers/RPBaseRenderer.cpp
  ../../xbmc/cores/RetroPlayer/rendering/VideoRenderers/RPBaseRenderer.h
  ../../xbmc/cores/RetroPlayer/rendering/VideoRenderers/RPRendererGuiTexture.cpp
  ../../xbmc/cores/RetroPlayer/rendering/VideoRenderers/RPRendererGuiTexture.h
  ../../xbmc/cores/RetroPlayer/rendering/VideoRenderers/RPWinRenderer.cpp
  ../../xbmc/cores/RetroPlayer/rendering/VideoRenderers/RPWinRenderer.h
  ../../xbmc/cores/RetroPlayer/rendering/VideoShaders/windows/RPWinOutputShader.cpp
  ../../xbmc/cores/RetroPlayer/rendering/VideoShaders/windows/RPWinOutputShader.h
  ../../xbmc/cores/RetroPlayer/RetroPlayer.cpp
  ../../xbmc/cores/RetroPlayer/RetroPlayer.h
  ../../xbmc/cores/RetroPlayer/RetroPlayerAutoSave.cpp
  ../../xbmc/cores/RetroPlayer/RetroPlayerAutoSave.h
  ../../xbmc/cores/RetroPlayer/RetroPlayerInput.cpp
  ../../xbmc/cores/RetroPlayer/RetroPlayerInput.h
  ../../xbmc/cores/RetroPlayer/RetroPlayerTypes.h
  ../../xbmc/cores/RetroPlayer/RetroPlayerUtils.cpp
  ../../xbmc/cores/RetroPlayer/RetroPlayerUtils.h
  ../../xbmc/cores/RetroPlayer/savestates/ISavestate.h
  ../../xbmc/cores/RetroPlayer/savestates/SavestateDatabase.cpp
  ../../xbmc/cores/RetroPlayer/savestates/SavestateDatabase.h
  ../../xbmc/cores/RetroPlayer/savestates/SavestateDefines.h
  ../../xbmc/cores/RetroPlayer/savestates/SavestateFlatBuffer.cpp
  ../../xbmc/cores/RetroPlayer/savestates/SavestateFlatBuffer.h
  ../../xbmc/cores/RetroPlayer/savestates/SavestateTranslator.h
  ../../xbmc/cores/RetroPlayer/savestates/SavestateTypes.h
  ../../xbmc/cores/RetroPlayer/savestates/SavestateUtils.cpp
  ../../xbmc/cores/RetroPlayer/savestates/SavestateUtils.h
  ../../xbmc/cores/RetroPlayer/streams/IRetroPlayerStream.h
  ../../xbmc/cores/RetroPlayer/streams/IStreamManager.h
  ../../xbmc/cores/RetroPlayer/streams/memory/BasicMemoryStream.cpp
  ../../xbmc/cores/RetroPlayer/streams/memory/BasicMemoryStream.h
  ../../xbmc/cores/RetroPlayer/streams/memory/DeltaPairMemoryStream.cpp
  ../../xbmc/cores/RetroPlayer/streams/memory/DeltaPairMemoryStream.h
  ../../xbmc/cores/RetroPlayer/streams/memory/IMemoryStream.h
  ../../xbmc/cores/RetroPlayer/streams/memory/LinearMemoryStream.cpp
  ../../xbmc/cores/RetroPlayer/streams/memory/LinearMemoryStream.h
  ../../xbmc/cores/RetroPlayer/streams/RetroPlayerAudio.cpp
  ../../xbmc/cores/RetroPlayer/streams/RetroPlayerAudio.h
  ../../xbmc/cores/RetroPlayer/streams/RetroPlayerStreamTypes.cpp
  ../../xbmc/cores/RetroPlayer/streams/RetroPlayerStreamTypes.h
  ../../xbmc/cores/RetroPlayer/streams/RetroPlayerVideo.cpp
  ../../xbmc/cores/RetroPlayer/streams/RetroPlayerVideo.h
  ../../xbmc/cores/RetroPlayer/streams/RPStreamManager.cpp
  ../../xbmc/cores/RetroPlayer/streams/RPStreamManager.h
  ../../xbmc/cores/VideoPlayer/AudioSinkAE.cpp
  ../../xbmc/cores/VideoPlayer/AudioSinkAE.h
  ../../xbmc/cores/VideoPlayer/DVDClock.cpp
  ../../xbmc/cores/VideoPlayer/DVDClock.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Audio/DVDAudioCodec.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Audio/DVDAudioCodecFFmpeg.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Audio/DVDAudioCodecFFmpeg.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Audio/DVDAudioCodecPassthrough.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Audio/DVDAudioCodecPassthrough.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/DVDCodecs.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/DVDCodecUtils.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/DVDCodecUtils.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/DVDFactoryCodec.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/DVDFactoryCodec.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/contrib/cc_decoder.c
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/contrib/cc_decoder.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/contrib/cc_decoder708.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/contrib/cc_decoder708.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlay.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodec.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodec.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecFFmpeg.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecFFmpeg.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecSSA.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecSSA.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecText.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecText.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecTX3G.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecTX3G.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayImage.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlaySpu.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlaySSA.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayText.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/AddonVideoCodec.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/AddonVideoCodec.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodec.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodec.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodecFFmpeg.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodecFFmpeg.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DVDVideoPPFFmpeg.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DVDVideoPPFFmpeg.h
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DXVA.cpp
  ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DXVA.h
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DemuxMultiSource.cpp
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DemuxMultiSource.h
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemux.cpp
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemux.h
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxBXA.cpp
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxBXA.h
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxCC.cpp
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxCC.h
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxCDDA.cpp
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxCDDA.h
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxClient.cpp
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxClient.h
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxFFmpeg.cpp
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxFFmpeg.h
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxUtils.cpp
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxUtils.h
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxVobsub.cpp
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxVobsub.h
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDFactoryDemuxer.cpp
  ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDFactoryDemuxer.h
  ../../xbmc/cores/VideoPlayer/DVDDemuxSPU.cpp
  ../../xbmc/cores/VideoPlayer/DVDDemuxSPU.h
  ../../xbmc/cores/VideoPlayer/DVDFileInfo.cpp
  ../../xbmc/cores/VideoPlayer/DVDFileInfo.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DllDvdNav.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DllDvdNav.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDFactoryInputStream.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDFactoryInputStream.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStream.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStream.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamBluray.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamBluray.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamFFmpeg.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamFFmpeg.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamFile.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamFile.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamMemory.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamMemory.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamNavigator.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamNavigator.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamStack.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamStack.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDStateSerializer.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDStateSerializer.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamAddon.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamAddon.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamMultiSource.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamMultiSource.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamMultiStreams.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamPVRBase.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamPVRBase.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamPVRChannel.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamPVRChannel.h
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamPVRRecording.cpp
  ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamPVRRecording.h
  ../../xbmc/cores/VideoPlayer/DVDMessage.cpp
  ../../xbmc/cores/VideoPlayer/DVDMessage.h
  ../../xbmc/cores/VideoPlayer/DVDMessageQueue.cpp
  ../../xbmc/cores/VideoPlayer/DVDMessageQueue.h
  ../../xbmc/cores/VideoPlayer/DVDOverlayContainer.cpp
  ../../xbmc/cores/VideoPlayer/DVDOverlayContainer.h
  ../../xbmc/cores/VideoPlayer/DVDResource.h
  ../../xbmc/cores/VideoPlayer/DVDStreamInfo.cpp
  ../../xbmc/cores/VideoPlayer/DVDStreamInfo.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DllLibass.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DllLibass.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDFactorySubtitle.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDFactorySubtitle.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleLineCollection.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleLineCollection.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParser.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserMicroDVD.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserMicroDVD.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserMPL2.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserMPL2.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserSami.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserSami.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserSSA.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserSSA.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserSubrip.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserSubrip.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserVplayer.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserVplayer.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitlesLibass.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitlesLibass.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleStream.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleStream.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleTagMicroDVD.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleTagMicroDVD.h
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleTagSami.cpp
  ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleTagSami.h
  ../../xbmc/cores/VideoPlayer/Edl.cpp
  ../../xbmc/cores/VideoPlayer/Edl.h
  ../../xbmc/cores/VideoPlayer/Interface/Addon/DemuxCrypto.h
  ../../xbmc/cores/VideoPlayer/Interface/Addon/DemuxPacket.h
  ../../xbmc/cores/VideoPlayer/Interface/Addon/TimingConstants.h
  ../../xbmc/cores/VideoPlayer/Interface/StreamInfo.h
  ../../xbmc/cores/VideoPlayer/IVideoPlayer.h
  ../../xbmc/cores/VideoPlayer/Process/ProcessInfo.cpp
  ../../xbmc/cores/VideoPlayer/Process/ProcessInfo.h
  ../../xbmc/cores/VideoPlayer/Process/VideoBuffer.cpp
  ../../xbmc/cores/VideoPlayer/Process/VideoBuffer.h
  ../../xbmc/cores/VideoPlayer/Process/windows/ProcessInfoWin.cpp
  ../../xbmc/cores/VideoPlayer/Process/windows/ProcessInfoWin.h
  ../../xbmc/cores/VideoPlayer/PTSTracker.cpp
  ../../xbmc/cores/VideoPlayer/PTSTracker.h
  ../../xbmc/cores/VideoPlayer/VideoPlayer.cpp
  ../../xbmc/cores/VideoPlayer/VideoPlayer.h
  ../../xbmc/cores/VideoPlayer/VideoPlayerAudio.cpp
  ../../xbmc/cores/VideoPlayer/VideoPlayerAudio.h
  ../../xbmc/cores/VideoPlayer/VideoPlayerRadioRDS.cpp
  ../../xbmc/cores/VideoPlayer/VideoPlayerRadioRDS.h
  ../../xbmc/cores/VideoPlayer/VideoPlayerSubtitle.cpp
  ../../xbmc/cores/VideoPlayer/VideoPlayerSubtitle.h
  ../../xbmc/cores/VideoPlayer/VideoPlayerTeletext.cpp
  ../../xbmc/cores/VideoPlayer/VideoPlayerTeletext.h
  ../../xbmc/cores/VideoPlayer/VideoPlayerVideo.cpp
  ../../xbmc/cores/VideoPlayer/VideoPlayerVideo.h
  ../../xbmc/cores/VideoPlayer/VideoReferenceClock.cpp
  ../../xbmc/cores/VideoPlayer/VideoReferenceClock.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/BaseRenderer.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/BaseRenderer.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/ColorManager.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/ColorManager.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/DebugRenderer.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/DebugRenderer.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/FrameBufferObject.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/FrameBufferObject.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/HwDecRender/DXVAHD.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/HwDecRender/DXVAHD.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRenderer.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRenderer.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRendererDX.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRendererDX.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRendererGUI.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRendererGUI.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRendererQt.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRendererQt.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRendererUtil.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRendererUtil.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderCapture.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderCapture.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/RendererQt.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/RendererQt.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderFactory.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderFactory.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderFlags.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderFlags.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderInfo.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderManager.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderManager.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/ConversionMatrix.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/ConversionMatrix.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/ConvolutionKernels.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/ConvolutionKernels.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/dither.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/ShaderFormats.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/VideoFilterShaderQt.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/VideoFilterShaderQt.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/WinVideoFilter.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/WinVideoFilter.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/YUV2RGBShaderQt.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/YUV2RGBShaderQt.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/YUVMatrix.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/YUVMatrix.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/WinRenderBuffer.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/WinRenderBuffer.h
  ../../xbmc/cores/VideoPlayer/VideoRenderers/WinRenderer.cpp
  ../../xbmc/cores/VideoPlayer/VideoRenderers/WinRenderer.h
  ../../xbmc/cores/VideoSettings.cpp
  ../../xbmc/cores/VideoSettings.h
  ../../xbmc/CueDocument.cpp
  ../../xbmc/CueDocument.h
  ../../xbmc/DatabaseManager.cpp
  ../../xbmc/DatabaseManager.h
  ../../xbmc/DbUrl.cpp
  ../../xbmc/DbUrl.h
  ../../xbmc/dbwrappers/Database.cpp
  ../../xbmc/dbwrappers/Database.h
  ../../xbmc/dbwrappers/DatabaseQuery.cpp
  ../../xbmc/dbwrappers/DatabaseQuery.h
  ../../xbmc/dbwrappers/dataset.cpp
  ../../xbmc/dbwrappers/dataset.h
  ../../xbmc/dbwrappers/mysqldataset.cpp
  ../../xbmc/dbwrappers/mysqldataset.h
  ../../xbmc/dbwrappers/qry_dat.cpp
  ../../xbmc/dbwrappers/qry_dat.h
  ../../xbmc/dbwrappers/sqlitedataset.cpp
  ../../xbmc/dbwrappers/sqlitedataset.h
  ../../xbmc/dialogs/GUIDialogBoxBase.cpp
  ../../xbmc/dialogs/GUIDialogBoxBase.h
  ../../xbmc/dialogs/GUIDialogBusy.cpp
  ../../xbmc/dialogs/GUIDialogBusy.h
  ../../xbmc/dialogs/GUIDialogBusyNoCancel.cpp
  ../../xbmc/dialogs/GUIDialogBusyNoCancel.h
  ../../xbmc/dialogs/GUIDialogButtonMenu.cpp
  ../../xbmc/dialogs/GUIDialogButtonMenu.h
  ../../xbmc/dialogs/GUIDialogCache.cpp
  ../../xbmc/dialogs/GUIDialogCache.h
  ../../xbmc/dialogs/GUIDialogContextMenu.cpp
  ../../xbmc/dialogs/GUIDialogContextMenu.h
  ../../xbmc/dialogs/GUIDialogExtendedProgressBar.cpp
  ../../xbmc/dialogs/GUIDialogExtendedProgressBar.h
  ../../xbmc/dialogs/GUIDialogFileBrowser.cpp
  ../../xbmc/dialogs/GUIDialogFileBrowser.h
  ../../xbmc/dialogs/GUIDialogGamepad.cpp
  ../../xbmc/dialogs/GUIDialogGamepad.h
  ../../xbmc/dialogs/GUIDialogKaiToast.cpp
  ../../xbmc/dialogs/GUIDialogKaiToast.h
  ../../xbmc/dialogs/GUIDialogKeyboardGeneric.cpp
  ../../xbmc/dialogs/GUIDialogKeyboardGeneric.h
  ../../xbmc/dialogs/GUIDialogKeyboardTouch.cpp
  ../../xbmc/dialogs/GUIDialogKeyboardTouch.h
  ../../xbmc/dialogs/GUIDialogMediaFilter.cpp
  ../../xbmc/dialogs/GUIDialogMediaFilter.h
  ../../xbmc/dialogs/GUIDialogMediaSource.cpp
  ../../xbmc/dialogs/GUIDialogMediaSource.h
  ../../xbmc/dialogs/GUIDialogNumeric.cpp
  ../../xbmc/dialogs/GUIDialogNumeric.h
  ../../xbmc/dialogs/GUIDialogOK.cpp
  ../../xbmc/dialogs/GUIDialogOK.h
  ../../xbmc/dialogs/GUIDialogPlayEject.cpp
  ../../xbmc/dialogs/GUIDialogPlayEject.h
  ../../xbmc/dialogs/GUIDialogPlayerControls.cpp
  ../../xbmc/dialogs/GUIDialogPlayerControls.h
  ../../xbmc/dialogs/GUIDialogPlayerProcessInfo.cpp
  ../../xbmc/dialogs/GUIDialogPlayerProcessInfo.h
  ../../xbmc/dialogs/GUIDialogProgress.cpp
  ../../xbmc/dialogs/GUIDialogProgress.h
  ../../xbmc/dialogs/GUIDialogSeekBar.cpp
  ../../xbmc/dialogs/GUIDialogSeekBar.h
  ../../xbmc/dialogs/GUIDialogSelect.cpp
  ../../xbmc/dialogs/GUIDialogSelect.h
  ../../xbmc/dialogs/GUIDialogSimpleMenu.cpp
  ../../xbmc/dialogs/GUIDialogSimpleMenu.h
  ../../xbmc/dialogs/GUIDialogSlider.cpp
  ../../xbmc/dialogs/GUIDialogSlider.h
  ../../xbmc/dialogs/GUIDialogSmartPlaylistEditor.cpp
  ../../xbmc/dialogs/GUIDialogSmartPlaylistEditor.h
  ../../xbmc/dialogs/GUIDialogSmartPlaylistRule.cpp
  ../../xbmc/dialogs/GUIDialogSmartPlaylistRule.h
  ../../xbmc/dialogs/GUIDialogSubMenu.cpp
  ../../xbmc/dialogs/GUIDialogSubMenu.h
  ../../xbmc/dialogs/GUIDialogTextViewer.cpp
  ../../xbmc/dialogs/GUIDialogTextViewer.h
  ../../xbmc/dialogs/GUIDialogVolumeBar.cpp
  ../../xbmc/dialogs/GUIDialogVolumeBar.h
  ../../xbmc/dialogs/GUIDialogYesNo.cpp
  ../../xbmc/dialogs/GUIDialogYesNo.h
  ../../xbmc/dialogs/IGUIVolumeBarCallback.h
  ../../xbmc/DllPaths.h
  ../../xbmc/DllPaths_win32.h
  ../../xbmc/drm/CryptoSession.cpp
  ../../xbmc/drm/CryptoSession.h
  ../../xbmc/DynamicDll.cpp
  ../../xbmc/DynamicDll.h
  ../../xbmc/events/AddonEvent.cpp
  ../../xbmc/events/AddonEvent.h
  ../../xbmc/events/AddonManagementEvent.cpp
  ../../xbmc/events/AddonManagementEvent.h
  ../../xbmc/events/BaseEvent.cpp
  ../../xbmc/events/BaseEvent.h
  ../../xbmc/events/EventLog.cpp
  ../../xbmc/events/EventLog.h
  ../../xbmc/events/EventLogManager.cpp
  ../../xbmc/events/EventLogManager.h
  ../../xbmc/events/IEvent.h
  ../../xbmc/events/MediaLibraryEvent.cpp
  ../../xbmc/events/MediaLibraryEvent.h
  ../../xbmc/events/NotificationEvent.h
  ../../xbmc/events/UniqueEvent.h
  ../../xbmc/events/windows/GUIViewStateEventLog.cpp
  ../../xbmc/events/windows/GUIViewStateEventLog.h
  ../../xbmc/events/windows/GUIWindowEventLog.cpp
  ../../xbmc/events/windows/GUIWindowEventLog.h
  ../../xbmc/favourites/ContextMenus.h
  ../../xbmc/favourites/ContextMenus3.cpp
  ../../xbmc/favourites/FavouritesService.cpp
  ../../xbmc/favourites/GUIDialogFavourites.cpp
  ../../xbmc/favourites/GUIDialogFavourites.h
  ../../xbmc/FileItem.cpp
  ../../xbmc/FileItem.h
  ../../xbmc/FileItemListModification.cpp
  ../../xbmc/FileItemListModification.h
  ../../xbmc/filesystem/AddonsDirectory.cpp
  ../../xbmc/filesystem/AddonsDirectory.h
  ../../xbmc/filesystem/AndroidAppDirectory.cpp
  ../../xbmc/filesystem/AndroidAppDirectory.h
  ../../xbmc/filesystem/AndroidAppFile.cpp
  ../../xbmc/filesystem/AndroidAppFile.h
  ../../xbmc/filesystem/APKDirectory.cpp
  ../../xbmc/filesystem/APKDirectory.h
  ../../xbmc/filesystem/APKFile.cpp
  ../../xbmc/filesystem/APKFile.h
  ../../xbmc/filesystem/AudioBookFileDirectory.cpp
  ../../xbmc/filesystem/AudioBookFileDirectory.h
  ../../xbmc/filesystem/BlurayCallback.cpp
  ../../xbmc/filesystem/BlurayCallback.h
  ../../xbmc/filesystem/BlurayDirectory.cpp
  ../../xbmc/filesystem/BlurayDirectory.h
  ../../xbmc/filesystem/BlurayFile.cpp
  ../../xbmc/filesystem/BlurayFile.h
  ../../xbmc/filesystem/CacheStrategy.cpp
  ../../xbmc/filesystem/CacheStrategy.h
  ../../xbmc/filesystem/CDDADirectory.cpp
  ../../xbmc/filesystem/CDDADirectory.h
  ../../xbmc/filesystem/CDDAFile.cpp
  ../../xbmc/filesystem/CDDAFile.h
  ../../xbmc/filesystem/CircularCache.cpp
  ../../xbmc/filesystem/CircularCache.h
  ../../xbmc/filesystem/CurlFile.cpp
  ../../xbmc/filesystem/CurlFile.h
  ../../xbmc/filesystem/DAVCommon.cpp
  ../../xbmc/filesystem/DAVCommon.h
  ../../xbmc/filesystem/DAVDirectory.cpp
  ../../xbmc/filesystem/DAVDirectory.h
  ../../xbmc/filesystem/DAVFile.cpp
  ../../xbmc/filesystem/DAVFile.h
  ../../xbmc/filesystem/Directorization.h
  ../../xbmc/filesystem/Directory.cpp
  ../../xbmc/filesystem/Directory.h
  ../../xbmc/filesystem/DirectoryCache.cpp
  ../../xbmc/filesystem/DirectoryCache.h
  ../../xbmc/filesystem/DirectoryFactory.cpp
  ../../xbmc/filesystem/DirectoryFactory.h
  ../../xbmc/filesystem/DirectoryHistory.cpp
  ../../xbmc/filesystem/DirectoryHistory.h
  ../../xbmc/filesystem/DllLibCurl.cpp
  ../../xbmc/filesystem/DllLibCurl.h
  ../../xbmc/filesystem/DllLibNfs.cpp
  ../../xbmc/filesystem/DllLibNfs.h
  ../../xbmc/filesystem/EventsDirectory.cpp
  ../../xbmc/filesystem/EventsDirectory.h
  ../../xbmc/filesystem/FavouritesDirectory.cpp
  ../../xbmc/filesystem/FavouritesDirectory.h
  ../../xbmc/filesystem/File.h
  ../../xbmc/filesystem/File2.cpp
  ../../xbmc/filesystem/FileCache.cpp
  ../../xbmc/filesystem/FileCache.h
  ../../xbmc/filesystem/FileDirectoryFactory.cpp
  ../../xbmc/filesystem/FileDirectoryFactory.h
  ../../xbmc/filesystem/FileFactory.cpp
  ../../xbmc/filesystem/FileFactory.h
  ../../xbmc/filesystem/FTPDirectory.cpp
  ../../xbmc/filesystem/FTPDirectory.h
  ../../xbmc/filesystem/FTPParse.cpp
  ../../xbmc/filesystem/FTPParse.h
  ../../xbmc/filesystem/HTTPDirectory.cpp
  ../../xbmc/filesystem/HTTPDirectory.h
  ../../xbmc/filesystem/IDirectory.cpp
  ../../xbmc/filesystem/IDirectory.h
  ../../xbmc/filesystem/IFile.cpp
  ../../xbmc/filesystem/IFile.h
  ../../xbmc/filesystem/IFileDirectory.h
  ../../xbmc/filesystem/IFileTypes.h
  ../../xbmc/filesystem/ImageFile.cpp
  ../../xbmc/filesystem/ImageFile.h
  ../../xbmc/filesystem/iso9660.cpp
  ../../xbmc/filesystem/iso9660.h
  ../../xbmc/filesystem/ISO9660Directory.cpp
  ../../xbmc/filesystem/ISO9660Directory.h
  ../../xbmc/filesystem/ISOFile.cpp
  ../../xbmc/filesystem/ISOFile.h
  ../../xbmc/filesystem/LibraryDirectory.cpp
  ../../xbmc/filesystem/LibraryDirectory.h
  ../../xbmc/filesystem/MultiPathDirectory.cpp
  ../../xbmc/filesystem/MultiPathDirectory.h
  ../../xbmc/filesystem/MultiPathFile.cpp
  ../../xbmc/filesystem/MultiPathFile.h
  ../../xbmc/filesystem/MusicDatabaseDirectory.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNode.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNode2.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbum.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbum.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumCompilations.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumCompilations.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumCompilationsSongs.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumCompilationsSongs.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyAdded.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyAdded.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyAddedSong.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyAddedSong.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyPlayed.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyPlayed.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyPlayedSong.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyPlayedSong.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumTop100.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumTop100.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumTop100Song.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumTop100Song.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeArtist.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeArtist.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeGrouped.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeGroupedMusic.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeMusic.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeMusic.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeOverview.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeOverviewMusic.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeRoot.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSingles.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSingles.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSong.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSong.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSongTop100.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSongTop100.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeTop100.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeTop100.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeYearAlbum.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeYearAlbum.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeYearSong.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeYearSong.h
  ../../xbmc/filesystem/MusicDatabaseDirectory/QueryParamsMusic.cpp
  ../../xbmc/filesystem/MusicDatabaseDirectory/QueryParamsMusic.h
  ../../xbmc/filesystem/MusicDatabaseFile.cpp
  ../../xbmc/filesystem/MusicDatabaseFile.h
  ../../xbmc/filesystem/MusicFileDirectory.cpp
  ../../xbmc/filesystem/MusicFileDirectory.h
  ../../xbmc/filesystem/MusicSearchDirectory.cpp
  ../../xbmc/filesystem/MusicSearchDirectory.h
  ../../xbmc/filesystem/NFSDirectory.cpp
  ../../xbmc/filesystem/NFSDirectory.h
  ../../xbmc/filesystem/NFSFile.cpp
  ../../xbmc/filesystem/NFSFile.h
  ../../xbmc/filesystem/NptXbmcFile.cpp
  ../../xbmc/filesystem/OverrideDirectory.cpp
  ../../xbmc/filesystem/OverrideDirectory.h
  ../../xbmc/filesystem/OverrideFile.cpp
  ../../xbmc/filesystem/OverrideFile.h
  ../../xbmc/filesystem/PipeFile.cpp
  ../../xbmc/filesystem/PipeFile.h
  ../../xbmc/filesystem/PipesManager.cpp
  ../../xbmc/filesystem/PipesManager.h
  ../../xbmc/filesystem/PlaylistDirectory.cpp
  ../../xbmc/filesystem/PlaylistDirectory.h
  ../../xbmc/filesystem/PlaylistFileDirectory.cpp
  ../../xbmc/filesystem/PlaylistFileDirectory.h
  ../../xbmc/filesystem/PluginDirectory.cpp
  ../../xbmc/filesystem/PluginDirectory.h
  ../../xbmc/filesystem/PVRDirectory.cpp
  ../../xbmc/filesystem/PVRDirectory.h
  ../../xbmc/filesystem/ResourceDirectory.cpp
  ../../xbmc/filesystem/ResourceDirectory.h
  ../../xbmc/filesystem/ResourceFile.cpp
  ../../xbmc/filesystem/ResourceFile.h
  ../../xbmc/filesystem/RSSDirectory.cpp
  ../../xbmc/filesystem/RSSDirectory.h
  ../../xbmc/filesystem/ShoutcastFile.cpp
  ../../xbmc/filesystem/ShoutcastFile.h
  ../../xbmc/filesystem/SmartPlaylistDirectory.cpp
  ../../xbmc/filesystem/SmartPlaylistDirectory.h
  ../../xbmc/filesystem/SMBDirectory.h
  ../../xbmc/filesystem/SMBFile.h
  ../../xbmc/filesystem/SourcesDirectory.cpp
  ../../xbmc/filesystem/SourcesDirectory.h
  ../../xbmc/filesystem/SpecialProtocol.cpp
  ../../xbmc/filesystem/SpecialProtocol.h
  ../../xbmc/filesystem/SpecialProtocolDirectory.cpp
  ../../xbmc/filesystem/SpecialProtocolDirectory.h
  ../../xbmc/filesystem/SpecialProtocolFile.cpp
  ../../xbmc/filesystem/SpecialProtocolFile.h
  ../../xbmc/filesystem/StackDirectory.cpp
  ../../xbmc/filesystem/StackDirectory.h
  ../../xbmc/filesystem/udf25.cpp
  ../../xbmc/filesystem/udf25.h
  ../../xbmc/filesystem/UDFDirectory.cpp
  ../../xbmc/filesystem/UDFDirectory.h
  ../../xbmc/filesystem/UDFFile.cpp
  ../../xbmc/filesystem/UDFFile.h
  ../../xbmc/filesystem/UPnPDirectory.cpp
  ../../xbmc/filesystem/UPnPDirectory.h
  ../../xbmc/filesystem/UPnPFile.cpp
  ../../xbmc/filesystem/UPnPFile.h
  ../../xbmc/filesystem/VideoDatabaseDirectory.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeEpisodes.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeEpisodes.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeGrouped.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeGroupedVideo.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeInProgressTvShows.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeInProgressTvShows.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeMoviesOverview.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeMoviesOverview.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeMusicVideosOverview.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeMusicVideosOverview.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeOverviewVideo.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeOverviewVideo.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedEpisodes.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedEpisodes.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedMovies.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedMovies.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedMusicVideos.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedMusicVideos.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRoot.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRoot.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeSeasons.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeSeasons.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleMovies.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleMovies.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleMusicVideos.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleMusicVideos.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleTvShows.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleTvShows.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTvShowsOverview.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTvShowsOverview.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeVideo.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeVideo.h
  ../../xbmc/filesystem/VideoDatabaseDirectory/QueryParamsVideo.cpp
  ../../xbmc/filesystem/VideoDatabaseDirectory/QueryParamsVideo.h
  ../../xbmc/filesystem/VideoDatabaseFile.cpp
  ../../xbmc/filesystem/VideoDatabaseFile.h
  ../../xbmc/filesystem/VirtualDirectory.cpp
  ../../xbmc/filesystem/VirtualDirectory.h
  ../../xbmc/filesystem/XbtDirectory.cpp
  ../../xbmc/filesystem/XbtDirectory.h
  ../../xbmc/filesystem/XbtFile.cpp
  ../../xbmc/filesystem/XbtFile.h
  ../../xbmc/filesystem/XbtManager.cpp
  ../../xbmc/filesystem/XbtManager.h
  ../../xbmc/filesystem/ZeroconfDirectory.cpp
  ../../xbmc/filesystem/ZeroconfDirectory.h
  ../../xbmc/filesystem/ZipDirectory.cpp
  ../../xbmc/filesystem/ZipDirectory.h
  ../../xbmc/filesystem/ZipFile.cpp
  ../../xbmc/filesystem/ZipFile.h
  ../../xbmc/filesystem/ZipManager.cpp
  ../../xbmc/filesystem/ZipManager.h
  ../../xbmc/fmt/format.cpp
  ../../xbmc/fmt/format.h
  ../../xbmc/fmt/locale.h
  ../../xbmc/fmt/ostream.cpp
  ../../xbmc/fmt/ostream.h
  ../../xbmc/fmt/posix.cpp
  ../../xbmc/fmt/posix.h
  ../../xbmc/fmt/time.h
  ../../xbmc/games/addons/GameClient.cpp
  ../../xbmc/games/addons/GameClient.h
  ../../xbmc/games/addons/GameClientCallbacks.h
  ../../xbmc/games/addons/GameClientInGameSaves.cpp
  ../../xbmc/games/addons/GameClientInGameSaves.h
  ../../xbmc/games/addons/GameClientProperties.cpp
  ../../xbmc/games/addons/GameClientProperties.h
  ../../xbmc/games/addons/GameClientSubsystem.cpp
  ../../xbmc/games/addons/GameClientSubsystem.h
  ../../xbmc/games/addons/GameClientTranslator.cpp
  ../../xbmc/games/addons/GameClientTranslator.h
  ../../xbmc/games/addons/input/GameClientController.cpp
  ../../xbmc/games/addons/input/GameClientController.h
  ../../xbmc/games/addons/input/GameClientDevice.cpp
  ../../xbmc/games/addons/input/GameClientDevice.h
  ../../xbmc/games/addons/input/GameClientHardware.cpp
  ../../xbmc/games/addons/input/GameClientHardware.h
  ../../xbmc/games/addons/input/GameClientInput.cpp
  ../../xbmc/games/addons/input/GameClientInput.h
  ../../xbmc/games/addons/input/GameClientJoystick.cpp
  ../../xbmc/games/addons/input/GameClientJoystick.h
  ../../xbmc/games/addons/input/GameClientKeyboard.cpp
  ../../xbmc/games/addons/input/GameClientKeyboard.h
  ../../xbmc/games/addons/input/GameClientMouse.cpp
  ../../xbmc/games/addons/input/GameClientMouse.h
  ../../xbmc/games/addons/input/GameClientPort.cpp
  ../../xbmc/games/addons/input/GameClientPort.h
  ../../xbmc/games/addons/input/GameClientTopology.cpp
  ../../xbmc/games/addons/input/GameClientTopology.h
  ../../xbmc/games/addons/streams/GameClientStreamAudio.cpp
  ../../xbmc/games/addons/streams/GameClientStreamAudio.h
  ../../xbmc/games/addons/streams/GameClientStreams.cpp
  ../../xbmc/games/addons/streams/GameClientStreams.h
  ../../xbmc/games/addons/streams/GameClientStreamSwFramebuffer.cpp
  ../../xbmc/games/addons/streams/GameClientStreamSwFramebuffer.h
  ../../xbmc/games/addons/streams/GameClientStreamVideo.cpp
  ../../xbmc/games/addons/streams/GameClientStreamVideo.h
  ../../xbmc/games/addons/streams/IGameClientStream.h
  ../../xbmc/games/controllers/Controller.cpp
  ../../xbmc/games/controllers/Controller.h
  ../../xbmc/games/controllers/ControllerDefinitions.h
  ../../xbmc/games/controllers/ControllerFeature.cpp
  ../../xbmc/games/controllers/ControllerFeature.h
  ../../xbmc/games/controllers/ControllerIDs.h
  ../../xbmc/games/controllers/ControllerLayout.cpp
  ../../xbmc/games/controllers/ControllerLayout.h
  ../../xbmc/games/controllers/ControllerManager.cpp
  ../../xbmc/games/controllers/ControllerManager.h
  ../../xbmc/games/controllers/ControllerTopology.cpp
  ../../xbmc/games/controllers/ControllerTopology.h
  ../../xbmc/games/controllers/ControllerTranslator.cpp
  ../../xbmc/games/controllers/ControllerTranslator.h
  ../../xbmc/games/controllers/ControllerTypes.h
  ../../xbmc/games/controllers/dialogs/GUIDialogAxisDetection.cpp
  ../../xbmc/games/controllers/dialogs/GUIDialogAxisDetection.h
  ../../xbmc/games/controllers/dialogs/GUIDialogButtonCapture.cpp
  ../../xbmc/games/controllers/dialogs/GUIDialogButtonCapture.h
  ../../xbmc/games/controllers/dialogs/GUIDialogIgnoreInput.cpp
  ../../xbmc/games/controllers/dialogs/GUIDialogIgnoreInput.h
  ../../xbmc/games/controllers/guicontrols/GUICardinalFeatureButton.cpp
  ../../xbmc/games/controllers/guicontrols/GUICardinalFeatureButton.h
  ../../xbmc/games/controllers/guicontrols/GUIControllerButton.cpp
  ../../xbmc/games/controllers/guicontrols/GUIControllerButton.h
  ../../xbmc/games/controllers/guicontrols/GUIControlTypes.h
  ../../xbmc/games/controllers/guicontrols/GUIFeatureButton.cpp
  ../../xbmc/games/controllers/guicontrols/GUIFeatureButton.h
  ../../xbmc/games/controllers/guicontrols/GUIFeatureControls.cpp
  ../../xbmc/games/controllers/guicontrols/GUIFeatureControls.h
  ../../xbmc/games/controllers/guicontrols/GUIFeatureFactory.cpp
  ../../xbmc/games/controllers/guicontrols/GUIFeatureFactory.h
  ../../xbmc/games/controllers/guicontrols/GUIFeatureTranslator.cpp
  ../../xbmc/games/controllers/guicontrols/GUIFeatureTranslator.h
  ../../xbmc/games/controllers/guicontrols/GUIGameController.cpp
  ../../xbmc/games/controllers/guicontrols/GUIGameController.h
  ../../xbmc/games/controllers/guicontrols/GUIScalarFeatureButton.cpp
  ../../xbmc/games/controllers/guicontrols/GUIScalarFeatureButton.h
  ../../xbmc/games/controllers/guicontrols/GUISelectKeyButton.cpp
  ../../xbmc/games/controllers/guicontrols/GUISelectKeyButton.h
  ../../xbmc/games/controllers/guicontrols/GUIThrottleButton.cpp
  ../../xbmc/games/controllers/guicontrols/GUIThrottleButton.h
  ../../xbmc/games/controllers/guicontrols/GUIWheelButton.cpp
  ../../xbmc/games/controllers/guicontrols/GUIWheelButton.h
  ../../xbmc/games/controllers/types/ControllerGrid.cpp
  ../../xbmc/games/controllers/types/ControllerGrid.h
  ../../xbmc/games/controllers/types/ControllerTree.cpp
  ../../xbmc/games/controllers/types/ControllerTree.h
  ../../xbmc/games/controllers/windows/ControllerInstaller.cpp
  ../../xbmc/games/controllers/windows/ControllerInstaller.h
  ../../xbmc/games/controllers/windows/GUIConfigurationWizard.cpp
  ../../xbmc/games/controllers/windows/GUIConfigurationWizard.h
  ../../xbmc/games/controllers/windows/GUIControllerDefines.h
  ../../xbmc/games/controllers/windows/GUIControllerList.cpp
  ../../xbmc/games/controllers/windows/GUIControllerList.h
  ../../xbmc/games/controllers/windows/GUIControllerWindow.cpp
  ../../xbmc/games/controllers/windows/GUIControllerWindow.h
  ../../xbmc/games/controllers/windows/GUIFeatureList.cpp
  ../../xbmc/games/controllers/windows/GUIFeatureList.h
  ../../xbmc/games/controllers/windows/IConfigurationWindow.h
  ../../xbmc/games/dialogs/GUIDialogSelectGameClient.cpp
  ../../xbmc/games/dialogs/GUIDialogSelectGameClient.h
  ../../xbmc/games/dialogs/osd/DialogGameAdvancedSettings.cpp
  ../../xbmc/games/dialogs/osd/DialogGameAdvancedSettings.h
  ../../xbmc/games/dialogs/osd/DialogGameOSD.cpp
  ../../xbmc/games/dialogs/osd/DialogGameOSD.h
  ../../xbmc/games/dialogs/osd/DialogGameOSDHelp.cpp
  ../../xbmc/games/dialogs/osd/DialogGameOSDHelp.h
  ../../xbmc/games/dialogs/osd/DialogGameStretchMode.cpp
  ../../xbmc/games/dialogs/osd/DialogGameStretchMode.h
  ../../xbmc/games/dialogs/osd/DialogGameVideoFilter.cpp
  ../../xbmc/games/dialogs/osd/DialogGameVideoFilter.h
  ../../xbmc/games/dialogs/osd/DialogGameVideoRotation.cpp
  ../../xbmc/games/dialogs/osd/DialogGameVideoRotation.h
  ../../xbmc/games/dialogs/osd/DialogGameVideoSelect.cpp
  ../../xbmc/games/dialogs/osd/DialogGameVideoSelect.h
  ../../xbmc/games/dialogs/osd/DialogGameVolume.cpp
  ../../xbmc/games/dialogs/osd/DialogGameVolume.h
  ../../xbmc/games/GameServices.cpp
  ../../xbmc/games/GameServices.h
  ../../xbmc/games/GameSettings.h
  ../../xbmc/games/GameSettings2.cpp
  ../../xbmc/games/GameTypes.h
  ../../xbmc/games/GameUtils.cpp
  ../../xbmc/games/GameUtils.h
  ../../xbmc/games/ports/InputSink.cpp
  ../../xbmc/games/ports/InputSink.h
  ../../xbmc/games/ports/Port.cpp
  ../../xbmc/games/ports/Port.h
  ../../xbmc/games/tags/GameInfoTag.cpp
  ../../xbmc/games/tags/GameInfoTag.h
  ../../xbmc/games/windows/GUIViewStateWindowGames.cpp
  ../../xbmc/games/windows/GUIViewStateWindowGames.h
  ../../xbmc/games/windows/GUIWindowGames.cpp
  ../../xbmc/games/windows/GUIWindowGames.h
  ../../xbmc/GoTvCoreUtil.cpp
  ../../xbmc/GoTvCoreUtil.h
  ../../xbmc/GoTvUrl.cpp
  ../../xbmc/GoTvUrl.h
  ../../xbmc/guiinfo/GUIInfoLabels.h
  ../../xbmc/GUIInfoManager.cpp
  ../../xbmc/GUIInfoManager.h
  ../../xbmc/GUILargeTextureManager.cpp
  ../../xbmc/GUILargeTextureManager.h
  ../../xbmc/guilib/D3DResource.cpp
  ../../xbmc/guilib/D3DResource.h
  ../../xbmc/guilib/DDSImage.cpp
  ../../xbmc/guilib/DDSImage.h
  ../../xbmc/guilib/DirectXGraphics.cpp
  ../../xbmc/guilib/DirectXGraphics.h
  ../../xbmc/guilib/DirtyRegion.h
  ../../xbmc/guilib/DirtyRegionSolvers.cpp
  ../../xbmc/guilib/DirtyRegionSolvers.h
  ../../xbmc/guilib/DirtyRegionTracker.cpp
  ../../xbmc/guilib/DirtyRegionTracker.h
  ../../xbmc/guilib/DispResource.h
  ../../xbmc/guilib/FFmpegImage.cpp
  ../../xbmc/guilib/FFmpegImage.h
  ../../xbmc/guilib/gui3d.h
  ../../xbmc/guilib/GUIAction.cpp
  ../../xbmc/guilib/GUIAction.h
  ../../xbmc/guilib/GUIAudioManager.cpp
  ../../xbmc/guilib/GUIAudioManager.h
  ../../xbmc/guilib/GUIBaseContainer.cpp
  ../../xbmc/guilib/GUIBaseContainer.h
  ../../xbmc/guilib/GUIBorderedImage.cpp
  ../../xbmc/guilib/GUIBorderedImage.h
  ../../xbmc/guilib/GUIButtonControl.cpp
  ../../xbmc/guilib/GUIButtonControl.h
  ../../xbmc/guilib/GUICallback.h
  ../../xbmc/guilib/GUIColorManager.cpp
  ../../xbmc/guilib/GUIColorManager.h
  ../../xbmc/guilib/GUIComponent.cpp
  ../../xbmc/guilib/GUIComponent.h
  ../../xbmc/guilib/GUIControl.cpp
  ../../xbmc/guilib/GUIControl.h
  ../../xbmc/guilib/GUIControlFactory.cpp
  ../../xbmc/guilib/GUIControlFactory.h
  ../../xbmc/guilib/GUIControlGroup.cpp
  ../../xbmc/guilib/GUIControlGroup.h
  ../../xbmc/guilib/GUIControlGroupList.cpp
  ../../xbmc/guilib/GUIControlGroupList.h
  ../../xbmc/guilib/GUIControlLookup.cpp
  ../../xbmc/guilib/GUIControlLookup.h
  ../../xbmc/guilib/GUIControlProfiler.cpp
  ../../xbmc/guilib/GUIControlProfiler.h
  ../../xbmc/guilib/GUIDialog.cpp
  ../../xbmc/guilib/GUIDialog.h
  ../../xbmc/guilib/GUIEditControl.cpp
  ../../xbmc/guilib/GUIEditControl.h
  ../../xbmc/guilib/GUIFadeLabelControl.cpp
  ../../xbmc/guilib/GUIFadeLabelControl.h
  ../../xbmc/guilib/GUIFixedListContainer.cpp
  ../../xbmc/guilib/GUIFixedListContainer.h
  ../../xbmc/guilib/GUIFont.cpp
  ../../xbmc/guilib/GUIFont.h
  ../../xbmc/guilib/GUIFontCache.cpp
  ../../xbmc/guilib/GUIFontCache.h
  ../../xbmc/guilib/GUIFontManager.cpp
  ../../xbmc/guilib/GUIFontManager.h
  ../../xbmc/guilib/GUIFontTTF.cpp
  ../../xbmc/guilib/GUIFontTTF.h
  ../../xbmc/guilib/GUIFontTTFDX.cpp
  ../../xbmc/guilib/GUIFontTTFDX.h
  ../../xbmc/guilib/GUIFontTTFGL.cpp
  ../../xbmc/guilib/GUIFontTTFGL.h
  ../../xbmc/guilib/GUIFontTTFQt.cpp
  ../../xbmc/guilib/GUIFontTTFQt.h
  ../../xbmc/guilib/GUIImage.cpp
  ../../xbmc/guilib/GUIImage.h
  ../../xbmc/guilib/GUIIncludes.cpp
  ../../xbmc/guilib/GUIIncludes.h
  ../../xbmc/guilib/guiinfo/AddonsGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/AddonsGUIInfo.h
  ../../xbmc/guilib/guiinfo/GamesGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/GamesGUIInfo.h
  ../../xbmc/guilib/guiinfo/GUIControlsGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/GUIControlsGUIInfo.h
  ../../xbmc/guilib/guiinfo/GUIInfo.cpp
  ../../xbmc/guilib/guiinfo/GUIInfo.h
  ../../xbmc/guilib/guiinfo/GUIInfoBool.cpp
  ../../xbmc/guilib/guiinfo/GUIInfoBool.h
  ../../xbmc/guilib/guiinfo/GUIInfoColor.cpp
  ../../xbmc/guilib/guiinfo/GUIInfoColor.h
  ../../xbmc/guilib/guiinfo/GUIInfoHelper.cpp
  ../../xbmc/guilib/guiinfo/GUIInfoHelper.h
  ../../xbmc/guilib/guiinfo/GUIInfoLabel.cpp
  ../../xbmc/guilib/guiinfo/GUIInfoLabel.h
  ../../xbmc/guilib/guiinfo/GUIInfoLabels.h
  ../../xbmc/guilib/guiinfo/GUIInfoProvider.h
  ../../xbmc/guilib/guiinfo/GUIInfoProviders.cpp
  ../../xbmc/guilib/guiinfo/GUIInfoProviders.h
  ../../xbmc/guilib/guiinfo/IGUIInfoProvider.h
  ../../xbmc/guilib/guiinfo/LibraryGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/LibraryGUIInfo.h
  ../../xbmc/guilib/guiinfo/MusicGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/MusicGUIInfo.h
  ../../xbmc/guilib/guiinfo/PicturesGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/PicturesGUIInfo.h
  ../../xbmc/guilib/guiinfo/PlayerGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/PlayerGUIInfo.h
  ../../xbmc/guilib/guiinfo/SkinGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/SkinGUIInfo.h
  ../../xbmc/guilib/guiinfo/SystemGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/SystemGUIInfo.h
  ../../xbmc/guilib/guiinfo/VideoGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/VideoGUIInfo.h
  ../../xbmc/guilib/guiinfo/VisualisationGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/VisualisationGUIInfo.h
  ../../xbmc/guilib/guiinfo/WeatherGUIInfo.cpp
  ../../xbmc/guilib/guiinfo/WeatherGUIInfo.h
  ../../xbmc/guilib/GUIKeyboard.h
  ../../xbmc/guilib/GUIKeyboardFactory.cpp
  ../../xbmc/guilib/GUIKeyboardFactory.h
  ../../xbmc/guilib/GUILabel.cpp
  ../../xbmc/guilib/GUILabel.h
  ../../xbmc/guilib/GUILabelControl.cpp
  ../../xbmc/guilib/GUILabelControl.h
  ../../xbmc/guilib/GUIListContainer.cpp
  ../../xbmc/guilib/GUIListContainer.h
  ../../xbmc/guilib/GUIListGroup.cpp
  ../../xbmc/guilib/GUIListGroup.h
  ../../xbmc/guilib/GUIListItem.cpp
  ../../xbmc/guilib/GUIListItem.h
  ../../xbmc/guilib/GUIListItemLayout.cpp
  ../../xbmc/guilib/GUIListItemLayout.h
  ../../xbmc/guilib/GUIListLabel.cpp
  ../../xbmc/guilib/GUIListLabel.h
  ../../xbmc/guilib/GUIMessage.cpp
  ../../xbmc/guilib/GUIMessage.h
  ../../xbmc/guilib/GUIMoverControl.cpp
  ../../xbmc/guilib/GUIMoverControl.h
  ../../xbmc/guilib/GUIMultiImage.cpp
  ../../xbmc/guilib/GUIMultiImage.h
  ../../xbmc/guilib/GUIPanelContainer.cpp
  ../../xbmc/guilib/GUIPanelContainer.h
  ../../xbmc/guilib/GUIProgressControl.cpp
  ../../xbmc/guilib/GUIProgressControl.h
  ../../xbmc/guilib/GUIRadioButtonControl.cpp
  ../../xbmc/guilib/GUIRadioButtonControl.h
  ../../xbmc/guilib/GUIRenderingControl.cpp
  ../../xbmc/guilib/GUIRenderingControl.h
  ../../xbmc/guilib/GUIResizeControl.cpp
  ../../xbmc/guilib/GUIResizeControl.h
  ../../xbmc/guilib/GUIRSSControl.cpp
  ../../xbmc/guilib/GUIRSSControl.h
  ../../xbmc/guilib/GUIScrollBarControl.cpp
  ../../xbmc/guilib/GUIScrollBarControl.h
  ../../xbmc/guilib/GUISettingsSliderControl.cpp
  ../../xbmc/guilib/GUISettingsSliderControl.h
  ../../xbmc/guilib/GUIShaderDX.cpp
  ../../xbmc/guilib/GUIShaderDX.h
  ../../xbmc/guilib/GUISliderControl.cpp
  ../../xbmc/guilib/GUISliderControl.h
  ../../xbmc/guilib/GUISpinControl.cpp
  ../../xbmc/guilib/GUISpinControl.h
  ../../xbmc/guilib/GUISpinControlEx.cpp
  ../../xbmc/guilib/GUISpinControlEx.h
  ../../xbmc/guilib/GUIStaticItem.cpp
  ../../xbmc/guilib/GUIStaticItem.h
  ../../xbmc/guilib/GUITextBox.cpp
  ../../xbmc/guilib/GUITextBox.h
  ../../xbmc/guilib/GUITextLayout.cpp
  ../../xbmc/guilib/GUITextLayout.h
  ../../xbmc/guilib/GUITexture.cpp
  ../../xbmc/guilib/GUITexture.h
  ../../xbmc/guilib/GUITextureD3D.cpp
  ../../xbmc/guilib/GUITextureD3D.h
  ../../xbmc/guilib/GUITextureGL.cpp
  ../../xbmc/guilib/GUITextureGL.h
  ../../xbmc/guilib/GUITextureGLES.cpp
  ../../xbmc/guilib/GUITextureGLES.h
  ../../xbmc/guilib/GUITextureQt.cpp
  ../../xbmc/guilib/GUITextureQt.h
  ../../xbmc/guilib/GUIToggleButtonControl.cpp
  ../../xbmc/guilib/GUIToggleButtonControl.h
  ../../xbmc/guilib/GUIVideoControl.cpp
  ../../xbmc/guilib/GUIVideoControl.h
  ../../xbmc/guilib/GUIVisualisationControl.cpp
  ../../xbmc/guilib/GUIVisualisationControl.h
  ../../xbmc/guilib/GUIWindow.cpp
  ../../xbmc/guilib/GUIWindow.h
  ../../xbmc/guilib/GUIWindowManager.cpp
  ../../xbmc/guilib/GUIWindowManager.h
  ../../xbmc/guilib/GUIWrappingListContainer.cpp
  ../../xbmc/guilib/GUIWrappingListContainer.h
  ../../xbmc/guilib/IAudioDeviceChangedCallback.h
  ../../xbmc/guilib/IDirtyRegionSolver.h
  ../../xbmc/guilib/IGUIContainer.h
  ../../xbmc/guilib/iimage.h
  ../../xbmc/guilib/imagefactory.cpp
  ../../xbmc/guilib/imagefactory.h
  ../../xbmc/guilib/IMsgTargetCallback.h
  ../../xbmc/guilib/IRenderingCallback.h
  ../../xbmc/guilib/ISliderCallback.h
  ../../xbmc/guilib/IWindowManagerCallback.cpp
  ../../xbmc/guilib/IWindowManagerCallback.h
  ../../xbmc/guilib/LocalizeStrings.cpp
  ../../xbmc/guilib/LocalizeStrings.h
  ../../xbmc/guilib/Shader.cpp
  ../../xbmc/guilib/Shader.h
  ../../xbmc/guilib/StereoscopicsManager.cpp
  ../../xbmc/guilib/StereoscopicsManager.h
  ../../xbmc/guilib/Texture.cpp
  ../../xbmc/guilib/Texture.h
  ../../xbmc/guilib/TextureBundle.cpp
  ../../xbmc/guilib/TextureBundle.h
  ../../xbmc/guilib/TextureBundleXBT.cpp
  ../../xbmc/guilib/TextureBundleXBT.h
  ../../xbmc/guilib/TextureDX.cpp
  ../../xbmc/guilib/TextureDX.h
  ../../xbmc/guilib/TextureFormats.h
  ../../xbmc/guilib/TextureGL.cpp
  ../../xbmc/guilib/TextureGL.h
  ../../xbmc/guilib/TextureManager.cpp
  ../../xbmc/guilib/TextureManager.h
  ../../xbmc/guilib/TexturePi.cpp
  ../../xbmc/guilib/TexturePi.h
  ../../xbmc/guilib/TextureQt.cpp
  ../../xbmc/guilib/TextureQt.h
  ../../xbmc/guilib/TransformMatrix.h
  ../../xbmc/guilib/Tween.h
  ../../xbmc/guilib/VisibleEffect.cpp
  ../../xbmc/guilib/VisibleEffect.h
  ../../xbmc/guilib/WindowIDs.h
  ../../xbmc/guilib/XBTF.cpp
  ../../xbmc/guilib/XBTF.h
  ../../xbmc/guilib/XBTFReader.cpp
  ../../xbmc/guilib/XBTFReader.h
  ../../xbmc/GUIPassword.cpp
  ../../xbmc/GUIPassword.h
  ../../xbmc/GUIUserMessages.h
  ../../xbmc/IFileItemListModifier.h
  ../../xbmc/InfoScanner.cpp
  ../../xbmc/InfoScanner.h
  ../../xbmc/input/actions/Action.cpp
  ../../xbmc/input/actions/Action.h
  ../../xbmc/input/actions/ActionIDs.h
  ../../xbmc/input/actions/ActionTranslator.cpp
  ../../xbmc/input/actions/ActionTranslator.h
  ../../xbmc/input/AppTranslator.cpp
  ../../xbmc/input/AppTranslator.h
  ../../xbmc/input/ButtonTranslator.cpp
  ../../xbmc/input/ButtonTranslator.h
  ../../xbmc/input/CustomControllerTranslator.cpp
  ../../xbmc/input/CustomControllerTranslator.h
  ../../xbmc/input/GamepadTranslator.cpp
  ../../xbmc/input/GamepadTranslator.h
  ../../xbmc/input/hardware/IHardwareInput.h
  ../../xbmc/input/IButtonMapper.h
  ../../xbmc/input/IKeymap.h
  ../../xbmc/input/IKeymapEnvironment.h
  ../../xbmc/input/InertialScrollingHandler.cpp
  ../../xbmc/input/InertialScrollingHandler.h
  ../../xbmc/input/InputCodingTable.h
  ../../xbmc/input/InputCodingTableBaiduPY.cpp
  ../../xbmc/input/InputCodingTableBaiduPY.h
  ../../xbmc/input/InputCodingTableBasePY.cpp
  ../../xbmc/input/InputCodingTableBasePY.h
  ../../xbmc/input/InputCodingTableFactory.cpp
  ../../xbmc/input/InputCodingTableFactory.h
  ../../xbmc/input/InputCodingTableKorean.cpp
  ../../xbmc/input/InputCodingTableKorean.h
  ../../xbmc/input/InputManager.cpp
  ../../xbmc/input/InputManager.h
  ../../xbmc/input/InputTranslator.cpp
  ../../xbmc/input/InputTranslator.h
  ../../xbmc/input/InputTypes.h
  ../../xbmc/input/IRTranslator.cpp
  ../../xbmc/input/IRTranslator.h
  ../../xbmc/input/JoystickMapper.cpp
  ../../xbmc/input/JoystickMapper.h
  ../../xbmc/input/joysticks/DeadzoneFilter.cpp
  ../../xbmc/input/joysticks/DeadzoneFilter.h
  ../../xbmc/input/joysticks/dialogs/GUIDialogNewJoystick.cpp
  ../../xbmc/input/joysticks/dialogs/GUIDialogNewJoystick.h
  ../../xbmc/input/joysticks/DriverPrimitive.cpp
  ../../xbmc/input/joysticks/DriverPrimitive.h
  ../../xbmc/input/joysticks/generic/ButtonMapping.cpp
  ../../xbmc/input/joysticks/generic/ButtonMapping.h
  ../../xbmc/input/joysticks/generic/DriverReceiving.cpp
  ../../xbmc/input/joysticks/generic/DriverReceiving.h
  ../../xbmc/input/joysticks/generic/FeatureHandling.cpp
  ../../xbmc/input/joysticks/generic/FeatureHandling.h
  ../../xbmc/input/joysticks/generic/InputHandling.cpp
  ../../xbmc/input/joysticks/generic/InputHandling.h
  ../../xbmc/input/joysticks/interfaces/IButtonMap.h
  ../../xbmc/input/joysticks/interfaces/IButtonMapCallback.h
  ../../xbmc/input/joysticks/interfaces/IButtonMapper.h
  ../../xbmc/input/joysticks/interfaces/IButtonSequence.h
  ../../xbmc/input/joysticks/interfaces/IDriverHandler.h
  ../../xbmc/input/joysticks/interfaces/IDriverReceiver.h
  ../../xbmc/input/joysticks/interfaces/IInputHandler.h
  ../../xbmc/input/joysticks/interfaces/IInputProvider.h
  ../../xbmc/input/joysticks/interfaces/IInputReceiver.h
  ../../xbmc/input/joysticks/interfaces/IKeyHandler.h
  ../../xbmc/input/joysticks/interfaces/IKeymapHandler.h
  ../../xbmc/input/joysticks/JoystickEasterEgg.cpp
  ../../xbmc/input/joysticks/JoystickEasterEgg.h
  ../../xbmc/input/joysticks/JoystickIDs.h
  ../../xbmc/input/joysticks/JoystickMonitor.cpp
  ../../xbmc/input/joysticks/JoystickMonitor.h
  ../../xbmc/input/joysticks/JoystickTranslator.cpp
  ../../xbmc/input/joysticks/JoystickTranslator.h
  ../../xbmc/input/joysticks/JoystickTypes.h
  ../../xbmc/input/joysticks/JoystickUtils.cpp
  ../../xbmc/input/joysticks/JoystickUtils.h
  ../../xbmc/input/joysticks/keymaps/KeyHandler.cpp
  ../../xbmc/input/joysticks/keymaps/KeyHandler.h
  ../../xbmc/input/joysticks/keymaps/KeymapHandler.cpp
  ../../xbmc/input/joysticks/keymaps/KeymapHandler.h
  ../../xbmc/input/joysticks/keymaps/KeymapHandling.cpp
  ../../xbmc/input/joysticks/keymaps/KeymapHandling.h
  ../../xbmc/input/joysticks/RumbleGenerator.cpp
  ../../xbmc/input/joysticks/RumbleGenerator.h
  ../../xbmc/input/Key.cpp
  ../../xbmc/input/Key.h
  ../../xbmc/input/keyboard/generic/KeyboardInputHandling.cpp
  ../../xbmc/input/keyboard/generic/KeyboardInputHandling.h
  ../../xbmc/input/keyboard/interfaces/IActionMap.h
  ../../xbmc/input/keyboard/interfaces/IKeyboardDriverHandler.h
  ../../xbmc/input/keyboard/interfaces/IKeyboardInputHandler.h
  ../../xbmc/input/keyboard/interfaces/IKeyboardInputProvider.h
  ../../xbmc/input/keyboard/KeyboardEasterEgg.cpp
  ../../xbmc/input/keyboard/KeyboardEasterEgg.h
  ../../xbmc/input/keyboard/KeyboardTypes.h
  ../../xbmc/input/keyboard/KeymapActionMap.cpp
  ../../xbmc/input/keyboard/KeymapActionMap.h
  ../../xbmc/input/KeyboardLayout.cpp
  ../../xbmc/input/KeyboardLayout.h
  ../../xbmc/input/KeyboardLayoutManager.cpp
  ../../xbmc/input/KeyboardLayoutManager.h
  ../../xbmc/input/KeyboardStat.cpp
  ../../xbmc/input/KeyboardStat.h
  ../../xbmc/input/KeyboardTranslator.cpp
  ../../xbmc/input/KeyboardTranslator.h
  ../../xbmc/input/Keymap.cpp
  ../../xbmc/input/Keymap.h
  ../../xbmc/input/KeymapEnvironment.cpp
  ../../xbmc/input/KeymapEnvironment.h
  ../../xbmc/input/mouse/generic/MouseInputHandling.cpp
  ../../xbmc/input/mouse/generic/MouseInputHandling.h
  ../../xbmc/input/mouse/interfaces/IMouseDriverHandler.h
  ../../xbmc/input/mouse/interfaces/IMouseInputHandler.h
  ../../xbmc/input/mouse/interfaces/IMouseInputProvider.h
  ../../xbmc/input/mouse/MouseStat.cpp
  ../../xbmc/input/mouse/MouseStat.h
  ../../xbmc/input/mouse/MouseTranslator.cpp
  ../../xbmc/input/mouse/MouseTranslator.h
  ../../xbmc/input/mouse/MouseTypes.h
  ../../xbmc/input/remote/IRemoteControl.h
  ../../xbmc/input/remote/IRRemote.h
  ../../xbmc/input/touch/generic/GenericTouchActionHandler.cpp
  ../../xbmc/input/touch/generic/GenericTouchActionHandler.h
  ../../xbmc/input/touch/generic/GenericTouchInputHandler.cpp
  ../../xbmc/input/touch/generic/GenericTouchInputHandler.h
  ../../xbmc/input/touch/generic/GenericTouchPinchDetector.cpp
  ../../xbmc/input/touch/generic/GenericTouchPinchDetector.h
  ../../xbmc/input/touch/generic/GenericTouchRotateDetector.cpp
  ../../xbmc/input/touch/generic/GenericTouchRotateDetector.h
  ../../xbmc/input/touch/generic/GenericTouchSwipeDetector.cpp
  ../../xbmc/input/touch/generic/GenericTouchSwipeDetector.h
  ../../xbmc/input/touch/generic/IGenericTouchGestureDetector.h
  ../../xbmc/input/touch/ITouchActionHandler.h
  ../../xbmc/input/touch/ITouchInputHandler.h
  ../../xbmc/input/touch/ITouchInputHandling.cpp
  ../../xbmc/input/touch/ITouchInputHandling.h
  ../../xbmc/input/touch/TouchTypes.h
  ../../xbmc/input/TouchTranslator.cpp
  ../../xbmc/input/TouchTranslator.h
  ../../xbmc/input/WindowKeymap.cpp
  ../../xbmc/input/WindowKeymap.h
  ../../xbmc/input/WindowTranslator.cpp
  ../../xbmc/input/WindowTranslator.h
  ../../xbmc/input/XBMC_keyboard.h
  ../../xbmc/input/XBMC_keysym.h
  ../../xbmc/input/XBMC_keytable.cpp
  ../../xbmc/input/XBMC_keytable.h
  ../../xbmc/input/XBMC_vkeys.h
  ../../xbmc/interfaces/AnnouncementManager.h
  ../../xbmc/interfaces/AnnouncementManager3.cpp
  ../../xbmc/interfaces/builtins/AddonBuiltins.cpp
  ../../xbmc/interfaces/builtins/AddonBuiltins.h
  ../../xbmc/interfaces/builtins/AndroidBuiltins.h
  ../../xbmc/interfaces/builtins/ApplicationBuiltins.cpp
  ../../xbmc/interfaces/builtins/ApplicationBuiltins.h
  ../../xbmc/interfaces/builtins/Builtins.cpp
  ../../xbmc/interfaces/builtins/Builtins.h
  ../../xbmc/interfaces/builtins/CECBuiltins.cpp
  ../../xbmc/interfaces/builtins/CECBuiltins.h
  ../../xbmc/interfaces/builtins/GUIBuiltins.cpp
  ../../xbmc/interfaces/builtins/GUIBuiltins.h
  ../../xbmc/interfaces/builtins/GUIContainerBuiltins.cpp
  ../../xbmc/interfaces/builtins/GUIContainerBuiltins.h
  ../../xbmc/interfaces/builtins/GUIControlBuiltins.cpp
  ../../xbmc/interfaces/builtins/GUIControlBuiltins.h
  ../../xbmc/interfaces/builtins/LibraryBuiltins.cpp
  ../../xbmc/interfaces/builtins/LibraryBuiltins.h
  ../../xbmc/interfaces/builtins/OpticalBuiltins.cpp
  ../../xbmc/interfaces/builtins/OpticalBuiltins.h
  ../../xbmc/interfaces/builtins/PictureBuiltins.cpp
  ../../xbmc/interfaces/builtins/PictureBuiltins.h
  ../../xbmc/interfaces/builtins/PlayerBuiltins.cpp
  ../../xbmc/interfaces/builtins/PlayerBuiltins.h
  ../../xbmc/interfaces/builtins/ProfileBuiltins.cpp
  ../../xbmc/interfaces/builtins/ProfileBuiltins.h
  ../../xbmc/interfaces/builtins/PVRBuiltins.cpp
  ../../xbmc/interfaces/builtins/PVRBuiltins.h
  ../../xbmc/interfaces/builtins/SkinBuiltins.cpp
  ../../xbmc/interfaces/builtins/SkinBuiltins.h
  ../../xbmc/interfaces/builtins/SystemBuiltins.cpp
  ../../xbmc/interfaces/builtins/SystemBuiltins.h
  ../../xbmc/interfaces/builtins/WeatherBuiltins.cpp
  ../../xbmc/interfaces/builtins/WeatherBuiltins.h
  ../../xbmc/interfaces/generic/ILanguageInvocationHandler.h
  ../../xbmc/interfaces/generic/ILanguageInvoker.cpp
  ../../xbmc/interfaces/generic/ILanguageInvoker.h
  ../../xbmc/interfaces/generic/LanguageInvokerThread.cpp
  ../../xbmc/interfaces/generic/LanguageInvokerThread.h
  ../../xbmc/interfaces/generic/ScriptInvocationManager.cpp
  ../../xbmc/interfaces/generic/ScriptInvocationManager.h
  ../../xbmc/interfaces/IActionListener.h
  ../../xbmc/interfaces/IAnnouncer.h
  ../../xbmc/interfaces/info/InfoBool.cpp
  ../../xbmc/interfaces/info/InfoBool.h
  ../../xbmc/interfaces/info/InfoExpression.cpp
  ../../xbmc/interfaces/info/InfoExpression.h
  ../../xbmc/interfaces/info/SkinVariable.cpp
  ../../xbmc/interfaces/info/SkinVariable.h
  ../../xbmc/interfaces/json-rpc/AddonsOperations.cpp
  ../../xbmc/interfaces/json-rpc/AddonsOperations.h
  ../../xbmc/interfaces/json-rpc/ApplicationOperations.cpp
  ../../xbmc/interfaces/json-rpc/ApplicationOperations.h
  ../../xbmc/interfaces/json-rpc/AudioLibrary.cpp
  ../../xbmc/interfaces/json-rpc/AudioLibrary.h
  ../../xbmc/interfaces/json-rpc/FavouritesOperations.cpp
  ../../xbmc/interfaces/json-rpc/FavouritesOperations.h
  ../../xbmc/interfaces/json-rpc/FileItemHandler.cpp
  ../../xbmc/interfaces/json-rpc/FileItemHandler.h
  ../../xbmc/interfaces/json-rpc/FileOperations.cpp
  ../../xbmc/interfaces/json-rpc/FileOperations.h
  ../../xbmc/interfaces/json-rpc/GUIOperations.cpp
  ../../xbmc/interfaces/json-rpc/GUIOperations.h
  ../../xbmc/interfaces/json-rpc/IClient.h
  ../../xbmc/interfaces/json-rpc/IJSONRPCAnnouncer.h
  ../../xbmc/interfaces/json-rpc/InputOperations.cpp
  ../../xbmc/interfaces/json-rpc/InputOperations.h
  ../../xbmc/interfaces/json-rpc/ITransportLayer.h
  ../../xbmc/interfaces/json-rpc/JSONRPC.cpp
  ../../xbmc/interfaces/json-rpc/JSONRPC.h
  ../../xbmc/interfaces/json-rpc/JSONRPCUtils.h
  ../../xbmc/interfaces/json-rpc/JSONServiceDescription.cpp
  ../../xbmc/interfaces/json-rpc/JSONServiceDescription.h
  ../../xbmc/interfaces/json-rpc/JSONUtils.h
  ../../xbmc/interfaces/json-rpc/PlayerOperations.cpp
  ../../xbmc/interfaces/json-rpc/PlayerOperations.h
  ../../xbmc/interfaces/json-rpc/PlaylistOperations.cpp
  ../../xbmc/interfaces/json-rpc/PlaylistOperations.h
  ../../xbmc/interfaces/json-rpc/ProfilesOperations.cpp
  ../../xbmc/interfaces/json-rpc/ProfilesOperations.h
  ../../xbmc/interfaces/json-rpc/PVROperations.cpp
  ../../xbmc/interfaces/json-rpc/PVROperations.h
  ../../xbmc/interfaces/json-rpc/SettingsOperations.cpp
  ../../xbmc/interfaces/json-rpc/SettingsOperations.h
  ../../xbmc/interfaces/json-rpc/SystemOperations.cpp
  ../../xbmc/interfaces/json-rpc/SystemOperations.h
  ../../xbmc/interfaces/json-rpc/TextureOperations.cpp
  ../../xbmc/interfaces/json-rpc/TextureOperations.h
  ../../xbmc/interfaces/json-rpc/VideoLibrary.cpp
  ../../xbmc/interfaces/json-rpc/VideoLibrary.h
  ../../xbmc/interfaces/json-rpc/XBMCOperations.cpp
  ../../xbmc/interfaces/json-rpc/XBMCOperations.h
  ../../xbmc/interfaces/legacy/Addon.h
  ../../xbmc/interfaces/legacy/Addon3.cpp
  ../../xbmc/interfaces/legacy/AddonCallback.h
  ../../xbmc/interfaces/legacy/AddonCallback3.cpp
  ../../xbmc/interfaces/legacy/AddonClass.h
  ../../xbmc/interfaces/legacy/AddonClass3.cpp
  ../../xbmc/interfaces/legacy/AddonString.h
  ../../xbmc/interfaces/legacy/AddonUtil3s.cpp
  ../../xbmc/interfaces/legacy/AddonUtils.h
  ../../xbmc/interfaces/legacy/Alternative.h
  ../../xbmc/interfaces/legacy/aojsonrpc.h
  ../../xbmc/interfaces/legacy/CallbackFunction.h
  ../../xbmc/interfaces/legacy/CallbackFunction3.cpp
  ../../xbmc/interfaces/legacy/CallbackHandler.h
  ../../xbmc/interfaces/legacy/CallbackHandler3.cpp
  ../../xbmc/interfaces/legacy/Control.h
  ../../xbmc/interfaces/legacy/Control3.cpp
  ../../xbmc/interfaces/legacy/Dialog.h
  ../../xbmc/interfaces/legacy/Dialog3.cpp
  ../../xbmc/interfaces/legacy/Dictionary.h
  ../../xbmc/interfaces/legacy/DrmCryptoSession.h
  ../../xbmc/interfaces/legacy/DrmCryptoSession3.cpp
  ../../xbmc/interfaces/legacy/Exception.h
  ../../xbmc/interfaces/legacy/File.h
  ../../xbmc/interfaces/legacy/File3.cpp
  ../../xbmc/interfaces/legacy/InfoTagMusic.h
  ../../xbmc/interfaces/legacy/InfoTagMusic3.cpp
  ../../xbmc/interfaces/legacy/InfoTagRadioRDS.h
  ../../xbmc/interfaces/legacy/InfoTagRadioRDS3.cpp
  ../../xbmc/interfaces/legacy/InfoTagVideo.h
  ../../xbmc/interfaces/legacy/InfoTagVideo3.cpp
  ../../xbmc/interfaces/legacy/Keyboard.h
  ../../xbmc/interfaces/legacy/Keyboard3.cpp
  ../../xbmc/interfaces/legacy/LanguageHook.h
  ../../xbmc/interfaces/legacy/LanguageHook3.cpp
  ../../xbmc/interfaces/legacy/List.h
  ../../xbmc/interfaces/legacy/ListItem.h
  ../../xbmc/interfaces/legacy/ListItem3.cpp
  ../../xbmc/interfaces/legacy/ModuleXbmc.h
  ../../xbmc/interfaces/legacy/ModuleXbmc3.cpp
  ../../xbmc/interfaces/legacy/ModuleXbmcgui.h
  ../../xbmc/interfaces/legacy/ModuleXbmcgui3.cpp
  ../../xbmc/interfaces/legacy/ModuleXbmcplugin.h
  ../../xbmc/interfaces/legacy/ModuleXbmcplugin3.cpp
  ../../xbmc/interfaces/legacy/ModuleXbmcvfs.h
  ../../xbmc/interfaces/legacy/ModuleXbmcvfs3.cpp
  ../../xbmc/interfaces/legacy/Monitor.h
  ../../xbmc/interfaces/legacy/Monitor3.cpp
  ../../xbmc/interfaces/legacy/Player.h
  ../../xbmc/interfaces/legacy/Player3.cpp
  ../../xbmc/interfaces/legacy/PlayList.h
  ../../xbmc/interfaces/legacy/PlayList3.cpp
  ../../xbmc/interfaces/legacy/RenderCapture.h
  ../../xbmc/interfaces/legacy/Stat.h
  ../../xbmc/interfaces/legacy/String3.cpp
  ../../xbmc/interfaces/legacy/swighelper.h
  ../../xbmc/interfaces/legacy/Tuple.h
  ../../xbmc/interfaces/legacy/Window.h
  ../../xbmc/interfaces/legacy/Window3.cpp
  ../../xbmc/interfaces/legacy/WindowDialog.h
  ../../xbmc/interfaces/legacy/WindowDialog3.cpp
  ../../xbmc/interfaces/legacy/WindowDialogMixin.h
  ../../xbmc/interfaces/legacy/WindowDialogMixin3.cpp
  ../../xbmc/interfaces/legacy/WindowException.h
  ../../xbmc/interfaces/legacy/WindowInterceptor.h
  ../../xbmc/interfaces/legacy/WindowXML.h
  ../../xbmc/interfaces/legacy/WindowXML3.cpp
  ../../xbmc/interfaces/legacy/wsgi/WsgiErrorStream.cpp
  ../../xbmc/interfaces/legacy/wsgi/WsgiErrorStream.h
  ../../xbmc/interfaces/legacy/wsgi/WsgiInputStream.cpp
  ../../xbmc/interfaces/legacy/wsgi/WsgiInputStream.h
  ../../xbmc/interfaces/legacy/wsgi/WsgiResponse.cpp
  ../../xbmc/interfaces/legacy/wsgi/WsgiResponse.h
  ../../xbmc/interfaces/legacy/wsgi/WsgiResponseBody.cpp
  ../../xbmc/interfaces/legacy/wsgi/WsgiResponseBody.h
  ../../xbmc/interfaces/python/AddonPythonInvoker.cpp
  ../../xbmc/interfaces/python/AddonPythonInvoker.h
  ../../xbmc/interfaces/python/CallbackHandler.cpp
  ../../xbmc/interfaces/python/CallbackHandler.h
  ../../xbmc/interfaces/python/ContextItemAddonInvoker.cpp
  ../../xbmc/interfaces/python/ContextItemAddonInvoker.h
  ../../xbmc/interfaces/python/generated/AddonModuleXbmcwsgi.cpp
  ../../xbmc/interfaces/python/LanguageHook.cpp
  ../../xbmc/interfaces/python/LanguageHook.h
  ../../xbmc/interfaces/python/preamble.h
  ../../xbmc/interfaces/python/PyContext.cpp
  ../../xbmc/interfaces/python/PyContext.h
  ../../xbmc/interfaces/python/PythonInvoker.cpp
  ../../xbmc/interfaces/python/PythonInvoker.h
  ../../xbmc/interfaces/python/pythreadstate.h
  ../../xbmc/interfaces/python/swig.cpp
  ../../xbmc/interfaces/python/swig.h
  ../../xbmc/interfaces/python/XBPython.cpp
  ../../xbmc/interfaces/python/XBPython.h
  ../../xbmc/IProgressCallback.h
  ../../xbmc/LangInfoKodi.cpp
  ../../xbmc/LangInfoKodi.h
  ../../xbmc/listproviders/DirectoryProvider.cpp
  ../../xbmc/listproviders/DirectoryProvider.h
  ../../xbmc/listproviders/IListProvider.cpp
  ../../xbmc/listproviders/IListProvider.h
  ../../xbmc/listproviders/MultiProvider.cpp
  ../../xbmc/listproviders/MultiProvider.h
  ../../xbmc/listproviders/StaticProvider.cpp
  ../../xbmc/listproviders/StaticProvider.h
  ../../xbmc/media/MediaType.cpp
  ../../xbmc/media/MediaType.h
  ../../xbmc/MediaSource.cpp
  ../../xbmc/MediaSource.h
  ../../xbmc/messaging/ApplicationMessenger.cpp
  ../../xbmc/messaging/ApplicationMessenger.h
  ../../xbmc/messaging/helpers/DialogHelper.cpp
  ../../xbmc/messaging/helpers/DialogHelper.h
  ../../xbmc/messaging/helpers/DialogOKHelper.cpp
  ../../xbmc/messaging/helpers/DialogOKHelper.h
  ../../xbmc/messaging/IMessageTarget.h
  ../../xbmc/messaging/ThreadMessage.h
  ../../xbmc/music/Album.cpp
  ../../xbmc/music/Album.h
  ../../xbmc/music/Artist.cpp
  ../../xbmc/music/Artist.h
  ../../xbmc/music/ContextMenus.h
  ../../xbmc/music/ContextMenus4.cpp
  ../../xbmc/music/dialogs/GUIDialogInfoProviderSettings.cpp
  ../../xbmc/music/dialogs/GUIDialogInfoProviderSettings.h
  ../../xbmc/music/dialogs/GUIDialogMusicInfo.cpp
  ../../xbmc/music/dialogs/GUIDialogMusicInfo.h
  ../../xbmc/music/dialogs/GUIDialogMusicOSD.cpp
  ../../xbmc/music/dialogs/GUIDialogMusicOSD.h
  ../../xbmc/music/dialogs/GUIDialogSongInfo.cpp
  ../../xbmc/music/dialogs/GUIDialogSongInfo.h
  ../../xbmc/music/dialogs/GUIDialogVisualisationPresetList.cpp
  ../../xbmc/music/dialogs/GUIDialogVisualisationPresetList.h
  ../../xbmc/music/GUIViewStateMusic.cpp
  ../../xbmc/music/GUIViewStateMusic.h
  ../../xbmc/music/infoscanner/MusicAlbumInfo.cpp
  ../../xbmc/music/infoscanner/MusicAlbumInfo.h
  ../../xbmc/music/infoscanner/MusicArtistInfo.cpp
  ../../xbmc/music/infoscanner/MusicArtistInfo.h
  ../../xbmc/music/infoscanner/MusicInfoScanner.cpp
  ../../xbmc/music/infoscanner/MusicInfoScanner.h
  ../../xbmc/music/infoscanner/MusicInfoScraper.cpp
  ../../xbmc/music/infoscanner/MusicInfoScraper.h
  ../../xbmc/music/jobs/MusicLibraryCleaningJob.cpp
  ../../xbmc/music/jobs/MusicLibraryCleaningJob.h
  ../../xbmc/music/jobs/MusicLibraryExportJob.cpp
  ../../xbmc/music/jobs/MusicLibraryExportJob.h
  ../../xbmc/music/jobs/MusicLibraryJob.cpp
  ../../xbmc/music/jobs/MusicLibraryJob.h
  ../../xbmc/music/jobs/MusicLibraryProgressJob.cpp
  ../../xbmc/music/jobs/MusicLibraryProgressJob.h
  ../../xbmc/music/jobs/MusicLibraryScanningJob.cpp
  ../../xbmc/music/jobs/MusicLibraryScanningJob.h
  ../../xbmc/music/MusicDatabase.cpp
  ../../xbmc/music/MusicDatabase.h
  ../../xbmc/music/MusicDbUrl.cpp
  ../../xbmc/music/MusicDbUrl.h
  ../../xbmc/music/MusicInfoLoader.cpp
  ../../xbmc/music/MusicInfoLoader.h
  ../../xbmc/music/MusicLibraryQueue.cpp
  ../../xbmc/music/MusicLibraryQueue.h
  ../../xbmc/music/MusicThumbLoader.cpp
  ../../xbmc/music/MusicThumbLoader.h
  ../../xbmc/music/MusicUtils.cpp
  ../../xbmc/music/MusicUtils.h
  ../../xbmc/music/Song.cpp
  ../../xbmc/music/Song.h
  ../../xbmc/music/tags/ImusicInfoTagLoader.h
  ../../xbmc/music/tags/MusicInfoTag.cpp
  ../../xbmc/music/tags/MusicInfoTag.h
  ../../xbmc/music/tags/MusicInfoTagLoaderCDDA.cpp
  ../../xbmc/music/tags/MusicInfoTagLoaderCDDA.h
  ../../xbmc/music/tags/MusicInfoTagLoaderDatabase.cpp
  ../../xbmc/music/tags/MusicInfoTagLoaderDatabase.h
  ../../xbmc/music/tags/MusicInfoTagLoaderFactory.cpp
  ../../xbmc/music/tags/MusicInfoTagLoaderFactory.h
  ../../xbmc/music/tags/MusicInfoTagLoaderFFmpeg.cpp
  ../../xbmc/music/tags/MusicInfoTagLoaderFFmpeg.h
  ../../xbmc/music/tags/MusicInfoTagLoaderShn.cpp
  ../../xbmc/music/tags/MusicInfoTagLoaderShn.h
  ../../xbmc/music/tags/ReplayGain.cpp
  ../../xbmc/music/tags/ReplayGain.h
  ../../xbmc/music/tags/TagLibVFSStream.cpp
  ../../xbmc/music/tags/TagLibVFSStream.h
  ../../xbmc/music/tags/TagLoaderTagLib.cpp
  ../../xbmc/music/tags/TagLoaderTagLib.h
  ../../xbmc/music/windows/GUIWindowMusicBase.cpp
  ../../xbmc/music/windows/GUIWindowMusicBase.h
  ../../xbmc/music/windows/GUIWindowMusicNav.cpp
  ../../xbmc/music/windows/GUIWindowMusicNav.h
  ../../xbmc/music/windows/GUIWindowMusicPlaylist.cpp
  ../../xbmc/music/windows/GUIWindowMusicPlaylist.h
  ../../xbmc/music/windows/GUIWindowMusicPlaylistEditor.cpp
  ../../xbmc/music/windows/GUIWindowMusicPlaylistEditor.h
  ../../xbmc/music/windows/GUIWindowVisualisation.cpp
  ../../xbmc/music/windows/GUIWindowVisualisation.h
  ../../xbmc/music/windows/MusicFileItemListModifier.cpp
  ../../xbmc/music/windows/MusicFileItemListModifier.h
  ../../xbmc/network/AirPlayServer.cpp
  ../../xbmc/network/AirPlayServer.h
  ../../xbmc/network/AirTunesServer.cpp
  ../../xbmc/network/AirTunesServer.h
  ../../xbmc/network/cddb.cpp
  ../../xbmc/network/cddb.h
  ../../xbmc/network/dacp/dacp.cpp
  ../../xbmc/network/dacp/dacp.h
  ../../xbmc/network/DllLibPlist.cpp
  ../../xbmc/network/DllLibPlist.h
  ../../xbmc/network/DllLibShairplay.cpp
  ../../xbmc/network/DllLibShairplay.h
  ../../xbmc/network/DNSNameCache.cpp
  ../../xbmc/network/DNSNameCache.h
  ../../xbmc/network/EventClient.cpp
  ../../xbmc/network/EventClient.h
  ../../xbmc/network/EventPacket.cpp
  ../../xbmc/network/EventPacket.h
  ../../xbmc/network/EventServer.cpp
  ../../xbmc/network/EventServer.h
  ../../xbmc/network/GUIDialogAccessPoints.cpp
  ../../xbmc/network/GUIDialogAccessPoints.h
  ../../xbmc/network/GUIDialogNetworkSetup.cpp
  ../../xbmc/network/GUIDialogNetworkSetup.h
  ../../xbmc/network/httprequesthandler/HTTPFileHandler.cpp
  ../../xbmc/network/httprequesthandler/HTTPFileHandler.h
  ../../xbmc/network/httprequesthandler/HTTPImageHandler.cpp
  ../../xbmc/network/httprequesthandler/HTTPImageHandler.h
  ../../xbmc/network/httprequesthandler/HTTPImageTransformationHandler.cpp
  ../../xbmc/network/httprequesthandler/HTTPImageTransformationHandler.h
  ../../xbmc/network/httprequesthandler/HTTPJsonRpcHandler.cpp
  ../../xbmc/network/httprequesthandler/HTTPJsonRpcHandler.h
  ../../xbmc/network/httprequesthandler/HTTPPythonHandler.cpp
  ../../xbmc/network/httprequesthandler/HTTPPythonHandler.h
  ../../xbmc/network/httprequesthandler/HTTPRequestHandlerUtils.cpp
  ../../xbmc/network/httprequesthandler/HTTPRequestHandlerUtils.h
  ../../xbmc/network/httprequesthandler/HTTPVfsHandler.cpp
  ../../xbmc/network/httprequesthandler/HTTPVfsHandler.h
  ../../xbmc/network/httprequesthandler/HTTPWebinterfaceAddonsHandler.cpp
  ../../xbmc/network/httprequesthandler/HTTPWebinterfaceAddonsHandler.h
  ../../xbmc/network/httprequesthandler/HTTPWebinterfaceHandler.cpp
  ../../xbmc/network/httprequesthandler/HTTPWebinterfaceHandler.h
  ../../xbmc/network/httprequesthandler/IHTTPRequestHandler.cpp
  ../../xbmc/network/httprequesthandler/IHTTPRequestHandler.h
  ../../xbmc/network/httprequesthandler/python/HTTPPythonInvoker.cpp
  ../../xbmc/network/httprequesthandler/python/HTTPPythonInvoker.h
  ../../xbmc/network/httprequesthandler/python/HTTPPythonRequest.h
  ../../xbmc/network/httprequesthandler/python/HTTPPythonWsgiInvoker.cpp
  ../../xbmc/network/httprequesthandler/python/HTTPPythonWsgiInvoker.h
  ../../xbmc/network/mdns/ZeroconfBrowserMDNS.cpp
  ../../xbmc/network/mdns/ZeroconfBrowserMDNS.h
  ../../xbmc/network/mdns/ZeroconfMDNS.cpp
  ../../xbmc/network/mdns/ZeroconfMDNS.h
  ../../xbmc/network/Network.cpp
  ../../xbmc/network/Network.h
  ../../xbmc/network/NetworkServices.cpp
  ../../xbmc/network/NetworkServices.h
  ../../xbmc/network/Socket.cpp
  ../../xbmc/network/Socket.h
  ../../xbmc/network/TCPServer.cpp
  ../../xbmc/network/TCPServer.h
  ../../xbmc/network/UdpClient.cpp
  ../../xbmc/network/UdpClient.h
  ../../xbmc/network/upnp/UPnP.cpp
  ../../xbmc/network/upnp/UPnP.h
  ../../xbmc/network/upnp/UPnPInternal.cpp
  ../../xbmc/network/upnp/UPnPInternal.h
  ../../xbmc/network/upnp/UPnPPlayer.cpp
  ../../xbmc/network/upnp/UPnPPlayer.h
  ../../xbmc/network/upnp/UPnPRenderer.cpp
  ../../xbmc/network/upnp/UPnPRenderer.h
  ../../xbmc/network/upnp/UPnPServer.cpp
  ../../xbmc/network/upnp/UPnPServer.h
  ../../xbmc/network/upnp/UPnPSettings.cpp
  ../../xbmc/network/upnp/UPnPSettings.h
  ../../xbmc/network/WakeOnAccess.cpp
  ../../xbmc/network/WakeOnAccess.h
  ../../xbmc/network/WebServer.cpp
  ../../xbmc/network/WebServer.h
  ../../xbmc/network/websocket/sha1.hpp
  ../../xbmc/network/websocket/WebSocket.cpp
  ../../xbmc/network/websocket/WebSocket.h
  ../../xbmc/network/websocket/WebSocketManager.cpp
  ../../xbmc/network/websocket/WebSocketManager.h
  ../../xbmc/network/websocket/WebSocketV13.cpp
  ../../xbmc/network/websocket/WebSocketV13.h
  ../../xbmc/network/websocket/WebSocketV8.cpp
  ../../xbmc/network/websocket/WebSocketV8.h
  ../../xbmc/network/Zeroconf.cpp
  ../../xbmc/network/Zeroconf.h
  ../../xbmc/network/ZeroconfBrowser.cpp
  ../../xbmc/network/ZeroconfBrowser.h
  ../../xbmc/NfoFile.cpp
  ../../xbmc/NfoFile.h
  ../../xbmc/PartyModeManager.cpp
  ../../xbmc/PartyModeManager.h
  ../../xbmc/PasswordManager.cpp
  ../../xbmc/PasswordManager.h
  ../../xbmc/peripherals/addons/AddonButtonMap.cpp
  ../../xbmc/peripherals/addons/AddonButtonMap.h
  ../../xbmc/peripherals/addons/AddonButtonMapping.cpp
  ../../xbmc/peripherals/addons/AddonButtonMapping.h
  ../../xbmc/peripherals/addons/AddonInputHandling.cpp
  ../../xbmc/peripherals/addons/AddonInputHandling.h
  ../../xbmc/peripherals/addons/PeripheralAddon.cpp
  ../../xbmc/peripherals/addons/PeripheralAddon.h
  ../../xbmc/peripherals/addons/PeripheralAddonTranslator.cpp
  ../../xbmc/peripherals/addons/PeripheralAddonTranslator.h
  ../../xbmc/peripherals/bus/PeripheralBus.cpp
  ../../xbmc/peripherals/bus/PeripheralBus.h
  ../../xbmc/peripherals/bus/PeripheralBusUSB.h
  ../../xbmc/peripherals/bus/virtual/PeripheralBusAddon.cpp
  ../../xbmc/peripherals/bus/virtual/PeripheralBusAddon.h
  ../../xbmc/peripherals/bus/virtual/PeripheralBusApplication.cpp
  ../../xbmc/peripherals/bus/virtual/PeripheralBusApplication.h
  ../../xbmc/peripherals/bus/virtual/PeripheralBusCEC.cpp
  ../../xbmc/peripherals/bus/virtual/PeripheralBusCEC.h
  ../../xbmc/peripherals/devices/Peripheral.cpp
  ../../xbmc/peripherals/devices/Peripheral.h
  ../../xbmc/peripherals/devices/PeripheralBluetooth.cpp
  ../../xbmc/peripherals/devices/PeripheralBluetooth.h
  ../../xbmc/peripherals/devices/PeripheralCecAdapter.cpp
  ../../xbmc/peripherals/devices/PeripheralCecAdapter.h
  ../../xbmc/peripherals/devices/PeripheralDisk.cpp
  ../../xbmc/peripherals/devices/PeripheralDisk.h
  ../../xbmc/peripherals/devices/PeripheralHID.cpp
  ../../xbmc/peripherals/devices/PeripheralHID.h
  ../../xbmc/peripherals/devices/PeripheralImon.cpp
  ../../xbmc/peripherals/devices/PeripheralImon.h
  ../../xbmc/peripherals/devices/PeripheralJoystick.cpp
  ../../xbmc/peripherals/devices/PeripheralJoystick.h
  ../../xbmc/peripherals/devices/PeripheralKeyboard.cpp
  ../../xbmc/peripherals/devices/PeripheralKeyboard.h
  ../../xbmc/peripherals/devices/PeripheralMouse.cpp
  ../../xbmc/peripherals/devices/PeripheralMouse.h
  ../../xbmc/peripherals/devices/PeripheralNIC.cpp
  ../../xbmc/peripherals/devices/PeripheralNIC.h
  ../../xbmc/peripherals/devices/PeripheralNyxboard.cpp
  ../../xbmc/peripherals/devices/PeripheralNyxboard.h
  ../../xbmc/peripherals/devices/PeripheralTuner.cpp
  ../../xbmc/peripherals/devices/PeripheralTuner.h
  ../../xbmc/peripherals/dialogs/GUIDialogPeripherals.cpp
  ../../xbmc/peripherals/dialogs/GUIDialogPeripherals.h
  ../../xbmc/peripherals/dialogs/GUIDialogPeripheralSettings.cpp
  ../../xbmc/peripherals/dialogs/GUIDialogPeripheralSettings.h
  ../../xbmc/peripherals/EventLockHandle.cpp
  ../../xbmc/peripherals/EventLockHandle.h
  ../../xbmc/peripherals/EventPollHandle.cpp
  ../../xbmc/peripherals/EventPollHandle.h
  ../../xbmc/peripherals/EventScanner.cpp
  ../../xbmc/peripherals/EventScanner.h
  ../../xbmc/peripherals/IEventScannerCallback.h
  ../../xbmc/peripherals/Peripherals.cpp
  ../../xbmc/peripherals/Peripherals.h
  ../../xbmc/peripherals/PeripheralTypes.h
  ../../xbmc/pictures/ExifParse.cpp
  ../../xbmc/pictures/GUIDialogPictureInfo.cpp
  ../../xbmc/pictures/GUIDialogPictureInfo.h
  ../../xbmc/pictures/GUIViewStatePictures.cpp
  ../../xbmc/pictures/GUIViewStatePictures.h
  ../../xbmc/pictures/GUIWindowPictures.cpp
  ../../xbmc/pictures/GUIWindowPictures.h
  ../../xbmc/pictures/GUIWindowSlideShow.cpp
  ../../xbmc/pictures/GUIWindowSlideShow.h
  ../../xbmc/pictures/IptcParse.cpp
  ../../xbmc/pictures/JpegParse.cpp
  ../../xbmc/pictures/libexif.cpp
  ../../xbmc/pictures/Picture.cpp
  ../../xbmc/pictures/Picture.h
  ../../xbmc/pictures/PictureInfoLoader.cpp
  ../../xbmc/pictures/PictureInfoLoader.h
  ../../xbmc/pictures/PictureInfoTag.cpp
  ../../xbmc/pictures/PictureInfoTag.h
  ../../xbmc/pictures/PictureScalingAlgorithm.cpp
  ../../xbmc/pictures/PictureScalingAlgorithm.h
  ../../xbmc/pictures/PictureThumbLoader.cpp
  ../../xbmc/pictures/PictureThumbLoader.h
  ../../xbmc/pictures/SlideShowPicture.cpp
  ../../xbmc/pictures/SlideShowPicture.h
  ../../xbmc/platform/Environment.cpp
  ../../xbmc/platform/Environment.h
  ../../xbmc/platform/Filesystem.h
  ../../xbmc/platform/MessagePrinter.h
  ../../xbmc/platform/Platform.cpp
  ../../xbmc/platform/Platform.h
  ../../xbmc/platform/qt/KodiQtApp.cpp
  ../../xbmc/platform/qt/KodiQtApp.h
  ../../xbmc/platform/win32/CharsetConverter.h
  ../../xbmc/platform/win32/CharsetConverterWin32.cpp
  ../../xbmc/platform/win32/crts_caller.cpp
  ../../xbmc/platform/win32/crts_caller.h
  ../../xbmc/platform/win32/dirent.h
  ../../xbmc/platform/win32/dxerr.cpp
  ../../xbmc/platform/win32/dxerr.h
  ../../xbmc/platform/win32/EnvironmentWin32.cpp
  ../../xbmc/platform/win32/filesystem/Win32Directory.cpp
  ../../xbmc/platform/win32/filesystem/Win32Directory.h
  ../../xbmc/platform/win32/filesystem/Win32File.cpp
  ../../xbmc/platform/win32/filesystem/Win32File.h
  ../../xbmc/platform/win32/filesystem/Win32SMBDirectory.cpp
  ../../xbmc/platform/win32/filesystem/Win32SMBDirectory.h
  ../../xbmc/platform/win32/filesystem/Win32SMBFile.cpp
  ../../xbmc/platform/win32/filesystem/Win32SMBFile.h
  ../../xbmc/platform/win32/FilesystemWin32.cpp
  ../../xbmc/platform/win32/IMMNotificationClient.h
  ../../xbmc/platform/win32/input/IRServerSuite.cpp
  ../../xbmc/platform/win32/input/IRServerSuite.h
  ../../xbmc/platform/win32/input/IrssMessage.cpp
  ../../xbmc/platform/win32/input/IrssMessage.h
  ../../xbmc/platform/win32/MessagePrinterWin32.cpp
  ../../xbmc/platform/win32/my_ntddcdrm.h
  ../../xbmc/platform/win32/my_ntddscsi.h
  ../../xbmc/platform/win32/netdb.h
  ../../xbmc/platform/win32/network/NetworkWin32.cpp
  ../../xbmc/platform/win32/network/NetworkWin32.h
  ../../xbmc/platform/win32/pch.cpp
  ../../xbmc/platform/win32/pch.h
  ../../xbmc/platform/win32/peripherals/PeripheralBusUSB.cpp
  ../../xbmc/platform/win32/peripherals/PeripheralBusUSB.h
  ../../xbmc/platform/win32/PlatformDefs.h
  ../../xbmc/platform/win32/PlatformInclude.h
  ../../xbmc/platform/win32/powermanagement/Win32PowerSyscall.cpp
  ../../xbmc/platform/win32/powermanagement/Win32PowerSyscall.h
  ../../xbmc/platform/win32/powermanagement/WinIdleTimer.cpp
  ../../xbmc/platform/win32/resource.h
  ../../xbmc/platform/win32/stat_utf8.cpp
  ../../xbmc/platform/win32/stat_utf8.h
  ../../xbmc/platform/win32/stdio_utf8.cpp
  ../../xbmc/platform/win32/stdio_utf8.h
  ../../xbmc/platform/win32/storage/Win32StorageProvider.cpp
  ../../xbmc/platform/win32/storage/Win32StorageProvider.h
  ../../xbmc/platform/win32/strverscmp.cpp
  ../../xbmc/platform/win32/unistd.h
  ../../xbmc/platform/win32/utils/gpu_memcpy_sse4.h
  ../../xbmc/platform/win32/utils/memcpy_sse2.h
  ../../xbmc/platform/win32/utils/Win32InterfaceForCLog.cpp
  ../../xbmc/platform/win32/utils/Win32InterfaceForCLog.h
  ../../xbmc/platform/win32/WIN32Util.cpp
  ../../xbmc/platform/win32/WIN32Util.h
  ../../xbmc/platform/win32/WindowHelper.cpp
  ../../xbmc/platform/win32/WindowHelper.h
  ../../xbmc/platform/win32/WinMain.cpp
  ../../xbmc/platform/win32/WMIInterface.cpp
  ../../xbmc/platform/win32/WMIInterface.h
  ../../xbmc/platform/xbmc.cpp
  ../../xbmc/platform/xbmc.h
  ../../xbmc/PlayListPlayer.cpp
  ../../xbmc/PlayListPlayer.h
  ../../xbmc/playlists/PlayList.cpp
  ../../xbmc/playlists/PlayList.h
  ../../xbmc/playlists/PlayListB4S.cpp
  ../../xbmc/playlists/PlayListB4S.h
  ../../xbmc/playlists/PlayListFactory.cpp
  ../../xbmc/playlists/PlayListFactory.h
  ../../xbmc/playlists/PlayListM3U.cpp
  ../../xbmc/playlists/PlayListM3U.h
  ../../xbmc/playlists/PlayListPLS.cpp
  ../../xbmc/playlists/PlayListPLS.h
  ../../xbmc/playlists/PlayListURL.cpp
  ../../xbmc/playlists/PlayListURL.h
  ../../xbmc/playlists/PlayListWPL.cpp
  ../../xbmc/playlists/PlayListWPL.h
  ../../xbmc/playlists/PlayListXML.cpp
  ../../xbmc/playlists/PlayListXML.h
  ../../xbmc/playlists/PlayListXSPF.cpp
  ../../xbmc/playlists/PlayListXSPF.h
  ../../xbmc/playlists/SmartPlayList.cpp
  ../../xbmc/playlists/SmartPlayList.h
  ../../xbmc/playlists/SmartPlaylistFileItemListModifier.cpp
  ../../xbmc/playlists/SmartPlaylistFileItemListModifier.h
  ../../xbmc/powermanagement/DPMSSupport.cpp
  ../../xbmc/powermanagement/DPMSSupport.h
  ../../xbmc/powermanagement/IPowerSyscall.cpp
  ../../xbmc/powermanagement/IPowerSyscall.h
  ../../xbmc/powermanagement/PowerManager.cpp
  ../../xbmc/powermanagement/PowerManager.h
  ../../xbmc/powermanagement/WinIdleTimer.h
  ../../xbmc/profiles/dialogs/GUIDialogLockSettings.cpp
  ../../xbmc/profiles/dialogs/GUIDialogLockSettings.h
  ../../xbmc/profiles/dialogs/GUIDialogProfileSettings.cpp
  ../../xbmc/profiles/dialogs/GUIDialogProfileSettings.h
  ../../xbmc/profiles/Profile.cpp
  ../../xbmc/profiles/Profile.h
  ../../xbmc/profiles/ProfileManager.cpp
  ../../xbmc/profiles/ProfileManager.h
  ../../xbmc/profiles/windows/GUIWindowSettingsProfile.cpp
  ../../xbmc/profiles/windows/GUIWindowSettingsProfile.h
  ../../xbmc/programs/GUIViewStatePrograms.cpp
  ../../xbmc/programs/GUIViewStatePrograms.h
  ../../xbmc/programs/GUIWindowPrograms.cpp
  ../../xbmc/programs/GUIWindowPrograms.h
  ../../xbmc/pvr/addons/PVRClients.cpp
  ../../xbmc/pvr/addons/PVRClients.h
  ../../xbmc/pvr/channels/PVRChannel.cpp
  ../../xbmc/pvr/channels/PVRChannel.h
  ../../xbmc/pvr/channels/PVRChannelGroup.cpp
  ../../xbmc/pvr/channels/PVRChannelGroup.h
  ../../xbmc/pvr/channels/PVRChannelGroupInternal.cpp
  ../../xbmc/pvr/channels/PVRChannelGroupInternal.h
  ../../xbmc/pvr/channels/PVRChannelGroups.cpp
  ../../xbmc/pvr/channels/PVRChannelGroups.h
  ../../xbmc/pvr/channels/PVRChannelGroupsContainer.cpp
  ../../xbmc/pvr/channels/PVRChannelGroupsContainer.h
  ../../xbmc/pvr/channels/PVRChannelNumber.cpp
  ../../xbmc/pvr/channels/PVRChannelNumber.h
  ../../xbmc/pvr/channels/PVRRadioRDSInfoTag.cpp
  ../../xbmc/pvr/channels/PVRRadioRDSInfoTag.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRChannelGuide.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRChannelGuide.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRChannelManager.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRChannelManager.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRChannelsOSD.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRChannelsOSD.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRClientPriorities.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRClientPriorities.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRGroupManager.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRGroupManager.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRGuideInfo.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRGuideInfo.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRGuideSearch.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRGuideSearch.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRItemsViewBase.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRItemsViewBase.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRRadioRDSInfo.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRRadioRDSInfo.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRRecordingInfo.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRRecordingInfo.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRRecordingSettings.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRRecordingSettings.h
  ../../xbmc/pvr/dialogs/GUIDialogPVRTimerSettings.cpp
  ../../xbmc/pvr/dialogs/GUIDialogPVRTimerSettings.h
  ../../xbmc/pvr/epg/Epg.cpp
  ../../xbmc/pvr/epg/Epg.h
  ../../xbmc/pvr/epg/EpgContainer.cpp
  ../../xbmc/pvr/epg/EpgContainer.h
  ../../xbmc/pvr/epg/EpgDatabase.cpp
  ../../xbmc/pvr/epg/EpgDatabase.h
  ../../xbmc/pvr/epg/EpgInfoTag.cpp
  ../../xbmc/pvr/epg/EpgInfoTag.h
  ../../xbmc/pvr/epg/EpgSearchFilter.cpp
  ../../xbmc/pvr/epg/EpgSearchFilter.h
  ../../xbmc/pvr/PVRActionListener.cpp
  ../../xbmc/pvr/PVRActionListener.h
  ../../xbmc/pvr/PVRChannelNumberInputHandler.cpp
  ../../xbmc/pvr/PVRChannelNumberInputHandler.h
  ../../xbmc/pvr/PVRContextMenus.cpp
  ../../xbmc/pvr/PVRContextMenus.h
  ../../xbmc/pvr/PVRDatabase.cpp
  ../../xbmc/pvr/PVRDatabase.h
  ../../xbmc/pvr/PVREvent.h
  ../../xbmc/pvr/PVRGUIActions.cpp
  ../../xbmc/pvr/PVRGUIActions.h
  ../../xbmc/pvr/PVRGUIChannelNavigator.cpp
  ../../xbmc/pvr/PVRGUIChannelNavigator.h
  ../../xbmc/pvr/PVRGUIInfo.cpp
  ../../xbmc/pvr/PVRGUIInfo.h
  ../../xbmc/pvr/PVRGUIProgressHandler.cpp
  ../../xbmc/pvr/PVRGUIProgressHandler.h
  ../../xbmc/pvr/PVRGUITimerInfo.cpp
  ../../xbmc/pvr/PVRGUITimerInfo.h
  ../../xbmc/pvr/PVRGUITimesInfo.cpp
  ../../xbmc/pvr/PVRGUITimesInfo.h
  ../../xbmc/pvr/PVRItem.cpp
  ../../xbmc/pvr/PVRItem.h
  ../../xbmc/pvr/PVRJobs.cpp
  ../../xbmc/pvr/PVRJobs.h
  ../../xbmc/pvr/PVRManager.cpp
  ../../xbmc/pvr/PVRManager.h
  ../../xbmc/pvr/PVRSettings.cpp
  ../../xbmc/pvr/PVRSettings.h
  ../../xbmc/pvr/PVRTypes.h
  ../../xbmc/pvr/recordings/PVRRecording.cpp
  ../../xbmc/pvr/recordings/PVRRecording.h
  ../../xbmc/pvr/recordings/PVRRecordings.cpp
  ../../xbmc/pvr/recordings/PVRRecordings.h
  ../../xbmc/pvr/recordings/PVRRecordingsPath.cpp
  ../../xbmc/pvr/recordings/PVRRecordingsPath.h
  ../../xbmc/pvr/timers/PVRTimerInfoTag.cpp
  ../../xbmc/pvr/timers/PVRTimerInfoTag.h
  ../../xbmc/pvr/timers/PVRTimers.cpp
  ../../xbmc/pvr/timers/PVRTimers.h
  ../../xbmc/pvr/timers/PVRTimerType.cpp
  ../../xbmc/pvr/timers/PVRTimerType.h
  ../../xbmc/pvr/windows/GUIEPGGridContainer.cpp
  ../../xbmc/pvr/windows/GUIEPGGridContainer.h
  ../../xbmc/pvr/windows/GUIEPGGridContainerModel.cpp
  ../../xbmc/pvr/windows/GUIEPGGridContainerModel.h
  ../../xbmc/pvr/windows/GUIViewStatePVR.cpp
  ../../xbmc/pvr/windows/GUIViewStatePVR.h
  ../../xbmc/pvr/windows/GUIWindowPVRBase.cpp
  ../../xbmc/pvr/windows/GUIWindowPVRBase.h
  ../../xbmc/pvr/windows/GUIWindowPVRChannels.cpp
  ../../xbmc/pvr/windows/GUIWindowPVRChannels.h
  ../../xbmc/pvr/windows/GUIWindowPVRGuide.cpp
  ../../xbmc/pvr/windows/GUIWindowPVRGuide.h
  ../../xbmc/pvr/windows/GUIWindowPVRRecordings.cpp
  ../../xbmc/pvr/windows/GUIWindowPVRRecordings.h
  ../../xbmc/pvr/windows/GUIWindowPVRSearch.cpp
  ../../xbmc/pvr/windows/GUIWindowPVRSearch.h
  ../../xbmc/pvr/windows/GUIWindowPVRTimerRules.cpp
  ../../xbmc/pvr/windows/GUIWindowPVRTimerRules.h
  ../../xbmc/pvr/windows/GUIWindowPVRTimers.cpp
  ../../xbmc/pvr/windows/GUIWindowPVRTimers.h
  ../../xbmc/pvr/windows/GUIWindowPVRTimersBase.cpp
  ../../xbmc/pvr/windows/GUIWindowPVRTimersBase.h
  ../../xbmc/rendering/dx/DeviceResources.cpp
  ../../xbmc/rendering/dx/DeviceResources.h
  ../../xbmc/rendering/dx/DirectXHelper.h
  ../../xbmc/rendering/dx/RenderContext.h
  ../../xbmc/rendering/dx/RenderSystemDX.cpp
  ../../xbmc/rendering/dx/RenderSystemDX.h
  ../../xbmc/rendering/gles/GLESShader.cpp
  ../../xbmc/rendering/gles/GLESShader.h
  ../../xbmc/rendering/gles/RenderSystemGLES.cpp
  ../../xbmc/rendering/gles/RenderSystemGLES.h
  ../../xbmc/rendering/qt/RenderSystemQt.cpp
  ../../xbmc/rendering/qt/RenderSystemQt.h
  ../../xbmc/rendering/RenderSystem.cpp
  ../../xbmc/rendering/RenderSystem.h
  ../../xbmc/rendering/RenderSystemTypes.h
  ../../xbmc/SectionLoader.cpp
  ../../xbmc/SectionLoader.h
  ../../xbmc/SeekHandler.cpp
  ../../xbmc/SeekHandler.h
  ../../xbmc/ServiceBroker.cpp
  ../../xbmc/ServiceBroker.h
  ../../xbmc/ServiceDescription.h
  ../../xbmc/ServiceManager.cpp
  ../../xbmc/ServiceManager.h
  ../../xbmc/settings/AdvancedSettings.cpp
  ../../xbmc/settings/AdvancedSettings.h
  ../../xbmc/settings/dialogs/GUIDialogContentSettings.cpp
  ../../xbmc/settings/dialogs/GUIDialogContentSettings.h
  ../../xbmc/settings/dialogs/GUIDialogLibExportSettings.cpp
  ../../xbmc/settings/dialogs/GUIDialogLibExportSettings.h
  ../../xbmc/settings/dialogs/GUIDialogSettingsBase.cpp
  ../../xbmc/settings/dialogs/GUIDialogSettingsBase.h
  ../../xbmc/settings/dialogs/GUIDialogSettingsManagerBase.cpp
  ../../xbmc/settings/dialogs/GUIDialogSettingsManagerBase.h
  ../../xbmc/settings/dialogs/GUIDialogSettingsManualBase.cpp
  ../../xbmc/settings/dialogs/GUIDialogSettingsManualBase.h
  ../../xbmc/settings/DiscSettings.cpp
  ../../xbmc/settings/DiscSettings.h
  ../../xbmc/settings/DisplaySettings.cpp
  ../../xbmc/settings/DisplaySettings.h
  ../../xbmc/settings/GameSettings.cpp
  ../../xbmc/settings/GameSettings.h
  ../../xbmc/settings/lib/ISetting.cpp
  ../../xbmc/settings/lib/ISetting.h
  ../../xbmc/settings/lib/ISettingCallback.h
  ../../xbmc/settings/lib/ISettingControl.cpp
  ../../xbmc/settings/lib/ISettingControl.h
  ../../xbmc/settings/lib/ISettingControlCreator.h
  ../../xbmc/settings/lib/ISettingCreator.h
  ../../xbmc/settings/lib/ISettingsHandler.h
  ../../xbmc/settings/lib/ISubSettings.h
  ../../xbmc/settings/lib/Setting.cpp
  ../../xbmc/settings/lib/Setting.h
  ../../xbmc/settings/lib/SettingCategoryAccess.cpp
  ../../xbmc/settings/lib/SettingCategoryAccess.h
  ../../xbmc/settings/lib/SettingConditions.h
  ../../xbmc/settings/lib/SettingConditions2.cpp
  ../../xbmc/settings/lib/SettingDefinitions.h
  ../../xbmc/settings/lib/SettingDependency.cpp
  ../../xbmc/settings/lib/SettingDependency.h
  ../../xbmc/settings/lib/SettingLevel.h
  ../../xbmc/settings/lib/SettingRequirement.cpp
  ../../xbmc/settings/lib/SettingRequirement.h
  ../../xbmc/settings/lib/SettingSection.cpp
  ../../xbmc/settings/lib/SettingSection.h
  ../../xbmc/settings/lib/SettingsManager.cpp
  ../../xbmc/settings/lib/SettingsManager.h
  ../../xbmc/settings/lib/SettingType.h
  ../../xbmc/settings/lib/SettingUpdate.cpp
  ../../xbmc/settings/lib/SettingUpdate.h
  ../../xbmc/settings/LibExportSettings.cpp
  ../../xbmc/settings/LibExportSettings.h
  ../../xbmc/settings/MediaSettings.cpp
  ../../xbmc/settings/MediaSettings.h
  ../../xbmc/settings/MediaSourceSettings.cpp
  ../../xbmc/settings/MediaSourceSettings.h
  ../../xbmc/settings/SettingAddon.cpp
  ../../xbmc/settings/SettingAddon.h
  ../../xbmc/settings/SettingConditions.cpp
  ../../xbmc/settings/SettingConditions.h
  ../../xbmc/settings/SettingControl.cpp
  ../../xbmc/settings/SettingControl.h
  ../../xbmc/settings/SettingCreator.cpp
  ../../xbmc/settings/SettingCreator.h
  ../../xbmc/settings/SettingDateTime.cpp
  ../../xbmc/settings/SettingDateTime.h
  ../../xbmc/settings/SettingPath.cpp
  ../../xbmc/settings/SettingPath.h
  ../../xbmc/settings/Settings.cpp
  ../../xbmc/settings/Settings.h
  ../../xbmc/settings/SettingsBase.cpp
  ../../xbmc/settings/SettingsBase.h
  ../../xbmc/settings/SettingsComponent.cpp
  ../../xbmc/settings/SettingsComponent.h
  ../../xbmc/settings/SettingUtils.cpp
  ../../xbmc/settings/SettingUtils.h
  ../../xbmc/settings/SkinSettings.cpp
  ../../xbmc/settings/SkinSettings.h
  ../../xbmc/settings/windows/GUIControlSettings.cpp
  ../../xbmc/settings/windows/GUIControlSettings.h
  ../../xbmc/settings/windows/GUIWindowSettings.cpp
  ../../xbmc/settings/windows/GUIWindowSettings.h
  ../../xbmc/settings/windows/GUIWindowSettingsCategory.cpp
  ../../xbmc/settings/windows/GUIWindowSettingsCategory.h
  ../../xbmc/settings/windows/GUIWindowSettingsScreenCalibration.cpp
  ../../xbmc/settings/windows/GUIWindowSettingsScreenCalibration.h
  ../../xbmc/SortFileItem.h
  ../../xbmc/StaticDll.cpp
  ../../xbmc/StaticDll.h
  ../../xbmc/storage/AutorunMediaJob.cpp
  ../../xbmc/storage/AutorunMediaJob.h
  ../../xbmc/storage/cdioSupport.cpp
  ../../xbmc/storage/cdioSupport.h
  ../../xbmc/storage/DetectDVDType.cpp
  ../../xbmc/storage/DetectDVDType.h
  ../../xbmc/storage/IoSupport.cpp
  ../../xbmc/storage/IoSupport.h
  ../../xbmc/storage/IStorageProvider.h
  ../../xbmc/storage/MediaManager.cpp
  ../../xbmc/storage/MediaManager.h
  ../../xbmc/swig/AddonModuleXbmc.i.cpp
  ../../xbmc/swig/AddonModuleXbmcaddon.i.cpp
  ../../xbmc/swig/AddonModuleXbmcdrm.i.cpp
  ../../xbmc/swig/AddonModuleXbmcgui.i.cpp
  ../../xbmc/swig/AddonModuleXbmcplugin.i.cpp
  ../../xbmc/swig/AddonModuleXbmcvfs.i.cpp
  ../../xbmc/swig/AddonModuleXbmcwsgi.i.cpp
  ../../xbmc/system_gl.h
  ../../xbmc/SystemGlobals.cpp
  ../../xbmc/TextureCache.cpp
  ../../xbmc/TextureCache.h
  ../../xbmc/TextureCacheJob.cpp
  ../../xbmc/TextureCacheJob.h
  ../../xbmc/TextureDatabase.cpp
  ../../xbmc/TextureDatabase.h
  ../../xbmc/threads/Atomics.cpp
  ../../xbmc/threads/Atomics.h
  ../../xbmc/threads/Condition.h
  ../../xbmc/threads/CriticalSection.h
  ../../xbmc/threads/Event.cpp
  ../../xbmc/threads/Event.h
  ../../xbmc/threads/Helpers.h
  ../../xbmc/threads/IRunnable.h
  ../../xbmc/threads/Lockables.h
  ../../xbmc/threads/MipsAtomics.h
  ../../xbmc/threads/platform/win/ThreadImpl.h
  ../../xbmc/threads/platform/win/ThreadImplWin32.cpp
  ../../xbmc/threads/platform/win/ThreadImplWin32.h
  ../../xbmc/threads/platform/win/Win32Exception.cpp
  ../../xbmc/threads/platform/win/Win32Exception.h
  ../../xbmc/threads/SharedSection.h
  ../../xbmc/threads/SingleLock.h
  ../../xbmc/threads/SystemClock.cpp
  ../../xbmc/threads/SystemClock.h
  ../../xbmc/threads/Thread.cpp
  ../../xbmc/threads/Thread.h
  ../../xbmc/threads/ThreadImpl.h
  ../../xbmc/threads/Timer.cpp
  ../../xbmc/threads/Timer.h
  ../../xbmc/ThumbLoader.cpp
  ../../xbmc/ThumbLoader.h
  ../../xbmc/utils/ActorProtocol.cpp
  ../../xbmc/utils/ActorProtocol.h
  ../../xbmc/utils/AlarmClock.cpp
  ../../xbmc/utils/AlarmClock.h
  ../../xbmc/utils/AliasShortcutUtils.cpp
  ../../xbmc/utils/AliasShortcutUtils.h
  ../../xbmc/utils/AMLUtils.cpp
  ../../xbmc/utils/AMLUtils.h
  ../../xbmc/utils/Archive.cpp
  ../../xbmc/utils/Archive.h
  ../../xbmc/utils/auto_buffer.cpp
  ../../xbmc/utils/auto_buffer.h
  ../../xbmc/utils/Base64.cpp
  ../../xbmc/utils/Base64.h
  ../../xbmc/utils/BitstreamConverter.cpp
  ../../xbmc/utils/BitstreamConverter.h
  ../../xbmc/utils/BitstreamReader.cpp
  ../../xbmc/utils/BitstreamReader.h
  ../../xbmc/utils/BitstreamStats.cpp
  ../../xbmc/utils/BitstreamStats.h
  ../../xbmc/utils/BitstreamWriter.cpp
  ../../xbmc/utils/BitstreamWriter.h
  ../../xbmc/utils/BooleanLogic.cpp
  ../../xbmc/utils/BooleanLogic.h
  ../../xbmc/utils/CharsetConverter.cpp
  ../../xbmc/utils/CharsetConverter.h
  ../../xbmc/utils/CharsetDetection.cpp
  ../../xbmc/utils/CharsetDetection.h
  ../../xbmc/utils/Color.h
  ../../xbmc/utils/ColorUtils.cpp
  ../../xbmc/utils/ColorUtils.h
  ../../xbmc/utils/CPUInfo.cpp
  ../../xbmc/utils/CPUInfo.h
  ../../xbmc/utils/Crc32.cpp
  ../../xbmc/utils/Crc32.h
  ../../xbmc/utils/CryptThreading.cpp
  ../../xbmc/utils/CryptThreading.h
  ../../xbmc/utils/DatabaseUtils.cpp
  ../../xbmc/utils/DatabaseUtils.h
  ../../xbmc/utils/Digest.cpp
  ../../xbmc/utils/Digest.h
  ../../xbmc/utils/EmbeddedArt.cpp
  ../../xbmc/utils/EmbeddedArt.h
  ../../xbmc/utils/EndianSwap.cpp
  ../../xbmc/utils/endianswap.h
  ../../xbmc/utils/EventStream.h
  ../../xbmc/utils/EventStreamDetail.h
  ../../xbmc/utils/Fanart.cpp
  ../../xbmc/utils/Fanart.h
  ../../xbmc/utils/FileExtensionProvider.cpp
  ../../xbmc/utils/FileExtensionProvider.h
  ../../xbmc/utils/FileOperationJob.cpp
  ../../xbmc/utils/FileOperationJob.h
  ../../xbmc/utils/FileUtils.cpp
  ../../xbmc/utils/FileUtils.h
  ../../xbmc/utils/fstrcmp.c
  ../../xbmc/utils/fstrcmp.h
  ../../xbmc/utils/Geometry.h
  ../../xbmc/utils/GlobalsHandling.h
  ../../xbmc/utils/GLUtils.cpp
  ../../xbmc/utils/GLUtils.h
  ../../xbmc/utils/GroupUtils.cpp
  ../../xbmc/utils/GroupUtils.h
  ../../xbmc/utils/HTMLUtil.cpp
  ../../xbmc/utils/HTMLUtil.h
  ../../xbmc/utils/HttpHeader.cpp
  ../../xbmc/utils/HttpHeader.h
  ../../xbmc/utils/HttpParser.cpp
  ../../xbmc/utils/HttpParser.h
  ../../xbmc/utils/HttpRangeUtils.cpp
  ../../xbmc/utils/HttpRangeUtils.h
  ../../xbmc/utils/HttpResponse.cpp
  ../../xbmc/utils/HttpResponse.h
  ../../xbmc/utils/IArchivable.h
  ../../xbmc/utils/IBufferObject.h
  ../../xbmc/utils/ILocalizer.h
  ../../xbmc/utils/InfoLoader.cpp
  ../../xbmc/utils/InfoLoader.h
  ../../xbmc/utils/IRssObserver.h
  ../../xbmc/utils/ISerializable.h
  ../../xbmc/utils/ISortable.h
  ../../xbmc/utils/IXmlDeserializable.h
  ../../xbmc/utils/Job.h
  ../../xbmc/utils/JobManager.cpp
  ../../xbmc/utils/JobManager.h
  ../../xbmc/utils/JSONVariantParser.cpp
  ../../xbmc/utils/JSONVariantParser.h
  ../../xbmc/utils/JSONVariantWriter.cpp
  ../../xbmc/utils/JSONVariantWriter.h
  ../../xbmc/utils/LabelFormatter.cpp
  ../../xbmc/utils/LabelFormatter.h
  ../../xbmc/utils/LangCodeExpander.cpp
  ../../xbmc/utils/LangCodeExpander.h
  ../../xbmc/utils/LegacyPathTranslation.cpp
  ../../xbmc/utils/LegacyPathTranslation.h
  ../../xbmc/utils/Literals.h
  ../../xbmc/utils/Locale.cpp
  ../../xbmc/utils/Locale.h
  ../../xbmc/utils/log.cpp
  ../../xbmc/utils/log.h
  ../../xbmc/utils/MathUtils.h
  ../../xbmc/utils/Mime.cpp
  ../../xbmc/utils/Mime.h
  ../../xbmc/utils/Observer.cpp
  ../../xbmc/utils/Observer.h
  ../../xbmc/utils/params_check_macros.h
  ../../xbmc/utils/POUtils.cpp
  ../../xbmc/utils/POUtils.h
  ../../xbmc/utils/ProgressJob.cpp
  ../../xbmc/utils/ProgressJob.h
  ../../xbmc/utils/Random.h
  ../../xbmc/utils/RecentlyAddedJob.cpp
  ../../xbmc/utils/RecentlyAddedJob.h
  ../../xbmc/utils/RegExp.cpp
  ../../xbmc/utils/RegExp.h
  ../../xbmc/utils/rfft.cpp
  ../../xbmc/utils/rfft.h
  ../../xbmc/utils/RingBuffer.cpp
  ../../xbmc/utils/RingBuffer.h
  ../../xbmc/utils/RssManager.cpp
  ../../xbmc/utils/RssManager.h
  ../../xbmc/utils/RssReader.cpp
  ../../xbmc/utils/RssReader.h
  ../../xbmc/utils/SaveFileStateJob.cpp
  ../../xbmc/utils/SaveFileStateJob.h
  ../../xbmc/utils/ScopeGuard.h
  ../../xbmc/utils/ScraperParser.cpp
  ../../xbmc/utils/ScraperParser.h
  ../../xbmc/utils/ScraperUrl.cpp
  ../../xbmc/utils/ScraperUrl.h
  ../../xbmc/utils/Screenshot.cpp
  ../../xbmc/utils/Screenshot.h
  ../../xbmc/utils/ScreenshotAML.cpp
  ../../xbmc/utils/ScreenshotAML.h
  ../../xbmc/utils/sha1.hpp
  ../../xbmc/utils/SortUtils.cpp
  ../../xbmc/utils/SortUtils.h
  ../../xbmc/utils/Speed.cpp
  ../../xbmc/utils/Speed.h
  ../../xbmc/utils/Stopwatch.cpp
  ../../xbmc/utils/Stopwatch.h
  ../../xbmc/utils/StreamDetails.cpp
  ../../xbmc/utils/StreamDetails.h
  ../../xbmc/utils/StreamUtils.cpp
  ../../xbmc/utils/StreamUtils.h
  ../../xbmc/utils/StringUtils.h
  ../../xbmc/utils/StringUtils2.cpp
  ../../xbmc/utils/StringValidation.cpp
  ../../xbmc/utils/StringValidation.h
  ../../xbmc/utils/SysfsUtils.cpp
  ../../xbmc/utils/SysfsUtils.h
  ../../xbmc/utils/SystemInfo.cpp
  ../../xbmc/utils/SystemInfo.h
  ../../xbmc/utils/Temperature.cpp
  ../../xbmc/utils/Temperature.h
  ../../xbmc/utils/TextSearch.cpp
  ../../xbmc/utils/TextSearch.h
  ../../xbmc/utils/TimeUtils.h
  ../../xbmc/utils/TimeUtils2.cpp
  ../../xbmc/utils/TransformMatrix.h
  ../../xbmc/utils/URIUtils.cpp
  ../../xbmc/utils/URIUtils.h
  ../../xbmc/utils/UrlOptions.cpp
  ../../xbmc/utils/UrlOptions.h
  ../../xbmc/utils/Utf8Utils.cpp
  ../../xbmc/utils/Utf8Utils.h
  ../../xbmc/utils/uXstrings.h
  ../../xbmc/utils/Variant.cpp
  ../../xbmc/utils/Variant.h
  ../../xbmc/utils/VC1BitstreamParser.cpp
  ../../xbmc/utils/VC1BitstreamParser.h
  ../../xbmc/utils/Vector.cpp
  ../../xbmc/utils/Vector.h
  ../../xbmc/utils/XBMCTinyXML.cpp
  ../../xbmc/utils/XBMCTinyXML.h
  ../../xbmc/utils/XMLUtils.cpp
  ../../xbmc/utils/XMLUtils.h
  ../../xbmc/utils/XSLTUtils.cpp
  ../../xbmc/utils/XSLTUtils.h
  ../../xbmc/video/Bookmark.cpp
  ../../xbmc/video/Bookmark.h
  ../../xbmc/video/ContextMenus.h
  ../../xbmc/video/ContextMenusVideo.cpp
  ../../xbmc/video/ContextMenusVideo.h
  ../../xbmc/video/dialogs/GUIDialogAudioSettings.cpp
  ../../xbmc/video/dialogs/GUIDialogAudioSettings.h
  ../../xbmc/video/dialogs/GUIDialogCMSSettings.cpp
  ../../xbmc/video/dialogs/GUIDialogCMSSettings.h
  ../../xbmc/video/dialogs/GUIDialogFullScreenInfo.cpp
  ../../xbmc/video/dialogs/GUIDialogFullScreenInfo.h
  ../../xbmc/video/dialogs/GUIDialogSubtitles.cpp
  ../../xbmc/video/dialogs/GUIDialogSubtitles.h
  ../../xbmc/video/dialogs/GUIDialogSubtitleSettings.cpp
  ../../xbmc/video/dialogs/GUIDialogSubtitleSettings.h
  ../../xbmc/video/dialogs/GUIDialogTeletext.cpp
  ../../xbmc/video/dialogs/GUIDialogTeletext.h
  ../../xbmc/video/dialogs/GUIDialogVideoBookmarks.cpp
  ../../xbmc/video/dialogs/GUIDialogVideoBookmarks.h
  ../../xbmc/video/dialogs/GUIDialogVideoInfo.cpp
  ../../xbmc/video/dialogs/GUIDialogVideoInfo.h
  ../../xbmc/video/dialogs/GUIDialogVideoOSD.cpp
  ../../xbmc/video/dialogs/GUIDialogVideoOSD.h
  ../../xbmc/video/dialogs/GUIDialogVideoSettings.cpp
  ../../xbmc/video/dialogs/GUIDialogVideoSettings.h
  ../../xbmc/video/Episode.h
  ../../xbmc/video/GUIViewStateVideo.cpp
  ../../xbmc/video/GUIViewStateVideo.h
  ../../xbmc/video/jobs/VideoLibraryCleaningJob.cpp
  ../../xbmc/video/jobs/VideoLibraryCleaningJob.h
  ../../xbmc/video/jobs/VideoLibraryJob.cpp
  ../../xbmc/video/jobs/VideoLibraryJob.h
  ../../xbmc/video/jobs/VideoLibraryMarkWatchedJob.cpp
  ../../xbmc/video/jobs/VideoLibraryMarkWatchedJob.h
  ../../xbmc/video/jobs/VideoLibraryProgressJob.cpp
  ../../xbmc/video/jobs/VideoLibraryProgressJob.h
  ../../xbmc/video/jobs/VideoLibraryRefreshingJob.cpp
  ../../xbmc/video/jobs/VideoLibraryRefreshingJob.h
  ../../xbmc/video/jobs/VideoLibraryResetResumePointJob.cpp
  ../../xbmc/video/jobs/VideoLibraryResetResumePointJob.h
  ../../xbmc/video/jobs/VideoLibraryScanningJob.cpp
  ../../xbmc/video/jobs/VideoLibraryScanningJob.h
  ../../xbmc/video/PlayerController.cpp
  ../../xbmc/video/PlayerController.h
  ../../xbmc/video/tags/IVideoInfoTagLoader.h
  ../../xbmc/video/tags/VideoInfoTagLoaderFactory.cpp
  ../../xbmc/video/tags/VideoInfoTagLoaderFactory.h
  ../../xbmc/video/tags/VideoTagLoaderFFmpeg.cpp
  ../../xbmc/video/tags/VideoTagLoaderFFmpeg.h
  ../../xbmc/video/tags/VideoTagLoaderNFO.cpp
  ../../xbmc/video/tags/VideoTagLoaderNFO.h
  ../../xbmc/video/tags/VideoTagLoaderPlugin.cpp
  ../../xbmc/video/tags/VideoTagLoaderPlugin.h
  ../../xbmc/video/Teletext.cpp
  ../../xbmc/video/Teletext.h
  ../../xbmc/video/TeletextDefines.h
  ../../xbmc/video/VideoDatabase.cpp
  ../../xbmc/video/VideoDatabase.h
  ../../xbmc/video/VideoDbUrl.cpp
  ../../xbmc/video/VideoDbUrl.h
  ../../xbmc/video/VideoInfoDownloader.cpp
  ../../xbmc/video/VideoInfoDownloader.h
  ../../xbmc/video/VideoInfoScanner.cpp
  ../../xbmc/video/VideoInfoScanner.h
  ../../xbmc/video/VideoInfoTag.cpp
  ../../xbmc/video/VideoInfoTag.h
  ../../xbmc/video/VideoLibraryQueue.cpp
  ../../xbmc/video/VideoLibraryQueue.h
  ../../xbmc/video/VideoThumbLoader.cpp
  ../../xbmc/video/VideoThumbLoader.h
  ../../xbmc/video/ViewModeSettings.cpp
  ../../xbmc/video/ViewModeSettings.h
  ../../xbmc/video/windows/GUIWindowFullScreen.cpp
  ../../xbmc/video/windows/GUIWindowFullScreen.h
  ../../xbmc/video/windows/GUIWindowFullScreenDefines.h
  ../../xbmc/video/windows/GUIWindowVideoBase.cpp
  ../../xbmc/video/windows/GUIWindowVideoBase.h
  ../../xbmc/video/windows/GUIWindowVideoNav.cpp
  ../../xbmc/video/windows/GUIWindowVideoNav.h
  ../../xbmc/video/windows/GUIWindowVideoPlaylist.cpp
  ../../xbmc/video/windows/GUIWindowVideoPlaylist.h
  ../../xbmc/video/windows/VideoFileItemListModifier.cpp
  ../../xbmc/video/windows/VideoFileItemListModifier.h
  ../../xbmc/view/GUIViewControl.cpp
  ../../xbmc/view/GUIViewControl.h
  ../../xbmc/view/GUIViewState.cpp
  ../../xbmc/view/GUIViewState.h
  ../../xbmc/view/ViewDatabase.cpp
  ../../xbmc/view/ViewDatabase.h
  ../../xbmc/view/ViewState.h
  ../../xbmc/view/ViewStateSettings.cpp
  ../../xbmc/view/ViewStateSettings.h
  ../../xbmc/weather/GUIWindowWeather.cpp
  ../../xbmc/weather/GUIWindowWeather.h
  ../../xbmc/weather/WeatherJob.cpp
  ../../xbmc/weather/WeatherJob.h
  ../../xbmc/weather/WeatherManager.cpp
  ../../xbmc/weather/WeatherManager.h
  ../../xbmc/windowing/GraphicContext.cpp
  ../../xbmc/windowing/GraphicContext.h
  ../../xbmc/windowing/OSScreenSaver.cpp
  ../../xbmc/windowing/OSScreenSaver.h
  ../../xbmc/windowing/qt/OSScreenSaverQt.cpp
  ../../xbmc/windowing/qt/OSScreenSaverQt.h
  ../../xbmc/windowing/qt/QtUtils.cpp
  ../../xbmc/windowing/qt/QtUtils.h
  ../../xbmc/windowing/qt/VideoSyncQt.cpp
  ../../xbmc/windowing/qt/VideoSyncQt.h
  ../../xbmc/windowing/qt/WinEventsQt.cpp
  ../../xbmc/windowing/qt/WinEventsQt.h
  ../../xbmc/windowing/qt/WinKeyMapQt.h
  ../../xbmc/windowing/qt/WinSystemQt.cpp
  ../../xbmc/windowing/qt/WinSystemQt.h
  ../../xbmc/windowing/qt/WinSystemQtContext.cpp
  ../../xbmc/windowing/qt/WinSystemQtContext.h
  ../../xbmc/windowing/Resolution.cpp
  ../../xbmc/windowing/Resolution.h
  ../../xbmc/windowing/VideoSync.h
  ../../xbmc/windowing/windows/VideoSyncD3D.cpp
  ../../xbmc/windowing/windows/VideoSyncD3D.h
  ../../xbmc/windowing/windows/WinEventsWin32.cpp
  ../../xbmc/windowing/windows/WinEventsWin32.h
  ../../xbmc/windowing/windows/WinKeyMap.h
  ../../xbmc/windowing/windows/WinSystemWin32.cpp
  ../../xbmc/windowing/windows/WinSystemWin32.h
  ../../xbmc/windowing/windows/WinSystemWin32DX.cpp
  ../../xbmc/windowing/windows/WinSystemWin32DX.h
  ../../xbmc/windowing/WinEvents.h
  ../../xbmc/windowing/WinSystem.cpp
  ../../xbmc/windowing/WinSystem.h
  ../../xbmc/windowing/XBMC_events.h
  ../../xbmc/windows/GUIMediaWindow.cpp
  ../../xbmc/windows/GUIMediaWindow.h
  ../../xbmc/windows/GUIWindowDebugInfo.cpp
  ../../xbmc/windows/GUIWindowDebugInfo.h
  ../../xbmc/windows/GUIWindowFileManager.cpp
  ../../xbmc/windows/GUIWindowFileManager.h
  ../../xbmc/windows/GUIWindowHome.cpp
  ../../xbmc/windows/GUIWindowHome.h
  ../../xbmc/windows/GUIWindowLoginScreen.cpp
  ../../xbmc/windows/GUIWindowLoginScreen.h
  ../../xbmc/windows/GUIWindowPointer.cpp
  ../../xbmc/windows/GUIWindowPointer.h
  ../../xbmc/windows/GUIWindowScreensaver.cpp
  ../../xbmc/windows/GUIWindowScreensaver.h
  ../../xbmc/windows/GUIWindowScreensaverDim.cpp
  ../../xbmc/windows/GUIWindowScreensaverDim.h
  ../../xbmc/windows/GUIWindowSplash.cpp
  ../../xbmc/windows/GUIWindowSplash.h
  ../../xbmc/windows/GUIWindowStartup.cpp
  ../../xbmc/windows/GUIWindowStartup.h
  ../../xbmc/windows/GUIWindowSystemInfo.cpp
  ../../xbmc/windows/GUIWindowSystemInfo.h
  ../../xbmc/XBApplicationEx.cpp
  ../../xbmc/XBApplicationEx.h
  ../../xbmc/XBDateTime.cpp
  ../../xbmc/XBDateTime.h
)
# Additional include directories
set_property(TARGET libkodi
  APPEND PROPERTY INCLUDE_DIRECTORIES
  $<$<CONFIG:Release>:
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\sysheaders;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libpthread\;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\cores\AudioEngine\;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\Python-2.7.14\Include;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\openssl-1.0.2p\include;
    ${CMAKE_CURRENT_SOURCE_DIR};
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\ffmpeg;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libUPnP;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libUPnP\Platinum\Source\Devices\MediaRenderer;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libUPnP\Platinum\Source\Devices\MediaConnect;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libUPnP\Platinum\Source\Devices\MediaServer;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libUPnP\Platinum\Source\Platinum;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libUPnP\Platinum\Source\Core;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libUPnP\Platinum\Source\Extras;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libUPnP\Neptune\Source\Core;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libUPnP\Neptune\Source\System\Win32;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libpcre;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\Effects11\inc;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\libfreetype\include;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\..\DependLibs\PlatformLib\platform\win32;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\addons\library.kodi.guilib;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\addons\library.xbmc.addon;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\addons\library.xbmc.pvr;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\addons\library.GoTv.adsp;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\addons\library.xbmc.codec;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib\gtest\include;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\platform\win32;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\cores\VideoPlayer;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\build;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\project\BuildDependencies\win32\include;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\project\BuildDependencies\mingwlibs\win32\include;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\build\libdvd\include;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\project\BuildDependencies\win32\include\lzo;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\project\BuildDependencies\win32\include\python;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\cores\AudioEngine;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib\libUPnP;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib\libUPnP\Platinum\Source\Core;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib\libUPnP\Platinum\Source\Platinum;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib\libUPnP\Platinum\Source\Devices\MediaConnect;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib\libUPnP\Platinum\Source\Devices\MediaRenderer;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib\libUPnP\Platinum\Source\Devices\MediaServer;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib\libUPnP\Platinum\Source\Extras;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib\libUPnP\Neptune\Source\System\Posix;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib\libUPnP\Neptune\Source\Core;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\build\guilib;
    J:\NlcJunk\GoTvCore\build\VS2017\..\..\xbmc\lib\win32\Effects11\inc>
)

# Preprocessor definitions
target_compile_definitions(libkodi PRIVATE
  $<$<CONFIG:Release>:_MBCS;NDEBUG;RELEASE;TARGET_OS_WINDOWS;_LIB;LIB_STATIC;NOMINMAX;__STDC_CONSTANT_MACROS;WIN64;_WINDOWS>
)

# Minimal rebuild
if (MSVC)
  target_compile_options(libkodi PRIVATE
    "$<$<CONFIG:Release>:/Gm->"
  )
endif ()

# Multi-processor compilation
if (MSVC)
  target_compile_options(libkodi PRIVATE
    "$<$<CONFIG:Release>:/MP>"
  )
endif ()

# Precompiled header files
if (MSVC)
  target_compile_options(libkodi PRIVATE
    ""
  )
  set_property(SOURCE ../../xbmc/addons/AddonBuilder.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/BinaryAddonCache.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/AddonDatabase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/AddonInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/AddonInstaller.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/AddonManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/AddonStatusHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/AddonSystemSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/AddonVersion.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/AudioEncoder.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/ContextMenuAddon.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/FontResource.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/FilesystemInstaller.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/GameResource.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/GUIDialogAddonInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/GUIViewStateAddonBrowser.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/GUIWindowAddonBrowser.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/ImageDecoder.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/ImageResource.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/LanguageResource.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/PluginSource.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/PVRClient.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/Repository.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/RepositoryUpdater.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/Scraper.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/ScreenSaver.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/Service.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/Skin.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/UISoundsResource.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/VFSEntry.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/Visualization.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/Webinterface.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/binary-addons/BinaryAddonManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/binary-addons/AddonDll.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/binary-addons/AddonInstanceHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/binary-addons/BinaryAddonBase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/binary-addons/BinaryAddonExtensions.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/binary-addons/BinaryAddonType.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/AddonInterfaces.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/General.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/Filesystem.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/Addon/AddonCallbacksAddon.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/AddonCallbacksGUI.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/AddonGUIRenderingControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/AddonGUIWindow.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/controls/Button.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/controls/Edit.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/controls/FadeLabel.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/controls/Image.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/controls/Label.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/controls/RadioButton.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/controls/Rendering.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/controls/SettingsSlider.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/controls/Slider.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/controls/Spin.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/controls/TextBox.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/dialogs/ContextMenu.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/dialogs/ExtendedProgressBar.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/dialogs/FileBrowser.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/dialogs/Numeric.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/dialogs/Select.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/dialogs/TextViewer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/interfaces/GUI/dialogs/YesNo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/settings/AddonSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/settings/GUIDialogAddonSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/addons/settings/SettingUrlEncodedString.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cdrip/CDDARipJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cdrip/Encoder.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cdrip/EncoderFFmpeg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cdrip/CDDARipper.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/contrib/kissfft/kiss_fft.c
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/contrib/kissfft/kiss_fftr.c
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DataCacheCore.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/AEResampleFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/AESinkFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Encoders/AEEncoderFFmpeg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAE.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEBuffer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEFilter.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAESink.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEStream.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAESound.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAESettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Engines/ActiveAE/ActiveAEResampleFFMPEG.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Sinks/AESinkWASAPI.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Sinks/AESinkDirectSound.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Sinks/windows/AESinkFactoryWin.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Sinks/windows/AESinkFactoryWin32.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Utils/AEBitstreamPacker.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Utils/AEChannelInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Utils/AEDeviceInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Utils/AELimiter.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Utils/AEPackIEC61937.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Utils/AEStreamInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/AudioEngine/Utils/AEUtil.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/coff.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/dll.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/DllLoader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/DllLoaderContainer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/dll_tracker.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/dll_tracker_file.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/dll_tracker_library.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/dll_util.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/LibraryLoader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/Win32DllLoader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/exports/emu_dummy.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/exports/emu_msvcrt.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/DllLoader/exports/util/EmuFileWrapper.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/ExternalPlayer/ExternalPlayer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/RetroPlayer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/RetroPlayerAutoSave.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/RetroPlayerInput.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/RetroPlayerUtils.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/guibridge/GUIGameRenderManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/guibridge/GUIGameSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/guibridge/GUIGameVideoHandle.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/guibridge/GUIRenderHandle.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/guibridge/GUIRenderTarget.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/guibridge/GUIRenderTargetFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/guicontrols/GUIGameControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/guicontrols/GUIRenderSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/guiwindows/GameWindowFullScreen.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/guiwindows/GameWindowFullScreenText.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/process/windows/RPProcessInfoWin.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/rendering/RenderContext.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/rendering/RenderSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/rendering/RenderTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/rendering/RenderVideoSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/rendering/RPRenderManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/rendering/VideoRenderers/RPBaseRenderer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/rendering/VideoRenderers/RPRendererGuiTexture.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/rendering/VideoRenderers/RPWinRenderer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/RetroPlayer/rendering/VideoShaders/windows/RPWinOutputShader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/AudioSinkAE.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDClock.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDDemuxSPU.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDFileInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDMessage.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDMessageQueue.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDOverlayContainer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDStreamInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/PTSTracker.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/Edl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoPlayerAudio.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoPlayer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoPlayerRadioRDS.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoPlayerSubtitle.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoPlayerTeletext.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoPlayerVideo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoReferenceClock.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/DVDCodecUtils.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/DVDFactoryCodec.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Audio/DVDAudioCodecFFmpeg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Audio/DVDAudioCodecPassthrough.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodec.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecFFmpeg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecSSA.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecText.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/DVDOverlayCodecTX3G.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/contrib/cc_decoder.c
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Overlay/contrib/cc_decoder708.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/AddonVideoCodec.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodec.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DVDVideoCodecFFmpeg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DVDVideoPPFFmpeg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDCodecs/Video/DXVA.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDDemuxers/DemuxMultiSource.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemux.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxBXA.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxCC.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxCDDA.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxClient.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxFFmpeg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxUtils.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDDemuxVobsub.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDDemuxers/DVDFactoryDemuxer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDFactoryInputStream.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStream.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamFFmpeg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamMemory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamNavigator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamStack.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDStateSerializer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamAddon.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDInputStreams/InputStreamMultiSource.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDInputStreams/DVDInputStreamBluray.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDFactorySubtitle.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleLineCollection.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserMicroDVD.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserMPL2.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserSami.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserSubrip.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserVplayer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleStream.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitlesLibass.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleParserSSA.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleTagMicroDVD.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/DVDSubtitles/DVDSubtitleTagSami.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/Process/ProcessInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/Process/VideoBuffer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/Process/windows/ProcessInfoWin.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/BaseRenderer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/ColorManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRenderer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRendererGUI.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRendererUtil.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderCapture.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderFlags.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/RenderManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/DebugRenderer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/WinRenderer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/WinRenderBuffer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/OverlayRendererDX.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/HwDecRender/DXVAHD.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/ConvolutionKernels.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/ConversionMatrix.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/VideoPlayer/VideoRenderers/VideoShaders/WinVideoFilter.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/paplayer/AudioDecoder.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/paplayer/CodecFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/paplayer/PAPlayer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/paplayer/VideoPlayerCodec.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/playercorefactory/PlayerCoreFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/cores/playercorefactory/PlayerSelectionRule.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dbwrappers/Database.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dbwrappers/DatabaseQuery.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dbwrappers/dataset.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dbwrappers/qry_dat.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dbwrappers/sqlitedataset.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dbwrappers/mysqldataset.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogBoxBase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogBusy.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogButtonMenu.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogCache.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogContextMenu.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogExtendedProgressBar.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogFileBrowser.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogGamepad.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogKaiToast.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogKeyboardGeneric.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogKeyboardTouch.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogMediaFilter.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogMediaSource.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogNumeric.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogOK.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogPlayEject.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogPlayerControls.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogPlayerProcessInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogProgress.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogSeekBar.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogSelect.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogSimpleMenu.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogSlider.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogSmartPlaylistEditor.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogSmartPlaylistRule.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogSubMenu.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogTextViewer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogVolumeBar.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/dialogs/GUIDialogYesNo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/events/AddonEvent.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/events/AddonManagementEvent.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/events/BaseEvent.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/events/EventLog.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/events/EventLogManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/events/MediaLibraryEvent.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/events/windows/GUIViewStateEventLog.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/events/windows/GUIWindowEventLog.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/favourites/GUIDialogFavourites.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/favourites/FavouritesService.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/AddonsDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/AudioBookFileDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/CacheStrategy.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/CircularCache.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/CurlFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/DAVCommon.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/DAVDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/DAVFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/DirectoryCache.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/Directory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/DirectoryFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/DirectoryHistory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/DllLibCurl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/EventsDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/FavouritesDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/FileCache.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/FileDirectoryFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/FileFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/FTPDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/FTPParse.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/HTTPDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/IDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/IFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/ImageFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/iso9660.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/ISO9660Directory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/ISOFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/LibraryDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MultiPathDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MultiPathFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicFileDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicSearchDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/OverrideDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/OverrideFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/PipeFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/PipesManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/PlaylistDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/PlaylistFileDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/PluginDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/PVRDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/ResourceDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/ResourceFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/RSSDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/ShoutcastFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/SmartPlaylistDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/SourcesDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/SpecialProtocol.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/SpecialProtocolDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/SpecialProtocolFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/StackDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/udf25.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/UDFDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/UDFFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VirtualDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/XbtDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/XbtFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/XbtManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/ZeroconfDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/ZipDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/ZipFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/ZipManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/BlurayDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/BlurayFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/CDDADirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/CDDAFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/NFSDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/NFSFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/NptXbmcFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/UPnPDirectory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/UPnPFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumCompilations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumCompilationsSongs.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbum.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyAdded.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyAddedSong.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyPlayed.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumRecentlyPlayedSong.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumTop100.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeAlbumTop100Song.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeArtist.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSingles.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSong.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeSongTop100.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeTop100.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeYearAlbum.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/MusicDatabaseDirectory/DirectoryNodeYearSong.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeEpisodes.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeInProgressTvShows.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeMoviesOverview.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeMusicVideosOverview.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedEpisodes.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedMovies.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRecentlyAddedMusicVideos.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeRoot.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeSeasons.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleMovies.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleMusicVideos.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTitleTvShows.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/filesystem/VideoDatabaseDirectory/DirectoryNodeTvShowsOverview.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/GameServices.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/GameUtils.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/GameClient.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/GameClientProperties.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/GameClientSubsystem.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/GameClientTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/input/GameClientDevice.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/input/GameClientHardware.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/input/GameClientInput.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/input/GameClientJoystick.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/input/GameClientKeyboard.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/input/GameClientMouse.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/input/GameClientPort.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/addons/input/GameClientTopology.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/Controller.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/ControllerFeature.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/ControllerLayout.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/ControllerManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/ControllerTopology.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/ControllerTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/dialogs/GUIDialogAxisDetection.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/dialogs/GUIDialogButtonCapture.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/dialogs/GUIDialogIgnoreInput.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/guicontrols/GUICardinalFeatureButton.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/guicontrols/GUIControllerButton.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/guicontrols/GUIFeatureButton.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/guicontrols/GUIFeatureControls.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/guicontrols/GUIFeatureFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/guicontrols/GUIFeatureTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/guicontrols/GUIGameController.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/guicontrols/GUIScalarFeatureButton.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/guicontrols/GUISelectKeyButton.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/guicontrols/GUIThrottleButton.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/guicontrols/GUIWheelButton.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/types/ControllerGrid.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/types/ControllerTree.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/windows/GUIConfigurationWizard.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/windows/GUIControllerList.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/windows/GUIControllerWindow.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/controllers/windows/GUIFeatureList.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/dialogs/GUIDialogSelectGameClient.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/dialogs/osd/DialogGameOSD.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/dialogs/osd/DialogGameVideoFilter.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/dialogs/osd/DialogGameVideoSelect.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/dialogs/osd/DialogGameVolume.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/ports/InputSink.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/ports/Port.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/tags/GameInfoTag.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/windows/GUIViewStateWindowGames.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/games/windows/GUIWindowGames.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/DDSImage.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/DirtyRegionSolvers.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/DirtyRegionTracker.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/FFmpegImage.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIAction.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIAudioManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIBaseContainer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIBorderedImage.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIButtonControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIColorManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIControlFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIControlGroup.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIControlGroupList.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIControlLookup.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIControlProfiler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIDialog.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIEditControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIFadeLabelControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIFixedListContainer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIFont.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIFontCache.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIFontManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIFontTTF.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIImage.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIIncludes.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIKeyboardFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUILabelControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUILabel.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIListContainer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIListGroup.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIListItem.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIListItemLayout.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIListLabel.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIMessage.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIMoverControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIMultiImage.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIPanelContainer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIProgressControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIRadioButtonControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIRenderingControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIResizeControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIRSSControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIScrollBarControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUISettingsSliderControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUISliderControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUISpinControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUISpinControlEx.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIStaticItem.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUITextBox.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUITextLayout.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUITexture.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIToggleButtonControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIVideoControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIVisualisationControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIWindow.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIWindowManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIWrappingListContainer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/imagefactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/IWindowManagerCallback.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/LocalizeStrings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/StereoscopicsManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/TextureBundle.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/TextureBundleXBT.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/Texture.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/TextureManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/VisibleEffect.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/XBTF.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/XBTFReader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/D3DResource.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/DirectXGraphics.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIFontTTFDX.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUIShaderDX.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/GUITextureD3D.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/guilib/TextureDX.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/AppTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/ButtonTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/CustomControllerTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/GamepadTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/InertialScrollingHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/InputCodingTableBaiduPY.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/InputCodingTableBasePY.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/InputCodingTableFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/InputCodingTableKorean.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/InputManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/InputTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/IRTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/JoystickMapper.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/Key.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/KeyboardLayout.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/KeyboardLayoutManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/KeyboardStat.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/KeyboardTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/Keymap.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/KeymapEnvironment.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/TouchTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/WindowKeymap.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/WindowTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/XBMC_keytable.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/DeadzoneFilter.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/DriverPrimitive.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/JoystickEasterEgg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/JoystickMonitor.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/JoystickTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/JoystickUtils.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/RumbleGenerator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/dialogs/GUIDialogNewJoystick.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/generic/ButtonMapping.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/generic/DriverReceiving.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/generic/FeatureHandling.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/generic/InputHandling.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/keymaps/KeyHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/keymaps/KeymapHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/joysticks/keymaps/KeymapHandling.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/keyboard/KeyboardEasterEgg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/keyboard/KeymapActionMap.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/keyboard/generic/KeyboardInputHandling.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/mouse/MouseStat.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/mouse/MouseTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/mouse/generic/MouseInputHandling.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/touch/ITouchInputHandling.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/touch/generic/GenericTouchActionHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/touch/generic/GenericTouchInputHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/touch/generic/GenericTouchPinchDetector.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/touch/generic/GenericTouchRotateDetector.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/input/touch/generic/GenericTouchSwipeDetector.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/AddonBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/ApplicationBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/Builtins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/CECBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/GUIBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/GUIControlBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/GUIContainerBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/LibraryBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/OpticalBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/PictureBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/PlayerBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/ProfileBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/PVRBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/SkinBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/SystemBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/builtins/WeatherBuiltins.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/generic/ILanguageInvoker.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/generic/LanguageInvokerThread.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/generic/ScriptInvocationManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/info/InfoBool.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/info/InfoExpression.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/info/SkinVariable.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/AddonsOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/ApplicationOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/AudioLibrary.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/FavouritesOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/FileItemHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/FileOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/GUIOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/InputOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/JSONRPC.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/JSONServiceDescription.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/PlayerOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/PlaylistOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/ProfilesOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/PVROperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/SettingsOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/SystemOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/TextureOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/VideoLibrary.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/json-rpc/XBMCOperations.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/legacy/wsgi/WsgiErrorStream.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/legacy/wsgi/WsgiInputStream.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/legacy/wsgi/WsgiResponseBody.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/legacy/wsgi/WsgiResponse.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/python/AddonPythonInvoker.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/python/CallbackHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/python/ContextItemAddonInvoker.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/python/LanguageHook.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/python/PythonInvoker.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/python/XBPython.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/python/swig.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/interfaces/python/PyContext.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/listproviders/DirectoryProvider.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/listproviders/IListProvider.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/listproviders/MultiProvider.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/listproviders/StaticProvider.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/media/MediaType.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/messaging/ApplicationMessenger.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/messaging/helpers/DialogHelper.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/messaging/helpers/DialogOKHelper.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/Album.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/Artist.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/GUIViewStateMusic.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/MusicDatabase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/MusicDbUrl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/MusicInfoLoader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/MusicLibraryQueue.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/MusicThumbLoader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/Song.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/dialogs/GUIDialogInfoProviderSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/dialogs/GUIDialogMusicInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/dialogs/GUIDialogMusicOSD.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/dialogs/GUIDialogSongInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/dialogs/GUIDialogVisualisationPresetList.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/infoscanner/MusicAlbumInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/infoscanner/MusicArtistInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/infoscanner/MusicInfoScanner.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/infoscanner/MusicInfoScraper.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/jobs/MusicLibraryJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/jobs/MusicLibraryProgressJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/jobs/MusicLibraryCleaningJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/jobs/MusicLibraryExportJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/jobs/MusicLibraryScanningJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/tags/MusicInfoTag.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/tags/MusicInfoTagLoaderCDDA.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/tags/MusicInfoTagLoaderDatabase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/tags/MusicInfoTagLoaderFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/tags/MusicInfoTagLoaderFFmpeg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/tags/MusicInfoTagLoaderShn.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/tags/ReplayGain.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/tags/TagLibVFSStream.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/tags/TagLoaderTagLib.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/windows/GUIWindowMusicBase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/windows/GUIWindowMusicNav.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/windows/GUIWindowMusicPlaylist.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/windows/GUIWindowMusicPlaylistEditor.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/windows/GUIWindowVisualisation.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/music/windows/MusicFileItemListModifier.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/DNSNameCache.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/EventClient.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/EventPacket.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/EventServer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/GUIDialogAccessPoints.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/GUIDialogNetworkSetup.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/Network.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/NetworkServices.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/Socket.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/TCPServer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/UdpClient.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/WakeOnAccess.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/ZeroconfBrowser.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/Zeroconf.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/cddb.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/AirPlayServer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/AirTunesServer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/WebServer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/dacp/dacp.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/HTTPFileHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/HTTPImageHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/HTTPImageTransformationHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/HTTPJsonRpcHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/HTTPRequestHandlerUtils.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/HTTPVfsHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/HTTPWebinterfaceAddonsHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/HTTPWebinterfaceHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/IHTTPRequestHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/HTTPPythonHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/python/HTTPPythonInvoker.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/httprequesthandler/python/HTTPPythonWsgiInvoker.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/mdns/ZeroconfBrowserMDNS.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/mdns/ZeroconfMDNS.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/upnp/UPnP.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/upnp/UPnPInternal.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/upnp/UPnPPlayer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/upnp/UPnPRenderer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/upnp/UPnPServer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/upnp/UPnPSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/websocket/WebSocket.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/websocket/WebSocketManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/websocket/WebSocketV13.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/network/websocket/WebSocketV8.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/EventPollHandle.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/EventScanner.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/Peripherals.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/addons/AddonButtonMap.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/addons/AddonButtonMapping.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/addons/AddonInputHandling.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/addons/PeripheralAddon.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/addons/PeripheralAddonTranslator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/bus/PeripheralBus.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/bus/virtual/PeripheralBusAddon.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/bus/virtual/PeripheralBusApplication.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/bus/virtual/PeripheralBusCEC.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/Peripheral.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/PeripheralBluetooth.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/PeripheralDisk.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/PeripheralHID.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/PeripheralImon.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/PeripheralJoystick.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/PeripheralKeyboard.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/PeripheralMouse.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/PeripheralNIC.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/PeripheralNyxboard.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/PeripheralTuner.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/devices/PeripheralCecAdapter.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/dialogs/GUIDialogPeripherals.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/peripherals/dialogs/GUIDialogPeripheralSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/ExifParse.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/GUIDialogPictureInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/GUIViewStatePictures.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/GUIWindowPictures.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/GUIWindowSlideShow.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/IptcParse.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/JpegParse.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/libexif.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/Picture.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/PictureInfoLoader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/PictureInfoTag.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/PictureScalingAlgorithm.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/PictureThumbLoader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pictures/SlideShowPicture.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/playlists/PlayListB4S.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/playlists/PlayList.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/playlists/PlayListFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/playlists/PlayListM3U.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/playlists/PlayListPLS.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/playlists/PlayListURL.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/playlists/PlayListWPL.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/playlists/PlayListXML.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/playlists/SmartPlayList.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/playlists/SmartPlaylistFileItemListModifier.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/profiles/Profile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/profiles/dialogs/GUIDialogLockSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/profiles/dialogs/GUIDialogProfileSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/profiles/windows/GUIWindowSettingsProfile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/programs/GUIViewStatePrograms.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/programs/GUIWindowPrograms.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRActionListener.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRGUIInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRDatabase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRContextMenus.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRGUIActions.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRItem.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRChannelNumberInputHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRJobs.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRGUIChannelNavigator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/PVRGUIProgressHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/channels/PVRChannel.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/channels/PVRChannelGroup.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/channels/PVRChannelGroupInternal.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/channels/PVRChannelGroups.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/channels/PVRChannelGroupsContainer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/channels/PVRChannelNumber.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/channels/PVRRadioRDSInfoTag.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/dialogs/GUIDialogPVRChannelManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/dialogs/GUIDialogPVRChannelsOSD.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/dialogs/GUIDialogPVRGroupManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/dialogs/GUIDialogPVRGuideInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/dialogs/GUIDialogPVRChannelGuide.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/dialogs/GUIDialogPVRGuideSearch.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/dialogs/GUIDialogPVRRadioRDSInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/dialogs/GUIDialogPVRRecordingInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/dialogs/GUIDialogPVRRecordingSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/dialogs/GUIDialogPVRTimerSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/dialogs/GUIDialogPVRClientPriorities.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/epg/EpgContainer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/epg/Epg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/epg/EpgDatabase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/epg/EpgInfoTag.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/epg/EpgSearchFilter.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/recordings/PVRRecording.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/recordings/PVRRecordings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/recordings/PVRRecordingsPath.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/timers/PVRTimerInfoTag.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/timers/PVRTimers.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/timers/PVRTimerType.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/windows/GUIViewStatePVR.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/windows/GUIWindowPVRBase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/windows/GUIWindowPVRChannels.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/windows/GUIWindowPVRGuide.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/windows/GUIWindowPVRRecordings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/windows/GUIWindowPVRSearch.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/windows/GUIWindowPVRTimers.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/windows/GUIWindowPVRTimersBase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/windows/GUIWindowPVRTimerRules.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/windows/GUIEPGGridContainer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/pvr/windows/GUIEPGGridContainerModel.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/rendering/RenderSystem.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/rendering/dx/DeviceResources.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/rendering/dx/RenderSystemDX.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/AdvancedSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/DisplaySettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/GameSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/LibExportSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/MediaSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/MediaSourceSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/SettingAddon.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/SettingControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/SettingCreator.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/SettingDateTime.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/SettingPath.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/Settings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/SettingsBase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/SettingUtils.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/SkinSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/DiscSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/dialogs/GUIDialogContentSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/dialogs/GUIDialogLibExportSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/dialogs/GUIDialogSettingsBase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/dialogs/GUIDialogSettingsManagerBase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/dialogs/GUIDialogSettingsManualBase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/windows/GUIControlSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/windows/GUIWindowSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/windows/GUIWindowSettingsCategory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/settings/windows/GUIWindowSettingsScreenCalibration.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/Bookmark.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/GUIViewStateVideo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/PlayerController.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/Teletext.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/VideoDatabase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/VideoDbUrl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/VideoInfoDownloader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/VideoInfoScanner.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/VideoInfoTag.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/VideoLibraryQueue.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/VideoThumbLoader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/ViewModeSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/dialogs/GUIDialogFullScreenInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/dialogs/GUIDialogSubtitles.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/dialogs/GUIDialogTeletext.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/dialogs/GUIDialogVideoBookmarks.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/dialogs/GUIDialogVideoInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/dialogs/GUIDialogVideoOSD.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/dialogs/GUIDialogVideoSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/dialogs/GUIDialogCMSSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/jobs/VideoLibraryCleaningJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/jobs/VideoLibraryJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/jobs/VideoLibraryMarkWatchedJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/jobs/VideoLibraryProgressJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/jobs/VideoLibraryRefreshingJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/jobs/VideoLibraryScanningJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/jobs/VideoLibraryResetResumePointJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/tags/VideoInfoTagLoaderFactory.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/tags/VideoTagLoaderFFmpeg.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/tags/VideoTagLoaderNFO.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/windows/GUIWindowFullScreen.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/windows/GUIWindowVideoBase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/windows/GUIWindowVideoNav.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/windows/GUIWindowVideoPlaylist.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/video/windows/VideoFileItemListModifier.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/view/GUIViewControl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/view/GUIViewState.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/view/ViewDatabase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/view/ViewStateSettings.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/weather/GUIWindowWeather.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/weather/WeatherJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/weather/WeatherManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windowing/OSScreenSaver.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windowing/WinSystem.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windowing/windows/VideoSyncD3D.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windowing/windows/WinEventsWin32.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windowing/windows/WinSystemWin32.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windowing/windows/WinSystemWin32DX.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windows/GUIMediaWindow.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windows/GUIWindowDebugInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windows/GUIWindowFileManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windows/GUIWindowHome.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windows/GUIWindowLoginScreen.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windows/GUIWindowPointer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windows/GUIWindowScreensaver.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windows/GUIWindowScreensaverDim.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windows/GUIWindowSplash.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windows/GUIWindowStartup.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/windows/GUIWindowSystemInfo.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/Application.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/ApplicationPlayer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/ApplicationStackHelper.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/AppParamParser.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/AutoSwitch.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/BackgroundInfoLoader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/ContextMenuItem.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/ContextMenuManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/ContextMenus.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/CueDocument.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/DatabaseManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/DbUrl.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/DynamicDll.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/FileItem.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/FileItemListModification.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/GUIInfoManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/GUILargeTextureManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/GUIPassword.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/InfoScanner.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/MediaSource.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/NfoFile.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/PasswordManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/PlayListPlayer.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/PartyModeManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/SectionLoader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/SeekHandler.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/ServiceBroker.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/ServiceManager.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/SystemGlobals.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/TextureCache.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/TextureCacheJob.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/TextureDatabase.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/ThumbLoader.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/XBApplicationEx.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/XBDateTime.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
  set_property(SOURCE ../../xbmc/Autorun.cpp
    APPEND_STRING PROPERTY COMPILE_FLAGS
    "$<$<CONFIG:Release>:>")
endif ()
