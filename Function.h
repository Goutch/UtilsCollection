#pragma once

#include "functional"

namespace HBE {


	template<typename Return, typename... Args>
	class FunctionRef {
		std::function<Return(Args...)> f;
	public:
		FunctionRef(std::function<Return(Args...)> f) : f(f) {}

		FunctionRef(Return (*f)(Args...)) : f(f) {}

		template<typename Object>
		FunctionRef(Object *instance, std::function<Return(Args...)> f):f(Attach<Object>(f, instance)) {}

		template<typename Object>
		FunctionRef(Object *instance, Return(Object::* f)(Args...)):f(Attach<Object>(f, instance)) {}

		template<typename Object>
		FunctionRef(Object &instance, std::function<Return(Args...)> f):f(Attach<Object>(f, &instance)) {}

		template<typename Object>
		FunctionRef(Object &instance, Return(Object::* f)(Args...)):f(Attach<Object>(f, &instance)) {}


		Return operator()(Args... args) const {
			return f(args...);
		}
	private :
		template<typename Object>
		std::function<Return(Args...)> Attach(Return (Object::*function)(Args...), Object *instance) {
			return [instance, function](Args... args) -> Return { return (instance->*function)(args...); };
		};
	};

}