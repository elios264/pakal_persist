#include "XmlReader.h"
#include "pugixml.hpp"
#include "Element.h"

using namespace Pakal;
using namespace pugi;

void XmlReader::parse(std::istream& stream, Element* root)
{
	xml_document doc;
	xml_parse_result result = doc.load(stream);
	ASSERT_MSG(result.status == status_ok, "error reading the xml document: %s", result.description());

	parse_element(&doc, root);	
}

void XmlReader::parse_element(xml_node* node, Element* element)
{
	for (xml_attribute& attr : node->attributes())
	{
		element->add_attribute(Attribute(attr.name(), attr.value()));
	}

	for (xml_node& child : node->children())
	{
		parse_element(&child, element->add_element(Element(child.name())));
	}

}