#include <fftune.hpp>
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>

constexpr fftune::config conf {.algorithm = fftune::pitch_detection_method::Yin, .buffer_size = 1024};

struct pw_data {
	struct pw_main_loop *loop = nullptr;
	struct pw_context *context = nullptr;
	struct pw_core *core = nullptr;
	struct pw_stream *stream = nullptr;
	fftune::pitch_detector<conf> detector {conf};
	fftune::sample_buffer sbuf {conf.buffer_size};
};

void on_process(void *data) {
	// pipewire callback, when a buffer of audio data is ready
	auto *d = static_cast<pw_data *>(data);
	struct pw_buffer *b;

	if ((b = pw_stream_dequeue_buffer(d->stream)) == nullptr) {
		return;
	}
	spa_data &buf = b->buffer->datas[0];
	if (!buf.data || buf.chunk->size < conf.buffer_size) {
		return;
	}

	d->sbuf.read(static_cast<float *>(buf.data), buf.chunk->size / sizeof(float));

	auto notes = d->detector.detect(d->sbuf);
	std::cout << notes << std::endl;

	pw_stream_queue_buffer(d->stream, b);
}

const struct pw_stream_events events = {.version = PW_VERSION_STREAM_EVENTS, .process = on_process};

void quit(void *data, int signal) {
	auto *d = static_cast<pw_data *>(data);
	pw_main_loop_quit(d->loop);
}

int main(int argc, char *argv[]) {
	pw_data data;
	const struct spa_pod *params[1];
	uint8_t buffer[conf.buffer_size];
	struct spa_pod_builder b = SPA_POD_BUILDER_INIT(buffer, sizeof(buffer));
	// initialize our preferred audio format
	auto info = SPA_AUDIO_INFO_RAW_INIT(.format = SPA_AUDIO_FORMAT_F32, .rate = static_cast<uint32_t>(conf.sample_rate), .channels = 1);
	params[0] = spa_format_audio_raw_build(&b, SPA_PARAM_EnumFormat, &info);

	pw_init(nullptr, nullptr);
	// set node properties
	auto *props = pw_properties_new(PW_KEY_MEDIA_TYPE, "Audio", PW_KEY_MEDIA_CATEGORY, "Capture", PW_KEY_MEDIA_ROLE, "Music", nullptr);
	data.loop = pw_main_loop_new(nullptr);
	if (!data.loop) {
		return 1;
	}
	// handle UNIX signals to quit the program
	pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGINT, quit, &data);
	pw_loop_add_signal(pw_main_loop_get_loop(data.loop), SIGTERM, quit, &data);

	// create our stream and autoconnect it
	data.stream = pw_stream_new_simple(pw_main_loop_get_loop(data.loop), "pipewire-realtime", props, &events, &data);
	pw_stream_connect(data.stream, PW_DIRECTION_INPUT, PW_ID_ANY, static_cast<pw_stream_flags>(PW_STREAM_FLAG_AUTOCONNECT | PW_STREAM_FLAG_MAP_BUFFERS | PW_STREAM_FLAG_RT_PROCESS), params, 1);
	// run until we get a signal interrupt
	pw_main_loop_run(data.loop);

	return 0;
}
