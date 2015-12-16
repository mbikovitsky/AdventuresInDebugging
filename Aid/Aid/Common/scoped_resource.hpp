
//         Copyright Andrew L. Sandoval 2012 - 2013.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//
// scoped_resource - Reference Implementation
//
////////////////////////////////////////////////////////////////////////////////
// Andrew L. Sandoval (thanks to others on C++ std-proposals list)
// In particular, thanks to comments made in the C++ std-proposal list for the
// idea of using an unspecified type (T) instead of std::function<T ()>, and
// also for the idea of using the make_scoped_function and make_scoped_resource
// generator functions.
//
// What follows are three classes: scoped_function, scoped_resource_unchecked, and
// scoped_resource each have a corresponding make_scoped_function() or
// make_scoped_resource() template function to handle type deduction.
//
// scoped_function simply executes a function/functor/lambda on scope-exit.
// It is "generated" with make_scoped_function(function/functor/lambda);
//
// scoped_resource_unchecked manages a resource, by calling a deleter function,
// functor or lambda passing the resource as the sole argument, on scope-exit.
// It does NO check for validity before invoking the deleter function.
// It is generated with make_scoped_resource(resource, function/functor/lambda);
// or make_scoped_resource_unchecked(resource, function/functor/lambda), where
// the function/functor/lambda takes a single argument of the type of the
// resource.
//
// scoped_resource also manages a resource by calling a deleter function,
// functor, or lambda passing the resource as the sole argument, on scope-exit.
// It DOES provide validity checking before invoking the deleter function.
// It is generated with make_scoped_resource(resource, function/functor/lambda,
// nodelete); where if (resource == nodelete), the deleter function is not
// invoked on scope-exit.
// make_scoped_resource<bool>(resource, function/functor/lambda) causes the
// scoped_resource to throw failed_resource_initialization if the value
// passed to the constructor equals the nodelete value.  Be sure NOT
// to use this with the default constructor!
// The default constructor initializes the resource as the nodelete
// value so that later assignment with a different resource value
// can be done.
//
// The final type, unique_resource is available through an
// intermediary type unique_resource_impl.  This type does not have a 
// function template generator.  Instead, it uses static non-type template
// arguments for construction.  This substantially reduces the size of
// the generated code, making it roughly the equivalent of adding
// if(resource != nodelete) { deleter(resource); } at a common exit
// point for the scope.
// In order to simplify specification of template parameters, a very
// simple macro called UNIQUE_DELETER is provided to allow the first
// three template parameters -- the deleter function type, and the 
// deleter function itself (non-type template argument) as well as
// false to the throw_on_init_failure parameter, to be
// compressed down to a more readable set of arguments, that draw
// attention to the deleter function itself.
// UNIQUE_DELETER_THROW can also be used to pass true to the
// throw_on_init_failure template parameter.
//
// Additionally, some template meta-programming is used to determine
// the type of the resource based on the type of the sole parameter
// to the deleter function.
//
// unique_resource is meant to be used only with static deleter
// functions in which they argument to the function can be determined
// using TFuncInfo1.  It is particularly well suited to resources that
// have a deleter function taking the resource as an argument, such as
// int ifd = open(); matched to close(ifd);
//
//
#include <utility>		// std::move
#ifdef _KERNEL_MODE
namespace std
{
	class exception			// Kernel-mode code will not have this -- provided to allow compilation, but usage may be problematic
	{
		virtual const char *what() const throw()
		{
			return "Exception";
		}
	};
}
#else
#include <exception>
#endif

namespace std
{
	////////////////////////////////////////////////////////////////////////////
	// failed_resource_initialization
	// An exception thrown when a no-delete value is passed to a scoped_resource
	// or unique_resource constructor, assignment, or reset IF the template
	// arguments allow for throwing.  Throwing is optional so that these classes
	// may be used in environment where exception can not be handled (e.g.
	// kernel drivers, etc.)  In those cases the resource should be validated
	// using .valid() prior to use.
	////////////////////////////////////////////////////////////////////////////
	class failed_resource_initialization : public exception
	{
	public:
		explicit failed_resource_initialization(const char * = nullptr) throw()
		{
		}

		virtual const char * what() const throw()
		{
			return "resource initialization failed";
		}
	};
	
	////////////////////////////////////////////////////////////////////////////
	// scoped_function:  For use with lambdas, etc.  Does not provide track a
	// resource but executes m_func on scope exit
	////////////////////////////////////////////////////////////////////////////
	template<typename T>
	class scoped_function
	{
	private:
		T m_func;
		bool m_bExecute;

	public:
		explicit scoped_function(T&& arg) throw() : m_func(std::move(arg)), m_bExecute(true)
		{
		}

		explicit scoped_function(const T& arg) throw() : m_func(arg), m_bExecute(true)
		{
		}

		~scoped_function()
		{
			if(m_bExecute)
			{
				m_func();
			}
		}

		//
		// release - Don't call on scope exit!
		scoped_function& release() throw()
		{
			m_bExecute = false;
			return *this;
		}

		//
		// reset
		scoped_function& reset(T&& arg)
		{
			if(m_bExecute)
			{
				m_func();
			}
			m_bExecute = true;
			m_func = std::move(arg);
			return *this;
		}

		scoped_function& reset(const T& arg)
		{
			if(m_bExecute)
			{
				m_func();
			}
			m_bExecute = true;
			m_func = arg;
			return *this;
		}

		void operator=(T&& arg)
		{
			reset(arg);
		}

		void operator=(const T& arg)
		{
			reset(arg);
		}

		//
		// functor - run the clean-up early and release
		void operator()()
		{
			invoke(true);
		}

		//
		// invoke...(e.g. early invocation, invoke regardless of m_bExecute)
		scoped_function& invoke(bool bRelease = true)
		{
			m_func();
			m_bExecute = !bRelease;
			return *this;
		}
	};

	//
	// Make a scoped_function
	// Takes 1 parameter - the function to be run on scope exit...
	template<typename T>
	scoped_function<T> make_scoped_function(T t) throw()
	{
		return scoped_function<T>(std::move(t));
	}

	////////////////////////////////////////////////////////////////////////////
	// scoped_resource_unchecked:
	// Tracks a resource and it's deleter.  Deleter is invoked on scope exit.
	// Provides cast operator and getter for resource.  Does not check for a
	// no-delete value.
	////////////////////////////////////////////////////////////////////////////
	template<typename T, typename R>
	class scoped_resource_unchecked
	{
	private:
		T m_deleter;
		R m_resource;
		bool m_bExecute;

	public:
		explicit scoped_resource_unchecked(T&& arg, R&& resource) throw() :
			m_deleter(std::move(arg)), m_resource(std::move(resource)), m_bExecute(true)
		{
		}

		explicit scoped_resource_unchecked(const T& arg, const R& resource) throw() :
			m_deleter(arg), m_resource(resource), m_bExecute(true)
		{
		}

		~scoped_resource_unchecked()
		{
			if(m_bExecute)
			{
				m_deleter(m_resource);
			}
		}

		//
		// release
		R release() throw()
		{
			m_bExecute = false;
			return m_resource;
		}

		//
		// reset -- only resets the resource, not the deleter!
		scoped_resource_unchecked& reset(R&& resource)
		{
			if(m_bExecute)
			{
				m_deleter(m_resource);
			}
			m_bExecute = true;
			m_resource = std::move(resource);
			return *this;
		}

		scoped_resource_unchecked& reset(const R& resource)
		{
			if(m_bExecute)
			{
				m_deleter(m_resource);
			}
			m_bExecute = true;
			m_resource = resource;
			return *this;
		}

		//
		// functor - run the clean-up early and release
		void operator()()
		{
			invoke(true);
		}

		//
		// get - for cases where cast operator is undesirable (e.g. as a ... parameter)
		R get() const throw()
		{
			return m_resource;
		}

		//
		// cast operator
		operator R() const throw()
		{
			return m_resource;
		}

		//
		// operator-> for accessing members on pointer types
		R operator->() const
		{
			return m_resource;
		}

		//
		// operator* for dereferencing pointer types
		// typename std::add_reference<typename std::remove_pointer<R>::type>::type operator* () const
		// {
		// 	return *m_resource;		// If applicable
		// }

		//
		// operator& for getting the address of the resource - not const!
		R* operator&()
		{
			return &m_resource;
		}

		//
		// Access the deleter (non const)...
		T& get_deleter()
		{
			return m_deleter;
		}

		//
		// Access the deleter, when const ...
		const T& get_deleter() const
		{
			return m_deleter;
		}

		//
		// invoke the deleter...(e.g. early invocation), if allowed
		scoped_resource_unchecked& invoke(bool bRelease = true)
		{
			if(m_bExecute)
			{
				m_deleter(m_resource);
			}
			m_bExecute = !bRelease;
			return *this;
		}
	};

	//
	// Make a scoped_resource_unchecked (unchecked)
	// Takes 2 arguments: The resource and the deleter
	template<typename T, typename R>
	scoped_resource_unchecked<T, R> make_scoped_resource_unchecked(R r, T t) throw()		// Specific request
	{
		return scoped_resource_unchecked<T, R>(std::move(t), std::move(r));
	}

	template<typename T, typename R>
	scoped_resource_unchecked<T, R> make_scoped_resource(R r, T t) throw()					// Generic request
	{
		return scoped_resource_unchecked<T, R>(std::move(t), std::move(r));
	}

	////////////////////////////////////////////////////////////////////////////
	// scoped_resource
	// Tracks a resource, it's deleter, and a no-delete value.
	// throws failed_resource_initialization if the no-delete value is used
	// on initialization, reset, or assignment, if the 3rd template parameter is
	// true.
	// Deleter is invoked on scope exit, if the resource != the no-delete value
	// Provides cast operator and getter for resource.
	////////////////////////////////////////////////////////////////////////////
	template<typename T, typename R, const bool throw_on_init_failure = false>
	class scoped_resource
	{
	private:
		T m_deleter;
		R m_resource;
		R m_noDeleteValue;

	public:
		explicit scoped_resource(T&& arg, R&& resource, R&& nodelete) :
			m_deleter(std::move(arg)), m_resource(std::move(resource)), m_noDeleteValue(std::move(nodelete))
		{
			if(throw_on_init_failure && m_resource == m_noDeleteValue)
			{
				throw failed_resource_initialization();
			}
		}

		explicit scoped_resource(const T& arg, const R& resource, const R& nodelete) :
			m_deleter(arg), m_resource(resource), m_noDeleteValue(std::move(nodelete))
		{
			if(throw_on_init_failure && m_resource == m_noDeleteValue)
			{
				throw failed_resource_initialization();
			}
		}

		~scoped_resource()
		{
			invoke(true);
		}

		//
		// release
		R release() throw()
		{
			R tempCopy = m_resource;
			m_resource = m_noDeleteValue;
			return tempCopy;
		}

		//
		// reset - replace the resource, fire (if needed)
		// the deleter on the old resource
		scoped_resource& reset(R&& resource)
		{
			if(m_resource != m_noDeleteValue)
			{
				m_deleter(m_resource);
			}
			m_resource = std::move(resource);
			if(throw_on_init_failure && m_resource == m_noDeleteValue)
			{
				throw failed_resource_initialization();
			}
			return *this;
		}

		scoped_resource& reset(R const& resource)
		{
			if(m_resource != m_noDeleteValue)
			{
				m_deleter(m_resource);
			}
			m_resource = resource;
			if(throw_on_init_failure && m_resource == m_noDeleteValue)
			{
				throw failed_resource_initialization();
			}
			return *this;
		}

		//
		// functor - release resource immediately
		void operator()()
		{
			invoke(true);
		}

		//
		// getter for access to resource
		R get() const throw()
		{
			return m_resource;
		}

		//
		// Cast operator - for access to resource
		// WARNING: This can cause make_scoped_resource*(R, T) to return a temporary that initializes something else!  Be careful with usage!
		operator R() const throw()
		{
			return m_resource;
		}

		T& get_deleter()
		{
			return m_deleter;
		}

		const T& get_deleter() const
		{
			return m_deleter;
		}

		//
		// operator-> for accessing members on pointer types
		R operator->() const
		{
			return m_resource;
		}

		//
		// operator* for dereferencing pointer types
		// typename std::add_reference<typename std::remove_pointer<R>::type>::type operator* () const
		// {
		// 	return *m_resource;		// If applicable
		// }

		//
		// operator& for getting the address of the resource - not const!
		R* operator&()
		{
			return &m_resource;
		}

		//
		// invoke...(e.g. early invocation), allows for repeated
		// invocation, if bRelease = false.  Use with caution!
		scoped_resource& invoke(bool bRelease = true)
		{
			if(valid())
			{
				m_deleter(m_resource);
				if(bRelease)
				{
					m_resource = m_noDeleteValue;
				}
			}
			return *this;
		}

		//
		// Check for a valid / usable resource
		bool __inline valid() const throw()
		{
			return m_resource != m_noDeleteValue;
		}
	};

	//
	// Makes a scoped_resource <true/false> overrides default of false for throw on init failure
	// Takes 3 parameters: The Resource, the Deleter, and a don't-delete value (nodelete)
	template<typename T, typename R>
	scoped_resource<T, R, false> make_scoped_resource(R r, T t, R nd)
	{
		return scoped_resource<T, R, false>(std::move(t), std::move(r), std::move(nd));
	}

	template<bool t_throw_on_init_failure, typename T, typename R>
	scoped_resource<T, R, t_throw_on_init_failure> make_scoped_resource(R r, T t, R nd)
	{
		return scoped_resource<T, R, t_throw_on_init_failure>(std::move(t), std::move(r), std::move(nd));
	}

	////////////////////////////////////////////////////////////////////////////
	// unique_resource_impl
	// Tracks a resource, it's deleter, and a no-delete value.
	// If the throw_on_init_failure parameter is true and the no-delete value
	// is passed to the constructor, the reset method, or the copy operator
	// it will throw failed_resource_initialization.
	// Deleter is invoked on scope exit, if the resource is != no-delete value
	// Provides cast operator and getter for resource.
	////////////////////////////////////////////////////////////////////////////
	template<typename TResource, typename TDeleter, TDeleter t_deleter, bool throw_on_init_failure, TResource t_nodelete>
	class unique_resource_impl
	{
	private:
		TResource m_resource;

		unique_resource_impl(const unique_resource_impl &);
		void operator=(const unique_resource_impl &);

	public:
		unique_resource_impl() : m_resource(t_nodelete)	// Allows for operator=(TResource) later...
		{
		}

		explicit unique_resource_impl(TResource&& resource) : m_resource(std::move(resource))
		{
			if(throw_on_init_failure && t_nodelete == m_resource)
			{
				throw failed_resource_initialization();
			}
		}

		explicit unique_resource_impl(const TResource& resource) : m_resource(resource)
		{
			if(throw_on_init_failure && t_nodelete == m_resource)
			{
				throw failed_resource_initialization();
			}
		}

		~unique_resource_impl()
		{
			invoke(true);
		}

		//
		// release
		TResource release() throw()
		{
			TResource tempCopy = m_resource;
			m_resource = t_nodelete;
			return tempCopy;
		}

		//
		// reset - replace the resource, fire (if needed)
		// the deleter on the old resource
		unique_resource_impl& reset(TResource&& resource)
		{
			invoke(false);		// false because we are going to change it manually here...
			m_resource = std::move(resource);
			if(throw_on_init_failure && t_nodelete == m_resource)
			{
				throw failed_resource_initialization();
			}
			return *this;
		}

		unique_resource_impl& reset(TResource const& resource)
		{
			invoke(false);		// false because we are going to change it manually here...
			m_resource = resource;
			if(throw_on_init_failure && t_nodelete == m_resource)
			{
				throw failed_resource_initialization();
			}
			return *this;
		}

		//
		// functor - release resource immediately
		void operator()()
		{
			invoke(true);
		}

		//
		// getter for access to resource
		TResource get() const throw()
		{
			return m_resource;
		}

		void operator=(TResource &&res)
		{
			reset(res);
		}

		void operator=(TResource const &res)
		{
			reset(res);
		}

		//
		// Cast operator - for access to resource
		// WARNING: This can cause make_scoped_resource*(R, T) to return a temporary that initializes something else!  Be careful with usage!
		// Nevertheless, it provides transparency for API calls, etc.
		operator TResource() const throw()
		{
			return m_resource;
		}

		//
		// operator-> for accessing members on pointer types
		TResource operator->() const
		{
			return m_resource;
		}

		//
		// operator* for dereferencing pointer types
		// typename std::add_reference<typename std::remove_pointer<TResource>::type>::type operator* () const
		// {
		// 	return *m_resource;		// If applicable
		// }

		//
		// operator& for getting the address of the resource - not const!
		TResource* operator&()
		{
			return &m_resource;
		}

		const TDeleter get_deleter() const
		{
			return t_deleter;
		}

		//
		// invoke...(e.g. early invocation), allows for repeated
		// invocation, if bRelease = false.  Use with caution!
		unique_resource_impl& invoke(bool bRelease = true)
		{
			if(valid())
			{
				t_deleter(m_resource);
				if(bRelease)
				{
					m_resource = t_nodelete;
				}
			}
			return *this;
		}

		//
		// Check for a valid / usable resource
		bool __inline valid() const throw()
		{
			return m_resource != t_nodelete;
		}
	};

	//
	// Extract First Parameter Type
	//
	template <typename T>
	struct TFuncInfo1;

	#if defined(_WIN32) && !defined(_WIN64)		// Support Microsoft calling conventions
	template <typename TReturn, typename TParam>
	struct TFuncInfo1<TReturn (__stdcall *)(TParam)>
	{
		typedef TReturn tReturnType;
		typedef TParam tParameter1;
	};

	template <typename TReturn, typename TParam>
	struct TFuncInfo1<TReturn (__cdecl *)(TParam)>
	{
		typedef TReturn tReturnType;
		typedef TParam tParameter1;
	};

	template <typename TReturn, typename TParam>
	struct TFuncInfo1<TReturn (__fastcall *)(TParam)>
	{
		typedef TReturn tReturnType;
		typedef TParam tParameter1;
	};
	#else	// All other compilers
	template <typename TReturn, typename TParam>
	struct TFuncInfo1<TReturn (*)(TParam)>
	{
		typedef TReturn tReturnType;
		typedef TParam tParameter1;
	};
	#endif	// _WIN32 && !_WIN64

	//
	// Simplify for constexpr template arguments:
	////////////////////////////////////////////////////////////////////////////
	// unique_resource
	// Tracks a resource, it's deleter, and a no-delete value.
	// If the throw_on_init_failure parameter is true and the no-delete value
	// is passed to the constructor, the reset method, or the copy operator
	// it will throw failed_resource_initialization.
	// Deleter is invoked on scope exit, if the resource is != no-delete value
	// Provides cast operator and getter for resource.
	// Limits arguments by deduction from 1st parameter of deleter function
	////////////////////////////////////////////////////////////////////////////
	template<typename TDeleter,
		TDeleter t_deleter,
		bool throw_on_init_failure = false,		// Set to true if you want throw(failed_resource_initialization) when m_resource == t_nodelete_value
		typename TFuncInfo1<TDeleter>::tParameter1 t_nodelete_value = static_cast<typename TFuncInfo1<TDeleter>::tParameter1>(nullptr)>
	struct unique_resource : public
		unique_resource_impl<typename TFuncInfo1<TDeleter>::tParameter1, TDeleter, t_deleter, throw_on_init_failure, t_nodelete_value> 
	{
		typedef typename TFuncInfo1<TDeleter>::tParameter1 TResource;

		unique_resource() : 
			unique_resource_impl<typename TFuncInfo1<TDeleter>::tParameter1, TDeleter, t_deleter, throw_on_init_failure, t_nodelete_value>()
		{
		}

		unique_resource(TResource &&resource) : 
			unique_resource_impl<typename TFuncInfo1<TDeleter>::tParameter1, TDeleter, t_deleter, throw_on_init_failure, t_nodelete_value>(std::move(resource))
		{
		}

		unique_resource(TResource const &resource) : 
			unique_resource_impl<typename TFuncInfo1<TDeleter>::tParameter1, TDeleter, t_deleter, throw_on_init_failure, t_nodelete_value>(resource)
		{
		}

		void operator=(TResource &&resource)
		{
			this->reset(resource);
		}

		void operator=(TResource const &resource)
		{
			this->reset(resource);
		}
	};
}

//
// The following macros simplify template parameters for unique_resource
#define UNIQUE_DELETER(deleter_fn) decltype(&deleter_fn), deleter_fn, false
#define UNIQUE_DELETER_THROW(deleter_fn) decltype(&deleter_fn), deleter_fn, true
#define UNIQUE_DELETER_NOTHROW(deleter_fn) decltype(&deleter_fn), deleter_fn, false
