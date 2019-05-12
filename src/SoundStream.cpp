#include "SoundStream.h"
#include "dlog.h"
#include "TizenError.h"

Kystsoft::SoundStream::~SoundStream() noexcept
{
	try { releaseFocus(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { destroy(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

sound_type_e Kystsoft::SoundStream::soundType() const
{
	sound_type_e soundType = SOUND_TYPE_SYSTEM;
	int error = sound_manager_get_sound_type(streamInfo, &soundType);
	if (error != SOUND_MANAGER_ERROR_NONE)
		throw TizenError("sound_manager_get_sound_type", error);
	return soundType;
}

void Kystsoft::SoundStream::acquireFocus(sound_stream_focus_mask_e focusMask)
{
	int oldFocus = focus();
	if ((oldFocus & focusMask) == focusMask)
		return; // focus already acquired
	int error = sound_manager_acquire_focus(streamInfo, focusMask, SOUND_BEHAVIOR_NONE, nullptr);
	if (error != SOUND_MANAGER_ERROR_NONE)
		throw TizenError("sound_manager_acquire_focus", error);
	int newFocus = focus();
	if (newFocus != oldFocus)
		onFocusChanged(newFocus); // required since sound_manager_acquire_focus does not trigger the focusChangedCallback
}

void Kystsoft::SoundStream::releaseFocus(sound_stream_focus_mask_e focusMask)
{
	int oldFocus = focus();
	if ((oldFocus & focusMask) == 0)
		return; // focus already released
	int error = sound_manager_release_focus(streamInfo, focusMask, SOUND_BEHAVIOR_NONE, nullptr);
	if (error != SOUND_MANAGER_ERROR_NONE)
		throw TizenError("sound_manager_release_focus", error);
	int newFocus = focus();
	if (newFocus != oldFocus)
		onFocusChanged(newFocus); // required since sound_manager_release_focus does not trigger the focusChangedCallback
}

void Kystsoft::SoundStream::releaseFocus()
{
	int streamFocus = focus();
	if (streamFocus != 0)
		releaseFocus(sound_stream_focus_mask_e(streamFocus));
}

int Kystsoft::SoundStream::focus() const
{
	sound_stream_focus_state_e playbackState = SOUND_STREAM_FOCUS_STATE_RELEASED;
	sound_stream_focus_state_e recordingState = SOUND_STREAM_FOCUS_STATE_RELEASED;
	int error = sound_manager_get_focus_state(streamInfo, &playbackState, &recordingState);
	if (error != SOUND_MANAGER_ERROR_NONE)
		throw TizenError("sound_manager_get_focus_state", error);
	int focus = 0;
	if (playbackState == SOUND_STREAM_FOCUS_STATE_ACQUIRED)
		focus |= SOUND_STREAM_FOCUS_FOR_PLAYBACK;
	if (recordingState == SOUND_STREAM_FOCUS_STATE_ACQUIRED)
		focus |= SOUND_STREAM_FOCUS_FOR_RECORDING;
	return focus;
}

void Kystsoft::SoundStream::create(sound_stream_type_e type)
{
	int error = sound_manager_create_stream_information(type, focusChangedCallback, this, &streamInfo);
	if (error != SOUND_MANAGER_ERROR_NONE)
		throw TizenError("sound_manager_create_stream_information", error);
}

void Kystsoft::SoundStream::destroy()
{
	int error = sound_manager_destroy_stream_information(streamInfo);
	if (error != SOUND_MANAGER_ERROR_NONE)
		throw TizenError("sound_manager_destroy_stream_information", error);
}

void Kystsoft::SoundStream::focusChangedCallback(
	sound_stream_info_h /*stream_info*/,
	sound_stream_focus_mask_e focus_mask,
	sound_stream_focus_state_e focus_state,
	sound_stream_focus_change_reason_e /*reason*/,
	int /*sound_behavior*/,
	const char* /*extra_info*/,
	void* user_data)
{
	SoundStream* soundStream = static_cast<SoundStream*>(user_data);
	if (soundStream)
	{
		int newFocus = 0;
		if (focus_state == SOUND_STREAM_FOCUS_STATE_ACQUIRED)
			newFocus |= focus_mask;
		soundStream->onFocusChanged(newFocus);
	}
}
