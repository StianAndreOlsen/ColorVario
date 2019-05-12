#ifndef KYSTSOFT_SOUNDSTREAM_H
#define KYSTSOFT_SOUNDSTREAM_H

#include "Signal.h"
#include <sound_manager.h>

namespace Kystsoft {

class SoundStream
{
public:
	SoundStream(sound_stream_type_e type) { create(type); }
	SoundStream(const SoundStream& other) = delete;
	SoundStream& operator=(const SoundStream& rhs) = delete;
	~SoundStream() noexcept;
	operator sound_stream_info_h() const { return streamInfo; }
	sound_type_e soundType() const;
	void acquireFocus(sound_stream_focus_mask_e focusMask);
	void releaseFocus(sound_stream_focus_mask_e focusMask);
	void releaseFocus(); // release the current focus
	void acquirePlaybackFocus() { acquireFocus(SOUND_STREAM_FOCUS_FOR_PLAYBACK); }
	void acquireRecordingFocus() { acquireFocus(SOUND_STREAM_FOCUS_FOR_RECORDING); }
	void acquirePlaybackAndRecordingFocus() { acquireFocus(SOUND_STREAM_FOCUS_FOR_BOTH); }
	void releasePlaybackFocus() { releaseFocus(SOUND_STREAM_FOCUS_FOR_PLAYBACK); }
	void releaseRecordingFocus() { releaseFocus(SOUND_STREAM_FOCUS_FOR_RECORDING); }
	void releasePlaybackAndRecordingFocus() { releaseFocus(SOUND_STREAM_FOCUS_FOR_BOTH); }
	int focus() const; // current focus
	bool hasFocus(sound_stream_focus_mask_e focusMask) const { return (focus() & focusMask) == focusMask; }
	bool hasPlaybackFocus() const { return hasFocus(SOUND_STREAM_FOCUS_FOR_PLAYBACK); }
	bool hasRecordingFocus() const { return hasFocus(SOUND_STREAM_FOCUS_FOR_RECORDING); }
	bool hasPlaybackAndRecordingFocus() const { return hasFocus(SOUND_STREAM_FOCUS_FOR_BOTH); }
	const auto& focusChangedSignal() const { return focusChangedSignl; }
private:
	void create(sound_stream_type_e type);
	void destroy();
	static void focusChangedCallback(
		sound_stream_info_h stream_info,
		sound_stream_focus_mask_e focus_mask,
		sound_stream_focus_state_e focus_state,
		sound_stream_focus_change_reason_e reason,
		int sound_behavior,
		const char* extra_info,
		void* user_data);
	void onFocusChanged(int newFocus) { focusChangedSignl.emit(newFocus); }
	sound_stream_info_h streamInfo = nullptr;
	Signal<int> focusChangedSignl;
};

} // namespace Kystsoft

#endif // KYSTSOFT_SOUNDSTREAM_H
