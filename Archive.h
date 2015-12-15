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

#include <string>
#include "PersistUtils.h"
#include <map>
#include <vector>
#include <cassert>
#include <functional>


namespace Pakal
{
	enum class  ArchiveType
	{
		Reader,
		Writer,
		Resolver,
	};

	class IFactoryManager
	{
	public:
		virtual ~IFactoryManager() {}
		virtual void* create_object(const std::string& className) = 0;
	};

	class SimpleFactoyManager : public IFactoryManager
	{
		std::map<std::string, std::function<void*()>> m_factories;

	public:
		void* create_object(const std::string& className) override;

		template<class T>
		void declare_object(const std::string& objectName, std::function<T*()> factory)
		{
			m_factories[objectName] = [factory]() { return static_cast<void*>(factory()); };
		}
	};


	class  Archive
	{
		ArchiveType m_type;
		std::map<void*, std::vector<const void*>> m_insertion_order;
		bool m_is_pointer;

		Archive(const Archive& other) = delete;
		Archive& operator=(const Archive& other) = delete;

		template< typename C, std::enable_if_t<!trait_utils::has_reserve<C>::value>* = nullptr >
		void try_reserve(C&, std::size_t) {}
		template< typename C, std::enable_if_t<trait_utils::has_reserve<C>::value>* = nullptr> 
		void try_reserve(C& c, std::size_t n)
		{
			c.reserve(c.size() + n);
		}

		template<class T, std::enable_if_t<trait_utils::has_persist<T>::value>* = nullptr>
		void container_value(const T& object);

		template<class T, std::enable_if_t<!trait_utils::has_persist<T>::value>* = nullptr >
		void container_value(const T& object);

		template<class T>
		T* create_polymorphic_object()
		{
			auto className = get_object_class_name();
			auto newObject = *className && ObjectFactory ? ObjectFactory->create_object(className) : nullptr;

			return newObject ? static_cast<T*>(newObject) : create_non_polymorphic_object<T>();
		}

		template<class T, std::enable_if_t<std::is_constructible<T>::value>* = nullptr >
		T* create_non_polymorphic_object() { return new T(); }

		template<class T, std::enable_if_t<!std::is_constructible<T>::value>* = nullptr >
		T* create_non_polymorphic_object()
		{
			assert(("could not instanciate object for deserialization since is not constructible nor in the factory", false));
			return nullptr;
		}

	protected:
		static constexpr const char* address_kwd = "address";
		static constexpr const char* class_kwd = "class";


		virtual void begin_object(const char* name,bool isContainer = false) = 0;
		virtual void end_object_as_reference() = 0;
		virtual void end_object_as_value(const void* address) = 0;

		virtual void refer_object(const char* name, void*& value) = 0;

		virtual size_t		get_children_name_count(const char* name) = 0;
		virtual void		set_object_class_name(const char* className) = 0;
		virtual const char* get_object_class_name() = 0;

		inline void set_type(ArchiveType type) { m_type = type; }
		inline void clear_read_cache() { m_insertion_order.clear(); }
		inline void assert_if_reserved(const char* name)
		{
			assert(("sorry keywords address and class are reserved for internal use", strcmp(address_kwd, name) != 0 && strcmp(class_kwd, name) != 0));
		};

		explicit Archive(ArchiveType type) : m_type(type), m_is_pointer(false) {}
		virtual ~Archive() {}

	public:	

		static IFactoryManager* ObjectFactory;
		inline ArchiveType get_type() { return m_type;  }
		virtual void set_type_name(const std::string& typeName)
		{
			if (m_type == ArchiveType::Writer && m_is_pointer)
			{
				set_object_class_name(typeName.c_str());
			}
		}

		virtual void value(const char* name, bool& value) = 0;
		virtual void value(const char* name, char& value) = 0;
		virtual void value(const char* name, signed char& value) = 0;
		virtual void value(const char* name, unsigned char& value) = 0;
		virtual void value(const char* name, short& value) = 0;
		virtual void value(const char* name, unsigned short& value) = 0;
		virtual void value(const char* name, int& value) = 0;
		virtual void value(const char* name, unsigned int& value) = 0;
		virtual void value(const char* name, long& value) = 0;
		virtual void value(const char* name, unsigned long& value) = 0;
		virtual void value(const char* name, float& value) = 0;
		virtual void value(const char* name, double& value) = 0;
		virtual void value(const char* name, char* value, size_t max) = 0;
		virtual void value(const char* name, std::string& value) = 0;

		template<class T, std::enable_if_t<std::is_pointer<T>::value>* = nullptr>
		void value(const char* name, T& object);

		//for an object that has a member called persist
		template<class T, std::enable_if_t<trait_utils::has_persist<T>::value>* = nullptr>
		void value(const char* name, T& object);

		//for an enum, is treated as the underlying type 
		template<class T, std::enable_if_t<std::is_enum<T>::value>* = nullptr>
		void value(const char* name, T& object);

		//for stl container or array with default childName to "item"
		template<class T, std::enable_if_t<trait_utils::has_persist<T>::value == false && std::is_enum<T>::value == false && std::is_pointer<T>::value==false>* = nullptr >
		void value(const char* name, T& object);

		//for stl container that is not associative
		template <template<typename ...> class stl_container, typename T,typename...etc, std::enable_if_t<!trait_utils::iterates_with_pair<stl_container<T,etc...>>::value>* = nullptr>
		void value(const char* name, const char* childName, stl_container<T, etc...>& container);
		
		//for an associative stl container like a map
		template <template<typename ...> class stl_container, typename Key,typename Value,typename...etc, std::enable_if_t<trait_utils::iterates_with_pair<stl_container<Key,Value,etc...>>::value>* = nullptr>
		void value(const char* name, const char* childName, stl_container<Key,Value,etc...>& container);

		////for an array 
		template <class T, size_t Length>
		void value(const char* name, const char* childName, T (&container)[Length]);

		//----------------Pointers as values--------------------------------------------

		template<class T, std::enable_if_t<trait_utils::has_persist<T>::value>* = nullptr>
		void value(const char* name, T*& object);

		template<class T, std::enable_if_t<!trait_utils::has_persist<T>::value>* = nullptr>
		void value(const char* name, T*& object);

		template <template<typename ...> class stl_container, typename T, typename...etc ,std::enable_if_t<!trait_utils::iterates_with_pair<stl_container<T*, etc...>>::value>* = nullptr>
		void value(const char* name, const char* childName, stl_container<T*, etc...>& container);

		template <template<typename ...> class stl_container, typename Key, typename Value, typename...etc, std::enable_if_t<trait_utils::iterates_with_pair<stl_container<Key, Value*, etc...>>::value>* = nullptr>
		void value(const char* name, const char* childName, stl_container<Key, Value*, etc...>& container);

		template <class T, size_t Length>
		void value(const char* name, const char* childName, T*(&container)[Length]);

		//---------Pointers as references------------------------------

		//for a pointer to an object just store the address
		template <class T> 
		void refer(const char* name, T*& object);

		//for stl container of pointers just store the addresses
		template <template<typename ...> class stl_container, typename T, typename...etc, std::enable_if_t<!trait_utils::iterates_with_pair<stl_container<T*, etc...>>::value>* = nullptr>
		void refer(const char* name, const char* childName, stl_container<T*,etc...>& container);

		//for associative stl container just store the addresses of the value field, 
		template <template<typename ...> class stl_container,typename Key,typename Value,typename...etc, std::enable_if_t<trait_utils::iterates_with_pair<stl_container<Key,Value*,etc...>>::value>* = nullptr>
		void refer(const char* name, const char* childName, stl_container<Key,Value*,etc...>& container);

		//for an array of pointers, just store the adresses
		template <class T, size_t Length> 
		void refer(const char* name, const char* childName, T*(&values)[Length]);

	};

	template<class T, std::enable_if_t<std::is_pointer<T>::value>*>
	void Archive::value(const char* name, T& object)
	{
		typename std::remove_pointer<T>::type* pointerObject = object;

		value(name, pointerObject);
	}

	template<class T, std::enable_if_t<trait_utils::has_persist<T>::value>*>
	void Archive::value(const char* name, T& object)
	{
		begin_object(name);
			object.persist(this);
		end_object_as_value(&object);
	}

	template<class T, std::enable_if_t<std::is_enum<T>::value>*>
	void Archive::value(const char* name, T& Enum)
	{
		auto n = static_cast<typename std::underlying_type<T>::type>(Enum);
		value(name, n);
		Enum = static_cast<T>(n);
	}

	template<class T, std::enable_if_t<trait_utils::has_persist<T>::value == false && std::is_enum<T>::value == false && std::is_pointer<T>::value == false>* >
	void Archive::value(const char* name, T& object)
	{
		static_assert(trait_utils::is_container<T>::value || std::is_array<T>::value, "T/Key must have a persist method or to be a stl container or an array or a pointer or an enum or a basic type");

		value(name, "item", object);
	}

	template<template<typename...> class stl_container, typename T, typename... etc, std::enable_if_t<!trait_utils::iterates_with_pair<stl_container<T, etc...>>::value>*>
	void Archive::value(const char* name, const char* childName, stl_container<T, etc...>& container)
	{
		if (*name)
			begin_object(name,true);

		switch (m_type)
		{
			case ArchiveType::Reader:
			{
				size_t count = get_children_name_count(childName);
				try_reserve(container, count);

				std::vector<const void*>& orderList = m_insertion_order[&container];
				orderList.reserve(count);

				for (size_t i = 0; i < count; i++)
				{
					begin_object(childName);
						T&& value = T();
						container_value(value);
						
						const void* finalAddress = &*container.insert(container.end(), value);
						orderList.push_back(finalAddress);
					end_object_as_value(finalAddress);
				}
			}
			break;
			case ArchiveType::Resolver:
			{
				std::vector<const void*>& orderList = m_insertion_order[&container];

				for(const void* elementAddr : orderList)
				{
					const T* element = static_cast<const T*>(elementAddr);

					begin_object(childName);
						container_value(*element);
					end_object_as_value(element);
				}

				m_insertion_order.erase(&container);

			}
			break;
			case ArchiveType::Writer:
			{
				for (const T& element : container)
				{
					begin_object(childName);
						container_value(element);
					end_object_as_value(&element);
				}
			}
			break;
		}

		if (*name)
			end_object_as_value(&container);
	}

	template<template <typename ...> class stl_container, typename Key, typename Value, typename ... etc, std::enable_if_t<trait_utils::iterates_with_pair<stl_container<Key, Value, etc...>>::value>*>
	void Archive::value(const char* name, const char* childName, stl_container<Key, Value, etc...>& container)	
	{
		static_assert(!std::is_pointer<Key>::value, "pointers are not currently supported as key on a map");

		if (*name)
			begin_object(name,true);

		switch (m_type)
		{
			case ArchiveType::Reader:
			{		
				size_t count = get_children_name_count(childName);

				std::vector<const void*>& orderList = m_insertion_order[&container];
				orderList.reserve(count);

				for (size_t i = 0; i < count; i++)
				{
					begin_object(childName);
					{
						begin_object("key");
							Key&& key = Key();
							container_value(key);

							auto& addresses = *container.insert(std::make_pair(key, Value())).first;
							orderList.push_back(&addresses);
						end_object_as_value(&addresses.first);

						begin_object("value");
							container_value(addresses.second);
						end_object_as_value(&addresses.second);
					}
					end_object_as_value(nullptr);
				}
			}
			break;
			case ArchiveType::Resolver:
			{
				std::vector<const void*>& orderList = m_insertion_order[&container];

				for (const void* elementAddr : orderList)
				{
					const auto* element = static_cast<const typename stl_container<Key, Value, etc...>::value_type*>(elementAddr);

					begin_object(childName);
					{
						begin_object("key");
							container_value(element->first);
						end_object_as_value(&element->first);

						begin_object("value");
							container_value(element->second);
						end_object_as_value(&element->second);
					}
					end_object_as_value(&element);

				}
				m_insertion_order.erase(&container);

			}
			break;
			case ArchiveType::Writer:
			{
				for (auto& element : container)
				{
					begin_object(childName);
					{
						begin_object("key");
							container_value(element.first);
						end_object_as_value(&element.first);

						begin_object("value");
							container_value(element.second);
						end_object_as_value(&element.second);
					}
					end_object_as_value(&element);
				}
			}
			break;
		}

		if (*name)
			end_object_as_value(&container);
	}

	template<class T, size_t Length>
	void Archive::value(const char* name, const char* childName, T(& container)[Length])
	{
		if (*name)
			begin_object(name,true);

		size_t count = Length;

		if (m_type == ArchiveType::Reader || m_type == ArchiveType::Resolver)
		{
			count = get_children_name_count(childName) > Length ? Length : get_children_name_count(childName);
		}

		for (size_t i = 0; i < count; i++)
		{
			begin_object(childName);
				container_value(container[i]);
			end_object_as_value(&container[i]);
		}

		if (*name)
			end_object_as_value(&container);
	}

	template<class T, std::enable_if_t<!trait_utils::has_persist<T>::value>*>
	void Archive::value(const char* name, T*& object)
	{
		if (m_type == ArchiveType::Reader)
		{
			object = create_non_polymorphic_object<T>();
		}

		value(name, *object);
	}

	template<class T, std::enable_if_t<trait_utils::has_persist<T>::value>*>
	void Archive::value(const char* name, T*& object)
	{
		begin_object(name);
		if (m_type == ArchiveType::Reader)
		{
			object = create_polymorphic_object<T>();
		}
		m_is_pointer = true;
		object->persist(this);
		m_is_pointer = false;
		end_object_as_value(object);
	}

	template<template <typename ...> class stl_container, typename T, typename ... etc, std::enable_if_t<!trait_utils::iterates_with_pair<stl_container<T*, etc...>>::value>*>
	void Archive::value(const char* name, const char* childName, stl_container<T*, etc...>& container)
	{
		if (*name)
			begin_object(name,true);

		switch (m_type)
		{
			case ArchiveType::Reader:
			{
				size_t count = get_children_name_count(childName);
				try_reserve(container, count);

				std::vector<const void*>& orderList = m_insertion_order[&container];
				orderList.reserve(count);

				for (size_t i = 0; i < count; i++)
				{
					begin_object(childName);
						T* object = create_polymorphic_object<T>();
						container_value(*object);
						orderList.push_back(object);
						container.insert(container.end(), object);
					end_object_as_value(object);
				}
			}
			break;
			case ArchiveType::Resolver:
			{
				std::vector<const void*>& orderList = m_insertion_order[&container];

				for (const void* elementAddr : orderList)
				{
					const T* element = static_cast<const T*>(elementAddr);

					begin_object(childName);
						container_value(*element);
					end_object_as_value(element);
				}

				m_insertion_order.erase(&container);
			}
			break;
			case ArchiveType::Writer:
			{
				for (const T* element : container)
				{
					begin_object(childName);
						m_is_pointer = true;
						container_value(*element);
						m_is_pointer = false;
					end_object_as_value(element);
				}
			}
			break;
		}

		if (*name)
			end_object_as_value(&container);
	}

	template<template <typename ...> class stl_container, typename Key, typename Value, typename ... etc, std::enable_if_t<trait_utils::iterates_with_pair<stl_container<Key, Value*, etc...>>::value>*>
	void Archive::value(const char* name, const char* childName, stl_container<Key, Value*, etc...>& container)
	{
		static_assert(!std::is_pointer<Key>::value, "pointers are not currently supported as key on a map");

		if (*name)
			begin_object(name,true);

		switch (m_type)
		{
			case ArchiveType::Reader:
			{
				size_t count = get_children_name_count(childName);

				std::vector<const void*>& orderList = m_insertion_order[&container];
				orderList.reserve(count);

				for (size_t i = 0; i < count; i++)
				{
					begin_object(childName);
					{
						begin_object("key");
							Key&& key = Key();
							container_value(key);
							auto& addresses = *container.insert(std::make_pair(key,nullptr)).first;
							orderList.push_back(&addresses);
						end_object_as_value(&addresses.first);

						begin_object("value");
							addresses.second = create_polymorphic_object<Value>();
							container_value(*addresses.second);
						end_object_as_value(addresses.second);
					}
					end_object_as_value(nullptr);
				}
			}
			break;
			case ArchiveType::Resolver:
			{
				std::vector<const void*>& orderList = m_insertion_order[&container];

				for (const void* elementAddr : orderList)
				{
					const auto* element = static_cast<const typename stl_container<Key, Value, etc...>::value_type*>(elementAddr);

					begin_object(childName);
					{
						begin_object("key");
							container_value(element->first);
						end_object_as_value(&element->first);

						begin_object("value");
							container_value(element->second);
						end_object_as_value(&element->second);
					}
					end_object_as_value(&element);

				}
				m_insertion_order.erase(&container);
			}
			break;
			case ArchiveType::Writer:
			{
				for (auto& element : container)
				{
					begin_object(childName);
					{
						begin_object("key");
							container_value(element.first);
						end_object_as_value(&element.first);

						begin_object("value");
							m_is_pointer = true;
							container_value(*element.second);
							m_is_pointer = false;
						end_object_as_value(element.second);
					}
					end_object_as_value(&element);
				}
			}
			break;
		}

		if (*name)
			end_object_as_value(&container);
	}

	template<class T, size_t Length>
	void Archive::value(const char* name, const char* childName, T*(&container)[Length])
	{
		if (*name)
			begin_object(name,true);

		size_t count = Length;

		if (m_type == ArchiveType::Reader || m_type == ArchiveType::Resolver)
		{
			count = get_children_name_count(childName) > Length ? Length : get_children_name_count(childName);
		}

		switch (m_type)
		{
			case ArchiveType::Reader:
			{
				for (size_t i = 0; i < count; i++)
				{
					begin_object(childName);
						T* object = create_polymorphic_object<T>();
						container_value(*object);
						container[i] = object;
					end_object_as_value(object);
				}
			}
			break;
			case ArchiveType::Resolver:
			case ArchiveType::Writer:
			{
				for (size_t i = 0; i < count; i++)
				{
					begin_object(childName);
						m_is_pointer = true;
						container_value(*container[i]);
						m_is_pointer = false;
					end_object_as_value(container[i]);
				}
			}
			break;
		}

		if (*name)
			end_object_as_value(&container);
	}

	template<class T>
	void Archive::refer(const char* name, T*& object)
	{
		if (m_type == ArchiveType::Resolver || m_type == ArchiveType::Writer)
		{
			refer_object(name, *reinterpret_cast<void**>(&object));
		}
	}

	template<template <typename ...> class stl_container, typename T, typename ... etc, std::enable_if_t<!trait_utils::iterates_with_pair<stl_container<T*, etc...>>::value>*>
	void Archive::refer(const char* name, const char* childName, stl_container<T*, etc...>& container)
	{
		if (*name)
			begin_object(name,true);

		switch (m_type)
		{
			case ArchiveType::Resolver: 
			{	
				size_t count = get_children_name_count(childName);
				try_reserve(container, count);

				for (size_t i = 0; i < count; i++)
				{
					begin_object(childName);
						T* pointer = nullptr;
						refer(address_kwd, pointer);
						container.insert(container.end(), pointer);
					end_object_as_reference();
				}
			}
			break;
			case ArchiveType::Writer: 
			{
				for (const T* e : container)
				{
					begin_object(childName);
						T* ptr = const_cast<T*>(e);
						refer(address_kwd, ptr);
					end_object_as_reference();

				}
			} 
			break;
			case ArchiveType::Reader: 
			break;
		}

		if (*name)
			end_object_as_value(&container);

	}

	template<template <typename ...> class stl_container, typename Key, typename Value, typename ... etc, std::enable_if_t<trait_utils::iterates_with_pair<stl_container<Key, Value*, etc...>>::value>*>
	void Archive::refer(const char* name, const char* childName, stl_container<Key, Value*, etc...>& container)
	{
		static_assert(!std::is_pointer<Key>::value, "pointers are not currently supported as key on a map");

		if (*name)
			begin_object(name,true);

		switch (m_type)
		{
			case ArchiveType::Resolver: 
			{
				size_t count = get_children_name_count(childName);
				for (size_t i = 0; i < count; i++)
				{
					begin_object(childName);
					{
						begin_object("key");
							Key&& key = Key();
							container_value(key);

							auto& addresses = *container.insert(std::make_pair(key, nullptr)).first;
						end_object_as_value(&addresses.first);

						refer("value", addresses.second);
					}
					end_object_as_value(nullptr);
				}
			}
			break;
			case ArchiveType::Writer: 
			{
				for (auto& e : container)
				{
					begin_object(childName);
					{
						begin_object("key");
							container_value(e.first);
						end_object_as_value(&e.first);

						refer("value", e.second);
					}
					end_object_as_value(&e);
				}
			}
			break;
			case ArchiveType::Reader:
			break;
		}

		if (*name)
			end_object_as_value(&container);
	}

	template<class T, size_t Length>
	void Archive::refer(const char* name, const char* childName, T*(& container)[Length])
	{
		if (*name)
			begin_object(name,true);

		size_t count = m_type == ArchiveType::Resolver 
			? (get_children_name_count(childName) > Length ? Length : get_children_name_count(childName)) 
			: Length;

		switch (m_type)
		{
			case ArchiveType::Resolver:
			case ArchiveType::Writer:
			{
				for (size_t i = 0; i < count; i++)
				{
					begin_object(childName);
						refer(address_kwd, container[i]);
					end_object_as_reference();
				}
			}
			break;
			case ArchiveType::Reader: 
			break;
		}

		if (*name)
			end_object_as_value(&container);
	}

	template<class T, std::enable_if_t<trait_utils::has_persist<T>::value>*>
	void Archive::container_value(const T& obj)
	{
		T& object = const_cast<T&>(obj);
		object.persist(this);
	}

	template<class T, std::enable_if_t<!trait_utils::has_persist<T>::value>*>
	void Archive::container_value(const T& obj)
	{
		T& object = const_cast<T&>(obj);
		value("value", object);
	}

}