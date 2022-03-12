#pragma once

#include "pa_lib.hpp"

namespace pax {
namespace portaudio {

class Stream
{

public:

	struct Info
	{
		PaTime input_latency;
		PaTime output_latency;
		double sample_rate;
	};

	struct Config
	{
		const PaStreamParameters* input_parameters;
		const PaStreamParameters* output_parameters;
		double sample_rate;
		unsigned long frames_per_buffer;
		PaStreamFlags flags;
		PaStreamCallback* callback;
		void* user_data;
	};

	PaStream* const stream;
	const PaHostApiTypeId host_type;
	const Info info;

	Stream(const Config& config);
	~Stream();

	auto abort() -> void;
	auto start() -> void;
	auto stop() -> void;
	auto is_active() const -> bool;
	auto set_finished_callback(PaStreamFinishedCallback* streamFinishedCallback) -> void;

	auto get_time() -> PaTime;
	auto get_cpu_load() -> double;
};

namespace detail
{

static inline auto open_stream(const Stream::Config& config)
{
	// Tries to open the stream in a loop
	// Works around shitty ASIO driver bugs
	const auto HACK_brute_force_open_stream = [&](PaStream** stream, const Stream::Config& config)
	{
		constexpr auto MAX_ATTEMPTS { 3 };

		PaError err { paNoError };

		for (int i = 0; i < MAX_ATTEMPTS; i++)
		{
			err =
				Pa_OpenStream(
					stream,
					config.input_parameters,
					config.output_parameters,
					config.sample_rate,
					config.frames_per_buffer,
					config.flags,
					config.callback,
					config.user_data);
			
			if (err == paNoError) return err;
		}

		return err;
	};

	PaStream* out;

	const auto err { HACK_brute_force_open_stream(&out, config) };

	if (err != paNoError)
	{
		std::stringstream ss;

		ss << "Failed to open audio stream: " << Pa_GetErrorText(err);

		throw std::runtime_error(ss.str());
	}

	return out;
}

static inline auto get_info(PaStream* stream)
{
	Stream::Info out;

	const auto info { Library::C::GetStreamInfo(stream) };

	out.input_latency = info->inputLatency;
	out.output_latency = info->outputLatency;
	out.sample_rate = info->sampleRate;

	return out;
}

static inline auto get_host_type(const Stream::Config& config)
{
	const auto device_info { Library::C::GetDeviceInfo(config.output_parameters->device) };
	const auto host_info { Library::C::GetHostApiInfo(device_info->hostApi) };

	return host_info->type;
}

} // detail

inline Stream::Stream(const Config& config)
	: stream { detail::open_stream(config) }
	, host_type { detail::get_host_type(config) }
	, info { detail::get_info(stream) }
{
}

inline Stream::~Stream()
{
	Library::C::CloseStream(stream);
}

inline auto Stream::abort() -> void
{
	if (is_active())
	{
		Library::C::AbortStream(stream);
	}
}

inline auto Stream::start() -> void
{
	Library::C::StartStream(stream);
}

inline auto Stream::stop() -> void
{
	if (is_active())
	{
		Library::C::StopStream(stream);
	}
}

inline auto Stream::is_active() const -> bool
{
	return Library::C::IsStreamActive(stream) == 1;
}

inline auto Stream::set_finished_callback(PaStreamFinishedCallback* streamFinishedCallback) -> void
{
	Library::C::SetStreamFinishedCallback(stream, streamFinishedCallback);
}

inline auto Stream::get_time() -> PaTime
{
	return Library::C::GetStreamTime(stream);
}

inline auto Stream::get_cpu_load() -> double
{
	return Library::C::GetStreamCpuLoad(stream);
}

} // portaudio
} // pax