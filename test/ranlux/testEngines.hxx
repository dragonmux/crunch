// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef TEST_ENGINES__HXX
#define TEST_ENGINES__HXX

#include <limits>
#include <cstdint>
#include <cstddef>
#include <random>
#include <array>

namespace impl
{
	template<typename UIntType, std::size_t w, bool = w < static_cast<std::size_t>
		(std::numeric_limits<UIntType>::digits)>
	struct shift { static const UIntType value = 0; };

	template<typename UIntType, std::size_t w> struct shift<UIntType, w, true>
		{ static const UIntType value = UIntType{1U} << w; };
}

// This is a copy of std::subtract_with_carry_engine, modified to swap
// the short and long lags in the variate computation step.
template<typename UIntType, std::size_t w, std::size_t s, std::size_t r>
class subtract_with_borrow_engine
{
	static_assert(std::is_unsigned<UIntType>::value,
		"result_type must be an unsigned integral type");
	static_assert(0U < s && s < r, "0 < s < r");
	static_assert(w == std::numeric_limits<UIntType>::digits,
		"template argument substituting w out of bounds");

public:
	using result_type = UIntType;

	// parameter values
	constexpr static std::size_t wordSize{w};
	constexpr static std::size_t shortLag{s};
	constexpr static std::size_t longLag{r};
	constexpr static result_type defaultSeed{19780503U};
	constexpr static std::size_t wordLongCount{(wordSize + 31) / 32};

	subtract_with_borrow_engine() : subtract_with_borrow_engine{defaultSeed} {}
	explicit subtract_with_borrow_engine(UIntType value) { seed(value); }

	void seed(UIntType value = defaultSeed)
	{
		std::linear_congruential_engine<UIntType, 40014U, 0U, 2147483563U>
			lcg{value == 0U ? defaultSeed : value};

		for (std::size_t i = 0; i < longLag; ++i)
		{
			UIntType sum{0U};
			UIntType factor{1U};
			for (std::size_t j = 0; j < wordLongCount; ++j)
			{
				sum += (lcg() & 0xFFFFFFFFU) * factor;
				factor *= impl::shift<UIntType, 32>::value;
			}
			x[i] = sum;
		}
		carry = (x[longLag - 1] == 0) ? 1 : 0;
		p = 0;
	}

	UIntType operator()()
	{
		// Derive short lag index from current index.
		int64_t ps = p - shortLag;
		if (ps < 0)
			ps += longLag;

		// Calculate new x(i) without overflow or division.
		// NB: Thanks to the requirements for _UIntType, x[p] + carry
		// cannot overflow.
		UIntType xi{};
		if (x[p] >= x[ps] + carry)
		{
			xi = x[p] - x[ps] - carry;
			carry = 0;
		}
		else
		{
			xi = 0 - x[ps] - carry + x[p];
			carry = 1;
		}
		x[p] = xi;

		// Adjust current index to loop around in ring buffer.
		if (++p >= longLag)
			p = 0;

		return xi;
	}

	void discard(uint64_t amount)
	{
		for (; amount; --amount)
			(*this)();
	}

private:
	/// The state of the generator.  This is a ring buffer.
	std::array<UIntType, longLag> x{};
	UIntType carry{}; ///< The carry
	std::size_t p{}; ///< Current index of x(i - r).
};

#endif /*TEST_ENGINES__HXX*/
