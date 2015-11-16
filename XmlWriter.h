#pragma once

#include "TextWriter.h"
#include "ResourceManager.h"

namespace pugi
{
	class xml_node;
}


namespace Pakal
{
	class _PAKALExport XmlWriter : private TextWriter
	{
		void write(std::ostream& ostream, Element* element);
		void write_element(pugi::xml_node* node, Element* element);


	public:
		template <class Type> void write(const char* filename, const char* name, Type& object)
		{
			ASSERT(filename != nullptr);

			auto stream = ResourceManager::instance().open_write_resource(filename);
			
			write(*stream, name, object);
		}

		template <class Type> void write(std::ostream& stream, const char* name, Type& object)
		{
			Archive::value<Type>(name, object);
			solve_references();
			write(stream, get_root());
		}

	};
}