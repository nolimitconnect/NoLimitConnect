
cmake_minimum_required (VERSION 3.22.1)

project(openmpt, LANGUAGES C CXX)

set(openmpt_SOURCES)

list(APPEND openmpt_SOURCES 
    common/ComponentManager.cpp
    common/FileReader.cpp
    common/Logging.cpp
    common/Profiler.cpp
    common/misc_util.cpp
    common/mptCPU.cpp
    common/mptFileIO.cpp
    common/mptIO.cpp
    common/mptLibrary.cpp
    common/mptOS.cpp
    common/mptPathString.cpp
    common/mptRandom.cpp
    common/mptString.cpp
    common/mptStringFormat.cpp
    common/mptStringParse.cpp
    common/mptTime.cpp
    common/mptUUID.cpp
    common/mptWine.cpp
    common/serialization_utils.cpp
    common/stdafx.cpp
    common/typedefs.cpp
    common/version.cpp
    libopenmpt/libopenmpt_c.cpp
    libopenmpt/libopenmpt_cxx.cpp
    libopenmpt/libopenmpt_ext_impl.cpp
    libopenmpt/libopenmpt_impl.cpp
    sounddsp/AGC.cpp
    sounddsp/DSP.cpp
    sounddsp/EQ.cpp
    sounddsp/Reverb.cpp
    soundlib/AudioCriticalSection.cpp
    soundlib/ContainerMMCMP.cpp
    soundlib/ContainerPP20.cpp
    soundlib/ContainerUMX.cpp
    soundlib/ContainerXPK.cpp
    soundlib/Dither.cpp
    soundlib/Dlsbank.cpp
    soundlib/Fastmix.cpp
    soundlib/ITCompression.cpp
    soundlib/ITTools.cpp
    soundlib/InstrumentExtensions.cpp
    soundlib/Load_669.cpp
    soundlib/Load_amf.cpp
    soundlib/Load_ams.cpp
    soundlib/Load_dbm.cpp
    soundlib/Load_digi.cpp
    soundlib/Load_dmf.cpp
    soundlib/Load_dsm.cpp
    soundlib/Load_dtm.cpp
    soundlib/Load_far.cpp
    soundlib/Load_gdm.cpp
    soundlib/Load_imf.cpp
    soundlib/Load_it.cpp
    soundlib/Load_itp.cpp
    soundlib/Load_mdl.cpp
    soundlib/Load_med.cpp
    soundlib/Load_mid.cpp
    soundlib/Load_mo3.cpp
    soundlib/Load_mod.cpp
    soundlib/Load_mt2.cpp
    soundlib/Load_mtm.cpp
    soundlib/Load_okt.cpp
    soundlib/Load_plm.cpp
    soundlib/Load_psm.cpp
    soundlib/Load_ptm.cpp
    soundlib/Load_s3m.cpp
    soundlib/Load_sfx.cpp
    soundlib/Load_stm.cpp
    soundlib/Load_stp.cpp
    soundlib/Load_uax.cpp
    soundlib/Load_ult.cpp
    soundlib/Load_wav.cpp
    soundlib/Load_xm.cpp
    soundlib/MIDIEvents.cpp
    soundlib/MIDIMacros.cpp
    soundlib/MPEGFrame.cpp
    soundlib/Message.cpp
    soundlib/MixFuncTable.cpp
    soundlib/MixerLoops.cpp
    soundlib/MixerSettings.cpp
    soundlib/ModChannel.cpp
    soundlib/ModInstrument.cpp
    soundlib/ModSample.cpp
    soundlib/ModSequence.cpp
    soundlib/OggStream.cpp
    soundlib/Paula.cpp
    soundlib/RowVisitor.cpp
    soundlib/S3MTools.cpp
    soundlib/SampleFormatFLAC.cpp
    soundlib/SampleFormatMP3.cpp
    soundlib/SampleFormatMediaFoundation.cpp
    soundlib/SampleFormatOpus.cpp
    soundlib/SampleFormatVorbis.cpp
    soundlib/SampleFormats.cpp
    soundlib/SampleIO.cpp
    soundlib/Snd_flt.cpp
    soundlib/Snd_fx.cpp
    soundlib/Sndfile.cpp
    soundlib/Sndmix.cpp
    soundlib/SoundFilePlayConfig.cpp
    soundlib/Tables.cpp
    soundlib/Tagging.cpp
    soundlib/UMXTools.cpp
    soundlib/UpgradeModule.cpp
    soundlib/WAVTools.cpp
    soundlib/WindowedFIR.cpp
    soundlib/XMTools.cpp
    soundlib/load_j2b.cpp
    soundlib/mod_specifications.cpp
    soundlib/modcommand.cpp
    soundlib/modsmp_ctrl.cpp
    soundlib/pattern.cpp
    soundlib/patternContainer.cpp
    soundlib/plugins/DigiBoosterEcho.cpp
    soundlib/plugins/LFOPlugin.cpp
    soundlib/plugins/PlugInterface.cpp
    soundlib/plugins/PluginManager.cpp
    soundlib/plugins/dmo/Chorus.cpp
    soundlib/plugins/dmo/Compressor.cpp
    soundlib/plugins/dmo/DMOPlugin.cpp
    soundlib/plugins/dmo/Distortion.cpp
    soundlib/plugins/dmo/Echo.cpp
    soundlib/plugins/dmo/Flanger.cpp
    soundlib/plugins/dmo/Gargle.cpp
    soundlib/plugins/dmo/I3DL2Reverb.cpp
    soundlib/plugins/dmo/ParamEq.cpp
    soundlib/plugins/dmo/WavesReverb.cpp
    soundlib/tuning.cpp
    soundlib/tuningCollection.cpp
    soundlib/tuningbase.cpp
)

add_library(openmpt STATIC ${openmpt_SOURCES})

target_compile_definitions(openmpt
    PRIVATE
    LIBOPENMPT_BUILD
    LIBOPENMPT_BUILD_FULL
    MPT_ENABLE_DLOPEN
)

target_include_directories(openmpt
  PRIVATE
    include
    include/ogg/include
    include/vorbis/include
    include/zlib
    common
    soundlib
    build/svn_version
)

if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
    if(CI_PIPELINE)
        # remove *.o and *.d files to avoid gitlab docker size restriction
        add_custom_command(POST_BUILD TARGET openmpt
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMAND find . -type f -name '*.o' -print -delete
            COMMAND find . -type f -name '*.d' -print -delete
        )
    endif()
endif()

