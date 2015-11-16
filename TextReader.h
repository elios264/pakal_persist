#pragma once

#include "Archive.h"
#include "Element.h"

#include <stack>
#include <map>


namespace Pakal
{
	class Element;

	class _PAKALExport TextReader : public Archive
	{
		Element* m_root;
		Element	 m_empty;
		std::stack<Element*> m_context;

		std::map<void*,void*> m_solved;

	protected:
		explicit TextReader();
		virtual ~TextReader();

		inline Element* get_current_element() { ASSERT(!m_context.empty()); return m_context.top(); };
		inline Element* get_root() { return m_root; };

		void begin_object(const char* name) override;
		void end_object_as_reference(void*& address) override;
		void end_object_as_value(const void* address) override;

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