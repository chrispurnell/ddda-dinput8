#pragma once
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if defined(__GNUC__)
namespace std
{
template<class T> class initializer_list
{
private:
	const T* arr;
	size_t len;

	constexpr initializer_list(const T* a, size_t l) : arr(a), len(l) { }
public:
	constexpr initializer_list() : arr(nullptr), len(0) { }

	size_t size() const { return len; }

	const T* begin() const { return arr; }
	const T* end() const { return arr + len; }
};
}
#else
#include <initializer_list>
#endif

inline void *operator new(size_t siz) { return malloc(siz); }
inline void *operator new[](size_t siz) { return malloc(siz); }

inline void operator delete(void *ptr) { free(ptr); }
inline void operator delete[](void *ptr) { free(ptr); }

inline void *operator new(size_t, void *ptr) { return ptr; }
inline void *operator new[](size_t, void *ptr) { return ptr; }

template<class T> class vector
{
private:
	T *arr;
	size_t len;
	size_t cap;

	static T *allocate(size_t n)
	{
		return static_cast<T*>(operator new(n * sizeof(T)));
	}

	void deallocate()
	{
		if (arr) operator delete(arr);
	}

	void destruct()
	{
		T *a = arr;
		for (size_t i = 0; i < len; ++i)
			a[i].~T();
	}

	void reallocate(size_t n)
	{
		T *a = allocate(n);
		T *b = arr;

		for (size_t i = 0; i < len; ++i)
		{
			new(a + i) T(static_cast<T&&>(b[i]));
			b[i].~T();
		}

		deallocate();
		arr = a;
		cap = n;
	}

	void move(vector &v)
	{
		arr = v.arr;
		len = v.len;
		cap = v.cap;

		v.arr = nullptr;
		v.len = 0;
		v.cap = 0;
	}

public:
	constexpr vector() : arr(nullptr), len(0), cap(0) { }

	vector(const vector &v)
	{
		size_t n = v.size();
		T *a = nullptr;

		if (n)
		{
			a = allocate(n);
			for (size_t i = 0; i < n; ++i)
				new (a + i) T(v[i]);
		}

		arr = a;
		len = n;
		cap = n;
	}

	vector(vector &&v)
	{
		move(v);
	}

	vector(const T *first, const T *last)
	{
		size_t n = last - first;
		T *a = nullptr;

		if (n)
		{
			a = allocate(n);
			for (size_t i = 0; i < n; ++i)
				new (a + i) T(*(first++));
		}

		arr = a;
		len = n;
		cap = n;
	}

	vector(std::initializer_list<T> il)
	{
		size_t n = il.size();
		T *a = nullptr;

		if (n)
		{
			a = allocate(n);

			auto p = il.begin();
			for (size_t i = 0; i < n; ++i)
				new (a + i) T(*(p++));
		}

		arr = a;
		len = n;
		cap = n;
	}

	~vector()
	{
		destruct();
		deallocate();
	}

	vector& operator=(const vector &v)
	{
		destruct();

		size_t n = v->size();

		if (n > cap)
		{
			deallocate(n);
			arr = allocate(n);
			cap = n;
		}

		for (size_t i = 0; i < n; ++i)
			new (arr + i) T(v[i]);
		len = n;
	}

	vector& operator=(vector &&v)
	{
		destruct();
		deallocate();
		move(v);
		return *this;
	}

	size_t size() const { return len; }
	size_t capacity() const { return cap; }
	bool empty() const { return len == 0; };

	T* data() { return arr; }
	const T* data() const { return arr; }

	T& operator[](size_t i) { return arr[i]; }
	const T& operator[](size_t i) const { return arr[i]; }

	T& at(size_t i) { return arr[i]; }
	const T& at(size_t i) const { return arr[i]; }

	T& front() { return arr[0]; }
	const T& front() const { return arr[0]; }

	T& back() { return arr[len - 1]; }
	const T& back() const { return arr[len - 1]; }

	T* begin() { return arr; };
	const T* begin() const { return arr; };

	T* end() { return arr + len; }
	const T* end() const { return arr + len; }

	void reserve(size_t n)
	{
		if (cap < n)
			reallocate(n);
	}

	void push_back(const T &v)
	{
		if (cap <= len)
			reallocate(cap == 0 ? 1 : cap * 2);
		new(arr + len) T(v);
		++len;
	}

	void push_back(T &&v)
	{
		if (cap <= len)
			reallocate(cap == 0 ? 1 : cap * 2);
		new(arr + len) T(static_cast<T&&>(v));
		++len;
	}

	template <class... P> void emplace_back(P... a)
	{
		if (cap <= len)
			reallocate(cap == 0 ? 1 : cap * 2);
		new(arr + len) T(a...);
		++len;
	}

	T* erase(T *pos)
	{
		T *loc = pos;
		T *end  = arr + len - 1;
		while (loc < end)
		{
			T *p = ++loc;
			*loc = static_cast<T&&>(*p);
			loc = p;
		}
		loc->~T();
		--len;
		return pos;
	}

	void clear()
	{
		destruct();
		len = 0;
	}
};

template<class T, size_t N> struct array
{
	T _arr[N];

	constexpr array() : _arr() { }
	template <class... P> constexpr array(P... a) : _arr{a...} { }

	size_t size() const { return N; }

	T* data() { return _arr; }
	const T* data() const { return _arr; }
	
	T* begin() { return _arr; }
	const T* begin() const { return _arr; }

	T* end() { return _arr + N; }
	const T* end() const { return _arr + N; }

	T& operator[](size_t i) { return _arr[i]; }
	const T& operator[](size_t i) const { return _arr[i]; }

	T& at(size_t i) { return _arr[i]; }
	const T& at(size_t i) const { return _arr[i]; }

	void fill(const T& v)
	{
		for (size_t i = 0; i < N; ++i)
			_arr[i] = v;
	}
};

template<class T1, class T2> struct pair
{
	T1 first;
	T2 second;

	constexpr pair() : first(), second() { }
	constexpr pair(T1 f, T2 s) : first(f), second(s) { }
};

template<class T> inline const T& min(const T& a, const T& b)
{
	return b < a ? b : a;
}

template<class I, class T> inline I find(I first, I last, const T& value)
{
	while (first < last && *first != value)
		++first;
	return first;
}

template<class I, class O> inline O copy(I first, I last, O d_first)
{
	while (first < last)
		*(d_first++) = *(first++);
	return d_first;
}

template<class I, class S, class T> inline I fill_n(I first, S count, const T& value)
{
	for (S i = 0; i < count; ++i)
		*(first++) = value;
	return first;
}

template<class I, class T> inline T accumulate(I first, I last, T init)
{
	while (first < last)
		init += *(first++);
	return init;
}

template <class T> inline void swap(T &a, T &b)
{
	T t = static_cast<T&&>(a);
	a = static_cast<T&&>(b);
	b = static_cast<T&&>(t);
}

template <class I> void sort(I first, I last)
{
	auto num = last - first;
	if (num <= 1)
		return;
	I pivot = last;
	--pivot;
	if (num > 2)
	{
		I middle = first;
		middle += num/2;
		swap(*middle, *pivot);
	}
	I left = first;
	I right = pivot;
	while (left != right)
	{
		while (!(*pivot < *left) && left != right)
			++left;
		while (!(*right < *pivot) && left != right)
			--right;
		if (left != right)
			swap(*left, *right);
	} 
	if (pivot != left && *pivot < *left)
		swap(*pivot, *left); 
	sort(first, left);
	sort(++left, last);
}

class string
{
private:
	char *str;

public:
	constexpr string() : str(nullptr) { }
	
	string(const char *s)
	{
		str = s ? strdup(s) : nullptr;
	}

	string(const string &) = delete;

	string(string &&s)
	{
		str = s.str;
		s.str = nullptr;
	}

	~string()
	{
		if (str) free(str);
	}

	string &operator=(const string &) = delete;

	string &operator=(string &&s)
	{
		if (str) free(str);
		str = s.str;
		s.str = nullptr;
		return *this;
	}

	string &operator=(const char *s)
	{
		if (str) free(str);
		str = s ? strdup(s) : nullptr;
		return *this;
	}

	const char *c_str() const { return str; } 
};
