#ifndef GRAPH_RECOGNITION_CLI_MAIN_H
#define GRAPH_RECOGNITION_CLI_MAIN_H

/**
 * @file cli_main.h
 * @brief Shared exception boundary and input helpers of the command-line tools
 *
 * Every CLI source file (src/..._main.cpp) writes its program as cli_main() and lets main()
 * delegate to run_cli_main(), so that an exception escaping the recognizer or
 * enumerator ends the program with a diagnostic and a non-zero exit status
 * instead of std::terminate and a core dump.
 *
 * Exit status contract of the CLIs:
 *   - 0: the answer was printed
 *   - 1: malformed or unsupported input, reported on stderr as "error: ...":
 *        a graph that does not follow the "n m" + edge-list format, a missing
 *        or negative vertex count, a vertex count outside the enumerator's
 *        representable range, a missing parameter (k, girth) or an unknown
 *        command-line option. A std::invalid_argument escaping the algorithm
 *        (the enumerators throw it for an unrepresentable n, see
 *        util/vertex_count.h) ends the same way.
 *   - 2: no answer -- an allocation failed (std::bad_alloc) or the algorithm
 *        gave up (std::runtime_error, e.g. a search budget or a recursion
 *        limit); the reason goes to stderr as "error: ...; result unknown"
 *        and nothing is printed to stdout that could be read as YES/NO
 *
 * Several recognizers use Theta(n^2) or more memory even on sparse inputs
 * (this project accepts O(n^2), O(n^3) and O(2^n) algorithms; see the
 * complexity policy in CLAUDE.md), so a large input can legitimately end in
 * status 2 on a machine without enough memory.
 *
 * The enumeration CLIs (src/..._enum_main.cpp) share the parsing below:
 * cli_parse_enum_options() for the command line, cli_read_vertex_count() /
 * cli_read_int() for the stdin parameters and cli_print_enumerated_graphs()
 * for the output format ("count, then one graph per line: m u1 v1 u2 v2 ...").
 * The maximum n a CLI passes to cli_read_vertex_count() is the enumerator's
 * own representable range, never a size limit chosen by the CLI.
 */

#include "util/vertex_count.h"

#include <cstring>
#include <exception>
#include <iostream>
#include <new>
#include <stdexcept>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Reports an exception that escaped a CLI body and returns its exit status
 *
 * std::invalid_argument means the input was outside what the algorithm
 * accepts (status 1, like malformed input); everything else means the
 * algorithm could not answer (status 2).
 */
inline int cli_report_exception(const std::exception& e, bool out_of_memory,
                                bool invalid_input = false) {
    if (out_of_memory) {
        std::cerr << "error: out of memory (std::bad_alloc); result unknown\n";
        return 2;
    }
    if (invalid_input) {
        std::cerr << "error: " << e.what() << "\n";
        return 1;
    }
    std::cerr << "error: " << e.what() << "; result unknown\n";
    return 2;
}

/**
 * @brief Runs a CLI body, turning an escaping exception into a diagnostic and a non-zero status
 * @param body The program (the former main()), returning its exit status
 */
inline int run_cli_main(int (*body)()) {
    try {
        return body();
    } catch (const std::bad_alloc& e) {
        return cli_report_exception(e, true);
    } catch (const std::invalid_argument& e) {
        return cli_report_exception(e, false, true);
    } catch (const std::exception& e) {
        return cli_report_exception(e, false);
    }
}

/**
 * @brief Same as run_cli_main(int (*)()) for a body that takes the command line
 */
inline int run_cli_main(int (*body)(int, char**), int argc, char** argv) {
    try {
        return body(argc, argv);
    } catch (const std::bad_alloc& e) {
        return cli_report_exception(e, true);
    } catch (const std::invalid_argument& e) {
        return cli_report_exception(e, false, true);
    } catch (const std::exception& e) {
        return cli_report_exception(e, false);
    }
}

/**
 * @brief Parses the command line of an enumeration CLI
 * @param argc Argument count as passed to main()
 * @param argv Argument vector as passed to main()
 * @param connected_only Set to true when "--connected" is given; pass 0 for a
 *        CLI that accepts no option at all
 * @return true on success; on an unknown option a diagnostic goes to stderr and
 *         false is returned (the caller exits with status 1)
 *
 * A mistyped option used to be ignored, which made the CLI answer a different
 * question ("--conected" enumerated all graphs instead of the connected ones)
 * with exit status 0.
 */
inline bool cli_parse_enum_options(int argc, char** argv, bool* connected_only) {
    for (int i = 1; i < argc; ++i) {
        if (connected_only != 0 && std::strcmp(argv[i], "--connected") == 0) {
            *connected_only = true;
            continue;
        }
        std::cerr << "error: unknown option " << argv[i]
                  << (connected_only != 0 ? " (only --connected is accepted)\n"
                                          : " (no option is accepted)\n");
        return false;
    }
    return true;
}

/**
 * @brief Reads one integer parameter of an enumeration CLI from stdin
 * @param in Input stream
 * @param name What the integer is, for the diagnostic ("n", "k", "girth")
 * @param lo Smallest accepted value
 * @param hi Largest accepted value
 * @param[out] value The integer read
 * @return true on success; when the integer is missing or unparsable, or lies
 *         outside lo..hi, a diagnostic goes to stderr and false is returned
 *         (the caller exits with status 1)
 */
inline bool cli_read_int(std::istream& in, const char* name, int lo, int hi,
                         int& value) {
    if (!(in >> value)) {
        std::cerr << "error: expected " << name << " on stdin\n";
        return false;
    }
    if (value < lo || value > hi) {
        std::cerr << "error: " << name << " = " << value
                  << " is outside the supported range " << lo << ".." << hi
                  << "\n";
        return false;
    }
    return true;
}

/**
 * @brief Reads the vertex count of an enumeration CLI from stdin
 * @param in Input stream
 * @param max_n Largest n the enumerator can represent (max_vertex_count when
 *        it has no tighter bound; 63 or 64 for the 64-bit bitmask enumerators,
 *        33 for the directed canonical form, ...). Take it from the
 *        enumerator's header, never invent one: an n beyond the enumerator's
 *        range used to be printed as the count 0, which is false for every
 *        class here.
 * @param[out] n The vertex count read
 * @return true on success; a missing, negative or too large n is reported on
 *         stderr and false is returned (the caller exits with status 1)
 */
inline bool cli_read_vertex_count(std::istream& in, int max_n, int& n) {
    return cli_read_int(in, "n", 0, max_n, n);
}

/**
 * @brief Prints one enumerated graph in the CLI format: "m u1 v1 u2 v2 ..."
 */
inline void cli_print_edge_line(const std::vector<std::pair<int, int> >& edges) {
    std::cout << edges.size();
    for (size_t j = 0; j < edges.size(); ++j) {
        std::cout << " " << edges[j].first << " " << edges[j].second;
    }
    std::cout << "\n";
}

/**
 * @brief Prints an enumeration result in the CLI format
 * @tparam G Any enumerated-graph type with a std::vector<std::pair<int, int> > `edges`
 *
 * The first line is the number of graphs, then one line per graph:
 * the edge count m followed by the m edges "u v" (1-indexed).
 */
template <class G>
inline void cli_print_enumerated_graphs(const std::vector<G>& graphs) {
    std::cout << graphs.size() << "\n";
    for (size_t i = 0; i < graphs.size(); ++i) {
        cli_print_edge_line(graphs[i].edges);
    }
}

}  // namespace graph_recognition

#endif  // GRAPH_RECOGNITION_CLI_MAIN_H
