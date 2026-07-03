#include "StageLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
std::vector<std::vector<int>> StageLoader::Load(const std::string& filePath)
{
    std::ifstream file(filePath);

    if (!file.is_open()) {
        throw std::runtime_error("CSVファイルを開けません");
    }

    std::vector<std::vector<int>> data;
    std::string line;

    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            row.push_back(std::stoi(cell));
        }

        data.push_back(row);
    }

    return data;
}
