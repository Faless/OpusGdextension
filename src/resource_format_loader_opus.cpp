#include "godot_cpp/classes/file_access.hpp"
#include "godot_cpp/classes/resource_uid.hpp"
#include "godot_cpp/classes/resource_loader.hpp"

#include <ogg/ogg.h>
#include <opus/opus.h>

#include "resource_format_loader_opus.h"
#include "audio_stream_opus.h"

using namespace godot;

namespace opus_gdextension {

PackedStringArray ResourceFormatLoaderOpus::_get_recognized_extensions() const {
	return { "oggopusstr_gdextension" };
}

bool ResourceFormatLoaderOpus::_handles_type(const StringName &p_type) const {
	return p_type == SNAME_AudioStreamOpus || ClassDB::is_parent_class(p_type, SNAME_AudioStreamOpus);
}

String ResourceFormatLoaderOpus::_get_resource_type(const String &p_path) const {
	String extension = p_path.get_extension().to_lower();
	if (extension == "oggopusstr_gdextension") {
		return "AudioStreamOpus";
	}
	return "";
}

Variant ResourceFormatLoaderOpus::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const {
	// Unfortunately, ResourceFormatLoaderBinaryInstance::load is not exposed, so we load manually

	static const uint32_t oggopusstr_gdextension_version = 1;

	Ref<FileAccess> load_file_access = FileAccess::open(p_path, FileAccess::READ);
	if (load_file_access.is_null()) {
		return ERR_FILE_CANT_OPEN;
	}

	uint32_t version = load_file_access->get_32();
	if (load_file_access->get_error()) {
		return ERR_FILE_CANT_READ;
	}
	if (version != oggopusstr_gdextension_version) {
		return ERR_FILE_UNRECOGNIZED;
	}
	bool loop = load_file_access->get_8() != 0;
	if (load_file_access->get_error()) {
		return ERR_FILE_CANT_READ;
	}
	double loop_offset = load_file_access->get_double();
	if (load_file_access->get_error()) {
		return ERR_FILE_CANT_READ;
	}
	double bpm = load_file_access->get_double();
	if (load_file_access->get_error()) {
		return ERR_FILE_CANT_READ;
	}
	int beat_count = (int)load_file_access->get_64();
	if (load_file_access->get_error()) {
		return ERR_FILE_CANT_READ;
	}
	int bar_beats = (int)load_file_access->get_64();
	if (load_file_access->get_error()) {
		return ERR_FILE_CANT_READ;
	}
	uint64_t data_size = load_file_access->get_64();
	if (load_file_access->get_error()) {
		return ERR_FILE_CANT_READ;
	}
	if (data_size > (load_file_access->get_length() - load_file_access->get_position())) {
		return ERR_FILE_CORRUPT;
	}
	PackedByteArray data = load_file_access->get_buffer(data_size);
	if (load_file_access->get_error()) {
		return ERR_FILE_CANT_READ;
	}

	Ref<AudioStreamOpus> opus_stream = AudioStreamOpus::load_from_buffer(data);
	if (opus_stream.is_null()) {
		return ERR_FILE_CANT_READ;
	}

	opus_stream->set_loop(loop);
	opus_stream->set_loop_offset(loop_offset);
	opus_stream->set_bpm(bpm);
	opus_stream->set_beat_count(beat_count);
	opus_stream->set_bar_beats(bar_beats);

	return opus_stream;
}

}
