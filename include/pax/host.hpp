#pragma once

#include <string_view>
#include "pa_lib.hpp"

namespace pax {

class Host
{
public:

	const PaHostApiIndex index;
	const PaHostApiInfo info;
	const std::string_view name;

	Host(PaHostApiIndex index_);
};

inline Host::Host(PaHostApiIndex index_)
	: index { index_ }
	, info { *portaudio::Library::C::GetHostApiInfo(index) }
	, name { info.name }
{
}

} // pax
