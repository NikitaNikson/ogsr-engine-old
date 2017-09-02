#pragma once

#include <luabind/object.hpp>
#include <luabind/function.hpp>
#include <luabind/detail/policy.hpp>

namespace luabind
{
	template <class result_type>
	class functor
	{
	public:
		functor()
		{
		}

		explicit functor(const luabind::object& func) : m_func(func)
		{
		}

		explicit operator bool() const
		{
			return is_valid();
		}

		bool operator==(const functor<result_type> & other) const
		{
			return m_func == other.m_func;
		}

		template<class... Args>
		result_type operator()(Args&&... args) const
		{
			return (result_type)luabind::call_function<result_type>(m_func, std::forward<Args>(args)...);
		}

		bool is_valid() const
		{
			return m_func.is_valid();
		}

		lua_State* interpreter() const
		{
			return m_func.interpreter();
		}

		void push(lua_State* L) const
		{
			m_func.push(L);
		}

	private:
		object m_func;
	};

	template <class result_type>
	struct default_converter<functor<result_type>>
		: native_converter_base<functor<result_type>>
	{
		static int compute_score(lua_State* L, int index)
		{
			auto type = lua_type(L, index);
			return (type == LUA_TFUNCTION  || type == LUA_TNIL) ? 0 : -1;
		}

		functor<result_type> from(lua_State* L, int index)
		{
			if (lua_type(L, index) == LUA_TNIL)
				return functor<result_type>();
			return functor<result_type>(object(from_stack(L, index)));
		}

		void to(lua_State* L, functor<result_type> const& x)
		{
			x.push(L);
		}
	};

	template <class result_type>
	struct default_converter<functor<result_type> const&>
		: default_converter<functor<result_type>>
	{};

	template<>
	struct default_converter<unsigned int const&>
		: default_converter<unsigned int>
	{};

	template<>
	struct default_converter<unsigned short const&>
		: default_converter<unsigned short>
	{};
}