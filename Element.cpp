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

	ASSERT(i != m_parent->m_elements.end());
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
