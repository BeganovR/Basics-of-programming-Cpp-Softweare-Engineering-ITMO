#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "../Argument/Argument.h"

namespace ArgumentParser {

class ArgParser {
   public:
    ArgParser(const std::string& name) : name_(name) {}

    // Parsing arguments
    bool Parse(const std::vector<std::string>& args);

    // String methods
    Argument& AddStringArgument(const std::string& long_name);
    Argument& AddStringArgument(char short_name, const std::string& long_name);
    Argument& AddStringArgument(char short_name, const std::string& long_name,
                                const std::string& description);
    const std::string GetStringValue(const std::string& name) const;

    // Int methods
    Argument& AddIntArgument(const std::string& long_name);
    Argument& AddIntArgument(const std::string& long_name,
                             const std::string& description);
    Argument& AddIntArgument(char short_name, const std::string& long_name);
    Argument& AddIntArgument(char short_name, const std::string& long_name,
                             const std::string& desription);
    int GetIntValue(const std::string& name, int default_value) const;
    int GetIntValue(const std::string& name) const {
        return GetIntValue(name, 0);
    }

    // Flag methods
    Argument& AddFlag(const std::string& long_name);
    Argument& AddFlag(char short_name, const std::string& long_name);
    Argument& AddFlag(char short_name, const std::string& long_name,
                      const std::string& description);
    bool GetFlag(const std::string& name) const;

    // Help
    Argument& AddHelp(char short_name, const std::string& long_name,
                      const std::string description);
    bool Help() const;

   private:
    std::string name_;
    std::unordered_map<std::string, Argument> string_arguments_;
    std::unordered_map<std::string, Argument> int_arguments_;
    std::unordered_map<std::string, Argument> flag_arguments_;
    std::unordered_map<std::string, Argument> help_arguments_;
    std::unordered_map<char, std::string> short_to_long_;
    std::vector<Argument*> positional_arguments_;
};

}  // namespace ArgumentParser