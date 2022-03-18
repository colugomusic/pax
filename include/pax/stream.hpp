#pragma once

#include <functional>
#include <memory>
#include <vector>
#include "device.hpp"
#include "pa_stream.hpp"

namespace pax {

class Stream
{
public:

	struct Config
	{
		struct
		{
			std::function<void(std::string info)> info;
			std::function<void(std::string error)> error;
			std::function<void(int SR)> sample_rate_changed;
			std::function<void()> started;
			std::function<void()> starting;
			std::function<void()> stopped;
		} callbacks;
	};

	struct StreamInfo
	{
		std::optional<PaStreamParameters> input_params {};
		PaStreamParameters output_params {};
		std::optional<Device> input_device {};
		Device output_device;
		unsigned long frames_per_buffer;
		int SR {};
		double latency {};
	};

	struct Request
	{
		std::optional<Device> input_device {};
		Device output_device;
		unsigned long frames_per_buffer;
		int SR {};
		double latency {};
	};

	using StreamFinishedTask = std::function<void()>;

	Stream(Config && config);

	auto abort() -> void;
	auto get_cpu_load() const -> double;
	auto get_host_type() const -> PaHostApiTypeId;
	auto get_info() const -> std::optional<StreamInfo>;
	auto get_input_channel_count() const -> int;
	auto get_output_latency() const -> double;
	auto get_time() const -> double;
	auto get_SR() const -> int;
	auto is_active() const -> bool;
	auto push_finished_task(StreamFinishedTask task) -> void;
	auto request(Request settings) -> void;
	auto set_callback(PaStreamCallback* callback, void* user_data) -> void;
	auto stop() -> void;

private:

	auto raise_error(std::string error) -> void;
	auto start() -> void;

	auto on_finished() -> void;
	auto callback(
		const void* input,
		void* output,
		unsigned long frame_count,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flags) -> int;

	static auto _on_finished(void* user_data) -> void;
	static auto _callback(
		const void* input,
		void* output,
		unsigned long frame_count,
		const PaStreamCallbackTimeInfo* time_info,
		PaStreamCallbackFlags status_flags,
		void* user_data) -> int;

	Config config_;
	std::unique_ptr<portaudio::Stream> stream_;
	std::optional<StreamInfo> requested_info_;
	std::string last_error_;
	std::vector<StreamFinishedTask> finished_tasks_;
	PaStreamCallback* callback_ {};
	void* user_data_ {};
};

inline Stream::Stream(Config && config)
	: config_ { std::move(config) }
{
}

inline auto Stream::abort() -> void
{
	if (!stream_) return;

	stream_->abort();
	stream_.reset();
}

inline auto Stream::get_cpu_load() const -> double
{
	if (!stream_) return 0.0;

	return stream_->get_cpu_load();
}

inline auto Stream::get_info() const -> std::optional<StreamInfo>
{
	return requested_info_;
}

inline auto Stream::get_input_channel_count() const -> int
{
	return requested_info_ && requested_info_->input_params ? requested_info_->input_params->channelCount : 0;
}

inline auto Stream::get_output_latency() const -> double
{
	return stream_->info.output_latency;
}

inline auto Stream::get_SR() const -> int
{
	return requested_info_ ? requested_info_->SR : 0;
}

inline auto Stream::get_host_type() const -> PaHostApiTypeId 
{
	if (!stream_) return PaHostApiTypeId(-1);

	return stream_->host_type;
}

inline auto Stream::get_time() const -> double
{
	if (!stream_) return 0.0;

	return stream_->get_time();
}

inline auto Stream::is_active() const -> bool
{
	return stream_ && stream_->is_active();
}

inline auto Stream::raise_error(std::string error) -> void
{
	last_error_ = error;
	config_.callbacks.error(error);
}

inline auto Stream::push_finished_task(StreamFinishedTask task) -> void
{
	finished_tasks_.push_back(task);
}

inline auto Stream::request(Request settings) -> void
{
	if (is_active()) return;

	stream_.reset();

	try
	{
		std::optional<PaStreamParameters> input_params;

		if (settings.input_device)
		{
			input_params = settings.input_device->make_input_stream_parameters(settings.latency);
		}

		const auto output_params { settings.output_device.make_output_stream_parameters(settings.latency) };

		const auto input_params_ptr { input_params ? &(*input_params) : nullptr };
		const auto output_params_ptr { &output_params };

		auto check_supported {
			portaudio::Library::C::IsFormatSupported(input_params_ptr, output_params_ptr, settings.SR)
		};
		
		const char* error_text {};

		if (check_supported != paFormatIsSupported)
		{
			error_text = portaudio::Library::C::GetErrorText(check_supported);

			// If the user tried requesting a non-default sample rate and it failed,
			// try using the default instead
			const auto default_SR { int(settings.output_device.info.defaultSampleRate) };

			if (settings.SR != default_SR)
			{
				check_supported = portaudio::Library::C::IsFormatSupported(input_params_ptr, output_params_ptr, default_SR);

				if (check_supported == paFormatIsSupported)
				{
					std::stringstream ss;

					ss << "Tried sample rate " << settings.SR << " but it didn't work. Falling back to default (" << default_SR << " Hz) instead";

					config_.callbacks.info(ss.str());

					settings.SR = default_SR;

					config_.callbacks.sample_rate_changed(settings.SR);
				}
			}

			if (check_supported != paFormatIsSupported)
			{
				raise_error(error_text);

				return;
			}
		}

		StreamInfo requested_info
		{
			input_params,
			output_params,
			settings.input_device,
			settings.output_device,
			settings.frames_per_buffer,
			settings.SR,
			settings.latency
		};

		requested_info_.emplace(std::move(requested_info));

		start();
	}
	catch (const std::exception& err)
	{
		raise_error(err.what());
	}
}

inline auto Stream::start() -> void
{
	try
	{
		if (!requested_info_) return;

		const auto input_params { requested_info_->input_params ? &(*requested_info_->input_params) : nullptr };
		const auto output_params { &requested_info_->output_params };
		
		pax::portaudio::Stream::Config config;

		config.callback = &Stream::_callback;
		config.flags = paNoFlag;
		config.frames_per_buffer = requested_info_->frames_per_buffer;
		config.input_parameters = input_params;
		config.output_parameters = output_params;
		config.sample_rate = requested_info_->SR;
		config.user_data = this;

		stream_.reset();
		stream_ = std::make_unique<pax::portaudio::Stream>(config);
		stream_->set_finished_callback(&Stream::_on_finished);
	}
	catch (const std::exception& err)
	{
		raise_error(err.what());
		return;
	}

	config_.callbacks.starting();
	stream_->start();
	config_.callbacks.started();
}

inline auto Stream::set_callback(PaStreamCallback* callback, void* user_data) -> void
{
	callback_ = callback;
	user_data_ = user_data;
}

inline auto Stream::stop() -> void
{
	if (!stream_) return;

	stream_->stop();
}

inline auto Stream::on_finished() -> void
{
	for (auto task : finished_tasks_)
	{
		task();
	}

	finished_tasks_.clear();
	config_.callbacks.stopped();
}

inline auto Stream::_on_finished(void* user_data) -> void
{
	reinterpret_cast<Stream*>(user_data)->on_finished();
}

inline auto Stream::callback(
	const void* input,
	void* output,
	unsigned long frame_count,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flags) -> int
{
	return callback_(input, output, frame_count, time_info, status_flags, user_data_);
}

inline auto Stream::_callback(
	const void* input,
	void* output,
	unsigned long frame_count,
	const PaStreamCallbackTimeInfo* time_info,
	PaStreamCallbackFlags status_flags,
	void* user_data) -> int
{
	return reinterpret_cast<Stream*>(user_data)->callback(input, output, frame_count, time_info, status_flags);
}

} // pax
