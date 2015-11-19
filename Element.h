/**
* Pakal persist serializer - version 1.0
* --------------------------------------------------------
* Copyright (C) 2015-2020, by Luis Alberto Gudiño Salinas (elios264@hotmail.com)
* Report bugs and download new versions at https://github.com/elios264/pakal_persist
*
* This library is distributed under the MIT License. See notice at the end
* of this comment.
*
* This work is based on the sweet persist, which is:
* Copyright (C) 2006 - 2011, by Charles Baker (http://www.sweetsoftware.co.nz/persist_overview.html)
*
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use,
* copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following
* conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*/


#pragma once

#include <string>
#include <list>
#include "Attribute.h"

namespace Pakal
{
	class Element
	{
		void* m_address;
		Element* m_parent;
		bool m_is_container;
		
		std::string m_name;
		std::list<Attribute> m_attributes;
		std::list<Element> m_elements;

	public:
		Element() : m_address(nullptr), m_parent(nullptr), m_is_container(false) { }
		Element(const std::string& name,bool isContainer);

		Element(const Element& other)
			: m_address { other.m_address },
			  m_parent { other.m_parent },
			  m_is_container { other.m_is_container },
			  m_name { other.m_name },
			  m_attributes { other.m_attributes },
			  m_elements { other.m_elements }
		{
			for (auto& e : m_elements)
			{
				e.m_parent = this;
			}
		}
		Element& operator=(const Element& other)
		{
			if (this == &other)
				return *this;

			m_address = other.m_address;
			m_parent = other.m_parent;
			m_is_container = other.m_is_container;
			m_name = other.m_name;
			m_attributes = other.m_attributes;
			m_elements = other.m_elements;

			for (auto& e : m_elements)
			{
				e.m_parent = this;
			}
			return *this;
		}

		const void* address() const;
		void address(void* addr);

		const std::string& name() const;
		inline bool is_container() { return m_is_container;  }

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
