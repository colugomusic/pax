#pragma once

#include <unordered_map>
#include <vector>
#include "device.hpp"
#include "host.hpp"

namespace pax {

class System
{
private:

	struct ScopedPA
	{
		ScopedPA() { portaudio::Library::C::Initialize(); }
		~ScopedPA() { portaudio::Library::C::Terminate(); }
	} scoped_pa_;

public:

	using Devices = std::unordered_map<PaDeviceIndex, Device>;
	using Hosts = std::unordered_map<PaHostApiIndex, Host>;
	using HostDevices = std::unordered_map<PaHostApiIndex, std::vector<PaDeviceIndex>>;

	const Hosts hosts;
	const Devices devices;
	const std::vector<PaDeviceIndex> input_devices;
	const std::vector<PaDeviceIndex> output_devices;
	const HostDevices host_devices;

	const PaHostApiIndex default_host;
	const PaDeviceIndex default_input_device;
	const PaDeviceIndex default_output_device;

	System();

	auto get_default_host() const -> Host;
	auto get_default_input_device() const -> Device;
	auto get_default_output_device() const -> Device;
	auto get_default_input_device(Host host) const -> Device;
	auto get_default_output_device(Host host) const -> Device;
	auto get_device(PaDeviceIndex index) const -> Device;
	auto get_host(Device device) const -> Host;
	auto get_host(PaHostApiIndex index) const -> Host;
};

namespace detail {

static inline auto enumerate_devices() -> System::Devices
{
	System::Devices out;

	const auto device_count { portaudio::Library::C::GetDeviceCount() };

	if (device_count < 1) return out;

	for (PaDeviceIndex index { 0 }; index < device_count; index++)
	{
		out.emplace(index, Device { index });
	}

	return out;
}

static inline auto enumerate_hosts() -> System::Hosts
{
	System::Hosts out;

	const auto host_count { portaudio::Library::C::GetHostApiCount() };

	if (host_count < 1) return out;

	for (PaHostApiIndex index { 0 }; index < host_count; index++)
	{
		Host host { index };

		if (host.info.deviceCount < 1) continue;

		out.emplace(index, std::move(host));
	}

	return out;
}

static inline auto enumerate_input_devices(const System::Devices& devices) -> std::vector<PaDeviceIndex>
{
	std::vector<PaDeviceIndex> out;

	for (const auto& [index, device] : devices)
	{
		if (device.type == Device::Type::Input || device.type == Device::Type::InputOutput)
		{
			out.push_back(index);
		}
	}

	return out;
}

static inline auto enumerate_output_devices(const System::Devices& devices) -> std::vector<PaDeviceIndex>
{
	std::vector<PaDeviceIndex> out;

	for (const auto& [index, device] : devices)
	{
		if (device.type == Device::Type::Output || device.type == Device::Type::InputOutput)
		{
			out.push_back(index);
		}
	}

	return out;
}

static inline auto enumerate_host_devices(const System::Devices& devices) -> System::HostDevices
{
	System::HostDevices out;

	for (const auto& [index, device] : devices)
	{
		out[device.info.hostApi].push_back(index);
	}

	return out;
}

static inline auto find_default_host(const System::Hosts& hosts) -> PaHostApiIndex
{
	auto out { portaudio::Library::C::GetDefaultHostApi() };

	if (!hosts.empty() && hosts.find(out) == hosts.end())
	{
		out = hosts.begin()->first;
	}

	return out;
}

static inline auto find_default_input_device(const std::vector<PaDeviceIndex>& input_devices) -> PaDeviceIndex
{
	auto out { portaudio::Library::C::GetDefaultInputDevice() };

	if (!input_devices.empty() && std::find(input_devices.begin(), input_devices.end(), out) == input_devices.end())
	{
		out = *input_devices.begin();
	}

	return out;
}

static inline auto find_default_output_device(const std::vector<PaDeviceIndex>& output_devices) -> PaDeviceIndex
{
	auto out { portaudio::Library::C::GetDefaultOutputDevice() };

	if (!output_devices.empty() && std::find(output_devices.begin(), output_devices.end(), out) == output_devices.end())
	{
		out = *output_devices.begin();
	}

	return out;
}

} // detail

inline System::System()
	: hosts { detail::enumerate_hosts() }
	, devices { detail::enumerate_devices() }
	, input_devices { detail::enumerate_input_devices(devices) }
	, output_devices { detail::enumerate_output_devices(devices) }
	, host_devices { detail::enumerate_host_devices(devices) }
	, default_host { detail::find_default_host(hosts) }
	, default_input_device { detail::find_default_input_device(input_devices) }
	, default_output_device { detail::find_default_output_device(output_devices) }
{
}

inline auto System::get_default_host() const -> Host
{
	return get_host(default_host);
}

inline auto System::get_default_input_device() const -> Device
{
	return get_device(default_input_device);
}

inline auto System::get_default_output_device() const -> Device
{
	return get_device(default_output_device);
}

inline auto System::get_default_input_device(Host host) const -> Device
{
	return get_device(host.info.defaultInputDevice);
}

inline auto System::get_default_output_device(Host host) const -> Device
{
	return get_device(host.info.defaultOutputDevice);
}

inline auto System::get_device(PaDeviceIndex index) const -> Device
{
	const auto pos { devices.find(index) };

	assert(pos != devices.end());

	return pos->second;
}

inline auto System::get_host(Device device) const -> Host
{
	return get_host(device.info.hostApi);
}

inline auto System::get_host(PaHostApiIndex index) const -> Host
{
	const auto pos { hosts.find(index) };

	assert(pos != hosts.end());

	return pos->second;
}

} // pax
