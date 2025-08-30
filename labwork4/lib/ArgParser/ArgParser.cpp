#include "ArgParser.h"

#include <iostream>

#include "../Functions/Functions.h"

namespace ArgumentParser {

// Work with string
Argument& ArgParser::AddStringArgument(const std::string& long_name) {
    auto& argument = string_arguments_[long_name];
    argument = Argument('\0', long_name);
    // Set as mandatory
    argument.SetRequired(true);

    // new
    if (argument.IsPositional()) {
        positional_arguments_.push_back(&argument);
    }
    //
    return argument;
}

Argument& ArgParser::AddStringArgument(char short_name,
                                       const std::string& long_name) {
    auto& argument = string_arguments_[long_name];
    argument = Argument(short_name, long_name);
    short_to_long_[short_name] = long_name;
    // Set as mandatory
    argument.SetRequired(true);
    return argument;
}

Argument& ArgParser::AddStringArgument(char short_name,
                                       const std::string& long_name,
                                       const std::string& description) {
    auto& argument = string_arguments_[long_name];
    argument = Argument(short_name, long_name);
    short_to_long_[short_name] = long_name;
    // Set as mandatory
    argument.SetRequired(true).SetDescription(description);
    return argument;
}

// Work with int

Argument& ArgParser::AddIntArgument(const std::string& long_name) {
    auto& argument = int_arguments_[long_name];
    argument = Argument('\0', long_name);
    argument.SetRequired(true).SetIsInt(true);
    return argument;
}

Argument& ArgParser::AddIntArgument(const std::string& long_name,
                                    const std::string& description) {
    auto& argument = int_arguments_[long_name];
    argument = Argument('\0', long_name);
    argument.SetRequired(true).SetIsInt(true).SetDescription(description);
    return argument;
}

Argument& ArgParser::AddIntArgument(char short_name,
                                    const std::string& long_name) {
    auto& argument = int_arguments_[long_name];
    argument = Argument(short_name, long_name);
    short_to_long_[short_name] = long_name;
    std::cout << "Mapping short name " << short_name << " to " << long_name
              << "\n";

    argument.SetRequired(true).SetIsInt(true);
    return argument;
}

Argument& ArgParser::AddIntArgument(char short_name,
                                    const std::string& long_name,
                                    const std::string& description) {
    auto& argument = int_arguments_[long_name];
    argument = Argument(short_name, long_name);
    std::cout << "Mapping short name " << short_name << " to " << long_name
              << "\n";

    short_to_long_[short_name] = long_name;
    argument.SetRequired(true).SetIsInt(true).SetDescription(description);
    return argument;
}

int ArgParser::GetIntValue(const std::string& name, int default_value) const {
    auto it = int_arguments_.find(name);
    if (it == int_arguments_.end()) {
        return default_value;
    }
    if (it->second.IsMultivalue()) {
        if (!it->second.GetIntValues().empty()) {
            return it->second.GetIntValues().front();
        }
        throw std::runtime_error(
            "No values provided for multivalue argument: " + name);
    }
    return it->second.GetIntValueA();
}

Argument& ArgParser::AddFlag(const std::string& long_name) {
    auto& argument = flag_arguments_[long_name];
    argument = Argument('\0', long_name);
    argument.SetAsFlag();
    return argument;
}

Argument& ArgParser::AddFlag(char short_name, const std::string& long_name) {
    if (flag_arguments_.count(long_name)) {
        throw std::runtime_error("Flag already exists: " + long_name);
    }
    auto& argument = flag_arguments_[long_name];
    argument = Argument(short_name, long_name);
    short_to_long_[short_name] = long_name;
    argument.SetAsFlag();
    return argument;
}

Argument& ArgParser::AddFlag(char short_name, const std::string& long_name,
                             const std::string& description) {
    if (flag_arguments_.count(long_name)) {
        throw std::runtime_error("Flag already exists: " + long_name);
    }
    auto& argument = flag_arguments_[long_name];
    argument = Argument(short_name, long_name);
    short_to_long_[short_name] = long_name;
    argument.SetAsFlag().SetDescription(description);
    return argument;
}

bool ArgParser::GetFlag(const std::string& name) const {
    auto it = flag_arguments_.find(name);
    if (it != flag_arguments_.end()) {
        return it->second.GetFlagValue();
    }
    throw std::runtime_error("Flag not found: " + name);
}

Argument& ArgParser::AddHelp(char short_name, const std::string& long_name,
                             const std::string description) {
    help_arguments_[long_name] = Argument(short_name, long_name);
    auto& argument = help_arguments_[long_name];
    short_to_long_[short_name] = long_name;
    argument.SetAsHelp().SetDescription(description);
    return argument;
}

bool ArgParser::Help() const {
    std::cout << "Usage: [options]" << std::endl;
    for (const auto& [long_name, argument] : help_arguments_) {
        std::cout << "  --" << long_name << ", -" << argument.GetShortName()
                  << ": " << argument.GetHelpDesctiption() << std::endl;
    }
    for (const auto& [long_name, argument] : string_arguments_) {
        if (!argument.IsHelp()) {
            std::cout << "  --" << long_name << ", -" << argument.GetShortName()
                      << ": " << argument.GetDescription() << std::endl;
        }
    }
    for (const auto& [long_name, argument] : int_arguments_) {
        std::cout << "  --" << long_name << ", -" << argument.GetShortName()
                  << ": " << argument.GetDescription() << std::endl;
    }
    for (const auto& [long_name, argument] : flag_arguments_) {
        std::cout << "  --" << long_name << ", -" << argument.GetShortName()
                  << ": " << argument.GetDescription() << std::endl;
    }
    return true;
}

const std::string ArgParser::GetStringValue(const std::string& name) const {
    auto it = string_arguments_.find(name);
    if (it == string_arguments_.end()) {
        throw std::runtime_error("String argument not found: " + name);
    }
    return it->second.GetStringValueA();
}

bool ArgParser::Parse(const std::vector<std::string>& args) {
    if (args.size() == 1) {
        std::cerr << "No arguments provided.\n";
        for (const auto& pair : string_arguments_) {
            const auto& argument = pair.second;
            if (argument.GetRequired() && argument.GetStringValueA().empty()) {
                std::cerr << "Required argument " << argument.GetLongName()
                          << " not provided.\n";
                return false;
            }
        }
        return true;
    }

    size_t positional_idx = 0;

    for (size_t i = 1; i < args.size(); ++i) {
        const std::string arg = args[i];
        size_t pos = arg.find('=');
        std::cerr << "Processing argument: " << arg << "\n";

        // Help
        if (arg == "--help" || arg == "-h") {
            Help();
            return true;
        }

        // Check is positional
        if (arg[0] == '-') {
            if (pos == std::string::npos) {
                // Long Flag
                if (arg.substr(0, 2) == "--" && arg.size() > 2) {
                    std::string name = arg.substr(2);

                    if (int_arguments_.count(name)) {
                        if (i + 1 < args.size() && args[i + 1][0] != '-') {
                            bool success = true;
                            int int_value =
                                StrToInt(args[i + 1].c_str(), success);
                            if (success) {
                                auto& argument = int_arguments_[name];
                                if (argument.IsMultivalue()) {
                                    argument.AddIntMulti(int_value);
                                } else {
                                    argument.SetIntValue(int_value);
                                }
                                ++i;
                            } else {
                                std::cerr
                                    << "Invalid integer value for argument: "
                                    << name << std::endl;
                                return false;
                            }
                        } else {
                            std::cerr
                                << "No value provided for integer argument: "
                                << name << std::endl;
                            return false;
                        }
                    } else if (flag_arguments_.count(name)) {
                        flag_arguments_[name].SetFlagValue(true);
                    } else {
                        std::cerr << "Unknown flag: " << name << std::endl;
                        return false;
                    }

                    // Short Flag
                } else if (arg[0] == '-' && arg.size() > 1) {
                    char short_name = arg[1];
                    auto it = short_to_long_.find(short_name);
                    if (it != short_to_long_.end()) {
                        const std::string& long_name = it->second;
                        if (flag_arguments_.count(long_name)) {
                            flag_arguments_[long_name].SetFlagValue(true);
                        } else if (int_arguments_.count(long_name)) {
                            if (i + 1 < args.size() && args[i + 1][0] != '-') {
                                bool success = true;
                                int int_value =
                                    StrToInt(args[i + 1].c_str(), success);
                                if (success) {
                                    int_arguments_[long_name].SetIntValue(
                                        int_value);
                                    ++i;
                                } else {
                                    std::cerr << "Invalid integer value for "
                                                 "argument: "
                                              << long_name << std::endl;
                                    return false;
                                }
                            } else {
                                std::cerr << "No value provided for integer "
                                             "argument: "
                                          << long_name << std::endl;
                                return false;
                            }
                        } else if (string_arguments_.count(long_name)) {
                            if (i + 1 < args.size() && args[i + 1][0] != '-') {
                                string_arguments_[long_name].SetStringValue(
                                    args[i + 1]);
                                ++i;
                            } else {
                                std::cerr
                                    << "No value provided for string argument: "
                                    << long_name << std::endl;
                                return false;
                            }
                        } else {
                            std::cerr << "Unknown flag mapped to short name: "
                                      << short_name << std::endl;
                            return false;
                        }
                    } else {
                        std::cerr << "Unknown short flag: " << short_name
                                  << std::endl;
                        return false;
                    }
                }
            } else if (pos != std::string::npos) {
                // If long argument (--key=argument)
                if (arg.substr(0, 2) == "--" && arg.size() > 2) {
                    std::string name = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);

                    if (int_arguments_.count(name)) {
                        auto& argument = int_arguments_[name];
                        bool success = true;
                        int int_value = StrToInt(value.c_str(), success);

                        if (!success) {
                            std::cerr << "Invalid integer value for argument: "
                                      << name << std::endl;
                            return false;
                        }
                        if (argument.IsMultivalue()) {
                            argument.AddIntMulti(int_value);
                        } else {
                            argument.SetIntValue(int_value);
                        }

                    } else if (string_arguments_.count(name)) {
                        string_arguments_[name].SetStringValue(value);
                    } else {
                        std::cerr << "Unknown argument: " << name << std::endl;
                        return false;
                    }
                    // if short argument (-k=argument)
                } else if (arg.size() > 1 && arg[0] == '-' &&
                           pos != std::string::npos) {
                    char short_name = arg[1];
                    std::string value = arg.substr(pos + 1);

                    auto it = short_to_long_.find(short_name);
                    if (it == short_to_long_.end()) {
                        std::cerr << "Unknown short argument: " << short_name
                                  << std::endl;
                        return false;
                    }
                    const std::string& long_name = it->second;

                    if (int_arguments_.count(long_name)) {
                        auto& argument = int_arguments_[long_name];
                        bool success = true;
                        int int_value = StrToInt(value.c_str(), success);
                        if (!success) {
                            std::cerr << "Invalid integer value for argument: "
                                      << long_name << std::endl;
                            return false;
                        }
                        argument.SetIntValue(int_value);
                    } else if (string_arguments_.count(long_name)) {
                        string_arguments_[long_name].SetStringValue(value);
                    } else {
                        std::cerr << "Unknown argument: " << long_name
                                  << std::endl;
                        return false;
                    }
                } else {
                    std::cerr << "Invalid argument: " << arg << "\n";
                    return false;
                }
            } else {
                std::cerr << "Invalid argument: " << arg << "\n";
                return false;
            }
        } else if (arg[0] != '-') {
            bool success = true;
            int value = StrToInt(arg.c_str(), success);

            if (!success) {
                std::cerr << "Invalid positional argument value: " << arg
                          << std::endl;
                return false;
            }

            bool handle = false;
            for (auto& [name, argument] : int_arguments_) {
                if (argument.IsPositional()) {
                    if (argument.IsMultivalue()) {
                        argument.AddIntMulti(value);
                    } else {
                        argument.SetIntValue(value);
                    }
                    handle = true;
                    break;
                }
            }
            if (!handle) {
                std::cerr << "Unexpected positional argument: " << arg
                          << std::endl;
                return false;
            }
            continue;
        }
    }

    for (const auto& [name, argument] : int_arguments_) {
        if (argument.GetRequired() && argument.IsMultivalue() &&
            argument.GetIntValues().empty()) {
            std::cerr << "Required multi-value integer argument " << name
                      << " not provided or empty.\n";
            return false;
        }
        if (argument.IsMultivalue() && !argument.IsValidMultiValue()) {
            std::cerr << "Argument " << name << " requires at least "
                      << argument.GetMinMultiValue() << " values, but "
                      << argument.GetIntValues().size() << " were provided.\n";
            return false;
        }
    }

    for (const auto& [name, argument] : string_arguments_) {
        if (argument.GetRequired() && argument.GetStringValueA().empty()) {
            std::cerr << "Required string argument " << name
                      << " not provided.\n";
            return false;
        }
    }

    for (auto& [name, flag] : flag_arguments_) {
        if (!flag.GetFlagValue()) {
            flag.SetFlagValue(true);
        }
    }

    return true;
}

}  // namespace ArgumentParser