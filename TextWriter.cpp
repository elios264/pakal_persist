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

#include "TextWriter.h"
#include "Element.h"

using namespace Pakal;


TextWriter::~TextWriter() 
{
	delete m_root;
}

void TextWriter::begin_object(const char* name)
{
	m_root == nullptr ?
		m_context.push(m_root = new Element(name)) :
		m_context.push(get_current_element()->add_element(Element(name)));
}

void TextWriter::end_object_as_reference(void*& address)
{
	get_current_element()->address(address);
	m_references.insert(address);
	m_context.pop();
}

void TextWriter::end_object_as_value(const void* address)
{
	get_current_element()->address(const_cast<void*>(address));
	m_context.pop();
}

size_t TextWriter::children_name_count(const char* name)
{
	return get_current_element() ? get_current_element()->elements_with_name(name) : 0;
}

void TextWriter::solve_references()
{
	if (m_references.empty())
	{
		return;
	}

	std::stack<Element*> m_stack;
	m_stack.push(get_root());

	while (!m_stack.empty())
	{
		Element* e = m_stack.top();
		m_stack.pop();

		if (m_references.find(e->address()) != m_references.end())
		{
			e->add_attribute(Attribute("address", e->address()));
		}

		for(Element& child : e->elements())
		{
			m_stack.push(&child);
		}
	}
	m_references.clear();
}

void TextWriter::value(const char* name, bool& value)
{
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, char& value)
{
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, signed char& value)
{
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, unsigned char& value)
{
	get_current_element()->add_attribute(Attribute(name, static_cast<unsigned>(value)));
}

void TextWriter::value(const char* name, short& value)
{
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, unsigned short& value)
{
	get_current_element()->add_attribute(Attribute(name, static_cast<unsigned>(value)));
}

void TextWriter::value(const char* name, int& value)
{
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, unsigned& value)
{
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, long& value)
{
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, unsigned long& value)
{
	get_current_element()->add_attribute(Attribute(name, static_cast<unsigned>(value)));
}

void TextWriter::value(const char* name, float& value)
{
	get_current_element()->add_attribute(Attribute(name, static_cast<double>(value)));
}

void TextWriter::value(const char* name, double& value)
{
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, char* value, size_t max)
{
	get_current_element()->add_attribute(Attribute(name, std::string(value, strnlen(value, max))));
}

void TextWriter::value(const char* name, std::string& value)
{
	get_current_element()->add_attribute(Attribute(name, value));
}