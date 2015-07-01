#pragma once

#include <functional>
#include <type_traits>

// SFINAE Enable If macro for parameter lists
// usage:
// template <typename T>
// void Function(T value
//                           SFINAE_ENABLE_IF(std::is_integral<T>::value));
#define SFINAE_ENABLE_IF(...)  , typename  std::enable_if<__VA_ARGS__>::type* = nullptr

namespace tag
{

template <typename T>
class Distribution;

namespace detail
{
    // Functor that returns always the same value (don't use lambda exprission because of Clang compiler bug)
	template <typename T>
	struct Constant
	{
	    explicit Constant(T value)
		 : value(value)
		{}
		
		T operator() () const
		{
		    return value;
		}
		
		T value;
	};
	
	// Metafunction for SFINAE and reasonable compiler errors
	template <typename Fn, typename T>
	struct IsCompatibleFunction
	{
	    // General case: Fn is a functor/function -> if it's not convertible to T (and this not a constnt), accept it
		static const bool value = !std::is_convertible<Fn, T>::value;
	};
	
	template <typename U, typename T>
	struct IsCompatibleFunction<Distribution<U>, T>
	{
	    // If Fn is another Distribution<U>, accept it if U is convertible to T (like all functions, but clearer error message)
		static const bool value = std::is_convertible<U, T>::value;
	};
} // detail

// Class holding a rule to create values with predefined properties
// Contains a callback that returns values on demand. These can be constant (always the same value), according to a
//  random distribution, or be read from a value elsewhere in your code. Generally, the callback can be any function, member
//  function or functor returning a value of type T and taking no arguments.
// tag::Distribution can be linked to random distributions of the standard library:
// eg:
// std::mt19937 engine;
// std::uniform_int_distribution<int> distr(0, 7);
// auto randomizer = std::bind(distr, engine);
//
// tag::Distribution<int> tagDistr(randomizer);
template <typename T>
class Distribution
{
private:
    typedef std::function<T()> FactoryFn;
	
public:
    // Construct from constant
	// constant: Constant value convertible to T. The distribution's operator() will always return this value
	template <typename U>
	Distribution(U constant
	             SFINAE_ENABLE_IF(std::is_convertible<U, T>::value))
     : mFactory(detail::Constant<T>(constant))
	{}
	
	// Construct from distribution function
	// function: Callable convertible to std::function<T()>. Every time the operator() of this distribution is invoked,
    // it return the return value of the specified function.
    template <typename Fn>
    Distribution(Fn function
                 SFINAE_ENABLE_IF(detail::IsCompatibleFunction<Fn, T>::value))
    : mFactory(function)
    {
    }
	
	// Returns a value according to the distribution
	T operator() () const
	{
	    return mFactory();
	}
	
private:
    FactoryFn mFactory;
};

}