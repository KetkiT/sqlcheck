// CHECKER HEADER

#pragma once

#include <regex>

#include "configuration.h"

namespace sqlcheck {

// Check a set of SQL statements
void Check(Configuration& state);

// Check a SQL statement
void CheckStatement(Configuration& state,
                    const std::string& sql_statement, int fileLline, std::vector<int> line_location);

// Check a pattern
void CheckPattern(Configuration& state,
                  const std::string& sql_statement,
                  bool& print_statement,
                  const std::regex& anti_pattern,
                  const RiskLevel pattern_level,
                  const PatternType pattern_type,
                  const std::string title,
                  const std::string message,
                  const bool exists,
                  int fileLline, std::vector<int> line_location, const size_t min_count = 0);

bool ends_with(std::string const & value, std::string const & ending);

}  // namespace machine
