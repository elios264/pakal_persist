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

#include "JsonReader.h"
#include "picojson/picojson.h"

#include <cassert>

using namespace Pakal;
using namespace picojson;


void JsonReader::parse_element(std::istream& stream, Element* element)
{
	picojson::value rootValue;
	std::string&& error = picojson::parse(rootValue, stream);

	assert(("error reading the json",error.empty()));
	
	parse_element(rootValue.get<object>(), element);
}

void JsonReader::parse_element(object& object, Element* element)
{
	for(auto& pair : object)
	{
		if (pair.second.is<picojson::object>())
		{
			parse_element(pair.second.get<picojson::object>(), element->add_element(Element(pair.first,false)));
		}
		else if (pair.second.is<array>())
		{
			array& children = pair.second.get<array>();
			parse_element(children, element->add_element(Element(pair.first,true)));
		}
		else
		{
			element->add_attribute(Attribute(pair.first,pair.second.to_str()));
		}
	}
}

void JsonReader::parse_element(array& array, Element* element)
{
	for(picojson::value& child : array)
	{
		object& object = child.get<picojson::object>();

		parse_element(object,element->add_element(Element()));
	}
}