#pragma once

#include <fstream>
#include <iostream>
#include <string>

void SaveToFile(std::string& filename, const std::string& data);

bool CheckiInFile(const std::string& filename, std::string& data);

void DeleteFromFile(std::string& filename);
