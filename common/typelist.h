#ifndef COMMON_TYPELIST_H
#define COMMON_TYPELIST_H

#include <type_traits>

template<typename ... T>
struct TypeList {
  static const size_t size = sizeof...(T);
};

// Concatenation
template<typename ... TL> struct ConcatTypeListsHelper {};
template<>
struct ConcatTypeListsHelper<> { typedef TypeList<> type; };
template<typename ... TL>
struct ConcatTypeListsHelper<TypeList<TL...>> {
  typedef TypeList<TL...> type;
};
template<typename ... TL1, typename ... TL2, typename ... REST>
  struct ConcatTypeListsHelper<
  TypeList<TL1...>,
  TypeList<TL2...>,
  REST...> {
  typedef typename ConcatTypeListsHelper<TypeList<TL1..., TL2...>, REST...>::type type;
};

// Use this one
template<typename... REST> using ConcatTypeLists = typename::ConcatTypeListsHelper<REST...>::type;

// First
template<typename TL> struct FirstTypeListHelper { };
template<typename FIRST, typename ... REST>
struct FirstTypeListHelper<TypeList<FIRST, REST...>> { typedef FIRST type; };
template<typename TL> using FirstTypeList = typename FirstTypeListHelper<TL>::type;

// Rest
template<typename TL> struct RestTypeListHelper { };
template<typename FIRST, typename ... REST>
struct RestTypeListHelper<TypeList<FIRST, REST...>> { typedef TypeList<REST...> type; };
template<typename TL> using RestTypeList = typename RestTypeListHelper<TL>::type;



// Tail
template<size_t n, typename TL> struct TailTypeListHelper {
  typedef typename TailTypeListHelper<n-1, RestTypeList<TL>>::type type;
};
template<typename TL>
struct TailTypeListHelper<0, TL> {
  typedef TL type;
};

template<size_t n, typename TL> using TailTypeList = typename::TailTypeListHelper<n, TL>::type;

// Head
template<size_t n, typename TL, typename prefix>
struct HeadTypeListHelper {
  typedef typename::HeadTypeListHelper<n-1, RestTypeList<TL>, ConcatTypeLists<prefix, TypeList<FirstTypeList<TL>>>>::type type;
};
template<typename TL, typename  prefix>
struct HeadTypeListHelper<0, TL, prefix> {
  typedef prefix type;
};

template<size_t n, typename TL> using HeadTypeList = typename::HeadTypeListHelper<n, TL, TypeList<>>::type;


template<typename TL>
struct SplitTypeList {
  typedef HeadTypeList<TL::size/2, TL> first_half;
  typedef TailTypeList<TL::size/2, TL> second_half;
};

#endif
