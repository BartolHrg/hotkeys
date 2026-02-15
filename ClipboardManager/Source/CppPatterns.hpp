#pragma once

//	Self.hpp
#define self (*this)



//	Defer.hpp
#include <type_traits>

#ifndef fun
#define fun
#endif

//	defers execution of a code block.
//	It can use variables declared before it.
//	defer {/* code */};
#define defer _DeferVar const _defer_macro1(__COUNTER__) = [&] noexcept
#define _defer_macro1(cnt) _defer_macro2(cnt)
#define _defer_macro2(cnt) tmp_defer_var_ ## cnt
template <typename F> requires (std :: is_nothrow_invocable_v<F>)
struct _DeferVar {
	F f;
	fun inline _DeferVar(F f) : f(f) {}
	fun inline ~_DeferVar() {
		f();
	}
};








//	e.g.
//	bool f() {
//		FILE* f = fopen(...);
//		if (!f) { return false; }
//		defer { fclose(f); };
//		//	file operations
//		int a;
//		return fscanf(f, "%d", &a) == 1;
//	}




//	void h() noexcept {
//		std :: cout << "from h with love" << std :: endl;
//	}
//	
//	// void m() {
//	auto& m() {
//		defer { h(); };
//		defer { //	this is preceeded by noexcept by-reference lambda: [&] noexcept 
//			std :: cout << "lambda" << std :: endl;
//		};
//		return std :: cout << "hello" << std :: endl;
//	}
//	int main() {
//		m();
//	}
//	//	hello
//	//	lambda
//	//	from h with love















//	Resource.hpp

#include <concepts>

#ifndef fun
#define fun
#endif




template <typename>
struct FunctionTraits;

template <typename T, typename... Args>
struct FunctionTraits<T(Args...)> {
	using ReturnType = T;
};
template <typename T, typename... Args>
struct FunctionTraits<T (*)(Args...)> {
	using ReturnType = T;
};
template <typename T, typename C, typename... Args>
struct FunctionTraits<T (C::*)(Args...)> {
	using ReturnType = T;
};
template <typename F>
using ReturnType = typename FunctionTraits<F> :: ReturnType;

template <typename T>
concept Resourceable = requires(T t) {
	requires std :: is_member_function_pointer_v<decltype(&T :: init)>;
	requires std :: convertible_to<ReturnType<decltype(&T :: init)>, bool>;
	{ t.finish() } noexcept;
};
template <typename T, typename... Args>
concept ActuallyResourceable = requires(T t, Args... args) {
	{ t.init(args...) } -> std :: convertible_to<bool>;
	{ t.finish() } noexcept;
};
template <typename T> requires (Resourceable<T>)
struct Resource : T {
	bool _resource_ok = true;
	template <typename... Args> requires (ActuallyResourceable<T, Args...>)
	fun explicit inline Resource(T t, Args&&... args) : T(t) {
		self._resource_ok = self.init(args...);
	}
	template <typename... Args> requires (ActuallyResourceable<T, Args...>)
	fun explicit inline Resource(Args&&... args) {
		self._resource_ok = self.init(args...);
	}
	fun inline ~Resource() {
		self.finish();
	}
	// Resource(Resource<T>) = delete;
	fun Resource(Resource<T>&) = delete;
	fun Resource(Resource<T>&&) = delete;
	fun operator bool() { return self._resource_ok; }
};


template <typename T> struct  IsResourceT              : std :: false_type {};
template <typename T> struct  IsResourceT<Resource<T>> : std ::  true_type {};
template <typename T> concept IsResource = IsResourceT<T>::value;
template <typename T> concept IsNotResource = !IsResource<T>;

//	RAII_init(var, Type) - bypass RAII 
//	you should call `new (&var) Type(args)` in init (this is called placement new)
//	and `var.~Type()` in finish
//	you will have to define destructor for enclosing class
#define RAII_init(var, ...) union { char _resource_ ## var = 0; __VA_ARGS__ (var); }
//	#define RAII_init(var, ...) union { char _resource_ ## var[sizeof(__VA_ARGS__)] {0?}; __VA_ARGS__ (var); }



// struct ResourceCollectionWatcher {
	
// };




//	struct that uses RAII
//	struct RAII_Struct {
//		RAII_Struct() { /* take resource */ }
//		~RAII_Struct() { /* release resource */ }
//	};
//	
//	struct A {
//		RAII_init(raii, RAII_Struct); //	bypass RAII
//		A(int x) {}
//		~A() {} //	you have to define destructor if using RAII_init
//		void init(float y) { new (&self.raii) RAII_Struct; }
//		void finish() noexcept { self.raii.~RAII_Struct(); }
//		void use() {}
//	};
//	int main() {
//		Resource<A> a({4}, 1.5f);
//		a.use();
//		/* you can even do */ /* but that is only needed if A has member ok that you would want to use */
//		A b = a;
//		b.use();
//	}









//	Result.hpp

template <typename T>
struct Result {
	T data;
	bool ok;
	
	constexpr Result(T data) : data(data), ok(true) {}
	constexpr static Result MY_NO_DATA{T(), false};
	
	protected: constexpr Result(T data, bool ok) : data(data), ok(ok) {}
};
