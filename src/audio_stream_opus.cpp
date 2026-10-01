#include "audio_stream_opus.h"

#include "godot_cpp/classes/file_access.hpp"

using namespace godot;

static const int OPUS_SAMPLERATE = 48000;

int32_t AudioStreamPlaybackOpus::_mix_resampled(AudioFrame *p_buffer, int p_frames) {
	if (!active) {
		return 0;
	}

	int todo = p_frames;
	bool mixed_was_zero = false; // for detecting infinite loop

	bool use_loop = looping_override ? looping : opus_stream->loop;

	while (todo && active) {
		float *buffer = (float *)(p_buffer + p_frames - todo);
		int mixed = op_read_float_stereo(opus_file, buffer, todo * 2);
		if (mixed > 0) {
			mixed_was_zero = false;
		}
		if (mixed < 0) {
			// error
			for (int i = p_frames - todo; i < p_frames; i++) {
				AudioFrame audio_frame;
				audio_frame.left = 0;
				audio_frame.right = 0;
				p_buffer[i] = audio_frame;
			}
			return p_frames - todo;
		}

		todo -= mixed;
		frames_mixed += mixed;

		if (mixed == 0) {
			//end of file!
			if (use_loop && !mixed_was_zero) {
				//loop
				_seek(opus_stream->loop_offset);
				loops++;
			} else {
				for (int i = p_frames - todo; i < p_frames; i++) {
					AudioFrame audio_frame;
					audio_frame.left = 0;
					audio_frame.right = 0;
					p_buffer[i] = audio_frame;
				}
				return 0;
			}
			mixed_was_zero = true;
		}
	}
	return p_frames - todo;
}

float AudioStreamPlaybackOpus::_get_stream_sampling_rate() const {
	return OPUS_SAMPLERATE;
}

void AudioStreamPlaybackOpus::_start(double p_from_pos) {
	active = true;
	_seek(p_from_pos);
	loops = 0;
	begin_resample();
}

void AudioStreamPlaybackOpus::_stop() {
	active = false;
}

bool AudioStreamPlaybackOpus::_is_playing() const {
	return active;
}

int AudioStreamPlaybackOpus::_get_loop_count() const {
	return loops;
}

double AudioStreamPlaybackOpus::_get_playback_position() const {
	return double(frames_mixed) / OPUS_SAMPLERATE;
}

void AudioStreamPlaybackOpus::_tag_used_streams() {
}

void AudioStreamPlaybackOpus::_set_parameter(const StringName &p_name, const Variant &p_value) {
	if (p_name == SNAME_looping) {
		if (p_value == Variant()) {
			looping_override = false;
			looping = false;
		} else {
			looping_override = true;
			looping = p_value;
		}
	}
}

Variant AudioStreamPlaybackOpus::_get_parameter(const StringName &p_name) const {
	if (looping_override && p_name == SNAME_looping) {
		return looping;
	}
	return Variant();
}

void AudioStreamPlaybackOpus::_seek(double p_time) {
	if (!active) {
		return;
	}

	if (p_time >= opus_stream->get_length()) {
		p_time = 0;
	}
	frames_mixed = uint32_t(OPUS_SAMPLERATE * p_time);

	int error = op_pcm_seek(opus_file, frames_mixed);
	ERR_FAIL_COND_MSG(error != 0, "Opus seek failed.");
}

void AudioStreamPlaybackOpus::_bind_methods() {
}

AudioStreamPlaybackOpus::~AudioStreamPlaybackOpus() {
	if (opus_file) {
		op_free(opus_file);
		opus_file = nullptr;
	}
}

Ref<AudioStreamPlayback> AudioStreamOpus::_instantiate_playback() const {
	Ref<AudioStreamPlaybackOpus> opus;

	ERR_FAIL_COND_V_MSG(data.is_empty(), nullptr,
			"This AudioStreamOpus does not have an audio file assigned "
			"to it. AudioStreamOpus should not be created from the "
			"inspector or with `.new()`. Instead, load an audio file.");

	opus.instantiate();
	opus->opus_stream = Ref<AudioStreamOpus>(this);
	opus->frames_mixed = 0;
	opus->active = false;
	opus->loops = 0;

	opus->opus_file = op_open_memory(data.ptr(), data.size(), nullptr);
	ERR_FAIL_NULL_V(opus->opus_file, Ref<AudioStreamPlaybackOpus>());

	return opus;
}

void AudioStreamOpus::clear_data() {
	data.clear();
}

void AudioStreamOpus::set_data(const PackedByteArray &p_data) {
	// Open file to fetch metadata
	OggOpusFile *opus_file = op_open_memory(p_data.ptr(), p_data.size(), nullptr);
	ERR_FAIL_NULL_MSG(opus_file, "Could not open opus stream.");

	int64_t length_i = op_pcm_total(opus_file, -1);
	length = (length_i > 0) ? (double(length_i) / OPUS_SAMPLERATE) : 0;

	// Tags (parsed same as OGG Vorbis)
	const OpusTags *opus_tags = op_tags(opus_file, -1);
	Dictionary dictionary;
	if (opus_tags != nullptr) {
		for (int i = 0; i < opus_tags->comments; i++) {
			String c = String::utf8(opus_tags->user_comments[i]);
			int equals = c.find("=");

#ifdef TOOLS_ENABLED
			if (equals == -1) {
				WARN_PRINT(vformat(R"(Invalid comment in Ogg Opus file "%s", should contain '=': "%s".)", get_path(), c));
				continue;
			}
#endif

			String tag = c.substr(0, equals);
			String tag_value = c.substr(equals + 1);

			dictionary[tag.to_lower()] = tag_value;
		}
	}
	tags = dictionary;

	// Close file
	op_free(opus_file);

	// Copy data
	data = p_data;
}

PackedByteArray AudioStreamOpus::get_data() const {
	return data;
}

void AudioStreamOpus::set_loop(bool p_enable) {
	loop = p_enable;
}

bool AudioStreamOpus::has_loop() const {
	return loop;
}

void AudioStreamOpus::set_loop_offset(double p_seconds) {
	loop_offset = p_seconds;
}

double AudioStreamOpus::get_loop_offset() const {
	return loop_offset;
}

double AudioStreamOpus::_get_length() const {
	return length;
}

Ref<AudioStreamOpus> AudioStreamOpus::load_from_buffer(const PackedByteArray &p_stream_data) {
	Ref<AudioStreamOpus> opus_stream;
	opus_stream.instantiate();

	opus_stream->set_data(p_stream_data);

	return opus_stream;
}

Ref<AudioStreamOpus> AudioStreamOpus::load_from_file(const String &p_path) {
	PackedByteArray stream_data = FileAccess::get_file_as_bytes(p_path);
	ERR_FAIL_COND_V_MSG(stream_data.is_empty(), Ref<AudioStreamOpus>(), vformat("Cannot open file '%s'.", p_path));
	return load_from_buffer(stream_data);
}

void AudioStreamOpus::set_bpm(double p_bpm) {
	ERR_FAIL_COND(p_bpm < 0);
	bpm = p_bpm;
	emit_changed();
}

double AudioStreamOpus::_get_bpm() const {
	return bpm;
}

void AudioStreamOpus::set_beat_count(int p_beat_count) {
	ERR_FAIL_COND(p_beat_count < 0);
	beat_count = p_beat_count;
	emit_changed();
}

int AudioStreamOpus::_get_beat_count() const {
	return beat_count;
}

void AudioStreamOpus::set_bar_beats(int p_bar_beats) {
	ERR_FAIL_COND(p_bar_beats < 2);
	bar_beats = p_bar_beats;
	emit_changed();
}

int AudioStreamOpus::get_bar_beats() const {
	return bar_beats;
}

void AudioStreamOpus::set_tags(const Dictionary &p_tags) {
	tags = p_tags;
}

Dictionary AudioStreamOpus::get_tags() const {
	return tags;
}

bool AudioStreamOpus::_is_monophonic() const {
	return false;
}

TypedArray<Dictionary> AudioStreamOpus::_get_parameter_list() const {
	TypedArray<Dictionary> options;

	Dictionary looping = Dictionary(PropertyInfo(Variant::BOOL, "looping", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_CHECKABLE));
	looping["default_value"] = Variant();
	options.push_back(looping);

	return options;
}

void AudioStreamOpus::_bind_methods() {
	ClassDB::bind_static_method("AudioStreamOpus", D_METHOD("load_from_buffer", "stream_data"), &AudioStreamOpus::load_from_buffer);
	ClassDB::bind_static_method("AudioStreamOpus", D_METHOD("load_from_file", "path"), &AudioStreamOpus::load_from_file);

	ClassDB::bind_method(D_METHOD("set_data", "data"), &AudioStreamOpus::set_data);
	ClassDB::bind_method(D_METHOD("get_data"), &AudioStreamOpus::get_data);

	ClassDB::bind_method(D_METHOD("set_loop", "enable"), &AudioStreamOpus::set_loop);
	ClassDB::bind_method(D_METHOD("has_loop"), &AudioStreamOpus::has_loop);

	ClassDB::bind_method(D_METHOD("set_loop_offset", "seconds"), &AudioStreamOpus::set_loop_offset);
	ClassDB::bind_method(D_METHOD("get_loop_offset"), &AudioStreamOpus::get_loop_offset);

	ClassDB::bind_method(D_METHOD("set_bpm", "bpm"), &AudioStreamOpus::set_bpm);
	ClassDB::bind_method(D_METHOD("get_bpm"), &AudioStreamOpus::_get_bpm);

	ClassDB::bind_method(D_METHOD("set_beat_count", "count"), &AudioStreamOpus::set_beat_count);
	ClassDB::bind_method(D_METHOD("get_beat_count"), &AudioStreamOpus::_get_beat_count);

	ClassDB::bind_method(D_METHOD("set_bar_beats", "count"), &AudioStreamOpus::set_bar_beats);
	ClassDB::bind_method(D_METHOD("get_bar_beats"), &AudioStreamOpus::get_bar_beats);

	ClassDB::bind_method(D_METHOD("set_tags", "tags"), &AudioStreamOpus::set_tags);
	ClassDB::bind_method(D_METHOD("get_tags"), &AudioStreamOpus::get_tags);

	ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_data", "get_data");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bpm", PROPERTY_HINT_RANGE, "0,400,0.01,or_greater"), "set_bpm", "get_bpm");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "beat_count", PROPERTY_HINT_RANGE, "0,512,1,or_greater"), "set_beat_count", "get_beat_count");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "bar_beats", PROPERTY_HINT_RANGE, "2,32,1,or_greater"), "set_bar_beats", "get_bar_beats");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "tags", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_tags", "get_tags");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "loop"), "set_loop", "has_loop");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "loop_offset"), "set_loop_offset", "get_loop_offset");
}
