#include <iostream>
#include <type_traits>

template <class... Ts> 
struct TypeTuple {};

// ================================================================================

template <class Head, class Tail>
struct Cons;
struct Nil;

template <class Tt>
concept TypeList = std::same_as<Tt, Nil> || 
    requires(Tt* t) {
        []<class A, class B>(Cons<A, B>*){}(t);
    };

static_assert(TypeList<Nil>);
static_assert(TypeList<Cons<int, Nil>>);
static_assert(TypeList<Cons<float, Cons<int, Nil>>>);

// ================================================================================

namespace detail {

    template <class Head = Nil, class... Ts>
    struct PackToList {
        using Impl = Cons<Head, typename PackToList<Ts...>::Impl>;
    };

    template <>
    struct PackToList<> {
        using Impl = Nil;
    };

};

template <class... Ts>
using PackToList = detail::PackToList<Ts...>::Impl;

static_assert(std::same_as<PackToList<>, Nil>);
static_assert(std::same_as<PackToList<int>, Cons<int, Nil>>);
static_assert(std::same_as<PackToList<float, int, bool>, Cons<float, Cons<int, Cons<bool, Nil>>>>);

namespace detail {

    template <class TL>
    struct Head;

    template <class H, class T>
    struct Head<Cons<H, T>> {
        using Impl = H;
    };

    template <>
    struct Head<Nil> {
        using Impl = Nil;
    };

};

template <class TL> // Cons<Head, Tail>
using Head = detail::Head<TL>::Impl;

static_assert(std::same_as<Head<Nil>, Nil>);
static_assert(std::same_as<Head<Cons<int, Nil>>, int>);
static_assert(std::same_as<Head<Cons<float, Cons<int, Cons<bool, Nil>>>>, float>);

namespace detail {

    template <class TL>
    struct Tail;

    template <class H, class T>
    struct Tail<Cons<H, T>> {
        using Impl = T;
    };

    template <>
    struct Tail<Nil> {
        using Impl = Nil;
    };

};

template <class TL>
using Tail = detail::Tail<TL>::Impl; 

static_assert(std::same_as<Tail<Nil>, Nil>);
static_assert(std::same_as<Tail<Cons<int, Nil>>, Nil>);
static_assert(std::same_as<Tail<Cons<float, Cons<int, Cons<bool, Nil>>>>, Cons<int, Cons<bool, Nil>>>);

namespace detail {

    template <class T, class TL>
    struct AppendFront {
        using Impl = Cons<T, TL>;
    };

};

template <class T, class TL>
using AppendFront = detail::AppendFront<T, TL>::Impl;

static_assert(std::same_as<AppendFront<float, Nil>, Cons<float, Nil>>);
static_assert(std::same_as<AppendFront<double, Cons<int, Nil>>, Cons<double, Cons<int, Nil>>>);
static_assert(std::same_as<AppendFront<char, Cons<float, Cons<int, Cons<bool, Nil>>>>, Cons<char, Cons<float, Cons<int, Cons<bool, Nil>>>>>);

namespace detail {

    template <class TL1, class TL2>
    struct Concatenate {
        using Impl = Cons<typename Head<TL1>::Impl, typename Concatenate<typename Tail<TL1>::Impl, TL2>::Impl>;
    };

    template <class TL2>
    struct Concatenate<Nil, TL2> {
        using Impl = TL2;
    };

};

template <class TL1, class TL2>
using Concatenate = detail::Concatenate<TL1, TL2>::Impl;

static_assert(std::same_as
    <Cons<float, Cons<bool, Nil>>
    , Concatenate<Nil, Cons<float, Cons<bool, Nil>>>
    >);

static_assert(std::same_as
    <Cons<float, Cons<bool, Cons<double, Nil>>>
    , Concatenate<Cons<float, Nil>, Cons<bool, Cons<double, Nil>>>
    >);

static_assert(std::same_as
    <Cons<int, Cons<float, Cons<bool, Cons<double, Nil>>>>
    , Concatenate<Cons<int, Cons<float, Nil>>, Cons<bool, Cons<double, Nil>>>
    >);

static_assert(std::same_as
    <Cons<int, Cons<float, Cons<char, Cons<bool, Cons<double, Nil>>>>>
    , Concatenate<Cons<int, Cons<float, Nil>>, Cons<char, Cons<bool, Cons<double, Nil>>>>
    >);

static_assert(std::same_as
    <Cons<long, Cons<int, Cons<float, Cons<char, Cons<bool, Cons<double, Nil>>>>>>
    , Concatenate<Cons<long, Cons<int, Cons<float, Nil>>>, Cons<char, Cons<bool, Cons<double, Nil>>>>
    >);

template <class TL>
static constexpr std::size_t Lenth = Lenth<Tail<TL>> + 1;

template <>
static constexpr std::size_t Lenth<Nil> = 0;

static_assert(Lenth<Nil> == 0);
static_assert(Lenth<Cons<float, Cons<bool, Nil>>> == 2);
static_assert(Lenth<Cons<float, Cons<bool, Cons<double, Nil>>>> == 3);

// ================================================================================

namespace detail {

    template <class TL, class Tail = Nil> 
    struct Reverse {
        using Impl = Reverse<typename detail::Tail<TL>::Impl, Cons<typename detail::Head<TL>::Impl, Tail>>::Impl;
    };

    template <class Tail>
    struct Reverse<Nil, Tail> {
        using Impl = Tail;
    };

};

template <class TL>
using Reverse = detail::Reverse<TL>::Impl;

static_assert(std::same_as<
    Cons<int, Nil>, 
    Reverse<Cons<int, Nil>>
    >);

static_assert(std::same_as<
    Cons<double, Cons<int, Nil>>, 
    Reverse<Cons<int, Cons<double, Nil>>>
    >);

static_assert(std::same_as<
    Cons<float, Cons<double, Cons<int, Nil>>>, 
    Reverse<Cons<int, Cons<double, Cons<float, Nil>>>>
    >);

static_assert(std::same_as<
    Cons<char, Cons<float, Cons<double, Cons<int, Nil>>>>, 
    Reverse<Cons<int, Cons<double, Cons<float, Cons<char, Nil>>>>>
    >);

namespace detail {

    template <template<class> class F, class TL>
    struct Map {
        using Impl = Cons<F<typename detail::Head<TL>::Impl>, typename detail::Map<F, typename detail::Tail<TL>::Impl>::Impl>;
    };

    template <template<class> class F>
    struct Map<F, Nil> {
        using Impl = Nil;
    };

};

template <template<class> class F, class TL>
using Map = detail::Map<F, TL>::Impl;

template <typename T>
using ToPointer = T*;

static_assert(std::same_as<
    Cons<int*, Nil>, 
    Map<ToPointer, Cons<int, Nil> >
    >);

static_assert(std::same_as<
    Cons<double*, Cons<int*, Nil>>, 
    Map<ToPointer, Cons<double, Cons<int, Nil>> >
    >);

static_assert(std::same_as<
    Cons<float*, Cons<int*, Cons<double*, Cons<int*, Nil>>>>, 
    Map<ToPointer, Cons<float, Cons<int, Cons<double, Cons<int, Nil>>>> >
    >);

namespace detail {

    template <class T, class TL>
    struct AppendBack {
        using Impl = Cons<typename detail::Head<TL>::Impl, typename detail::AppendBack<T, typename detail::Tail<TL>::Impl>::Impl>;
    };

    template <class T>
    struct AppendBack<T, Nil> {
        using Impl = Cons<T, Nil>;
    };

};

template <class T, class TL>
using AppendBack = detail::AppendBack<T, TL>::Impl; 

static_assert(std::same_as<AppendBack<float, Nil>, Cons<float, Nil>>);
static_assert(std::same_as<AppendBack<double, Cons<int, Nil>>, Cons<int, Cons<double, Nil>>>);
static_assert(std::same_as<AppendBack<char, Cons<float, Cons<int, Cons<bool, Nil>>>>, Cons<float, Cons<int, Cons<bool, Cons<char, Nil>>>>>);

namespace detail {

    template <class T, class TL>
    struct Intersperese {
        using Impl = Cons<typename detail::Head<TL>::Impl,
                          Cons<T, typename detail::Intersperese<T, typename detail::Tail<TL>::Impl>::Impl>
                          >;
    };

    template <class T, class U>
    struct Intersperese<T, Cons<U, Nil>> {
        using Impl = Cons<U, Nil>;
    };

    template <class T>
    struct Intersperese<T, Nil> {
        using Impl = Nil;
    };

};

template <class T, class TL>
using Intersperese = detail::Intersperese<T, TL>::Impl;

static_assert(std::same_as<Nil, Intersperese<int, Nil>>);
static_assert(std::same_as<Cons<float, Nil>, Intersperese<int, Cons<float, Nil>>>);
static_assert(std::same_as<Cons<float, Cons<int, Cons<float, Nil>>>, Intersperese<int, Cons<float, Cons<float, Nil>>>>);
static_assert(std::same_as<Cons<float, Cons<char, Cons<int, Nil>>>, Intersperese<char, Cons<float, Cons<int, Nil>>>>);

namespace detail {

    template <std::size_t N, class TL>
    struct First {
        using Impl = Cons<
        typename detail::Head<TL>::Impl, 
        typename detail::First<
            N - 1, 
            typename detail::Tail<TL>::Impl
            >::Impl
        >;
    }; 

    template <class TL> 
    struct First<0, TL> {
        using Impl = Nil;
    };

};

template <std::size_t N, class TL>
using First = detail::First<N, TL>::Impl;

static_assert(std::same_as<Cons<int, Nil>, First<1, Cons<int, Nil>>>);
static_assert(std::same_as<Cons<int, Nil>, First<1, Cons<int, Cons<double, Nil>>>>);
static_assert(std::same_as<Cons<int, Cons<double, Nil>>, First<2, Cons<int, Cons<double, Nil>>>>);
static_assert(std::same_as<Cons<int, Cons<double, Nil>>, First<2, Cons<int, Cons<double, Cons<float, Nil>>>>>);

namespace detail {

    template <std::size_t C, std::size_t Acc, class TL>
    struct Last {
        using Impl = Last<
        C + 1, 
        Acc, 
        typename detail::Tail<TL>::Impl
        >::Impl;
    };

    template <std::size_t C, class TL>
    struct Last<C, C, TL> {
        using Impl = TL;
    };

};

template <std::size_t N, class TL>
using Last = detail::Last<0, Lenth<TL> - N, TL>::Impl;

static_assert(std::same_as<Cons<int, Nil>, Last<1, Cons<int, Nil>>>);
static_assert(std::same_as<Cons<double, Nil>, Last<1, Cons<int, Cons<double, Nil>>>>);
static_assert(std::same_as<Cons<int, Cons<double, Nil>>, Last<2, Cons<int, Cons<double, Nil>>>>);
static_assert(std::same_as<Cons<double, Cons<float, Nil>>, Last<2, Cons<int, Cons<double, Cons<float, Nil>>>>>);
static_assert(std::same_as<Cons<double, Cons<float, Nil>>, Last<2, Cons<char, Cons<int, Cons<double, Cons<float, Nil>>>>>>);

// ================================================================================

template <class Ts1, class Ts2>
class GenerateAbstractFactory;

template<class... Ts1, class... Ts2>
class GenerateAbstractFactory<TypeTuple<Ts1...>, TypeTuple<Ts2...>>;

// ================================================================================

int main() {
    std::cout << "Hello, world!" << std::endl;
    return 0;
}