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

#include "XmlWriter.h"
#include "Element.h"
#include "pugixml//pugixml.hpp"

using namespace Pakal;
using namespace pugi;

void XmlWriter::write_element(std::ostream& ostream, Element* element)
{
	xml_document doc;

	xml_node node = doc.append_child(node_element);
	node.set_name(element->name().c_str());

	write_element(&node, element);

	doc.save(ostream);
}

void XmlWriter::write_element(xml_node* node, Element* element)
{
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
