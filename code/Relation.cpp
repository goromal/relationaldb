#include "Relation.h"

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

bool Relation::operator==(const Relation & other) const {
  assert(Header == other.get_Header());
  return Rows == other.get_Rows();
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

Relation Relation::Project(vector<parameter> _Att_list) {
  for (const auto& att : _Att_list) {
    assert(std::find(Header.Items.begin(), Header.Items.end(), att.value) != Header.Items.end());
  }
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

Relation Relation::Rename(parameter _Att, parameter _new_Att) {
  assert(_Att.token == ID && _new_Att.token == ID &&
    std::find(Header.Items.begin(), Header.Items.end(), _Att.value) != Header.Items.end());
  int pos = std::find(Header.Items.begin(), Header.Items.end(), _Att.value) - Header.Items.begin();
  Tuple tmp(Header);
  tmp.Items[pos] = _new_Att.value;
  return Relation(Name, tmp, Rows);
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
  for (const auto& tuple_row : tuple_row_vector) { // "Rows" changed to "t_r_v"
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

string Relation::get_Name() {
  return Name;
}

const string & Relation::get_Name() const {
  return Name;
}

Tuple Relation::get_Header() {
  return Header;
}

const Tuple & Relation::get_Header() const {
  return Header;
}

my_set Relation::get_Rows() {
  return Rows;
}

const my_set & Relation::get_Rows() const {
  return Rows;
}

Relation::~Relation() {}
