#include "Relation.h"

Tuple::Tuple(vector<parameter> _Parameters) {
  for (size_t i = 0; i < _Parameters.size(); i++) {
    Items.push_back(_Parameters[i].value);
  }
}

Tuple::Tuple(vector<string> _Items) {
  for (const auto& item : _Items) Items.push_back(item);
}

bool Tuple::operator==(const Tuple & other) const {
  if (Items.size() != other.Items.size()) return false;
  for (size_t i = 0; i < Items.size(); i++) {
    if (Items[i] != other.Items[i]) return false;
  }
  return true;
}

Tuple::Tuple(const Tuple & other) {
  for (size_t i = 0; i < other.Items.size(); i++) {
    Items.push_back(other.Items[i]);
  }
}

Tuple & Tuple::operator=(Tuple && other) noexcept {
  Items.clear();
  for (size_t i = 0; i < other.Items.size(); i++) {
    Items.push_back(other.Items[i]);
  }
  other.Items.clear();
  return *this;
}

Tuple & Tuple::operator=(const Tuple & other) {
  Tuple tmp(other);               // re-use Copy constructor
  *this = std::move(tmp);         // re-use Move assignment operator
  return *this;
}

index_tracker::index_tracker(size_t i1, size_t i2) {
  indexes[0] = i1;
  indexes[1] = i2;
}

index_tracker::index_tracker() {
  indexes[0] = 0;
  indexes[1] = 0;
}

size_t& index_tracker::operator()(unsigned int i) {
  assert(i <= 1);
  return indexes[i];
}

const size_t& index_tracker::operator()(unsigned int i) const {
  assert(i <= 1);
  return indexes[i];
}

nat_join_helper::nat_join_helper(const Relation& a, const Relation& b) {
  index_tracker _max_indexes(a.get_Header().Items.size(), b.get_Header().Items.size());
  max_indexes = _max_indexes;

  // Get index pairs and set up disjoint attribute acquisitions
  vector<size_t> first_index_domain;
  vector<size_t> second_index_domain;
  vector<size_t> first_temp_matches;
  vector<size_t> second_temp_matches;
  for (size_t i = 0; i < max_indexes(0); i++) {
    first_index_domain.push_back(i);
    for (size_t j = 0; j < max_indexes(1); j++) {
      if (i == 0) second_index_domain.push_back(j);
      if (a.get_Header().Items[i] == b.get_Header().Items[j]) {
        first_temp_matches.push_back(i);
        second_temp_matches.push_back(j);
        common_indexes.push_back(index_tracker(i, j));
      }}}

  // Get indexes of disjoint attributes
  first_disjoint_indexes.resize(max_indexes(0));
  second_disjoint_indexes.resize(max_indexes(1));

  vector<size_t>::iterator it1;
  it1 = std::set_difference(first_index_domain.begin(), first_index_domain.end(),
                           first_temp_matches.begin(), first_temp_matches.end(),
                           first_disjoint_indexes.begin());
  first_disjoint_indexes.resize(it1 - first_disjoint_indexes.begin());

  vector<size_t>::iterator it2;
  it2 = std::set_difference(second_index_domain.begin(), second_index_domain.end(),
                           second_temp_matches.begin(), second_temp_matches.end(),
                           second_disjoint_indexes.begin());
  second_disjoint_indexes.resize(it2 - second_disjoint_indexes.begin());
}

Relation::Relation() {
  Name = "";
  Tuple blank;
  Header = blank;
  num_columns = 0;
}

Relation::Relation(const predicate & definition) {
  Name = definition.name;
  Tuple _Header(definition.parameters);
  Header = _Header;
  num_columns = Header.Items.size();
}

Relation::Relation(const predicate & definition, const predicate & instance) :
  Relation(definition) {
  assert(definition.parameters.size() == instance.parameters.size());
  Tuple _Row(instance.parameters);
  Rows.insert(_Row);
}

Relation::Relation(string _Name, Tuple _Header, my_set _Rows) :
  Name(_Name), Header(_Header) {
  num_columns = Header.Items.size();
  Rows = _Rows;
}

void Relation::add_Row(predicate instance) {
  assert(instance.parameters.size() == num_columns);
  Tuple _new_Row(instance.parameters);
  Rows.insert(_new_Row);
}

Relation Relation::operator+(const Relation & other) const {
  assert(Header == other.get_Header());
  my_set united_set;
  united_set.insert(Rows.begin(), Rows.end());
  united_set.insert(other.get_Rows().begin(), other.get_Rows().end());
  return Relation(Name, Header, united_set);
}

Relation Relation::operator*(const Relation & other) const {
  assert(Header == other.get_Header());
  my_set intersected_set;
  unordered_set_intersection(
    Rows.begin(), Rows.end(),
    other.get_Rows().begin(), other.get_Rows().end(),
    std::inserter(intersected_set, intersected_set.begin()));
  return Relation(Name, Header, intersected_set);
}

Relation Relation::operator-(const Relation & other) const {
  assert(Header == other.get_Header());
  my_set subtracted_set;
  unordered_set_difference(
    Rows.begin(), Rows.end(),
    other.get_Rows().begin(), other.get_Rows().end(),
    std::inserter(subtracted_set, subtracted_set.begin()));
  return Relation(Name, Header, subtracted_set);
}

void tuple_element_filer(const nat_join_helper & _helper, vector<string> & _tuple_strings,
                    const Tuple & _Ta, const Tuple & _Tb) {
  for (const auto& common_index : _helper.common_indexes) {
    _tuple_strings.push_back(_Ta.Items[common_index(0)]);
  }
  for (const auto& f_d_j : _helper.first_disjoint_indexes) {
    _tuple_strings.push_back(_Ta.Items[f_d_j]);
  }
  for (const auto& s_d_j : _helper.second_disjoint_indexes) {
    _tuple_strings.push_back(_Tb.Items[s_d_j]);
  }
}

Relation Relation::operator%(const Relation & other) const {
  nat_join_helper helper(*this, other);

  // Compile Name
  stringstream new_name;
  new_name << Name << "-NJ-" << other.get_Name();

  // Compile Header
  vector<string> new_header_strings;
  tuple_element_filer(helper, new_header_strings, this->get_Header(), other.get_Header());

  // Compile Rows
  my_set new_rows;
  for (const auto& this_tuple : Rows) {
    for (const auto& other_tuple : other.Rows) {
      vector<string> new_row_vec;
      bool common_indexes_match = true;
      for (size_t i = 0; i < helper.common_indexes.size(); i++) {
        if (this_tuple.Items[helper.common_indexes[i](0)] !=
           other_tuple.Items[helper.common_indexes[i](1)]) common_indexes_match = false;
      }
      if (common_indexes_match) {
        tuple_element_filer(helper, new_row_vec, this_tuple, other_tuple);
        new_rows.insert(Tuple(new_row_vec));
      }
    }
  }

  return Relation(new_name.str(), Tuple(new_header_strings), new_rows);
}

bool Relation::operator==(const Relation & other) const {
  assert(Header == other.get_Header());
  if (Header == other.get_Header()) return Rows == other.get_Rows();
  else return false;
  //return Rows == other.get_Rows();
}

Relation Relation::Select(parameter _Att, Comparator _comp, parameter _Inst) {
  assert(std::find(Header.Items.begin(), Header.Items.end(), _Att.value) != Header.Items.end());
  my_set new_Rows;
  if (_Inst.token == ID) {
    assert(std::find(Header.Items.begin(), Header.Items.end(), _Inst.value) != Header.Items.end());
    size_t pos1 = std::find(Header.Items.begin(), Header.Items.end(), _Att.value) - Header.Items.begin();
    size_t pos2 = std::find(Header.Items.begin(), Header.Items.end(), _Inst.value) - Header.Items.begin();
    for (const auto& tuple_row : Rows) {
      if (tuple_row.Items[pos1] == tuple_row.Items[pos2]) new_Rows.insert(tuple_row);
    }
  }
  else {
    size_t pos = std::find(Header.Items.begin(), Header.Items.end(), _Att.value) - Header.Items.begin();
    for (const auto& tuple_row : Rows) {
      if (tuple_row.Items[pos] == _Inst.value) new_Rows.insert(tuple_row);
    }
  }
  return Relation(Name, Header, new_Rows);
}

Relation Relation::Project(const vector<parameter> & _Att_list) {
  /* // Commented out to avoid unused variable warning when assert is disabled
  for (const auto& att : _Att_list) {
    assert(std::find(Header.Items.begin(), Header.Items.end(), att.value) != Header.Items.end());
  }*/
  vector<size_t> indexes;
  Tuple new_Header;
  for (const auto& att : _Att_list) {
    size_t pos = std::find(Header.Items.begin(), Header.Items.end(), att.value) - Header.Items.begin();
    indexes.push_back(pos);
    new_Header.Items.push_back(Header.Items[pos]);
  }
  my_set new_Rows;
  for (const auto& tuple_row : Rows) {
    vector<string> new_tuple_items;
    for (const auto& ip : indexes) new_tuple_items.push_back(tuple_row.Items[ip]);
    Tuple new_tuple(new_tuple_items);
    new_Rows.insert(new_tuple);
  }
  return Relation(Name, new_Header, new_Rows);
}

Relation Relation::Rename(const parameter & _Att, const parameter & _new_Att) {
  assert(_Att.token == ID && _new_Att.token == ID &&
    std::find(Header.Items.begin(), Header.Items.end(), _Att.value) != Header.Items.end());
  int pos = std::find(Header.Items.begin(), Header.Items.end(), _Att.value) - Header.Items.begin();
  Tuple tmp(Header);
  tmp.Items[pos] = _new_Att.value;
  return Relation(Name, tmp, Rows);
}

Relation Relation::Rename(const vector<parameter> & new_header) {
  return Relation(Name, new_header, Rows);
}

string Relation::to_String() {
  // NOTE: The following sub-section is only meant for the stringent TA tests:
  vector<Tuple> tuple_row_vector;
  for (const auto& tuple_row : Rows) {
    tuple_row_vector.push_back(tuple_row);
  }
  std::sort(tuple_row_vector.begin(), tuple_row_vector.end(),
    [](const Tuple & t1, const Tuple & t2) -> bool {
      size_t tuple_size = t1.Items.size();
      size_t tuple_itnum = 0;
      while (tuple_itnum < tuple_size) {
        if (t1.Items[tuple_itnum] < t2.Items[tuple_itnum]) return true;
        else if (t1.Items[tuple_itnum] == t2.Items[tuple_itnum]) tuple_itnum++;
        else return false;
      }
      return false;
    });
  //==========================================================================
  stringstream ss;
  const char* indent = "  ";
  string delimiter;
  for (const auto& tuple_row : tuple_row_vector) { // "Rows"->"tuple_row_vector"
    delimiter = "";
    ss << indent;
    for (size_t i = 0; i < num_columns; i++) {
      ss << delimiter << Header.Items[i] << "=" << tuple_row.Items[i];
      delimiter = ", ";
    }
    ss << '\n';
  }
  return ss.str();
}

string Relation::to_String_test() {
  stringstream ss;
  int lmargin_width = Name.length() + 3;
  // Get Max Column Widths
  vector<size_t> column_sizes;
  int max_width = num_columns - 1;
  for (size_t i = 0; i < num_columns; i++) {
    column_sizes.push_back(Header.Items[i].size());
    for (const auto& tuple_row : Rows) {
      if (tuple_row.Items[i].length() > column_sizes[i]) {
        column_sizes[i] = tuple_row.Items[i].length();
      }
    }
    max_width += column_sizes[i];
  }
  // Compile table string...
  //  Header:
  ss << std::setw(lmargin_width) << std::left << Name;
  string divider = "";
  for (size_t i = 0; i < num_columns; i++) {
    ss << std::setw(divider.length()) << std::left << divider;
    ss << std::setw(column_sizes[i]) << centered(Header.Items[i]);
    divider = "|";
  }
  ss << '\n';
  ss << std::setw(lmargin_width) << " " << std::setfill('-')
     << std::setw(max_width) << '-' << '\n';
  //  Rows:
  ss << std::setfill(' ');
  for (const auto& tuple_row : Rows) {
    ss << std::setw(lmargin_width) << " ";
    divider = "";
    for (size_t i = 0; i < num_columns; i++) {
      ss << std::setw(divider.length()) << std::left << divider;
      ss << std::setw(column_sizes[i]) << centered(tuple_row.Items[i]);
      divider = "|";
    }
    ss << '\n';
  }
  return ss.str();
}

string & Relation::get_Name() {
  return Name;
}

const string & Relation::get_Name() const {
  return Name;
}

Tuple & Relation::get_Header() {
  return Header;
}

const Tuple & Relation::get_Header() const {
  return Header;
}

my_set & Relation::get_Rows() {
  return Rows;
}

const my_set & Relation::get_Rows() const {
  return Rows;
}

Relation::~Relation() {}
