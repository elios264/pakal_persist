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


#if  !defined(_MSC_VER) || _MSC_VER < 1900
#if __cplusplus <= 201103L

namespace std
{
	template<bool _Test,
	class _Ty = void>
		using enable_if_t = typename enable_if<_Test, _Ty>::type;
}
#endif
#endif

namespace Pakal
{
	class Archive;

	class IFactoryManager
	{
	public:
		virtual ~IFactoryManager() {}
		virtual void* create_object(const std::string& className) = 0;
	};

	template<class T>
	struct Persist
	{
		inline static void persist(Archive*, T&)
		{
			static_assert(false, "this method should've never been generated");
		}

		static constexpr bool is_implemented = false;
	};

	namespace trait_utils
	{
		template<typename T>
		struct has_static_persist
		{
			template <class C>
			static char(&f(typename std::enable_if<Persist<C>::is_implemented == false>::type*))[1];

			template<typename C>
			static char(&f(...))[2];

			static constexpr bool value = sizeof(f<T>(nullptr)) == 2;
		};

		template<typename T>
		struct has_member_persist
		{
			template <class C>
			static char(&f(typename std::enable_if<
				std::is_same<void, decltype(std::declval<C>().persist(nullptr))>::value, void>::type*))[1];

			template<typename C> 
			static char(&f(...))[2];

			static constexpr bool value = sizeof(f<T>(nullptr)) == 1;
		};

		template<typename T>
		struct has_persist
		{
			static constexpr bool value = has_member_persist<T>::value == true || has_static_persist<T>::value == true;
		};

		template<typename T>
		struct has_reserve
		{
			template <class C>
			static char(&f(typename std::enable_if<
				std::is_same<void, decltype(std::declval<C>().reserve(std::declval<size_t>()))>::value, void>::type*))[1];

			template<typename C> static char(&f(...))[2];

			static constexpr bool value = sizeof(f<T>(nullptr)) == 1;
		};

		template<typename T>
		struct iterates_with_pair
		{

			template <class C>
			static char(&f(typename std::enable_if<
				std::is_same<typename C::value_type, std::pair<const typename C::key_type, typename C::mapped_type>>::value, void>::type*))[1];

			template<typename C> static char(&f(...))[2];

			static constexpr bool value = sizeof(f<T>(nullptr)) == 1;
		};

		template<typename T>
		struct has_const_iterator
		{
		private:
			typedef char                      yes;
			typedef struct { char array[2]; } no;

			template<typename C> static yes test(typename C::const_iterator*);
			template<typename C> static no  test(...);
		public:
			static const bool value = sizeof(test<T>(0)) == sizeof(yes);
			typedef T type;
		};

		template <typename T>
		struct has_begin_end
		{
			template<typename C> static char(&f(typename std::enable_if<
				std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::begin)),
				typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

			template<typename C> static char(&f(...))[2];

			template<typename C> static char(&g(typename std::enable_if<
				std::is_same<decltype(static_cast<typename C::const_iterator(C::*)() const>(&C::end)),
				typename C::const_iterator(C::*)() const>::value, void>::type*))[1];

			template<typename C> static char(&g(...))[2];

			static bool const beg_value = sizeof(f<T>(0)) == 1;
			static bool const end_value = sizeof(g<T>(0)) == 1;
		};

		template<typename T>
		struct is_container : std::integral_constant < bool, has_const_iterator<T>::value && has_begin_end<T>::beg_value && has_begin_end<T>::end_value && !std::is_same<T, std::string>::value >
		{ };

		template<typename T>
		struct to_void
		{
			typedef void type;
		};

		template <typename Container, typename dummy = void>
		struct is_associative_container : std::false_type {};

		template <typename Container>
		struct is_associative_container
			<Container, typename to_void<typename Container::key_type>::type > : std::true_type {};

	}
}