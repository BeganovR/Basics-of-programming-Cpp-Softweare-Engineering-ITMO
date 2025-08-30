#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class Argument {
   public:
    Argument(char short_name = '\0', const std::string& long_name = "")
        : short_name_(short_name),
          long_name_(long_name),
          is_required_(false) {};

    // Getters
    char GetShortName() const { return short_name_; }
    const std::string GetLongName() const { return long_name_; }
    const std::string GetHelpDesctiption() const { return help_description_; }
    const std::string GetStringValueA() const {
        return string_value_.empty() && default_string_ ? *default_string_
                                                        : string_value_;
    };
    bool GetRequired() const { return is_required_; }
    int GetIntValueA() const {
        return (int_value_ == 0 && default_int_.has_value()) ? *default_int_
                                                             : int_value_;
    };
    std::vector<int> GetIntValues() const {
        return is_multivalue_ ? int_values_ : std::vector<int>{int_value_};
    }
    std::vector<int>* GetStoredValues() { return stored_values_; }
    size_t GetMinMultiValue() const { return min_multivalue_count_; }
    bool GetFlagValue() const { return flag_value_; }
    const std::string& GetDescription() const { return description_; }

    // Setters
    Argument& SetStringValue(const std::string& value);
    Argument& SetRequired(bool required);
    Argument& SetIntValue(int value);
    Argument& SetIsInt(bool is_int);
    Argument& SetAsFlag();
    Argument& SetAsHelp();
    Argument& SetDescription(const std::string& description);
    void SetFlagValue(bool value);
    void SetPositional(bool positional) { is_positional_ = positional; }

    // Defaults
    Argument& Default(const std::string default_string);
    Argument& Default(const int default_int);

    // StoreValue
    Argument& StoreValue(std::string& external_value);
    Argument& StoreValues(std::vector<int>& external_values);
    Argument& StoreValue(bool& external_flag_value);

    // Checkers
    bool IsIntArgument() const { return is_int_; }
    bool IsMultivalue() const { return is_multivalue_; }
    bool IsValidMultiValue() const {
        return int_values_.size() >= min_multivalue_count_;
    }
    bool IsFlag() const { return is_flag_; }
    bool IsPositional() const { return is_positional_; }
    bool IsHelp() const { return is_help_; }

    // MultiValue
    Argument& MultiValue();
    Argument& MultiValue(size_t min_count);
    Argument& AddIntMulti(int value);

    // Positional
    Argument& Positional();

   private:
    // Long and short names for flags
    char short_name_ = '\0';
    std::string long_name_;
    std::string help_description_;

    // Default values
    std::optional<int> default_int_;
    std::optional<std::string> default_string_;

    // For functions
    bool is_positional_ = false;
    bool is_multivalue_ = false;
    size_t min_multivalue_count_ = 0;
    bool is_required_;
    bool is_int_ = false;
    bool is_flag_ = false;
    bool flag_value_ = false;
    bool is_help_ = false;

    std::string description_;
    std::string string_value_;
    std::vector<int> int_values_;
    std::string* external_value_ = nullptr;
    std::vector<int>* stored_values_ = nullptr;
    bool* external_flag_value_ = nullptr;
    int int_value_ = 0;
};