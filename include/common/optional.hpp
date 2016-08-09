// clang-format off
#ifdef __has_include
#if __has_include(<optional>)

#include <optional>
template<typename T>
using Optional = std::optional<T>;

#elif __has_include(<experimental/optional>)

#include <experimental/optional>
template<typename T>
using Optional = std::experimental::optional<T>;

#else
#error "Missing <optional>"
#endif
#endif
// clang-format on
