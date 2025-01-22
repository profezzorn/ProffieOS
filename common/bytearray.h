#ifndef COMMON_BYTEARRAY_H
#define COMMON_BYTEARRAY_H

// Contains templates for arrays of bytes / strings.
// These can be concatenated generated from numbers or strings and zero-stripped at
// compile time.

#include <type_traits>

// Basic template
template<uint8_t... BYTES> struct ByteArray {
  // uint8_t array, no terminating zero.
  static const uint8_t data[sizeof...(BYTES)];

  // char[], with terminating zero.
  static constexpr char str[] = {BYTES..., 0};
};
template<uint8_t... BYTES>
const uint8_t ByteArray<BYTES...>::data[] = { BYTES... };
template <uint8_t... BYTES>
constexpr char ByteArray<BYTES...>::str[];

// Concatenation
template<class... ARRAYS> struct ConcatByteArraysHelper {};
template<> struct ConcatByteArraysHelper<> { typedef ByteArray<> type; };
template<class T> struct ConcatByteArraysHelper<T> { typedef T type; };

template<uint8_t... Bytes1, uint8_t... Bytes2, class... REST>
struct ConcatByteArraysHelper<ByteArray<Bytes1...>, ByteArray<Bytes2...>, REST...> {
  typedef typename::ConcatByteArraysHelper<ByteArray<Bytes1..., Bytes2...>, REST...>::type type;
};

// Use this one.
template<class... REST> using ConcatByteArrays = typename::ConcatByteArraysHelper<REST...>::type;


// NumberToByteArray<10>::type = ByteArray<'1', '0'>
template <std::size_t N>
struct NumberToByteArray
{
    using type = ConcatByteArrays<
      typename std::conditional<(N >= 10), typename NumberToByteArray<N / 10>::type, ByteArray<>>::type,
	ByteArray<'0' + N % 10>
	>;
};

template <>
struct NumberToByteArray<0>
{
    using type = ByteArray<'0'>;
};

// Strips zeroes, used by STRTYPE below.
template<class BA> struct StripZeroHelper {};
template<uint8_t... symbols> struct StripZeroHelper<ByteArray<0, symbols...>> { using type = ByteArray<>; };
template<uint8_t a, uint8_t... symbols> struct StripZeroHelper<ByteArray<a, symbols...>> { using type = ConcatByteArrays<ByteArray<a>, typename StripZeroHelper<ByteArray<symbols...>>::type>; };
template<class BA> using StripZeroes = typename StripZeroHelper<BA>::type;

constexpr char index_string_low(char const str[], size_t index, size_t len) {
  return index >= len ? 0 : str[index];
}

#define index_string(X, index) index_string_low((X), (index), sizeof((X)) - sizeof(""))

// Converts a string to a ByteArrayType.
// STRTYPE("foo") = ByteArray<'f', 'o', 'o'>
// This can be made longer if needed
#define STRTYPE(S)				\
  StripZeroes<ByteArray<			\
    index_string((S), 0),			\
    index_string((S), 1),			\
    index_string((S), 2),			\
    index_string((S), 3),			\
    index_string((S), 4),			\
    index_string((S), 5),			\
    index_string((S), 6),			\
    index_string((S), 7),			\
    index_string((S), 8),			\
    index_string((S), 9),			\
    index_string((S), 10),			\
    index_string((S), 11),			\
    index_string((S), 12),			\
    index_string((S), 13),			\
    index_string((S), 14),			\
    index_string((S), 15),			\
    index_string((S), 16),			\
    index_string((S), 17),			\
    index_string((S), 18),			\
    index_string((S), 19),			\
    index_string((S), 20)>>


#endif   // COMMON_BYTE_ARRAY_H

