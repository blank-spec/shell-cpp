#include "string_utils.hpp"


namespace utils {
    std::string Unquote(std::string_view s) {
        if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
            s.remove_prefix(1);
            s.remove_suffix(1);
        }

        return std::string{s};
    }
}