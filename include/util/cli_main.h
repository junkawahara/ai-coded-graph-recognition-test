#ifndef GRAPH_RECOGNITION_CLI_MAIN_H
#define GRAPH_RECOGNITION_CLI_MAIN_H

/**
 * @file cli_main.h
 * @brief Shared exception boundary of the command-line tools
 *
 * Every CLI source file (src/..._main.cpp) writes its program as cli_main() and lets main()
 * delegate to run_cli_main(), so that an exception escaping the recognizer or
 * enumerator ends the program with a diagnostic and a non-zero exit status
 * instead of std::terminate and a core dump.
 *
 * Exit status contract of the CLIs:
 *   - 0: the answer was printed
 *   - 1: malformed input (reported by the program itself)
 *   - 2: no answer -- an allocation failed (std::bad_alloc) or the algorithm
 *        gave up (std::runtime_error, e.g. a search budget); the reason goes to
 *        stderr and nothing is printed to stdout that could be read as YES/NO
 *
 * Several recognizers use Theta(n^2) or more memory even on sparse inputs
 * (this project accepts O(n^2), O(n^3) and O(2^n) algorithms; see the
 * complexity policy in CLAUDE.md), so a large input can legitimately end in
 * status 2 on a machine without enough memory.
 */

#include <exception>
#include <iostream>
#include <new>

namespace graph_recognition {

/**
 * @brief Reports an exception that escaped a CLI body and returns its exit status (2)
 */
inline int cli_report_exception(const std::exception& e, bool out_of_memory) {
    if (out_of_memory) {
        std::cerr << "error: out of memory (std::bad_alloc); result unknown\n";
    } else {
        std::cerr << "error: " << e.what() << "; result unknown\n";
    }
    return 2;
}

/**
 * @brief Runs a CLI body, turning an escaping exception into a diagnostic and exit status 2
 * @param body The program (the former main()), returning its exit status
 */
inline int run_cli_main(int (*body)()) {
    try {
        return body();
    } catch (const std::bad_alloc& e) {
        return cli_report_exception(e, true);
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
    } catch (const std::exception& e) {
        return cli_report_exception(e, false);
    }
}

}  // namespace graph_recognition

#endif  // GRAPH_RECOGNITION_CLI_MAIN_H
