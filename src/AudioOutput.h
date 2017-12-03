#ifndef KYSTSOFT_AUDIOOUTPUT_H
#define KYSTSOFT_AUDIOOUTPUT_H

#include <audio_io.h>
#include <functional>
#include <vector>

namespace Kystsoft {

class AudioOutput
{
public:
	AudioOutput(int sampleRate, audio_channel_e channel, audio_sample_type_e type);
	AudioOutput(const AudioOutput& other) = delete;
	AudioOutput& operator=(const AudioOutput& rhs) = delete;
	~AudioOutput() noexcept { audio_out_destroy(output); }
	operator audio_out_h() const { return output; }
	void setSoundStreamInfo(sound_stream_info_h streamInfo);
	void prepare();
	void unprepare();
	void pause();
	void resume();
	void drain();
	void flush();
	void write(std::vector<uint8_t>& buffer);
	void write(std::vector<int16_t>& buffer);
	int bufferSize() const;
	int sampleRate() const;
	audio_channel_e channel() const;
	audio_sample_type_e sampleType() const;
	sound_type_e soundType() const;
	using StreamWriter = std::function<void(AudioOutput& audioOutput, size_t bytesRequested)>;
	void setStreamWriter(StreamWriter writer) { streamWriter = writer; }
private:
	static void streamWriteCallback(audio_out_h handle, size_t nbytes, void* user_data);
	void onStreamWrite(size_t bytesRequested) { streamWriter(*this, bytesRequested); }
	audio_out_h output = nullptr;
	StreamWriter streamWriter;
};

} // namespace Kystsoft

#endif // KYSTSOFT_AUDIOOUTPUT_H
