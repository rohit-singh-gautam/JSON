//////////////////////////////////////////////////////////////////////////
// Copyright (C) 2024  Rohit Jairaj Singh (rohit@singh.org.in)          //
//                                                                      //
// This program is free software: you can redistribute it and/or modify //
// it under the terms of the GNU General Public License as published by //
// the Free Software Foundation, either version 3 of the License, or    //
// (at your option) any later version.                                  //
//                                                                      //
// This program is distributed in the hope that it will be useful,      //
// but WITHOUT ANY WARRANTY; without even the implied warranty of       //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        //
// GNU General Public License for more details.                         //
//                                                                      //
// You should have received a copy of the GNU General Public License    //
// along with this program.  If not, see <https://www.gnu.org/licenses/ //
//////////////////////////////////////////////////////////////////////////


#include <json.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>

std::string ReadStringFromFile(const std::filesystem::path &path) {
    if (!std::filesystem::is_regular_file(path)) {
        throw std::invalid_argument { "Not a valid file" };
    }
    std::ifstream filestream { path };
    std::stringstream buffer { };
    buffer << filestream.rdbuf();

    std::string content = buffer.str();
    filestream.close();
    return content;
}

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        std::cout << "Usage: " << argv[0] << " <input json file> [output json file]" << std::endl;
        return 0;
    }

    auto original_text = ReadStringFromFile(argv[1]);
    auto json = rohit::json::Ref(original_text);
    auto beautified_text = json.write(rohit::json::format::beautify);

    if (argc == 3) {
        std::ofstream outfile(argv[2]);
        outfile << beautified_text;
        outfile.close();
    } else {
        std::cout << beautified_text;
    }
    return 0;
}