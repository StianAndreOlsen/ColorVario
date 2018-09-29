#include "AudioOutput.h"
#include "dlog.h"
#include "TizenError.h"

Kystsoft::AudioOutput::AudioOutput(int sampleRate, audio_channel_e channel, audio_sample_type_e type)
{
	create(sampleRate, channel, type);
	try
	{
		setStreamWriteCallback();
	}
	catch (...)
	{
		destroy();
		throw;
	}
}

Kystsoft::AudioOutput::~AudioOutput() noexcept
{
	try { unprepare(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { unsetStreamWriteCallback(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
	try { destroy(); }
		catch (std::exception& e) { dlog(DLOG_ERROR) << e.what(); }
}

void Kystsoft::AudioOutput::setSoundStreamInfo(sound_stream_info_h streamInfo)
{
	int error = audio_out_set_sound_stream_info(output, streamInfo);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_set_sound_stream_info", error);
}

void Kystsoft::AudioOutput::setPrepared(bool prepared)
{
	if (prepared)
		prepare();
	else
		unprepare();
}

void Kystsoft::AudioOutput::prepare()
{
	if (prepared)
		return;
	int error = audio_out_prepare(output);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_prepare", error);
	prepared = true;
}

void Kystsoft::AudioOutput::unprepare()
{
	if (!prepared)
		return;
	int error = audio_out_unprepare(output);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_unprepare", error);
	prepared = false;
}

void Kystsoft::AudioOutput::setPaused(bool paused)
{
	if (paused)
		pause();
	else
		resume();
}

void Kystsoft::AudioOutput::pause()
{
	if (paused)
		return;
	int error = audio_out_pause(output);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_pause", error);
	paused = true;
}

void Kystsoft::AudioOutput::resume()
{
	if (!paused)
		return;
	int error = audio_out_resume(output);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_resume", error);
	paused = false;
}

void Kystsoft::AudioOutput::drain()
{
	int error = audio_out_drain(output);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_drain", error);
}

void Kystsoft::AudioOutput::flush()
{
	int error = audio_out_flush(output);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_flush", error);
}

void Kystsoft::AudioOutput::write(std::vector<uint8_t>& buffer)
{
	int bytes = audio_out_write(output, static_cast<void*>(buffer.data()), static_cast<unsigned int>(buffer.size()));
	if (bytes < 0)
		throw TizenError("audio_out_write", bytes);
}

void Kystsoft::AudioOutput::write(std::vector<int16_t>& buffer)
{
	int bytes = audio_out_write(output, static_cast<void*>(buffer.data()), static_cast<unsigned int>(2 * buffer.size()));
	if (bytes < 0)
		throw TizenError("audio_out_write", bytes);
}

int Kystsoft::AudioOutput::bufferSize() const
{
	int bufferSize = 0;
	int error = audio_out_get_buffer_size(output, &bufferSize);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_get_buffer_size", error);
	return bufferSize;
}

int Kystsoft::AudioOutput::sampleRate() const
{
	int sampleRate = 0;
	int error = audio_out_get_sample_rate(output, &sampleRate);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_get_sample_rate", error);
	return sampleRate;
}

audio_channel_e Kystsoft::AudioOutput::channel() const
{
	audio_channel_e channel = AUDIO_CHANNEL_MONO;
	int error = audio_out_get_channel(output, &channel);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_get_channel", error);
	return channel;
}

audio_sample_type_e Kystsoft::AudioOutput::sampleType() const
{
	audio_sample_type_e sampleType = AUDIO_SAMPLE_TYPE_U8;
	int error = audio_out_get_sample_type(output, &sampleType);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_get_sample_type", error);
	return sampleType;
}

sound_type_e Kystsoft::AudioOutput::soundType() const
{
	sound_type_e soundType = SOUND_TYPE_SYSTEM;
	int error = audio_out_get_sound_type(output, &soundType);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_get_sound_type", error);
	return soundType;
}

void Kystsoft::AudioOutput::create(int sampleRate, audio_channel_e channel, audio_sample_type_e type)
{
	int error = audio_out_create_new(sampleRate, channel, type, &output);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_create_new", error);
}

void Kystsoft::AudioOutput::destroy()
{
	int error = audio_out_destroy(output);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_destroy", error);
	output = nullptr;
}

void Kystsoft::AudioOutput::setStreamWriteCallback()
{
	int error = audio_out_set_stream_cb(output, streamWriteCallback, this);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_set_stream_cb", error);
}

void Kystsoft::AudioOutput::unsetStreamWriteCallback()
{
	int error = audio_out_unset_stream_cb(output);
	if (error != AUDIO_IO_ERROR_NONE)
		throw TizenError("audio_out_unset_stream_cb", error);
}

void Kystsoft::AudioOutput::streamWriteCallback(audio_out_h /*handle*/, size_t nbytes, void* user_data)
{
	AudioOutput* audioOutput = static_cast<AudioOutput*>(user_data);
	if (audioOutput)
		audioOutput->onStreamWrite(nbytes);
}
