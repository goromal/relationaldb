#include "graph.h"

#define TITLE_WIDTH 36

void run_POT_Test(int & tNum, graph & g, int snode, vector<int> & expected_output, int & tP);
void run_DFS_Test(int & tNum, graph & g, int snode, vector<int> & expected_output, int & tP);
void run_SCC_Test(int & tNum, graph & g, vector<vector<int>> & expected_output, int & tP);

void make_title(int & tNum, graph & g);
void make_result(bool test, int & tP);
vector<string> explode(const string& str, const char& ch);
graph create_graph(string edges_data);
set<set<int>> vec_to_set(vector<vector<int>> & vec_obj);

int main() {
  int tests_passed = 0;
  int test_num = 1;
// ==============================TEST 1:========================================
/**
      Test if Post-Order traversal is accurate for a disconnected graph
*/
  graph g1 = create_graph("0,3 ; 3,4 ; 1,0 ; 1,3 ; 4,1 ; 1,5; 2,2");
  vector<int> eo1 = {5, 1, 4, 3, 0, 2};
  run_POT_Test(test_num, g1, 0, eo1, tests_passed);
// ==============================TEST 2:========================================
/**
      Test Post-Order traversal for a totally connected graph, starting at a
      middle node
*/
  graph g2 = create_graph("0,1 ; 1,2 ; 2,3 ; 3,6 ; 6,3 ; 6,7 ; 7,5 ; 5,4");
  vector<int> eo2 = {3, 4, 5, 7, 6, 2, 1, 0};
  run_POT_Test(test_num, g2, 6, eo2, tests_passed);
// ==============================TEST 3:========================================
/**
      Test Post-Order traversal for a minimally-connected graph, starting at a
      middle node
*/
  graph g3 = create_graph("0,0 ; 3,3 ; 2,2 ; 5,5 ; 1,4");
  vector<int> eo3 = {4, 5, 0, 1, 2, 3};
  run_POT_Test(test_num, g3, 4, eo3, tests_passed);
// ==============================TEST 4:========================================
/**
      Test Depth-First search on a simple tree, starting at the root node
*/
  graph g4 = create_graph("0,1 ; 1,2 ; 1,3 ; 0,4 ; 4,5 ; 4,6");
  vector<int> eo4 = {0, 1, 2, 3, 4, 5, 6};
  run_DFS_Test(test_num, g4, 0, eo4, tests_passed);
// ==============================TEST 5:========================================
/**
      Test Depth-First search on a graph, starting at a middle node where all
      nodes are reachable
*/
  graph g5 = create_graph("4,1 ; 4,2 ; 4,5 ; 2,1 ; 2,5 ; 1,0 ; 0,3");
  vector<int> eo5 = {4, 1, 0, 3, 2, 5};
  run_DFS_Test(test_num, g5, 4, eo5, tests_passed);
// ==============================TEST 6:========================================
/**
      Test Depth-First search on a graph with unreachable nodes, starting at
      a middle node
*/
  graph g6 = create_graph("4,1 ; 4,2 ; 4,3 ; 4,5 ; 0,1 ; 0,4 ; 2,5 ; 6,6");
  vector<int> eo6 = {4, 1, 2, 5, 3};
  run_DFS_Test(test_num, g6, 4, eo6, tests_passed);
// ==============================TEST 7:========================================
/**
      Test if strongly-connected components can be deduced in a connected graph.
*/
  graph g7 = create_graph("0,1 ; 0,3 ; 1,3 ; 2,0 ; 2,5 ; 3,1 ; 3,2 ; 4,2 ; 4,5 ; 4,7 ; 6,7 ; 7,6 ; 8,6 ; 8,8");
  vector<vector<int>> eo7 = {{0,1,2,3},{4},{5},{6,7},{8}};
  run_SCC_Test(test_num, g7, eo7, tests_passed);
// ==============================TEST 8:========================================
/**
      Test if strongly-connected components can be deduced in a totally disconnected
      graph.
*/
  graph g8 = create_graph("0,0 ; 1,1 ; 2,2 ; 3,3 ; 4,4");
  vector<vector<int>> eo8 = {{0},{1},{2},{3},{4}};
  run_SCC_Test(test_num, g8, eo8, tests_passed);
// ==============================TEST 9:========================================
/**
      Test if strongly-connected components can be deduced in another connected
      graph.
*/
  graph g9 = create_graph("0,2 ; 2,4 ; 4,2 ; 1,0 ; 2,1 ; 2,3 ; 3,5 ; 5,6 ; 6,5 ; 6,7 ; 7,7");
  vector<vector<int>> eo9 = {{0,1,2,4},{3},{5,6},{7}};
  run_SCC_Test(test_num, g9, eo9, tests_passed);
// ==============================TEST 10:=======================================
/**
      Test if strongly-connected components can be deduced in a circuitous
      graph.
*/
  graph g10 = create_graph("0,1 ; 1,2 ; 2,3 ; 3,4 ; 4,5 ; 5,0");
  vector<vector<int>> eo10 = {{0,1,2,3,4,5}};
  run_SCC_Test(test_num, g10, eo10, tests_passed);
// =============================================================================
  cout << "TESTS PASSED: " << tests_passed << "/" << test_num - 1 << ". ("
       << 100.0*tests_passed/(test_num - 1) << "%)\n\n";
  return 0;
}

// Tests
void run_POT_Test(int & tNum, graph & g, int snode, vector<int> & expected_output, int & tP) {
  // expected_output expecting {1st, 2nd, 3rd, ...} order
  make_title(tNum, g);

  cout << "The following is a post-order traversal starting at node " << snode << ":\n";
  stack<int> POT_output;
  g.POT(snode, POT_output);
  // use another stack to reverse order of output
  stack<int> for_output;
  size_t size = POT_output.size();
  for (size_t i = 0; i < size; i++) {
    for_output.push(POT_output.top());
    POT_output.pop();
  }
  // print out results and prep comparison
  vector<int> for_comparison;
  for (size_t i = 0; i < size; i++) {
    cout << i + 1 << ": " << for_output.top() << endl;
    for_comparison.push_back(for_output.top());
    for_output.pop();
  }

  bool equal_to_expected = for_comparison.size() == expected_output.size();
  if (equal_to_expected)
    for (size_t i = 0; i < for_comparison.size(); i++)
      if (for_comparison[i] != expected_output[i])
        equal_to_expected = false;
  make_result(equal_to_expected, tP);
  tNum++;
}

void run_DFS_Test(int & tNum, graph & g, int snode, vector<int> & expected_output, int & tP) {
  make_title(tNum, g);

  cout << "The following is a depth-first search starting at node " << snode << ":\n";
  vector<int> DFS_output;
  g.DFS(snode, DFS_output);
  g.reSet();
  for (const auto& node : DFS_output)
    cout << node << " ";
  cout << endl;

  make_result(DFS_output == expected_output, tP);
  tNum++;
}

void run_SCC_Test(int & tNum, graph & g, vector<vector<int>> & expected_output, int & tP) {
  make_title(tNum, g);

  cout << "These are the strongly connected components:\n";
  vector<vector<int>> SCComponents;
  g.get_SCC(SCComponents);
  for (const auto& region_vec : SCComponents) {
    for (const auto& node : region_vec)
      cout << node << " ";
    cout << endl;
  }

  set<set<int>> actual_output_set = vec_to_set(SCComponents);
  set<set<int>> expected_output_set = vec_to_set(expected_output);
  make_result(actual_output_set == expected_output_set, tP);
  tNum++;
}

// Tools
void make_title(int & tNum, graph & g) {
  stringstream title;
  title << "TEST " << tNum;
  cout << std::setfill('=') << std::setw(TITLE_WIDTH) << '=' << '\n'
       << std::setfill('=') << std::setw(TITLE_WIDTH) << centered(title.str()) << '\n'
       << std::setfill('=') << std::setw(TITLE_WIDTH) << '=' << "\n\n";
  cout << "In the following graph:\n" << g.to_String() << endl;
}

void make_result(bool test, int & tP) {
  if (test) {
    cout << std::setfill('-') << std::setw(TITLE_WIDTH) << centered("PASSED");
    cout << "\n\n";
    tP++;
  }
  else {
    cout << std::setfill('-') << std::setw(TITLE_WIDTH) << centered("FAILED");
    cout << "\n\n";
  }
}

vector<string> explode(const string& str, const char& ch) {
  string next;
  vector<string> result;
  for (string::const_iterator it = str.begin(); it != str.end(); it++) {
    if (*it == ch) {
      if (!next.empty()) {
        result.push_back(next);
        next.clear();
      }
    } else {
      next += *it;
    }
  }
  if (!next.empty())
    result.push_back(next);
  return result;
}

graph create_graph(string edges_data) {
  graph g;
  vector<string> edges = explode(edges_data, ';');
  for (const auto& edge : edges) {
    vector<string> verts = explode(edge, ',');
    g.add_edge(stoi(verts[0]), stoi(verts[1]));
  }
  return g;
}

set<set<int>> vec_to_set(vector<vector<int>> & vec_obj) {
  set<set<int>> set_obj;
  for (const auto& vec : vec_obj) {
    set<int> set_;
    for (const auto& num : vec)
      set_.insert(num);
    set_obj.insert(set_);
  }
  return set_obj;
}
