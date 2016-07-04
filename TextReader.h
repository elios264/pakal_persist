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



#pragma once

#include "Archive.h"
#include "Element.h"

#include <stack>
#include <map>
#include <fstream>
#include <cassert>


namespace Pakal
{
	class Element;

	class TextReader : protected Archive
	{
		Element* m_root;
		Element	 m_empty;
		std::stack<Element*> m_context;
		std::map<void*,void*> m_solved;


		void push_root(Element* root);
		void pop_root();

		inline Element* get_current_element() { return m_context.top(); };

		void begin_object(const char* name, bool isContainer = false) override;
		void end_object_as_value(const void* address) override;
		bool has_object(const char* name) override;
		void end_object_as_reference() override;
		void refer_object(const char* name, void*& value) override;
		const char* get_object_class_name() override;
		void set_object_class_name(const char* className) override;

		size_t get_children_name_count(const char* name) override;

		void value(const char* name, bool& value) override;
		void value(const char* name, char& value) override;
		void value(const char* name, signed char& value) override;
		void value(const char* name, unsigned char& value) override;
		void value(const char* name, short& value) override;
		void value(const char* name, unsigned short& value) override;
		void value(const char* name, int& value) override;
		void value(const char* name, unsigned& value) override;
		void value(const char* name, long& value) override;
		void value(const char* name, unsigned long& value) override;
		void value(const char* name, float& value) override;
		void value(const char* name, double& value) override;
		void value(const char* name, char* value, size_t max) override;
		void value(const char* name, std::string& value) override;

	protected:

		explicit TextReader(IFactoryManager* factory);
		virtual ~TextReader();

		virtual bool parse_element(std::istream& stream, Element* root) = 0;

	public:

		template <class Type> bool read(const char* fileName, const char* name, Type& object)
		{
			std::ifstream stream(fileName);

			return read(stream, name, object);
		}

		template <class Type> bool read(std::istream& stream, const char* name, Type& object)
		{
			assert(*name);
			if((&stream) == nullptr)
				return false;

			//read the tree
			Element firstPass;
			Element secondPass;

			if (parse_element(stream, &firstPass))
			{
				secondPass = firstPass;
			}
			else
			{
				return false;
			}

			//resolve the value objects
			push_root(&firstPass);
				set_type(ArchiveType::Reader);
				Archive::value<Type>(name, object);
			pop_root();

			//resolve the reference objects
			if (!m_solved.empty())
			{
				push_root(&secondPass);
					set_type(ArchiveType::Resolver);
					Archive::value<Type>(name, object);
				pop_root();

				m_solved.clear();
			}

			clear_read_cache();

			return true;
		}

	};
}