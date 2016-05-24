#include <tuple>
#include <iostream>

template <typename T>
struct FunctionArgs : FunctionArgs<decltype(&T::operator())> {};

template <typename R, typename... Args>
struct FunctionArgsBase{
    using args  = std::tuple<Args...>;
    using arity = std::integral_constant<unsigned, sizeof...(Args)>;
    using result = R;
};

template <typename R, typename... Args>
struct FunctionArgs<R(*)(Args...)> : FunctionArgsBase<R, Args...> {};
template <typename R, typename C, typename... Args>
struct FunctionArgs<R(C::*)(Args...)> : FunctionArgsBase<R, Args...> {};
template <typename R, typename C, typename... Args>
struct FunctionArgs<R(C::*)(Args...) const> : FunctionArgsBase<R, Args...> {};

// forward declarations
template<class T, class Case, class ...Cases>
decltype(auto) match(T&& value, const Case& _case, const Cases&... cases);
template<class T, class Case>
decltype(auto) match(T&& value, const Case& _case);

namespace details {
    template<class T, class Case, class ...OtherCases>
    decltype(auto) match_call(const Case& _case, T&& value, std::true_type, const OtherCases&... other) {
        return _case(std::forward<T>(value));
    }

    template<class T, class Case, class ...OtherCases>
    decltype(auto) match_call(const Case& _case, T&& value, std::false_type, const OtherCases&... other) {
        return match(std::forward<T>(value), other...);
    }
}

template<class T, class Case, class ...Cases>
decltype(auto) match(T&& value, const Case& _case, const Cases&... cases) {
    using namespace std;
    using args = typename FunctionArgs<Case>::args;
    using arg = tuple_element_t<0, args>;
    using match = is_same<decay_t<arg>, decay_t<T>>;
    return details::match_call(_case, std::forward<T>(value), match{}, cases...);
}

// the last one is default
template<class T, class Case>
decltype(auto) match(T&& value, const Case& _case) {
    return _case(std::forward<T>(value));
}

int main()
{
	std::string s = "12";
	std::cout << match(s
	    ,[](int& i) { return "int"; }
	    ,[](bool& b) { return "bool"; }
	    ,[](std::string& s) -> auto& { s += " GAV"; return s; }
	    ,[](auto j) { std::cout << "default one"; return j; }
	);
}
