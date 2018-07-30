// LIST HEADER

#pragma once

#include "configuration.h"

namespace sqlcheck {

// LOGICAL DATABASE DESIGN

void CheckMultiValuedAttribute(Configuration& state,
                               const std::string& sql_statement,
                               bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckRecursiveDependency(Configuration& state,
                              const std::string& sql_statement,
                              bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckPrimaryKeyExists(Configuration& state,
                           const std::string& sql_statement,
                           bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckGenericPrimaryKey(Configuration& state,
                            const std::string& sql_statement,
                            bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckForeignKeyExists(Configuration& state,
                           const std::string& sql_statement,
                           bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckVariableAttribute(Configuration& state,
                            const std::string& sql_statement,
                            bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckMetadataTribbles(Configuration& state,
                           const std::string& sql_statement,
                           bool& print_statement, int fileLine, std::vector<int> line_location);

// PHYSICAL DATABASE DESIGN

void CheckFloat(Configuration& state,
                const std::string& sql_statement,
                bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckValuesInDefinition(Configuration& state,
                             const std::string& sql_statement,
                             bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckExternalFiles(Configuration& state,
                        const std::string& sql_statement,
                        bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckIndexCount(Configuration& state,
                     const std::string& sql_statement,
                     bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckIndexAttributeOrder(Configuration& state,
                              const std::string& sql_statement,
                              bool& print_statement, int fileLine, std::vector<int> line_location);

// QUERY

void CheckSelectStar(Configuration& state,
                     const std::string& sql_statement,
                     bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckNullUsage(Configuration& state,
                    const std::string& sql_statement,
                    bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckNotNullUsage(Configuration& state,
                       const std::string& sql_statement,
                       bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckConcatenation(Configuration& state,
                        const std::string& sql_statement,
                        bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckGroupByUsage(Configuration& state,
                       const std::string& sql_statement,
                       bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckOrderByRand(Configuration& state,
                      const std::string& sql_statement,
                      bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckPatternMatching(Configuration& state,
                          const std::string& sql_statement,
                          bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckSpaghettiQuery(Configuration& state,
                         const std::string& sql_statement,
                         bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckJoinCount(Configuration& state,
                         const std::string& sql_statement,
                         bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckDistinctCount(Configuration& state,
                        const std::string& sql_statement,
                        bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckImplicitColumns(Configuration& state,
                          const std::string& sql_statement,
                          bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckHaving(Configuration& state,
                 const std::string& sql_statement,
                 bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckNesting(Configuration& state,
                  const std::string& sql_statement,
                  bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckOr(Configuration& state,
             const std::string& sql_statement,
             bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckUnion(Configuration& state,
                const std::string& sql_statement,
                bool& print_statement, int fileLine, std::vector<int> line_location);

void CheckDistinctJoin(Configuration& state,
                       const std::string& sql_statement,
                       bool& print_statement, int fileLine, std::vector<int> line_location);

// APPLICATION

void CheckReadablePasswords(Configuration& state,
                            const std::string& sql_statement,
                            bool& print_statement, int fileLine, std::vector<int> line_location);


}  // namespace machine
