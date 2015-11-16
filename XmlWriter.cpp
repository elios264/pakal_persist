#include "XmlWriter.h"
#include "Element.h"
#include "pugixml.hpp"

using namespace Pakal;
using namespace pugi;

void XmlWriter::write(std::ostream& ostream, Element* element)
{
	ASSERT(element != nullptr);

	xml_document doc;

	xml_node node = doc.append_child(node_element);
	node.set_name(element->name().c_str());

	write_element(&node, element);

	doc.save(ostream);
}

void XmlWriter::write_element(xml_node* node, Element* element)
{
	ASSERT(element != nullptr);

	for (Attribute& attr : element->attributes())
	{
		node->append_attribute(attr.name().c_str()).set_value(attr.string().c_str());
	}

	for (Element& child : element->elements())
	{
		xml_node childNode = node->append_child(node_element);
		childNode.set_name(child.name().c_str());

		write_element(&childNode, &child);
	}

}
