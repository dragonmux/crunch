#ifndef MEMORY__HXX
#define MEMORY__HXX

#include <type_traits>
#include <memory>
#include <new>

template<typename T> struct makeUnique_ { using uniqueType = std::unique_ptr<T>; };
template<typename T> struct makeUnique_<T []> { using arrayType = std::unique_ptr<T []>; };
template<typename T, size_t N> struct makeUnique_<T [N]> { struct invalidType { }; };

template<typename T, typename... Args> inline typename makeUnique_<T>::uniqueType makeUnique(Args &&...args) noexcept
{
	typedef typename std::remove_const<T>::type ctorT;
	return std::unique_ptr<T>(new (std::nothrow) ctorT(std::forward<Args>(args)...));
}

template<typename T> inline typename makeUnique_<T>::arrayType makeUnique(const size_t num) noexcept
{
	typedef typename std::remove_const<typename std::remove_extent<T>::type>::type ctorT;
	return std::unique_ptr<T>(new (std::nothrow) ctorT[num]());
}

template<typename T, typename... Args> inline typename makeUnique_<T>::invalidType makeUnique(Args &&...) = delete;

#endif /*MEMORY__HXX*/
