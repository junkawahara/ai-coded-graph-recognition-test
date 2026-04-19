#include "test_helpers.h"

#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace graph_recognition {
namespace gtest_utils {

Graph load_graph(const std::string& path) {
    std::ifstream in(path.c_str());
    if (!in) {
        throw std::runtime_error("cannot open input: " + path);
    }
    return Graph::read(in);
}

DirectedInput load_arcs(const std::string& path) {
    std::ifstream in(path.c_str());
    if (!in) {
        throw std::runtime_error("cannot open input: " + path);
    }
    DirectedInput d;
    int m = 0;
    if (!(in >> d.n >> m)) {
        return d;
    }
    d.arcs.reserve(m);
    for (int i = 0; i < m; ++i) {
        int u = 0, v = 0;
        if (!(in >> u >> v)) break;
        d.arcs.push_back(std::make_pair(u, v));
    }
    return d;
}

std::string read_expected(const std::string& path) {
    std::ifstream in(path.c_str());
    if (!in) {
        throw std::runtime_error("cannot open expected: " + path);
    }
    std::stringstream ss;
    ss << in.rdbuf();
    std::string s = ss.str();
    while (!s.empty() && (s.back() == '\n' || s.back() == '\r' || s.back() == ' ' || s.back() == '\t')) {
        s.pop_back();
    }
    return s;
}

int read_n(const std::string& path) {
    std::ifstream in(path.c_str());
    if (!in) {
        throw std::runtime_error("cannot open input: " + path);
    }
    int n = 0;
    in >> n;
    return n;
}

std::vector<std::string> list_in_files(const std::string& dir) {
    std::vector<std::string> stems;
    DIR* d = opendir(dir.c_str());
    if (!d) {
        return stems;
    }
    struct dirent* entry;
    const std::string suffix = ".in";
    while ((entry = readdir(d)) != nullptr) {
        std::string name = entry->d_name;
        if (name.size() > suffix.size() &&
            name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0) {
            stems.push_back(name.substr(0, name.size() - suffix.size()));
        }
    }
    closedir(d);
    std::sort(stems.begin(), stems.end());
    return stems;
}

}  // namespace gtest_utils
}  // namespace graph_recognition
