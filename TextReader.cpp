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

#include "TextReader.h"
#include <cassert>

using namespace Pakal;


TextReader::TextReader(IFactoryManager* factory): Archive(ArchiveType::Reader,factory) , m_root(nullptr) {}
TextReader::~TextReader() {}


void TextReader::push_root(Element* root)
{
	m_root = root;
	m_context.push(root);
}

void TextReader::pop_root()
{
	assert(m_context.top() == m_root && m_context.size() == 1);

	m_root = nullptr;
	m_context.pop();
}


void TextReader::begin_object(const char* name,bool /*isContainer*/)
{
	if (get_current_element()->is_container())
	{
		auto& elements = get_current_element()->elements();
		m_context.push(elements.empty() ? &m_empty : &elements.front());
	}
	else
	{
		Element* current = get_current_element()->find_element(name);
		m_context.push(current ? current : &m_empty);
	}
}

void TextReader::end_object_as_value(const void* address)
{
	if (get_current_element() == &m_empty)
	{
		m_context.pop();
		return;
	}

	if (Attribute* attr = get_current_element()->find_attribute(address_kwd))
	{
		void* oldAddress = attr->address();
		m_solved.insert(std::make_pair(oldAddress, const_cast<void*>(address)));
	}

	get_current_element()->remove_from_parent();
	m_context.pop();
}

bool TextReader::has_object(const char* name)
{
	return get_current_element()->find_element(name) != nullptr;
}

void TextReader::end_object_as_reference()
{
	if (get_current_element() == &m_empty)
	{
		m_context.pop();
		return;
	}

	get_current_element()->remove_from_parent();
	m_context.pop();
}


void TextReader::refer_object(const char* name, void*& value)
{
	void* oldAddress = get_current_element()->find_attribute(name)->address();

	value = m_solved[oldAddress];
}

const char* TextReader::get_object_class_name()
{
	Attribute* attr = get_current_element()->find_attribute(class_kwd);

	return attr ? attr->string().c_str() : "";
}

void TextReader::set_object_class_name(const char* className)
{
}

size_t TextReader::get_children_name_count(const char* name)
{
	return get_current_element()->is_container() ? get_current_element()->elements().size() : get_current_element()->elements_with_name(name) ;
}

void TextReader::value(const char* name, bool& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = attribute->boolean();
	}
}

void TextReader::value(const char* name, char& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = static_cast<char>(attribute->integer());
	}
}

void TextReader::value(const char* name, signed char& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = static_cast<signed char>(attribute->integer());
	}
}

void TextReader::value(const char* name, unsigned char& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = static_cast<unsigned char>(attribute->unsigned_integer());
	}
}

void TextReader::value(const char* name, short& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = static_cast<short>(attribute->integer());
	}
}

void TextReader::value(const char* name, unsigned short& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = static_cast<unsigned short>(attribute->unsigned_integer());
	}
}

void TextReader::value(const char* name, int& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = attribute->integer();
	}
}

void TextReader::value(const char* name, unsigned& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = attribute->unsigned_integer();
	}
}

void TextReader::value(const char* name, long& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = static_cast<long>(attribute->integer());
	}
}

void TextReader::value(const char* name, unsigned long& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = static_cast<unsigned long>(attribute->unsigned_integer());
	}
}

void TextReader::value(const char* name, float& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = static_cast<float>(attribute->real());
	}
}

void TextReader::value(const char* name, double& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = attribute->real();
	}
}

void TextReader::value(const char* name, char* value, size_t max)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		strncpy_s(value, max, attribute->string().c_str(), max);
		value[max - 1] = '\0';
	}
}

void TextReader::value(const char* name, std::string& value)
{
	Attribute* attribute = get_current_element()->find_attribute(name);
	if (attribute)
	{
		value = attribute->string();
	}
}