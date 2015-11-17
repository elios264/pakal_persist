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


namespace Pakal
{
	class Element;

	class  TextReader : public Archive
	{
		Element* m_root;
		Element	 m_empty;
		std::stack<Element*> m_context;

		std::map<void*,void*> m_solved;

	protected:
		explicit TextReader();
		virtual ~TextReader();

		inline Element* get_current_element() { return m_context.top(); };
		inline Element* get_root() { return m_root; };

		void begin_object(const char* name) override;
		void end_object_as_value(const void* address) override;
		void end_object_as_reference() override;
		void refer_object(const char* name, void*& value) override;


		inline bool has_unsolved_references()
		{
			return !m_solved.empty();
		}

		size_t children_name_count(const char* name) override;
	public:
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
	};
}