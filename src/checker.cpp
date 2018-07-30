// CHECKER SOURCE

#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <memory>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <functional>
#include <regex>
#include <map>
#include <vector>
#include <algorithm>
#include "checker.h"
#include "include/configuration.h"
#include "include/list.h"
#include "include/color.h"

namespace sqlcheck {

	void Check(Configuration& state) {

		std::unique_ptr<std::istream> input;

		// Set up stream
		if (state.testing_mode == true) {
			input.reset(state.test_stream.release());
		}
		else if (state.file_name.empty()) {
			input.reset(&std::cin);
		}
		else {
			input.reset(new std::ifstream(state.file_name.c_str()));
		}

		std::stringstream sql_statement;
		const size_t fragment_size = 4096;
		char buffer[fragment_size];


		// Go over the input stream
		int fileLine = 0;
		int currentCommanCounter = 0;
		std::vector<int> line_location;
		bool commentedCode = false;
		bool before_comment_code_valid =false;
		std::string first_part;
		std::string second_part;
		while (!input->eof()) {

			// Get a line from the input stream
			input->getline(buffer, fragment_size);
			std::string statement_fragment(buffer);

			fileLine++;
			
			statement_fragment = std::regex_replace(statement_fragment, std::regex("^ +| +$|( ) +"), "$1");
			statement_fragment.erase(std::remove(statement_fragment.begin(), statement_fragment.end(), '\t'), statement_fragment.end());

			if (!commentedCode) {
				int startPos = statement_fragment.rfind("/*");
				if (startPos != -1) {
					commentedCode = true;

					if (startPos > 0) {
						//if pattern found in the middle of the statement
						first_part = statement_fragment.substr(0, startPos);
						first_part = std::regex_replace(first_part, std::regex("^ +| +$|( ) +"), "$1");
						if (first_part.empty() == false) {
							before_comment_code_valid = true;
						}
					}
				} //end of inner in block if(startPos > 0)
			}

			if (commentedCode) {
				int pos = statement_fragment.find("*/");
				if (pos != -1) {
					commentedCode = false;
					if (statement_fragment.size() - 1 > pos + 1) {
						//if pattern found in the middle of the statement
						second_part = statement_fragment.substr(pos + 2, statement_fragment.size());
						second_part = std::regex_replace(second_part, std::regex("^ +| +$|( ) +"), "$1");
						if (second_part.empty() == false) {
							first_part = first_part +" "+second_part;
						}
					}
					//pattern is found in the beginning 
					else {
						statement_fragment = "";
					}
				}
			}
			if(before_comment_code_valid){
				statement_fragment = first_part;
			}
			
			if ((commentedCode && !before_comment_code_valid)||statement_fragment.rfind("--", 0) == 0 || statement_fragment.empty() == true) {
				if (currentCommanCounter != 0) {
					int size = line_location.size();
					if (size > 0) {
						int last_row_len = line_location[size - 1];
						line_location.push_back(last_row_len);
					}
				}//end of if block
				continue;
			}

			if (currentCommanCounter == 0) {
				currentCommanCounter = fileLine;
			}

			sql_statement << statement_fragment << " ";
			//add fragment length to vector
			int size = line_location.size();
			if (size > 0) {
				int last_row_len = line_location[size - 1];
				line_location.push_back((last_row_len + statement_fragment.length() + 1));
			}
			else {
				line_location.push_back((statement_fragment.length() + 1));
			}

			// Check for delimiter in line
			before_comment_code_valid = false;
			
			std::size_t location = statement_fragment.find(state.delimiter);
			if (location != std::string::npos)
			{
				//sql_statement.str().pop_back();
			  // Check the statement
				CheckStatement(state, sql_statement.str(), currentCommanCounter, line_location);

				// Reset statement
				sql_statement.str(std::string());

				currentCommanCounter = 0;
				line_location.clear();

			}

		}//end of while loop

		// Skip destroying std::cin
		if (state.file_name.empty()) {
			input.release();
		}

	}
	bool ends_with(std::string const & value, std::string const & ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}
	// Wrap the text
	std::string WrapText(const std::string& text) {

		size_t line_length = 80;

		std::istringstream words(text);
		std::ostringstream wrapped;
		std::string word;
		bool newline = false;
		bool newpara = false;

		if (words >> word) {

			wrapped << word;

			size_t space_left = line_length - word.length();
			while (words >> word) {
				if (word == "●") {
					wrapped << "\n\n";
					newpara = true;
				}
				else {
					newpara = false;
				}

				if (space_left < word.length() + 1 || newline) {
					wrapped << '\n' << word;
					space_left = line_length - word.length();
				}
				else {
					if (newpara == false) {
						wrapped << ' ' << word;
					}
					else {
						wrapped << word;
					}
					space_left -= word.length() + 1;
				}

				if (word.back() == ':') {
					newline = true;
				}
				else {
					newline = false;
				}
			}

		}

		return wrapped.str();
	}

	void PrintMessage(Configuration& state,
		const std::string sql_statement,
		const bool print_statement,
		const RiskLevel pattern_risk_level,
		const PatternType pattern_type,
		const std::string title,
		const std::string message) {

		if (state.file_name.empty() == false) {
			std::cout << state.file_name << "," << title << ",";
		}


		// Print detailed message only in verbose mode
		if (state.verbose == true) {
			std::cout << WrapText(message) << "\n";
		}

		// Update checker stats
		state.checker_stats[pattern_risk_level]++;
		state.checker_stats[RISK_LEVEL_ALL]++;

	}

	void CheckPattern(Configuration& state,
		const std::string& sql_statement,
		bool& print_statement,
		const std::regex& anti_pattern,
		const RiskLevel pattern_risk_level,
		const PatternType pattern_type,
		const std::string title,
		const std::string message,
		const bool exists,
		int fileLine, std::vector<int> line_location, const size_t min_count) {

		//std::cout << "PATTERN LEVEL: " << pattern_risk_level << "\n";
		//std::cout << "CHECKER LEVEL: " << state.log_level << "\n";

		// Check log level
		if (pattern_risk_level < state.risk_level) {
			return;
		}

		bool found = false;
		std::smatch match;
		std::size_t count = 0;


		try {
			std::sregex_iterator next(sql_statement.begin(),
				sql_statement.end(),
				anti_pattern);
			std::sregex_iterator end;
			while (next != end) {
				match = *next;
				found = true;
				count++;
				next++;
			}
		}
		catch (std::regex_error& e) {
			// Syntax error in the regular expression
		}


		if (found == exists && count > min_count) {

			PrintMessage(state,
				sql_statement,
				print_statement,
				pattern_risk_level,
				pattern_type,
				title,
				message);
			//std::cout << "\n sql statement:"<< sql_statement << "\n";


			//following logic finds the line no in a file where matching expression is found.
			int offset = 0;

			/*for (int i = 0; i < line_location.size(); i++) {
				std::cout << line_location[i]<<"\n";
			}*/


			if (match.str(0).empty() == false) {
				int pos = sql_statement.find(match.str(0));
				//std::cout << "SQL statement : " << sql_statement << "\n";
				//std::cout << "Matching expression : " << match.str(0) << "\n";
				//std::cout << "Matching position : "<<pos  <<"\n";
				if (pos != std::string::npos) {
					for (int aa = 0; aa < line_location.size(); aa++) {
						if (pos < line_location[aa]) {
							break;
						}
						offset++;
					}//end of for loop
				}
			}//end of outer if block*/

			//std::cout << "Printing offset: " << offset<<"\n";

			if (exists == true) {
				ColorModifier blue(ColorCode::FG_BLUE, state.color_mode, true);
				ColorModifier regular(ColorCode::FG_DEFAULT, state.color_mode, false);
				//std::cout << "file line :" << fileLine<<"\n";
				std::cout << (fileLine + offset);
				//std::cout <<"fileLine+ offset:" <<(fileLine+ offset)<<"\n";


				std::cout << "\n";
			}

			// TOGGLE PRINT STATEMENT
			print_statement = false;
		}

	}

	void CheckStatement(Configuration& state,
		const std::string& sql_statement, int fileLine, std::vector<int> line_location) {

		// TRANSFORM TO LOWER CASE
		auto statement = sql_statement;

		std::transform(statement.begin(),
			statement.end(),
			statement.begin(),
			::tolower);

		// RESET
		bool print_statement = true;

		// LOGICAL DATABASE DESIGN

		CheckMultiValuedAttribute(state, statement, print_statement, fileLine, line_location);

		CheckRecursiveDependency(state, statement, print_statement, fileLine, line_location);

		CheckPrimaryKeyExists(state, statement, print_statement, fileLine, line_location);

		CheckGenericPrimaryKey(state, statement, print_statement, fileLine, line_location);

		CheckForeignKeyExists(state, statement, print_statement, fileLine, line_location);

		CheckVariableAttribute(state, statement, print_statement, fileLine, line_location);

		CheckMetadataTribbles(state, statement, print_statement, fileLine, line_location);

		// PHYSICAL DATABASE DESIGN

		CheckFloat(state, statement, print_statement, fileLine, line_location);

		CheckValuesInDefinition(state, statement, print_statement, fileLine, line_location);

		CheckExternalFiles(state, statement, print_statement, fileLine, line_location);

		CheckIndexCount(state, statement, print_statement, fileLine, line_location);

		CheckIndexAttributeOrder(state, statement, print_statement, fileLine, line_location);

		// QUERY

		CheckSelectStar(state, statement, print_statement, fileLine, line_location);

		CheckNullUsage(state, statement, print_statement, fileLine, line_location);

		CheckNotNullUsage(state, statement, print_statement, fileLine, line_location);

		CheckConcatenation(state, statement, print_statement, fileLine, line_location);

		CheckGroupByUsage(state, statement, print_statement, fileLine, line_location);

		CheckOrderByRand(state, statement, print_statement, fileLine, line_location);

		CheckPatternMatching(state, statement, print_statement, fileLine, line_location);

		CheckSpaghettiQuery(state, statement, print_statement, fileLine, line_location);

		CheckJoinCount(state, statement, print_statement, fileLine, line_location);

		CheckDistinctCount(state, statement, print_statement, fileLine, line_location);

		CheckImplicitColumns(state, statement, print_statement, fileLine, line_location);

		CheckHaving(state, statement, print_statement, fileLine, line_location);

		CheckNesting(state, statement, print_statement, fileLine, line_location);

		CheckOr(state, statement, print_statement, fileLine, line_location);

		CheckUnion(state, statement, print_statement, fileLine, line_location);

		CheckDistinctJoin(state, statement, print_statement, fileLine, line_location);

		// APPLICATION

		CheckReadablePasswords(state, statement, print_statement, fileLine, line_location);


	}

}  // namespace machine

