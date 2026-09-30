#include "godot_cpp/classes/resource_saver.hpp"
#include "godot_cpp/classes/file_access.hpp"

#include <ogg/ogg.h>
#include <opus/opus.h>

#include "audio_stream_opus.h"
#include "resource_format_saver_opus.h"

using namespace godot;

namespace opus_gdextension {

PackedStringArray ResourceFormatSaverOpus::_get_recognized_extensions(const Ref<Resource> &p_resource) const {
	return { "opus" };
}

bool ResourceFormatSaverOpus::_recognize(const Ref<Resource> &p_resource) const {
	return p_resource.is_valid() && p_resource->is_class("AudioStreamOpus");
}

Error ResourceFormatSaverOpus::_save(const Ref<Resource> &p_resource, const String &p_path, uint32_t p_flags) {
	Ref<AudioStreamOpus> opus_stream = p_resource;
	if (opus_stream.is_null()) {
		return ERR_INVALID_DATA;
	}

	Ref<FileAccess> save_file_access = FileAccess::open(p_path, FileAccess::WRITE);
	if (save_file_access.is_null()) {
		return ERR_CANT_OPEN;
	}

	save_file_access->store_buffer(opus_stream->get_data());
	save_file_access->close();
	return OK;
}

}
