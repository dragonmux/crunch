// SPDX-License-Identifier: LGPL-3.0-or-later
#ifndef TEST_ENGINES__HXX
#define TEST_ENGINES__HXX

#include <limits>

namespace impl
{
	template<typename UIntType, size_t w, bool = w < static_cast<size_t>
		(std::numeric_limits<UIntType>::digits)>
	struct shift { static const UIntType value = 0; };

	template<typename UIntType, size_t w> struct shift<UIntType, w, true>
		{ static const UIntType value = UIntType{1U} << w; };
}

// This is a copy of std::subtract_with_carry_engine, modified to swap
// the short and long lags in the variate computation step.
template<typename UIntType, size_t w, size_t s, size_t r>
class subtract_with_borrow_engine
{
	static_assert(std::is_unsigned<UIntType>::value,
		"result_type must be an unsigned integral type");
	static_assert(0u < s && s < r,
		"0 < s < r");
	static_assert(w == std::numeric_limits<UIntType>::digits,
		"template argument substituting w out of bounds");

public:
	using result_type = UIntType;

	// parameter values
	static constexpr size_t word_size = w;
	static constexpr size_t short_lag = s;
	static constexpr size_t long_lag = r;
	static constexpr result_type default_seed = 19780503u;

	subtract_with_borrow_engine() : subtract_with_borrow_engine{default_seed} {}
	explicit subtract_with_borrow_engine(result_type value) { seed(value); }

	void seed(result_type value = default_seed)
	{
		std::linear_congruential_engine<result_type, 40014u, 0u, 2147483563u>
			lcg{value == 0u ? default_seed : value};

		const size_t n = (w + 31) / 32;

		for (size_t i = 0; i < long_lag; ++i)
		{
			UIntType sum = 0u;
			UIntType factor = 1u;
			for (size_t __j = 0; __j < n; ++__j)
			{
				sum += (lcg() & 0xFFFFFFFFU) * factor;
				factor *= impl::shift<UIntType, 32>::value;
			}
			x[i] = sum;
		}
		carry = (x[long_lag - 1] == 0) ? 1 : 0;
		p = 0;
	}

	result_type operator()()
	{
		// Derive short lag index from current index.
		long ps = p - short_lag;
		if (ps < 0)
			ps += long_lag;

		// Calculate new x(i) without overflow or division.
		// NB: Thanks to the requirements for _UIntType, x[p] + carry
		// cannot overflow.
		UIntType xi;
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
		if (++p >= long_lag)
			p = 0;

		return xi;
	}

	void discard(unsigned long long __z)
	{
		for (; __z != 0ULL; --__z)
			(*this)();
	}

private:
	/// The state of the generator.  This is a ring buffer.
	UIntType x[long_lag];
	UIntType carry;		///< The carry
	size_t p;			///< Current index of x(i - r).
};

#endif /*TEST_ENGINES__HXX*/
