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
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, char& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, signed char& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, unsigned char& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, static_cast<unsigned>(value)));
}

void TextWriter::value(const char* name, short& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, unsigned short& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, static_cast<unsigned>(value)));
}

void TextWriter::value(const char* name, int& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, unsigned& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, long& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, static_cast<int>(value)));
}

void TextWriter::value(const char* name, unsigned long& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, static_cast<unsigned>(value)));
}

void TextWriter::value(const char* name, float& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, static_cast<double>(value)));
}

void TextWriter::value(const char* name, double& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, value));
}

void TextWriter::value(const char* name, char* value, size_t max)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, std::string(value, strnlen(value, max))));
}

void TextWriter::value(const char* name, std::string& value)
{
	ASSERT(name != nullptr);
	get_current_element()->add_attribute(Attribute(name, value));
}