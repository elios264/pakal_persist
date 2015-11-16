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

#include "Element.h"

using namespace Pakal;


Element::Element(const std::string& name)
	: 
	m_address(nullptr),
	m_parent(nullptr),
	m_name(name),
	m_attributes(),
	m_elements()
{
}

const std::string& Element::name() const
{
	return m_name;
}

const void* Element::address() const
{
	return m_address;
}
void Element::address(void* addr)
{
	m_address = addr;
}

std::list<Attribute>& Element::attributes()
{
	return m_attributes;
}

const std::list<Attribute>& Element::attributes() const
{
	return m_attributes;
}

std::list<Element>& Element::elements()
{
	return m_elements;
}

const std::list<Element>& Element::elements() const
{
	return m_elements;
}

std::size_t Element::elements_with_name(const char* name)
{
	int size = 0;
	for(auto& e : m_elements)
	{
		size += static_cast<int>(e.name() == name);
	}

	return size;
}

void Element::remove_from_parent()
{
	std::list<Element>::iterator i = m_parent->m_elements.begin();

	while (i != m_parent->m_elements.end() && i->name() != m_name)
	{
		++i;
	}

	m_parent->m_elements.erase(i);
}

Element* Element::add_element(Element&& element)
{
	element.m_parent = this;
	m_elements.emplace_back(element);
	return &m_elements.back();
}


void Element::add_attribute(Attribute&& attribute)
{
	m_attributes.emplace_back(attribute);
}

Attribute* Element::find_attribute(const std::string& name)
{
	std::list<Attribute>::iterator i = m_attributes.begin();
	while (i != m_attributes.end() && i->name() != name)
	{
		++i;
	}
	return i != m_attributes.end() ? &(*i) : nullptr;
}

Element* Element::find_element(const std::string& name)
{
	std::list<Element>::iterator i = m_elements.begin();
	while (i != m_elements.end() && i->name() != name)
	{
		++i;
	}
	return i != m_elements.end() ? &(*i) : nullptr;
}
