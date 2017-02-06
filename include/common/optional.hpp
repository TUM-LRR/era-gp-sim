// clang-format off

#if _MSC_VER == 1910

// Remove this once __has_include is not buggy anymore in VS2017.
#include <optional>
template<typename T>
using Optional = std::optional<T>;

#else

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

#endif
// clang-format on
