#ifndef COMMON_TYPELIST_H
#define COMMON_TYPELIST_H

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


// Head
template<size_t n, typename TL> struct HeadTypeListHelper {  };
template<typename ... TL>
struct HeadTypeListHelper<0, TypeList<TL...>> {
  typedef  TypeList<TL...> type;
};
template<size_t n, typename FIRST, typename ... TL>
  struct HeadTypeListHelper<n, TypeList<FIRST, TL...>> {
  typedef typename HeadTypeListHelper<n-1,TypeList<TL...>>::type type;
};

template<size_t n, typename TL> using HeadTypeList = typename::HeadTypeListHelper<n, TL>::type;

// Tail
template<size_t n, typename TL, typename prefix> struct TailTypeListHelper { };
template<typename TL, typename  prefix>
struct TailTypeListHelper<0, TL, prefix> {
  typedef prefix type;
};
template<size_t n, typename FIRST, typename ... TL, typename ... prefix>
  struct TailTypeListHelper<n, TypeList<FIRST, TL...>, TypeList<prefix...>> {
  typedef typename TailTypeListHelper<n-1, TypeList<TL...>, TypeList<prefix..., FIRST> >::type type;
};

template<size_t n, typename TL> using TailTypeList = typename::TailTypeListHelper<n, TL, TypeList<>>::type;

// First
template<typename TL> struct FirstTypeListHelper { };
template<typename FIRST, typename ... REST>
struct FirstTypeListHelper<TypeList<FIRST, REST...>> { typedef FIRST type; };
template<typename TL> using FirstTypeList = typename FirstTypeListHelper<TL>::type;


template<typename TL>
struct SplitTypeList {
  typedef HeadTypeList<TL::size/2, TL> first_half;
  typedef TailTypeList<TL::size/2, TL> second_half;
};

#endif
