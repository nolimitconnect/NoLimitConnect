# Opus File Record And Playback (MediaToolsLib)

This document explains how Opus audio is recorded to a file and played back from a file in this codebase.
It also captures bugs found during a focused review of MediaToolsLib.

## Scope

- Recording path: `SndWriter` -> `OpusFileEncoder` -> `OggStream` -> file
- Playback path: `SndReader` -> `OpusFileDecoder` -> `MediaProcessor` speaker output
- File metadata/signature path: `MyOpusHeader` and custom NoLimit signature block

## High-Level Data Flow

### Record To File

1. UI action starts recording through `SndWriter::fromGuiAssetAction()`.
2. `SndWriter::startSndWrite()` opens encoder state (`OpusFileEncoder::beginFileEncode()`).
3. `OpusFileEncoder` opens an Ogg stream (`OggStream::openOggStream()`) and writes:
	 - Opus ID header (`OpusHead`)
	 - Opus tags packet (`OpusTags` + custom strings)
4. `SndWriter` subscribes to `eMediaInputAudioOpus` via `MediaProcessor::wantMediaInput(...)`.
5. During capture, `MediaProcessor` invokes `SndWriter::callbackOpusEncoded(encodedAudio, opusLenBytes)`.
6. `SndWriter` forwards frames to `OpusFileEncoder::writeEncodedFrame()`.
7. `OpusFileEncoder` calls `OggStream::writeEncodedFrame()` which packetizes and flushes Ogg pages.
8. On stop, `SndWriter::stopSndWrite()` calls `OpusFileEncoder::finishFileEncode()`.
9. Finalization closes Ogg stream and writes custom total-frame metadata into file signature region.

### Playback From File

1. UI action starts playback through `SndReader::fromGuiAssetAction()`.
2. `SndReader::startSndRead()` calls `OpusFileDecoder::beginFileDecode(file, assetId, seekPos)`.
3. Decoder opens file, initializes Ogg sync/state, parses Opus header, optional seek alignment to `OggS` page.
4. Decoder subscribes to output-space callback using `wantMediaInput(... eMediaInputMixer ...)`.
5. `OpusFileDecoder::callbackAudioOutSpaceAvail()` pulls frames via `decodedNextFrame()`.
6. `decodedNextFrame()` reads Ogg pages, decodes Opus packets (`opus_decode`), converts to PCM frames (`opusPcmOutputToPcm()`), queues into `m_DecodedFrames`.
7. `moveOpusFramesToOutput()` pops one queued PCM frame, boosts volume, copies to output buffer.
8. `MediaProcessor::playAudio()` receives PCM and sends to audio output.
9. At EOF, decoder disables callback and sends `eAssetActionPlayEnd` to GUI.

## File Format Notes

- Container is Ogg Opus.
- Opus ID header starts with `OpusHead`.
- Tags packet starts with `OpusTags`.
- NoLimit custom signature expected at offset `0x9c` (`NO_LIMIT_OPUS_SIGNITURE_OFFS`) and includes:
	- Prefix `nolimitconnect.org v`
	- 16 hex chars for total sound frames
	- Version suffix
- Playback reads this custom total-frame value for progress reporting.
