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
#include <cassert>

using namespace Pakal;

TextWriter::~TextWriter() {}

void TextWriter::push_root(Element* root)
{
	m_root = root;
	m_context.push(root); 
}

void TextWriter::pop_root()
{
	assert(m_context.top() == m_root && m_context.size() == 1);

	m_root = nullptr;
	m_context.pop(); 
}

void TextWriter::begin_object(const char* name, bool isContainer)
{
	m_context.push(get_current_element()->add_element(Element(name,isContainer)));
}

void TextWriter::end_object_as_reference()
{
	m_context.pop();
}

void TextWriter::end_object_as_value(const void* address)
{
	get_current_element()->address(const_cast<void*>(address));
	m_context.pop();
}

size_t TextWriter::get_children_name_count(const char* name)
{
	return get_current_element() ? get_current_element()->elements_with_name(name) : 0;
}

const char* TextWriter::get_object_class_name()
{
	return "";
}
void TextWriter::set_object_class_name(const char* className)
{
	if (*className)
	{
		Attribute* attr = get_current_element()->find_attribute(class_kwd);
		if (attr == nullptr)
		{
			get_current_element()->add_attribute(Attribute(class_kwd, className));
		}
		else
		{
			attr->string(className);
		}
	}
}

void TextWriter::solve_references()
{
	if (m_references.empty())
	{
		return;
	}

	std::stack<Element*> m_stack;
	m_stack.push(m_root);

	while (!m_stack.empty())
	{
		Element* e = m_stack.top();
		m_stack.pop();

		if (e->address() && m_references.find(e->address()) != m_references.end())
		{
			e->add_attribute(Attribute(address_kwd, e->address()));
		}

		for(Element& child : e->elements())
		{
			m_stack.push(&child);
		}
	}
	m_references.clear();
}

void TextWriter::refer_object(const char* name, void*& value)
{
	get_current_element()->add_attribute(Attribute(name, value));
	m_references.insert(value);
}

void TextWriter::value(const char* name, bool& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, char& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, signed char& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, unsigned char& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, static_cast<unsigned>(value)));
}

void TextWriter::value(const char* name, short& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, unsigned short& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, static_cast<unsigned>(value)));
}

void TextWriter::value(const char* name, int& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, unsigned& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, long& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, unsigned long& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, static_cast<unsigned>(value)));
}

void TextWriter::value(const char* name, float& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, static_cast<double>(value)));
}

void TextWriter::value(const char* name, double& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, char* value, size_t max)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, std::string(value, strnlen(value, max))));
}

void TextWriter::value(const char* name, std::string& value)
{
	assert_if_reserved(name);
	get_current_element()->add_attribute(Attribute(name, value));
}