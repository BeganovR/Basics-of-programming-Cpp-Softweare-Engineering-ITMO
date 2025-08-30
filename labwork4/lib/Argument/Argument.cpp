#include "Argument.h"

#include <iostream>
#include <string>

#include "../Functions/Functions.h"

Argument& Argument::StoreValue(std::string& value) {
    external_value_ = &value;
    return *this;
}

Argument& Argument::StoreValues(std::vector<int>& external_values) {
    stored_values_ = &external_values;
    return *this;
}

Argument& Argument::StoreValue(bool& external_flag_value) {
    external_flag_value_ = &external_flag_value;
    return *this;
}

Argument& Argument::MultiValue() {
    is_multivalue_ = true;
    return *this;
}

Argument& Argument::MultiValue(size_t min_count) {
    is_multivalue_ = true;
    min_multivalue_count_ = min_count;
    return *this;
}

Argument& Argument::AddIntMulti(int value) {
    if (!is_multivalue_) {
        throw std::runtime_error("Argument is not multivalue");
    }
    int_values_.push_back(value);
    if (stored_values_) {
        stored_values_->push_back(value);
    }
    return *this;
}

Argument& Argument::Positional() {
    is_positional_ = true;
    return *this;
}

Argument& Argument::SetStringValue(const std::string& value) {
    string_value_ = value;
    if (external_value_) {
        *external_value_ = value;
    }
    return *this;
}

Argument& Argument::SetRequired(bool required) {
    is_required_ = required;
    return *this;
}

Argument& Argument::SetIntValue(int value) {
    int_value_ = value;
    is_int_ = true;
    return *this;
}

Argument& Argument::SetIsInt(bool is_int) {
    is_int_ = is_int;
    return *this;
}

Argument& Argument::SetAsFlag() {
    is_flag_ = true;
    flag_value_ = false;
    return *this;
}

Argument& Argument::SetAsHelp() {
    is_help_ = true;
    return *this;
}

Argument& Argument::SetDescription(const std::string& description) {
    description_ = description;
    return *this;
}

void Argument::SetFlagValue(bool value) {
    flag_value_ = value;
    if (external_flag_value_) {
        *external_flag_value_ = value;
    }
}

Argument& Argument::Default(const std::string default_string) {
    default_string_ = default_string;
    return *this;
}

Argument& Argument::Default(const int default_value) {
    default_int_ = default_value;
    return *this;
}