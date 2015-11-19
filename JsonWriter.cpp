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

#include "JsonWriter.h"
#include "picojson/picojson.h"
#include <cassert>


using namespace Pakal;
using namespace picojson;

void JsonWriter::write_element(std::ostream& ostream, Element* root)
{
	assert(root->elements().size() == 1);

	Element* data = &root->elements().front();

	object document;

	if (data->is_container())
	{
		array array;
		write_element(array, data);

		document[data->name()] = picojson::value(array);
	}
	else
	{
		object child;
		write_element(child, data);
		document[data->name()] = picojson::value(child);
	}

	std::string&& value = picojson::value(document).serialize(m_pretty);
	ostream.write(value.c_str(),value.size());
}

void JsonWriter::write_element(object& object, Element* element)
{
	for (Attribute& attr : element->attributes())
	{
		object[attr.name()] = picojson::value(attr.string());
	}

	for (Element& child : element->elements())
	{
		if (child.is_container())
		{
			array childNode;
			write_element(childNode, &child);

			auto inserted = object.insert(make_pair(child.name(), picojson::value(childNode))).second;
			assert(("Anonymus containers (empty string) are not allowed when serializing with json eg. archive->value('', 'animation', animations) ", inserted));
		}
		else
		{
			picojson::object childNode;
			write_element(childNode, &child);

			auto inserted = object.insert(make_pair(child.name(), picojson::value(childNode))).second;
			assert(("Anonymus containers (empty string) are not allowed when serializing with json eg. archive->value('', 'animation', animations) ", inserted));
		}
	}
}

void JsonWriter::write_element(array& array, Element* element)
{
	array.reserve(element->elements().size());

	for (Element& child : element->elements())
	{
		if (child.is_container())
		{
			picojson::array childNode;
			write_element(childNode, &child);
			array.emplace_back(picojson::value(childNode));
		}
		else
		{
			object childNode;
			write_element(childNode, &child);
			array.emplace_back(picojson::value(childNode));
		}
	}
}