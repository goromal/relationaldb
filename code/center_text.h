#ifndef _CENTEXT_
#define _CENTEXT_

#include <string>
#include <iostream>
#include <iomanip>

class center_helper {
public:
  center_helper() : str_("") {}
  center_helper(std::string str) : str_(str) {}
  friend std::ostream& operator<<(std::ostream& s, const center_helper& c);
  ~center_helper() {}
  std::string str_;
};

center_helper centered(std::string str);

#endif
