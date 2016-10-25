#include "center_text.h"

center_helper centered(std::string str) {
    return center_helper(str);
}

std::ostream& operator<<(std::ostream& s, const center_helper& c) {
    std::streamsize w = s.width();
    int obj_length = c.str_.length();
    if (w > obj_length) {
        std::streamsize left = (w - obj_length) / 2;
        s.width(left);
        s << "";
        s << c.str_;
        s.width(w - left - obj_length);
        s << "";
    } else {
        s << c.str_;
    }
    return s;
}
