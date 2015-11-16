#include "TextReader.h"

using namespace Pakal;


TextReader::TextReader(): Archive(ArchiveType::Reader)
{
	m_root = new Element();
	m_context.push(m_root);
}

TextReader::~TextReader()
{
	delete m_root;
}

void TextReader::begin_object(const char* name)
{
	Element* current = get_current_element()->find_element(name);

	m_context.push(current ? current : &m_empty );
}

void TextReader::end_object_as_value(const void* address)
{
	if (get_current_element() == &m_empty)
	{
		m_context.pop();
		return;
	}

	if (Attribute* attr = get_current_element()->find_attribute("address"))
	{
		void* oldAddress = attr->address();

		m_solved[oldAddress] = const_cast<void*>(address);
	}

	get_current_element()->remove_from_parent();
	m_context.pop();
}

void TextReader::end_object_as_reference(void*& address)
{
	if (!get_current_element())
	{
		m_context.pop();
		return;
	}

	void* oldAddress = get_current_element()->find_attribute("address")->address();

	address = m_solved[oldAddress];

	get_current_element()->remove_from_parent();
	m_context.pop();
}

size_t TextReader::children_name_count(const char* name)
{
	return get_current_element() ? get_current_element()->elements_with_name(name) : 0;
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