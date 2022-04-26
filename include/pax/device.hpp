#pragma once

#include <string_view>
#include "pa_lib.hpp"

namespace pax {

class Device
{
public:

	enum class Type
	{
		Input,
		Output,
		InputOutput,
	};

	const PaDeviceIndex index;
	const PaDeviceInfo info;
	const Type type;
	const bool is_wasapi_loopback;
	const std::string_view name;

	Device(PaDeviceIndex device_index);
};

namespace detail {

static inline auto get_type(const PaDeviceInfo& info)
{
	if (info.maxInputChannels >= 1)
	{
		if (info.maxOutputChannels >= 1)
		{
			return Device::Type::InputOutput;
		}

		return Device::Type::Input;
	}

	return Device::Type::Output;
}

} // detail

inline Device::Device(PaDeviceIndex index_)
	: index { index_ }
	, info { *portaudio::Library::C::GetDeviceInfo(index) }
	, type { detail::get_type(info) }
	, is_wasapi_loopback { portaudio::Library::C::WASAPI::IsLoopback(index) == 1 }
	, name { info.name }
{
}

} // pax
