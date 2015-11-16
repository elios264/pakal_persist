#pragma once

#include "TextReader.h"
#include "ResourceManager.h"
#include "Element.h"

namespace pugi
{
	class xml_node;
}

namespace Pakal
{
	class _PAKALExport XmlReader : private TextReader
	{
		void parse(std::istream& stream, Element* root);
		void parse_element(pugi::xml_node* node, Element* element);

	public:

		template <class Type> void read(const char* fileName,const char* name,Type& object)
		{
			ASSERT(fileName != nullptr);
			
			auto stream = ResourceManager::instance().open_read_resource(fileName,false);

			read(*stream, name, object);
		}

		template <class Type> void read(std::istream& stream, const char* name,Type& object)
		{
			//get values
			set_type(ArchiveType::Reader);
			parse(stream, get_root());
			Archive::value<Type>(name, object);

			if (has_unsolved_references())
			{
				//reset the stream
				stream.clear();
				stream.seekg(0);

				//solve the references
				set_type(ArchiveType::Resolver);
				parse(stream, get_root());
				Archive::value<Type>(name, object);
			}

		}

	};
}
