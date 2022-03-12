#pragma once

#include <memory>
#include <sstream>
#include <portaudio.h>

#ifdef _WIN32
#include <pa_asio.h>
#endif

namespace pax {
namespace portaudio

class Library
{
public:

    struct C
    {
        static auto GetVersion() -> int;
        static auto GetVersionText() -> const char*;
        static auto GetErrorText(PaError errorCode) -> const char*;
        static auto Initialize() -> void;
        static auto Terminate() -> void;
        static auto GetHostApiCount() -> PaHostApiIndex;
        static auto GetDefaultHostApi() -> PaHostApiIndex;
        static auto GetHostApiInfo(PaHostApiIndex hostApi) -> const PaHostApiInfo*;
        static auto HostApiTypeIdToHostApiIndex(PaHostApiTypeId type) -> PaHostApiIndex;
        static auto HostApiDeviceIndexToDeviceIndex(PaHostApiIndex hostApi, int hostApiDeviceIndex) -> PaDeviceIndex;
        static auto SetStreamFinishedCallback(PaStream* stream, PaStreamFinishedCallback* streamFinishedCallback) -> void;
        static auto GetDefaultOutputDevice() -> PaDeviceIndex;
        static auto GetDeviceInfo(PaDeviceIndex device) -> const PaDeviceInfo*;
        static auto GetLastHostErrorInfo() -> const PaHostErrorInfo*;
        static auto IsFormatSupported(const PaStreamParameters* inputParameters, const PaStreamParameters* outputParameters, double sampleRate) -> PaError;
        static auto OpenStream(PaStream** stream, const PaStreamParameters* inputParameters, const PaStreamParameters* outputParameters, double sampleRate, unsigned long framesPerBuffer, PaStreamFlags streamFlags, PaStreamCallback* streamCallback, void* userData) -> void;
        static auto CloseStream(PaStream* stream) -> void;
        static auto StartStream(PaStream* stream) -> void;
        static auto StopStream(PaStream* stream) -> void;
        static auto AbortStream(PaStream* stream) -> void;
        static auto IsStreamStopped(PaStream* stream) -> PaError;
        static auto IsStreamActive(PaStream* stream) -> PaError;
        static auto GetStreamInfo(PaStream* stream) -> const PaStreamInfo*;
        static auto GetStreamTime(PaStream* stream) -> PaTime;
        static auto GetStreamCpuLoad(PaStream* stream) -> double;
    };

private:

    template <class T>
    static auto check_error(T result);

	template <class T>
    static auto check_null_result(T* result, const std::string& func);
};

static auto Library::C::GetVersion() -> int
{
	return Pa_GetVersion();
}

static auto Library::C::GetVersionText() -> const char*
{
	return Pa_GetVersionText();
}

static auto Library::C::GetErrorText(PaError errorCode) -> const char*
{
	return Pa_GetErrorText(errorCode);
}

static auto Library::C::Initialize() -> void
{
	check_error(Pa_Initialize());
}

static auto Library::C::Terminate() -> void
{
	check_error(Pa_Terminate());
}

static auto Library::C::GetHostApiCount() -> PaHostApiIndex
{
	return check_error(Pa_GetHostApiCount());
}

static auto Library::C::GetDefaultHostApi() -> PaHostApiIndex
{
	return check_error(Pa_GetDefaultHostApi());
}

static auto Library::C::GetHostApiInfo(PaHostApiIndex hostApi) -> const PaHostApiInfo*
{
	return check_null_result(Pa_GetHostApiInfo(hostApi), "Pa_GetHostApiInfo");
}

static auto Library::C::HostApiTypeIdToHostApiIndex(PaHostApiTypeId type) -> PaHostApiIndex
{
	return check_error(Pa_HostApiTypeIdToHostApiIndex(type));
}

static auto Library::C::HostApiDeviceIndexToDeviceIndex(PaHostApiIndex hostApi, int hostApiDeviceIndex) -> PaDeviceIndex
{
	return check_error(Pa_HostApiDeviceIndexToDeviceIndex(hostApi, hostApiDeviceIndex));
}

static auto Library::C::SetStreamFinishedCallback(PaStream* stream, PaStreamFinishedCallback* streamFinishedCallback) -> void
{
	check_error(Pa_SetStreamFinishedCallback(stream, streamFinishedCallback));
}

static auto Library::C::GetDefaultOutputDevice() -> PaDeviceIndex
{
	return Pa_GetDefaultOutputDevice();
}

static auto Library::C::GetDeviceInfo(PaDeviceIndex device) -> const PaDeviceInfo*
{
	return check_null_result(Pa_GetDeviceInfo(device), "Pa_GetDeviceInfo");
}

static auto Library::C::GetLastHostErrorInfo() -> const PaHostErrorInfo*
{
	return Pa_GetLastHostErrorInfo();
}

static auto Library::C::IsFormatSupported(const PaStreamParameters* inputParameters, const PaStreamParameters* outputParameters, double sampleRate) -> PaError
{
	return Pa_IsFormatSupported(inputParameters, outputParameters, sampleRate);
}

static auto Library::C::OpenStream(PaStream** stream, const PaStreamParameters* inputParameters, const PaStreamParameters* outputParameters, double sampleRate, unsigned long framesPerBuffer, PaStreamFlags streamFlags, PaStreamCallback* streamCallback, void* userData) -> void
{
	check_error(Pa_OpenStream(stream, inputParameters, outputParameters, sampleRate, framesPerBuffer, streamFlags, streamCallback, userData));
}

static auto Library::C::CloseStream(PaStream* stream) -> void
{
	check_error(Pa_CloseStream(stream));
}

static auto Library::C::StartStream(PaStream* stream) -> void
{
	check_error(Pa_StartStream(stream));
}

static auto Library::C::StopStream(PaStream* stream) -> void
{
	check_error(Pa_StopStream(stream));
}

static auto Library::C::AbortStream(PaStream* stream) -> void
{
	check_error(Pa_AbortStream(stream));
}

static auto Library::C::IsStreamStopped(PaStream* stream) -> PaError
{
	return check_error(Pa_IsStreamStopped(stream));
}

static auto Library::C::IsStreamActive(PaStream* stream) -> PaError
{
	return check_error(Pa_IsStreamActive(stream));
}

static auto Library::C::GetStreamInfo(PaStream* stream) -> const PaStreamInfo*
{
	return check_null_result(Pa_GetStreamInfo(stream), "Pa_GetStreamInfo");
}

static auto Library::C::GetStreamTime(PaStream* stream) -> PaTime
{
	return Pa_GetStreamTime(stream);
}

static auto Library::C::GetStreamCpuLoad(PaStream* stream) -> double
{
	return Pa_GetStreamCpuLoad(stream);
}

template <class T>
static auto inline Library::check_error(T result)
{
	if (result < 0)
	{
		throw_runtime_error(result);
	}

	return result;
}

template <class T>
static auto inline Library::check_null_result(T* result, const std::string& func)
{
	if (!result)
	{
		std::stringstream ss;

		ss << "PortAudio error: " << func << " returned null";

		throw std::runtime_error(ss.str());
	}

	return result;
}

} // portaudio
} // pax
