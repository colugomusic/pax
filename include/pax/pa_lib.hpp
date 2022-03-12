#pragma once

#include <memory>
#include <sstream>
#include <portaudio.h>

#ifdef _WIN32
#include <pa_asio.h>
#include <pa_win_wasapi.h>
#endif

namespace pax {
namespace portaudio {

class Library
{
public:

    struct C
    {
        static auto AbortStream(PaStream* stream) -> void;
        static auto CloseStream(PaStream* stream) -> void;
        static auto GetDefaultHostApi() -> PaHostApiIndex;
        static auto GetDefaultInputDevice() -> PaDeviceIndex;
        static auto GetDefaultOutputDevice() -> PaDeviceIndex;
        static auto GetDeviceCount() -> PaDeviceIndex;
        static auto GetDeviceInfo(PaDeviceIndex device) -> const PaDeviceInfo*;
        static auto GetErrorText(PaError errorCode) -> const char*;
        static auto GetHostApiCount() -> PaHostApiIndex;
        static auto GetHostApiInfo(PaHostApiIndex hostApi) -> const PaHostApiInfo*;
        static auto GetLastHostErrorInfo() -> const PaHostErrorInfo*;
        static auto GetStreamCpuLoad(PaStream* stream) -> double;
        static auto GetStreamInfo(PaStream* stream) -> const PaStreamInfo*;
        static auto GetStreamTime(PaStream* stream) -> PaTime;
        static auto GetVersion() -> int;
        static auto GetVersionText() -> const char*;
        static auto HostApiDeviceIndexToDeviceIndex(PaHostApiIndex hostApi, int hostApiDeviceIndex) -> PaDeviceIndex;
        static auto HostApiTypeIdToHostApiIndex(PaHostApiTypeId type) -> PaHostApiIndex;
        static auto Initialize() -> void;
        static auto IsFormatSupported(const PaStreamParameters* inputParameters, const PaStreamParameters* outputParameters, double sampleRate) -> PaError;
        static auto IsStreamActive(PaStream* stream) -> PaError;
        static auto IsStreamStopped(PaStream* stream) -> PaError;
        static auto OpenStream(PaStream** stream, const PaStreamParameters* inputParameters, const PaStreamParameters* outputParameters, double sampleRate, unsigned long framesPerBuffer, PaStreamFlags streamFlags, PaStreamCallback* streamCallback, void* userData) -> void;
        static auto SetStreamFinishedCallback(PaStream* stream, PaStreamFinishedCallback* streamFinishedCallback) -> void;
        static auto StartStream(PaStream* stream) -> void;
        static auto StopStream(PaStream* stream) -> void;
        static auto Terminate() -> void;

#	ifdef _WIN32
		struct WASAPI
		{
			static auto IsLoopback(PaDeviceIndex device) -> int;
		};
#	endif
    };

private:

    template <class T>
    static auto check_error(T result) -> T;

	template <class T>
    static auto check_null_result(T* result, const std::string& func) -> T*;

	static auto throw_runtime_error(PaError errorCode) -> void;
};

inline auto Library::C::GetVersion() -> int
{
	return Pa_GetVersion();
}

inline auto Library::C::GetVersionText() -> const char*
{
	return Pa_GetVersionText();
}

inline auto Library::C::GetErrorText(PaError errorCode) -> const char*
{
	return Pa_GetErrorText(errorCode);
}

inline auto Library::C::Initialize() -> void
{
	check_error(Pa_Initialize());
}

inline auto Library::C::Terminate() -> void
{
	check_error(Pa_Terminate());
}

inline auto Library::C::GetHostApiCount() -> PaHostApiIndex
{
	return check_error(Pa_GetHostApiCount());
}

inline auto Library::C::GetDefaultHostApi() -> PaHostApiIndex
{
	return check_error(Pa_GetDefaultHostApi());
}

inline auto Library::C::GetHostApiInfo(PaHostApiIndex hostApi) -> const PaHostApiInfo*
{
	return check_null_result(Pa_GetHostApiInfo(hostApi), "Pa_GetHostApiInfo");
}

inline auto Library::C::HostApiTypeIdToHostApiIndex(PaHostApiTypeId type) -> PaHostApiIndex
{
	return check_error(Pa_HostApiTypeIdToHostApiIndex(type));
}

inline auto Library::C::HostApiDeviceIndexToDeviceIndex(PaHostApiIndex hostApi, int hostApiDeviceIndex) -> PaDeviceIndex
{
	return check_error(Pa_HostApiDeviceIndexToDeviceIndex(hostApi, hostApiDeviceIndex));
}

inline auto Library::C::SetStreamFinishedCallback(PaStream* stream, PaStreamFinishedCallback* streamFinishedCallback) -> void
{
	check_error(Pa_SetStreamFinishedCallback(stream, streamFinishedCallback));
}

inline auto Library::C::GetDefaultInputDevice() -> PaDeviceIndex
{
	return Pa_GetDefaultInputDevice();
}

inline auto Library::C::GetDefaultOutputDevice() -> PaDeviceIndex
{
	return Pa_GetDefaultOutputDevice();
}

inline auto Library::C::GetDeviceCount() -> PaDeviceIndex
{
	return Pa_GetDeviceCount();
}

inline auto Library::C::GetDeviceInfo(PaDeviceIndex device) -> const PaDeviceInfo*
{
	return check_null_result(Pa_GetDeviceInfo(device), "Pa_GetDeviceInfo");
}

inline auto Library::C::GetLastHostErrorInfo() -> const PaHostErrorInfo*
{
	return Pa_GetLastHostErrorInfo();
}

inline auto Library::C::IsFormatSupported(const PaStreamParameters* inputParameters, const PaStreamParameters* outputParameters, double sampleRate) -> PaError
{
	return Pa_IsFormatSupported(inputParameters, outputParameters, sampleRate);
}

inline auto Library::C::OpenStream(PaStream** stream, const PaStreamParameters* inputParameters, const PaStreamParameters* outputParameters, double sampleRate, unsigned long framesPerBuffer, PaStreamFlags streamFlags, PaStreamCallback* streamCallback, void* userData) -> void
{
	check_error(Pa_OpenStream(stream, inputParameters, outputParameters, sampleRate, framesPerBuffer, streamFlags, streamCallback, userData));
}

inline auto Library::C::CloseStream(PaStream* stream) -> void
{
	check_error(Pa_CloseStream(stream));
}

inline auto Library::C::StartStream(PaStream* stream) -> void
{
	check_error(Pa_StartStream(stream));
}

inline auto Library::C::StopStream(PaStream* stream) -> void
{
	check_error(Pa_StopStream(stream));
}

inline auto Library::C::AbortStream(PaStream* stream) -> void
{
	check_error(Pa_AbortStream(stream));
}

inline auto Library::C::IsStreamStopped(PaStream* stream) -> PaError
{
	return check_error(Pa_IsStreamStopped(stream));
}

inline auto Library::C::IsStreamActive(PaStream* stream) -> PaError
{
	return check_error(Pa_IsStreamActive(stream));
}

inline auto Library::C::GetStreamInfo(PaStream* stream) -> const PaStreamInfo*
{
	return check_null_result(Pa_GetStreamInfo(stream), "Pa_GetStreamInfo");
}

inline auto Library::C::GetStreamTime(PaStream* stream) -> PaTime
{
	return Pa_GetStreamTime(stream);
}

inline auto Library::C::GetStreamCpuLoad(PaStream* stream) -> double
{
	return Pa_GetStreamCpuLoad(stream);
}

#ifdef _WIN32

inline auto Library::C::WASAPI::IsLoopback(PaDeviceIndex device) -> int
{
	return PaWasapi_IsLoopback(device);
}

#endif

template <class T>
inline auto Library::check_error(T result) -> T
{
	if (result < 0)
	{
		throw_runtime_error(result);
	}

	return result;
}

template <class T>
inline auto Library::check_null_result(T* result, const std::string& func) -> T*
{
	if (!result)
	{
		std::stringstream ss;

		ss << "PortAudio error: " << func << " returned null";

		throw std::runtime_error(ss.str());
	}

	return result;
}

inline auto Library::throw_runtime_error(PaError errorCode) -> void
{
	std::stringstream ss;

	ss << "PortAudio error: " << C::GetErrorText(errorCode);

	throw std::runtime_error(ss.str());
}

} // portaudio
} // pax
