#pragma once

#include "godot_cpp/classes/audio_stream.hpp"
#include "godot_cpp/classes/audio_stream_playback_resampled.hpp"

#include <opus/opusfile.h>

using namespace godot;

class AudioStreamOpus;

class AudioStreamPlaybackOpus : public AudioStreamPlaybackResampled {
	GDCLASS(AudioStreamPlaybackOpus, AudioStreamPlaybackResampled);

	OggOpusFile *opus_file = nullptr;
	uint32_t frames_mixed = 0;
	bool active = false;
	bool looping_override = false;
	bool looping = false;
	int loops = 0;

	friend class AudioStreamOpus;

	Ref<AudioStreamOpus> opus_stream;

	bool _is_sample = false;
	Ref<AudioSamplePlayback> sample_playback;

	const StringName SNAME_looping = "looping";

public:
	virtual int32_t _mix_resampled(AudioFrame *p_buffer, int p_frames) override;
	virtual float _get_stream_sampling_rate() const override;

	virtual void _start(double p_from_pos = 0.0) override;
	virtual void _stop() override;
	virtual bool _is_playing() const override;

	virtual int _get_loop_count() const override; //times it looped

	virtual double _get_playback_position() const override;
	virtual void _seek(double p_time) override;

	virtual void _tag_used_streams() override;

	virtual void _set_parameter(const StringName &p_name, const Variant &p_value) override;
	virtual Variant _get_parameter(const StringName &p_name) const override;

	AudioStreamPlaybackOpus() {}
	~AudioStreamPlaybackOpus();
};

class AudioStreamOpus : public AudioStream {
	GDCLASS(AudioStreamOpus, AudioStream);

	friend class AudioStreamPlaybackOpus;

	PackedByteArray data;
	double length = 0;
	bool loop = false;
	double loop_offset = 0;
	void clear_data();

	double bpm = 0;
	int beat_count = 0;
	int bar_beats = 4;
	Dictionary tags;

protected:
	static void _bind_methods();

public:
	static Ref<AudioStreamOpus> load_from_file(const String &p_path);
	static Ref<AudioStreamOpus> load_from_buffer(const PackedByteArray &p_stream_data);

	void set_loop(bool p_enable);
	virtual bool has_loop() const;

	void set_loop_offset(double p_seconds);
	double get_loop_offset() const;

	void set_bpm(double p_bpm);
	virtual double _get_bpm() const override;

	void set_beat_count(int p_beat_count);
	virtual int _get_beat_count() const override;

	void set_bar_beats(int p_bar_beats);
	virtual int get_bar_beats() const;

	void set_tags(const Dictionary &p_tags);
	virtual Dictionary get_tags() const;

	virtual Ref<AudioStreamPlayback> _instantiate_playback() const override;

	void set_data(const PackedByteArray &p_data);
	PackedByteArray get_data() const;

	virtual double _get_length() const override;

	virtual bool _is_monophonic() const override;

	virtual TypedArray<Dictionary> _get_parameter_list() const override;
};
