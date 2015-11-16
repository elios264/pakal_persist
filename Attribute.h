#pragma once

#include "Config.h"
#include <string>
#include <sstream>

namespace Pakal
{
	class _PAKALExport Attribute
	{
		std::string m_name;
		mutable std::string m_value;
		
	public:
		const std::string& name() const;

		template<class T> Attribute(const std::string& name, T value);

		void* address() const;
		bool boolean() const;
		int integer() const;
		unsigned int unsigned_integer() const;
		double real() const;
		const std::string& string() const;
	};

	template<class T>
	Attribute::Attribute(const std::string& name, T address) : m_name(name)
	{
		std::ostringstream stream;
		stream << address;
		m_value = stream.str();
	}
}
