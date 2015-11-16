#pragma once

#include "Config.h"
#include <string>
#include <list>
#include "Attribute.h"

namespace Pakal
{
	class _PAKALExport Element
	{
		void* m_address;
		Element* m_parent;
		
		std::string m_name;
		std::list<Attribute> m_attributes;
		std::list<Element> m_elements;

	public:
		Element() : m_address(nullptr), m_parent(nullptr) { }
		Element(const std::string& name);

		const void* address() const;
		void address(void* addr);

		const std::string& name() const;

		std::list<Attribute>& attributes();
		const std::list<Attribute>& attributes() const;

		std::list<Element>& elements();
		const std::list<Element>& elements() const;

		std::size_t elements_with_name(const char* name);

		void remove_from_parent();

		Element* add_element(Element&& element);
		Element* find_element(const std::string& name);

		void add_attribute(Attribute&& attribute);
		Attribute* find_attribute(const std::string& name);
	};
}
