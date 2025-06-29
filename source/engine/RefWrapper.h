#pragma once
/*
* For storing game objects in GOF as references, and 
for storing components in game objects as references
In theory the get() function call shouldn't be much of a overhead
*/
#include <sol/sol.hpp>

template <typename T>
class Ref {
public:
	Ref(T& _ref) : ref(_ref) {}
	T& get() const { return ref; }

	// Define operator==
	bool operator==(const Ref& other) const {
		return &ref == &other.ref;
	}

	// Copy assignment operator
	Ref& operator=(const Ref& other) {
		if (&ref != &other.ref) {
			ref = other.ref;
		}
		return *this;
	}

	Ref& operator=(const T& other) {
		ref = other;
		return *this;
	}
	operator T&() const{ return ref; }

	static void CreateRefWrapperLuaBindings(std::shared_ptr<sol::state> lua) {
		lua->new_usertype<Ref<T>>("Ref",
			sol::constructors<Ref<T>(T&)>()
		);
	}
private:
	T& ref;
};