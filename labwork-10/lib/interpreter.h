#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <variant>

#include "AST.h"
#include "lexer.h"
#include "parser.h"

class FunctionDeclaration;
class NativeFunction;

using FunctionPtr = std::shared_ptr<FunctionDeclaration>;
using NativeFunctionPtr = std::shared_ptr<NativeFunction>;

struct ValueType;

using List = std::vector<ValueType>;

struct ValueType : std::variant<std::monostate,     // nil
                                std::string,        // string
                                FunctionPtr,        // user-defined function
                                NativeFunctionPtr,  // native function
                                List,               // list
                                bool,               // boolean
                                double              // number
                                > {
    using variant::variant;
};

using Value = ValueType;

class NativeFunction {
   public:
    using FunctionType = std::function<Value(const std::vector<Value>&)>;

    explicit NativeFunction(FunctionType function)
        : function_(std::move(function)) {}

    Value operator()(const std::vector<Value>& args) const {
        return function_(args);
    }

   private:
    FunctionType function_;
};

class Environment {
   public:
    std::unordered_map<std::string, Value> variables;

    void Set(const std::string& name, const Value& value) {
        variables[name] = value;
    }

    Value Get(const std::string& name) const {
        auto it = variables.find(name);
        if (it == variables.end())
            throw std::runtime_error("Undefined variable: " + name);
        return it->second;
    }
};

class Interpreter {
   public:
    explicit Interpreter(std::ostream& out_stream) : out_(out_stream) {
        InitializeStdLib();
    }

    void Interpret(const std::shared_ptr<ASTNode>& node) {
        Exec(node, global_env_);
    }

   private:
    std::ostream& out_;
    Environment global_env_;
    std::vector<std::string> call_stack_;

    void InitializeStdLib() {
        AddMathFunctions();
        AddStringFunctions();
        AddListFunctions();
        AddSystemFunctions();

        AddHigherOrderFunctions();
    }

    Value CreateNativeFunction(
        std::function<Value(const std::vector<Value>&)> func) {
        return std::make_shared<NativeFunction>(std::move(func));
    }

    void AddMathFunctions() {
        // abs(x) - абсолютное значение
        global_env_.Set(
            "abs",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "abs() expects exactly 1 argument");

                if (!std::holds_alternative<double>(args[0]))
                    throw std::runtime_error("abs() expects a number argument");

                return std::abs(std::get<double>(args[0]));
            }));

        // ceil(x) - округление вверх
        global_env_.Set(
            "ceil",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "ceil() expects exactly 1 argument");

                if (!std::holds_alternative<double>(args[0]))
                    throw std::runtime_error(
                        "ceil() expects a number argument");

                return std::ceil(std::get<double>(args[0]));
            }));

        // floor(x) - округление вниз
        global_env_.Set(
            "floor",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "floor() expects exactly 1 argument");

                if (!std::holds_alternative<double>(args[0]))
                    throw std::runtime_error(
                        "floor() expects a number argument");

                return std::floor(std::get<double>(args[0]));
            }));

        // round(x) - округление до ближайшего целого
        global_env_.Set(
            "round",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "round() expects exactly 1 argument");

                if (!std::holds_alternative<double>(args[0]))
                    throw std::runtime_error(
                        "round() expects a number argument");

                return std::round(std::get<double>(args[0]));
            }));

        // sqrt(x) - квадратный корень
        global_env_.Set(
            "sqrt",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "sqrt() expects exactly 1 argument");

                if (!std::holds_alternative<double>(args[0]))
                    throw std::runtime_error(
                        "sqrt() expects a number argument");

                double val = std::get<double>(args[0]);
                if (val < 0)
                    throw std::runtime_error(
                        "sqrt() argument must be non-negative");

                return std::sqrt(val);
            }));

        // rnd(n) - случайное целое от 0 до n-1
        global_env_.Set(
            "rnd",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "rnd() expects exactly 1 argument");

                if (!std::holds_alternative<double>(args[0]))
                    throw std::runtime_error("rnd() expects a number argument");

                int max = static_cast<int>(std::get<double>(args[0]));
                if (max <= 0)
                    throw std::runtime_error("rnd() argument must be positive");

                static std::random_device rd;
                static std::mt19937 gen(rd());
                std::uniform_int_distribution<> distrib(0, max - 1);

                return static_cast<double>(distrib(gen));
            }));

        // parse_num(s) - преобразует строку в число если возможно, иначе nil
        global_env_.Set(
            "parse_num",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "parse_num() expects exactly 1 argument");

                if (!std::holds_alternative<std::string>(args[0]))
                    throw std::runtime_error(
                        "parse_num() expects a string argument");

                const std::string& str = std::get<std::string>(args[0]);
                try {
                    return std::stod(str);
                } catch (...) {
                    return std::monostate{};  // nil
                }
            }));

        // to_string(n) - преобразует число в строку
        global_env_.Set(
            "to_string",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "to_string() expects exactly 1 argument");

                if (!std::holds_alternative<double>(args[0]))
                    throw std::runtime_error(
                        "to_string() expects a number argument");

                return std::to_string(std::get<double>(args[0]));
            }));
    }

    void AddStringFunctions() {
        // len(s) - длина строки
        global_env_.Set(
            "len",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "len() expects exactly 1 argument");

                if (std::holds_alternative<std::string>(args[0])) {
                    return static_cast<double>(
                        std::get<std::string>(args[0]).length());
                } else if (std::holds_alternative<List>(args[0])) {
                    return static_cast<double>(std::get<List>(args[0]).size());
                } else {
                    throw std::runtime_error(
                        "len() expects a string or list argument");
                }
            }));

        // lower(s) - в нижний регистр
        global_env_.Set(
            "lower",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "lower() expects exactly 1 argument");

                if (!std::holds_alternative<std::string>(args[0]))
                    throw std::runtime_error(
                        "lower() expects a string argument");

                std::string result = std::get<std::string>(args[0]);
                std::transform(result.begin(), result.end(), result.begin(),
                               [](unsigned char c) { return std::tolower(c); });
                return result;
            }));

        // upper(s) - в верхний регистр
        global_env_.Set(
            "upper",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "upper() expects exactly 1 argument");

                if (!std::holds_alternative<std::string>(args[0]))
                    throw std::runtime_error(
                        "upper() expects a string argument");

                std::string result = std::get<std::string>(args[0]);
                std::transform(result.begin(), result.end(), result.begin(),
                               [](unsigned char c) { return std::toupper(c); });
                return result;
            }));

        // split(s, delim) - разделение строки
        global_env_.Set(
            "split",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 2)
                    throw std::runtime_error(
                        "split() expects exactly 2 arguments");

                if (!std::holds_alternative<std::string>(args[0]) ||
                    !std::holds_alternative<std::string>(args[1]))
                    throw std::runtime_error(
                        "split() expects string arguments");

                const std::string& str = std::get<std::string>(args[0]);
                const std::string& delim = std::get<std::string>(args[1]);

                List result;
                size_t start = 0;
                size_t end = str.find(delim);

                while (end != std::string::npos) {
                    result.push_back(str.substr(start, end - start));
                    start = end + delim.length();
                    end = str.find(delim, start);
                }

                result.push_back(str.substr(start));
                return result;
            }));

        // join(list, delim) - объединение списка в строку
        global_env_.Set(
            "join",
            CreateNativeFunction(
                [this](const std::vector<Value>& args) -> Value {
                    if (args.size() != 2)
                        throw std::runtime_error(
                            "join() expects exactly 2 arguments");

                    if (!std::holds_alternative<List>(args[0]))
                        throw std::runtime_error(
                            "join() expects a list as first argument");

                    if (!std::holds_alternative<std::string>(args[1]))
                        throw std::runtime_error(
                            "join() expects a string as second argument");

                    const List& list = std::get<List>(args[0]);
                    const std::string& delim = std::get<std::string>(args[1]);

                    std::string result;
                    for (size_t i = 0; i < list.size(); ++i) {
                        if (i > 0) result += delim;
                        result += ValueToString(list[i], true);
                    }

                    return result;
                }));

        // replace(s, old, new) - замена подстроки
        global_env_.Set(
            "replace",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 3)
                    throw std::runtime_error(
                        "replace() expects exactly 3 arguments");

                if (!std::holds_alternative<std::string>(args[0]) ||
                    !std::holds_alternative<std::string>(args[1]) ||
                    !std::holds_alternative<std::string>(args[2]))
                    throw std::runtime_error(
                        "replace() expects string arguments");

                std::string str = std::get<std::string>(args[0]);
                const std::string& old_str = std::get<std::string>(args[1]);
                const std::string& new_str = std::get<std::string>(args[2]);

                size_t pos = 0;
                while ((pos = str.find(old_str, pos)) != std::string::npos) {
                    str.replace(pos, old_str.length(), new_str);
                    pos += new_str.length();
                }

                return str;
            }));
    }

    void AddListFunctions() {
        // range(start, end, step) - генерирует список чисел
        global_env_.Set(
            "range",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 3)
                    throw std::runtime_error(
                        "range() expects exactly 3 arguments");

                for (const auto& arg : args) {
                    if (!std::holds_alternative<double>(arg))
                        throw std::runtime_error(
                            "range() expects number arguments");
                }

                double start = std::get<double>(args[0]);
                double end = std::get<double>(args[1]);
                double step = std::get<double>(args[2]);

                if (step == 0)
                    throw std::runtime_error("range() step cannot be zero");

                List result;
                if (step > 0) {
                    for (double i = start; i < end; i += step) {
                        result.push_back(i);
                    }
                } else {
                    for (double i = start; i > end; i += step) {
                        result.push_back(i);
                    }
                }

                return result;
            }));

        // push(list, item) - добавляет элемент в конец списка
        global_env_.Set(
            "push",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 2)
                    throw std::runtime_error(
                        "push() expects exactly 2 arguments");

                if (!std::holds_alternative<List>(args[0]))
                    throw std::runtime_error(
                        "push() expects a list as first argument");

                List list = std::get<List>(args[0]);
                list.push_back(args[1]);

                return list;
            }));

        // pop(list) - удаляет и возвращает последний элемент
        global_env_.Set(
            "pop",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "pop() expects exactly 1 argument");

                if (!std::holds_alternative<List>(args[0]))
                    throw std::runtime_error("pop() expects a list argument");

                List list = std::get<List>(args[0]);

                if (list.empty())
                    throw std::runtime_error("Cannot pop from an empty list");

                Value last = list.back();
                list.pop_back();

                return last;
            }));

        // insert(list, index, item) - вставляет элемент по индексу
        global_env_.Set(
            "insert",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                List list = std::get<List>(args[0]);
                int index = static_cast<int>(std::get<double>(args[1]));

                if (index < 0) index += list.size();

                if (index < 0 || index > static_cast<int>(list.size()))
                    throw std::runtime_error("Insert index out of range");

                list.insert(list.begin() + index, args[2]);

                return list;
            }));

        // remove(list, index) - удаляет элемент по индексу
        global_env_.Set(
            "remove",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 2)
                    throw std::runtime_error(
                        "remove() expects exactly 2 arguments");

                if (!std::holds_alternative<List>(args[0]))
                    throw std::runtime_error(
                        "remove() expects a list as first argument");

                if (!std::holds_alternative<double>(args[1]))
                    throw std::runtime_error(
                        "remove() expects a number as second argument");

                List list = std::get<List>(args[0]);
                int index = static_cast<int>(std::get<double>(args[1]));

                if (index < 0) index += list.size();

                if (index < 0 || index >= static_cast<int>(list.size()))
                    throw std::runtime_error("Remove index out of range");

                list.erase(list.begin() + index);

                return list;
            }));

        // sort(list) - сортирует список
        global_env_.Set(
            "sort",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (args.size() != 1)
                    throw std::runtime_error(
                        "sort() expects exactly 1 argument");

                if (!std::holds_alternative<List>(args[0]))
                    throw std::runtime_error("sort() expects a list argument");

                List list = std::get<List>(args[0]);

                bool all_numbers = true;
                bool all_strings = true;

                for (const auto& item : list) {
                    if (!std::holds_alternative<double>(item))
                        all_numbers = false;
                    if (!std::holds_alternative<std::string>(item))
                        all_strings = false;
                }

                if (all_numbers) {
                    std::sort(list.begin(), list.end(),
                              [](const Value& a, const Value& b) {
                                  return std::get<double>(a) <
                                         std::get<double>(b);
                              });
                } else if (all_strings) {
                    std::sort(list.begin(), list.end(),
                              [](const Value& a, const Value& b) {
                                  return std::get<std::string>(a) <
                                         std::get<std::string>(b);
                              });
                } else {
                    throw std::runtime_error(
                        "sort() requires all list elements to be of the same "
                        "type (numbers or strings)");
                }

                return list;
            }));
    }

    void AddSystemFunctions() {
        // println(x) - вывод с переводом строки
        global_env_.Set("println",
                        CreateNativeFunction(
                            [this](const std::vector<Value>& args) -> Value {
                                if (args.size() != 1)
                                    throw std::runtime_error(
                                        "println() expects exactly 1 argument");

                                out_ << ValueToString(args[0]) << std::endl;
                                return std::monostate{};  // nil
                            }));

        // read() - чтение строки из ввода
        global_env_.Set(
            "read",
            CreateNativeFunction([](const std::vector<Value>& args) -> Value {
                if (!args.empty())
                    throw std::runtime_error(
                        "read() doesn't expect any arguments");

                std::string input;
                std::getline(std::cin, input);

                return input;
            }));

        // stacktrace() - возвращает стек вызовов
        global_env_.Set(
            "stacktrace",
            CreateNativeFunction([this](
                                     const std::vector<Value>& args) -> Value {
                if (!args.empty())
                    throw std::runtime_error(
                        "stacktrace() doesn't expect any arguments");

                std::string trace = "Stack trace:\n";

                if (call_stack_.empty()) {
                    trace += "  <empty stack>\n";
                } else {
                    for (size_t i = call_stack_.size(); i > 0; --i) {
                        trace += std::to_string(call_stack_.size() - i + 1) +
                                 ": " + call_stack_[i - 1] + "\n";
                    }
                }

                return trace;
            }));

        // print(x) - вывод без перевода строки
        global_env_.Set("print",
                        CreateNativeFunction(
                            [this](const std::vector<Value>& args) -> Value {
                                if (args.size() != 1)
                                    throw std::runtime_error(
                                        "print() expects exactly 1 argument");

                                // Используем параметр for_output=true для
                                // вывода строк без кавычек
                                out_ << ValueToString(args[0], true);
                                return std::monostate{};
                            }));
    }

    void AddHigherOrderFunctions() {
        // map(func, list) - применяет функцию к каждому элементу списка
        global_env_.Set(
            "map",
            CreateNativeFunction([this](
                                     const std::vector<Value>& args) -> Value {
                if (args.size() != 2)
                    throw std::runtime_error(
                        "map() expects exactly 2 arguments");

                if (!std::holds_alternative<FunctionPtr>(args[0]) &&
                    !std::holds_alternative<NativeFunctionPtr>(args[0]))
                    throw std::runtime_error(
                        "map() first argument must be a function");

                if (!std::holds_alternative<List>(args[1]))
                    throw std::runtime_error(
                        "map() second argument must be a list");

                const List& list = std::get<List>(args[1]);
                List result;
                result.reserve(list.size());

                for (const auto& item : list) {
                    std::vector<Value> call_args = {item};

                    if (std::holds_alternative<FunctionPtr>(args[0])) {
                        auto func = std::get<FunctionPtr>(args[0]);
                        Environment local_env;

                        for (const auto& [name, value] :
                             global_env_.variables) {
                            local_env.Set(name, value);
                        }

                        if (func->parameters.size() != 1) {
                            throw std::runtime_error(
                                "Function for map must take exactly 1 "
                                "argument");
                        }

                        local_env.Set(func->parameters[0], item);

                        try {
                            Exec(func->body, local_env);
                            result.push_back(std::monostate{});
                        } catch (const ReturnValue& ret) {
                            result.push_back(ret.value);
                        }
                    } else {
                        auto native_func = std::get<NativeFunctionPtr>(args[0]);
                        result.push_back((*native_func)(call_args));
                    }
                }

                return result;
            }));

        // filter(func, list) - фильтрует список по предикату
        global_env_.Set(
            "filter",
            CreateNativeFunction([this](
                                     const std::vector<Value>& args) -> Value {
                if (args.size() != 2)
                    throw std::runtime_error(
                        "filter() expects exactly 2 arguments");

                if (!std::holds_alternative<FunctionPtr>(args[0]) &&
                    !std::holds_alternative<NativeFunctionPtr>(args[0]))
                    throw std::runtime_error(
                        "filter() first argument must be a function");

                if (!std::holds_alternative<List>(args[1]))
                    throw std::runtime_error(
                        "filter() second argument must be a list");

                const List& list = std::get<List>(args[1]);
                List result;

                for (const auto& item : list) {
                    std::vector<Value> call_args = {item};
                    bool keep = false;

                    if (std::holds_alternative<FunctionPtr>(args[0])) {
                        auto func = std::get<FunctionPtr>(args[0]);
                        Environment local_env;

                        for (const auto& [name, value] :
                             global_env_.variables) {
                            local_env.Set(name, value);
                        }

                        if (func->parameters.size() != 1) {
                            throw std::runtime_error(
                                "Function for filter must take exactly 1 "
                                "argument");
                        }

                        local_env.Set(func->parameters[0], item);

                        try {
                            Exec(func->body, local_env);
                            keep = false;
                        } catch (const ReturnValue& ret) {
                            keep = std::holds_alternative<bool>(ret.value) &&
                                   std::get<bool>(ret.value);
                        }
                    } else {
                        auto native_func = std::get<NativeFunctionPtr>(args[0]);
                        Value res = (*native_func)(call_args);
                        keep = std::holds_alternative<bool>(res) &&
                               std::get<bool>(res);
                    }

                    if (keep) {
                        result.push_back(item);
                    }
                }

                return result;
            }));

        // reduce(func, list, initial) - агрегирует список с помощью функции
        global_env_.Set(
            "reduce",
            CreateNativeFunction([this](
                                     const std::vector<Value>& args) -> Value {
                if (args.size() != 3)
                    throw std::runtime_error(
                        "reduce() expects exactly 3 arguments");

                if (!std::holds_alternative<FunctionPtr>(args[0]) &&
                    !std::holds_alternative<NativeFunctionPtr>(args[0]))
                    throw std::runtime_error(
                        "reduce() first argument must be a function");

                if (!std::holds_alternative<List>(args[1]))
                    throw std::runtime_error(
                        "reduce() second argument must be a list");

                const List& list = std::get<List>(args[1]);
                Value accumulator = args[2];

                for (const auto& item : list) {
                    std::vector<Value> call_args = {accumulator, item};

                    if (std::holds_alternative<FunctionPtr>(args[0])) {
                        auto func = std::get<FunctionPtr>(args[0]);
                        Environment local_env;

                        for (const auto& [name, value] :
                             global_env_.variables) {
                            local_env.Set(name, value);
                        }

                        if (func->parameters.size() != 2) {
                            throw std::runtime_error(
                                "Function for reduce must take exactly 2 "
                                "arguments");
                        }

                        local_env.Set(func->parameters[0], accumulator);
                        local_env.Set(func->parameters[1], item);

                        try {
                            Exec(func->body, local_env);
                            accumulator =
                                std::monostate{}; 
                        } catch (const ReturnValue& ret) {
                            accumulator = ret.value;
                        }
                    } else {
                        auto native_func = std::get<NativeFunctionPtr>(args[0]);
                        accumulator = (*native_func)(call_args);
                    }
                }

                return accumulator;
            }));

        // each(func, list) - выполняет функцию для каждого элемента списка
        global_env_.Set(
            "each",
            CreateNativeFunction([this](
                                     const std::vector<Value>& args) -> Value {
                if (args.size() != 2)
                    throw std::runtime_error(
                        "each() expects exactly 2 arguments");

                if (!std::holds_alternative<FunctionPtr>(args[0]) &&
                    !std::holds_alternative<NativeFunctionPtr>(args[0]))
                    throw std::runtime_error(
                        "each() first argument must be a function");

                if (!std::holds_alternative<List>(args[1]))
                    throw std::runtime_error(
                        "each() second argument must be a list");

                const List& list = std::get<List>(args[1]);

                for (const auto& item : list) {
                    std::vector<Value> call_args = {item};

                    if (std::holds_alternative<FunctionPtr>(args[0])) {
                        auto func = std::get<FunctionPtr>(args[0]);
                        Environment local_env;

                        for (const auto& [name, value] :
                             global_env_.variables) {
                            local_env.Set(name, value);
                        }

                        if (func->parameters.size() != 1) {
                            throw std::runtime_error(
                                "Function for each must take exactly 1 "
                                "argument");
                        }

                        local_env.Set(func->parameters[0], item);

                        try {
                            Exec(func->body, local_env);
                        } catch (const ReturnValue&) {
                        }
                    } else {
                        auto native_func = std::get<NativeFunctionPtr>(args[0]);
                        (*native_func)(call_args);
                    }
                }

                return std::monostate{}; 
            }));
    }

    std::string ValueToString(const Value& v, bool for_output = false) {
        if (std::holds_alternative<std::monostate>(v)) {
            return "nil";
        }

        if (std::holds_alternative<double>(v)) {
            double num = std::get<double>(v);
            if (num == std::floor(num)) {
                return std::to_string(static_cast<int>(num));
            }

            std::ostringstream ss;
            ss << std::fixed << std::setprecision(2) << num;
            std::string str = ss.str();

            while (str.back() == '0' && str.find('.') != std::string::npos) {
                str.pop_back();
            }
            if (str.back() == '.') {
                str.pop_back();
            }
            return str;
        }

        if (std::holds_alternative<std::string>(v)) {
            const std::string& raw = std::get<std::string>(v);

            if (for_output) {
                return raw;
            }

            std::string escaped;
            for (char c : raw) {
                if (c == '\"')
                    escaped += "\\\"";
                else if (c == '\\')
                    escaped += "\\\\";
                else if (c == '\n')
                    escaped += "\\n";
                else if (c == '\t')
                    escaped += "\\t";
                else
                    escaped += c;
            }
            return "\"" + escaped + "\"";
        }
        if (std::holds_alternative<bool>(v)) {
            return std::get<bool>(v) ? "true" : "false";
        }
        if (std::holds_alternative<List>(v)) {
            const List& list = std::get<List>(v);
            std::string result = "[";
            for (size_t i = 0; i < list.size(); ++i) {
                result += ValueToString(list[i]);
                if (i + 1 < list.size()) result += ", ";
            }
            result += "]";
            return result;
        }
        if (std::holds_alternative<FunctionPtr>(v)) {
            return "<function>";
        }
        if (std::holds_alternative<NativeFunctionPtr>(v)) {
            return "<native function>";
        }
        throw std::runtime_error("Unsupported value in ValueToString");
    }

    Value EvalValue(const std::shared_ptr<ASTNode>& node, Environment& env) {
        if (auto unary = std::dynamic_pointer_cast<UnaryExpression>(node)) {
            Value operand_val = EvalValue(unary->operand, env);

            if (unary->operator_ == "-") {
                if (!std::holds_alternative<double>(operand_val)) {
                    throw std::runtime_error(
                        "Unary minus requires numeric operand");
                }
                return -std::get<double>(operand_val);
            } else if (unary->operator_ == "not") {
                if (std::holds_alternative<bool>(operand_val)) {
                    return !std::get<bool>(operand_val);
                } else if (std::holds_alternative<std::monostate>(
                               operand_val)) {
                    return true;   
                } else {
                    return false; 
                }
            }
        } else if (auto num = std::dynamic_pointer_cast<NumberLiteral>(node)) {
            return std::stod(num->value);
        } else if (auto str = std::dynamic_pointer_cast<StringLiteral>(node)) {
            return std::string(str->value);
        } else if (auto id = std::dynamic_pointer_cast<Identifier>(node)) {
            return env.Get(id->name);
        } else if (auto func_expr =
                       std::dynamic_pointer_cast<AnonymousFunction>(node)) {
            return FunctionPtr(std::make_shared<FunctionDeclaration>(
                "", func_expr->parameters, func_expr->body));
        } else if (auto func_decl =
                       std::dynamic_pointer_cast<FunctionDeclaration>(node)) {
            return func_decl;
        } else if (auto boolean =
                       std::dynamic_pointer_cast<BooleanLiteral>(node)) {
            return boolean->value;
        } else if (auto nil = std::dynamic_pointer_cast<NilLiteral>(node)) {
            return std::monostate{};
        } else if (auto list = std::dynamic_pointer_cast<ArrayLiteral>(node)) {
            List values;
            for (const auto& element : list->elements) {
                Value val = EvalValue(element, env);
                values.push_back(val);
            }
            return values;
        } else if (auto index = std::dynamic_pointer_cast<IndexAccess>(node)) {
            Value container = EvalValue(index->object, env);
            Value index_val = EvalValue(index->index, env);

            int idx;
            if (std::holds_alternative<double>(index_val)) {
                idx = static_cast<int>(std::get<double>(index_val));
            } else if (std::holds_alternative<std::string>(index_val)) {
                try {
                    idx = std::stoi(std::get<std::string>(index_val));
                } catch (...) {
                    throw std::runtime_error("Invalid index type");
                }
            } else {
                throw std::runtime_error("Index must be a number");
            }

            if (std::holds_alternative<std::string>(container)) {
                const std::string& str = std::get<std::string>(container);

                if (idx < 0) idx += str.length();

                if (idx < 0 || idx >= static_cast<int>(str.length())) {
                    throw std::runtime_error("String index out of range");
                }

                return std::string(1, str[idx]);
            } else if (std::holds_alternative<List>(container)) {
                const List& list = std::get<List>(container);

                if (idx < 0) idx += list.size();

                if (idx < 0 || idx >= static_cast<int>(list.size())) {
                    throw std::runtime_error("List index out of range");
                }

                return list[idx];
            }

            throw std::runtime_error("Cannot index this type");
        } else if (auto slice = std::dynamic_pointer_cast<SliceAccess>(node)) {
            Value container = EvalValue(slice->object, env);

            int start = 0;
            int end = 0;

            if (std::holds_alternative<std::string>(container)) {
                const std::string& s = std::get<std::string>(container);
                end = s.size();

                if (slice->start) {
                    Value start_val = EvalValue(slice->start, env);
                    if (std::holds_alternative<double>(start_val)) {
                        start = static_cast<int>(std::get<double>(start_val));
                    } else {
                        throw std::runtime_error(
                            "Slice start must be a number");
                    }
                }

                if (slice->end) {
                    Value end_val = EvalValue(slice->end, env);
                    if (std::holds_alternative<double>(end_val)) {
                        end = static_cast<int>(std::get<double>(end_val));
                    } else {
                        throw std::runtime_error("Slice end must be a number");
                    }
                }

                if (start < 0) start += s.size();
                if (end < 0) end += s.size();

                start =
                    std::max(0, std::min(start, static_cast<int>(s.size())));
                end = std::max(0, std::min(end, static_cast<int>(s.size())));

                if (start > end) return std::string("");

                return s.substr(start, end - start);
            } else if (std::holds_alternative<List>(container)) {
                const List& list = std::get<List>(container);
                end = list.size();

                if (slice->start) {
                    Value start_val = EvalValue(slice->start, env);
                    if (std::holds_alternative<double>(start_val)) {
                        start = static_cast<int>(std::get<double>(start_val));
                    } else {
                        throw std::runtime_error(
                            "Slice start must be a number");
                    }
                }

                if (slice->end) {
                    Value end_val = EvalValue(slice->end, env);
                    if (std::holds_alternative<double>(end_val)) {
                        end = static_cast<int>(std::get<double>(end_val));
                    } else {
                        throw std::runtime_error("Slice end must be a number");
                    }
                }

                if (start < 0) start += list.size();
                if (end < 0) end += list.size();

                start =
                    std::max(0, std::min(start, static_cast<int>(list.size())));
                end = std::max(0, std::min(end, static_cast<int>(list.size())));

                List result;
                for (int i = start; i < end; ++i) {
                    result.push_back(list[i]);
                }
                return result;
            }

            throw std::runtime_error("Cannot slice this type");
        } else if (auto call = std::dynamic_pointer_cast<FunctionCall>(node)) {
            Value func_val = EvalValue(call->target, env);

            std::vector<Value> args;
            for (const auto& arg : call->arguments) {
                args.push_back(EvalValue(arg, env));
            }

            if (std::holds_alternative<NativeFunctionPtr>(func_val)) {
                auto native_func = std::get<NativeFunctionPtr>(func_val);
                return (*native_func)(args);
            } else if (std::holds_alternative<FunctionPtr>(func_val)) {
                auto func = std::get<FunctionPtr>(func_val);

                if (args.size() != func->parameters.size()) {
                    throw std::runtime_error(
                        "Function called with wrong number of arguments");
                }

                Environment local_env;

                for (const auto& [name, value] : global_env_.variables) {
                    local_env.Set(name, value);
                }

                for (size_t i = 0; i < args.size(); ++i) {
                    local_env.Set(func->parameters[i], args[i]);
                }

                try {
                    Exec(func->body, local_env);
                    return std::monostate{};
                } catch (const ReturnValue& ret) {
                    return ret.value;
                }
            } else {
                throw std::runtime_error("Cannot call a non-function value");
            }
        } else if (auto bin =
                       std::dynamic_pointer_cast<BinaryExpression>(node)) {
            const std::string& op = bin->op;

            if (op == "and" || op == "or") {
                Value lhs_val = EvalValue(bin->left, env);
                bool lhs_bool = false;

                if (std::holds_alternative<bool>(lhs_val)) {
                    lhs_bool = std::get<bool>(lhs_val);
                } else if (std::holds_alternative<std::monostate>(lhs_val)) {
                    lhs_bool = false;
                } else {
                    lhs_bool = true;
                }

                if (op == "and" && !lhs_bool) return false;
                if (op == "or" && lhs_bool) return true;

                Value rhs_val = EvalValue(bin->right, env);

                if (std::holds_alternative<bool>(rhs_val)) {
                    return std::get<bool>(rhs_val);
                } else if (std::holds_alternative<std::monostate>(rhs_val)) {
                    return false;
                } else {
                    return true;
                }
            }

            Value lhs_val = EvalValue(bin->left, env);
            Value rhs_val = EvalValue(bin->right, env);

            if (std::holds_alternative<double>(lhs_val) &&
                std::holds_alternative<double>(rhs_val)) {
                double lhs = std::get<double>(lhs_val);
                double rhs = std::get<double>(rhs_val);

                if (op == "+") return lhs + rhs;
                if (op == "-") return lhs - rhs;
                if (op == "*") return lhs * rhs;
                if (op == "^") return std::pow(lhs, rhs);
                if (op == "/") {
                    if (rhs == 0) throw std::runtime_error("Division by zero");
                    return lhs / rhs;
                }
                if (op == "%") {
                    if (rhs == 0) throw std::runtime_error("Modulo by zero");
                    return std::fmod(lhs, rhs);
                }
                if (op == "^") return std::pow(lhs, rhs);
                if (op == "==") return lhs == rhs;
                if (op == "!=") return lhs != rhs;
                if (op == "<") return lhs < rhs;
                if (op == "<=") return lhs <= rhs;
                if (op == ">") return lhs > rhs;
                if (op == ">=") return lhs >= rhs;
            }

            else if (std::holds_alternative<List>(lhs_val) &&
                     std::holds_alternative<List>(rhs_val)) {
                const List& lhs_list = std::get<List>(lhs_val);
                const List& rhs_list = std::get<List>(rhs_val);

                if (op == "+") {
                    List result = lhs_list;
                    result.insert(result.end(), rhs_list.begin(),
                                  rhs_list.end());
                    return result;
                }

                throw std::runtime_error("Unsupported operation for lists: " +
                                         op);
            }

            else if (std::holds_alternative<List>(lhs_val) &&
                     std::holds_alternative<double>(rhs_val)) {
                const List& list = std::get<List>(lhs_val);
                int count = static_cast<int>(std::get<double>(rhs_val));

                if (op == "*") {
                    if (count <= 0) return List{};

                    List result;
                    result.reserve(list.size() * count);
                    for (int i = 0; i < count; ++i) {
                        result.insert(result.end(), list.begin(), list.end());
                    }
                    return result;
                }

                throw std::runtime_error(
                    "Unsupported operation for list and number: " + op);
            }

            else if (std::holds_alternative<double>(lhs_val) &&
                     std::holds_alternative<List>(rhs_val)) {
                int count = static_cast<int>(std::get<double>(lhs_val));
                const List& list = std::get<List>(rhs_val);

                if (op == "*") {
                    if (count <= 0) return List{};

                    List result;
                    result.reserve(list.size() * count);
                    for (int i = 0; i < count; ++i) {
                        result.insert(result.end(), list.begin(), list.end());
                    }
                    return result;
                }

                throw std::runtime_error(
                    "Unsupported operation for number and list: " + op);
            }

            else if (std::holds_alternative<std::string>(lhs_val) &&
                     std::holds_alternative<std::string>(rhs_val)) {
                const std::string& lhs = std::get<std::string>(lhs_val);
                const std::string& rhs = std::get<std::string>(rhs_val);

                if (op == "+") return lhs + rhs;
                if (op == "-") {
                    if (lhs.size() >= rhs.size() &&
                        lhs.substr(lhs.size() - rhs.size()) == rhs) {
                        return lhs.substr(0, lhs.size() - rhs.size());
                    }
                    return lhs;
                }
                if (op == "==") return lhs == rhs;
                if (op == "!=") return lhs != rhs;
                if (op == "<") return lhs < rhs;
                if (op == "<=") return lhs <= rhs;
                if (op == ">") return lhs > rhs;
                if (op == ">=") return lhs >= rhs;
            }

            else if (std::holds_alternative<std::string>(lhs_val) &&
                     std::holds_alternative<double>(rhs_val)) {
                const std::string& str = std::get<std::string>(lhs_val);
                int count = static_cast<int>(std::get<double>(rhs_val));

                if (op == "*") {
                    if (count <= 0) return std::string("");

                    std::string result;
                    result.reserve(str.size() * count);
                    for (int i = 0; i < count; ++i) {
                        result += str;
                    }
                    return result;
                }

                throw std::runtime_error(
                    "Unsupported operation for string and number: " + op);
            }

            else if (std::holds_alternative<double>(lhs_val) &&
                     std::holds_alternative<std::string>(rhs_val)) {
                int count = static_cast<int>(std::get<double>(lhs_val));
                const std::string& str = std::get<std::string>(rhs_val);

                if (op == "*") {
                    if (count <= 0) return std::string("");

                    std::string result;
                    result.reserve(str.size() * count);
                    for (int i = 0; i < count; ++i) {
                        result += str;
                    }
                    return result;
                }

                throw std::runtime_error(
                    "Unsupported operation for number and string: " + op);
            }

            else if (std::holds_alternative<std::monostate>(lhs_val) ||
                     std::holds_alternative<std::monostate>(rhs_val)) {
                if (op == "==") {
                    return std::holds_alternative<std::monostate>(lhs_val) &&
                           std::holds_alternative<std::monostate>(rhs_val);
                }
                if (op == "!=") {
                    return !(std::holds_alternative<std::monostate>(lhs_val) &&
                             std::holds_alternative<std::monostate>(rhs_val));
                }
            }

            throw std::runtime_error("Unsupported operation for given types: " +
                                     op);
        }

        throw std::runtime_error("Unsupported expression type");
    }

    class ReturnValue {
       public:
        Value value;
        explicit ReturnValue(Value v) : value(std::move(v)) {}
    };

    class BreakException : public std::exception {
       public:
        const char* what() const noexcept override {
            return "Break statement outside loop";
        }
    };

    class ContinueException : public std::exception {
       public:
        const char* what() const noexcept override {
            return "Continue statement outside loop";
        }
    };

    std::string Exec(const std::shared_ptr<ASTNode>& node, Environment& env) {
        if (std::dynamic_pointer_cast<BreakStatement>(node)) {
            throw BreakException();
        }

        if (std::dynamic_pointer_cast<ContinueStatement>(node)) {
            throw ContinueException();
        }
        if (auto block = std::dynamic_pointer_cast<BlockNode>(node)) {
            std::string last_result;
            for (const auto& stmt : block->statements) {
                last_result = Exec(stmt, env);
            }
            return last_result;
        } else if (auto ret =
                       std::dynamic_pointer_cast<ReturnStatement>(node)) {
            throw ReturnValue(EvalValue(ret->expression, env));
        } else if (auto break_stmt =
                       std::dynamic_pointer_cast<BreakStatement>(node)) {
            throw BreakException();
        } else if (auto continue_stmt =
                       std::dynamic_pointer_cast<ContinueStatement>(node)) {
            throw ContinueException();
        } else if (auto assign = std::dynamic_pointer_cast<Assignment>(node)) {
            Value val = EvalValue(assign->expression, env);
            env.Set(assign->name, val);
            return "";
        } else if (auto if_stmt =
                       std::dynamic_pointer_cast<IfStatement>(node)) {
            Value cond_value = EvalValue(if_stmt->condition, env);

            if (!std::holds_alternative<bool>(cond_value)) {
                throw std::runtime_error("If condition must be a boolean");
            }

            if (std::get<bool>(cond_value)) {
                return Exec(if_stmt->then_block, env);
            } else if (if_stmt->else_block) {
                return Exec(if_stmt->else_block, env);
            }
            return "";
        } else if (auto while_loop =
                       std::dynamic_pointer_cast<WhileLoop>(node)) {
            while (true) {
                Value cond_value = EvalValue(while_loop->condition, env);

                if (!std::holds_alternative<bool>(cond_value)) {
                    throw std::runtime_error(
                        "While condition must be a boolean");
                }

                if (!std::get<bool>(cond_value)) {
                    break;
                }

                try {
                    Exec(while_loop->body, env);
                } catch (const BreakException&) {
                    break;
                } catch (const ContinueException&) {
                    continue;
                }
            }
            return "";
        } else if (auto for_loop = std::dynamic_pointer_cast<ForLoop>(node)) {
            Value start_val = EvalValue(for_loop->start, env);
            Value end_val = EvalValue(for_loop->end, env);
            Value step_val = EvalValue(for_loop->step, env);

            if (!std::holds_alternative<double>(start_val) ||
                !std::holds_alternative<double>(end_val) ||
                !std::holds_alternative<double>(step_val)) {
                throw std::runtime_error(
                    "For loop range values must be numbers");
            }

            double start = std::get<double>(start_val);
            double end = std::get<double>(end_val);
            double step = std::get<double>(step_val);

            if (step == 0) {
                throw std::runtime_error("For loop step cannot be zero");
            }

            Environment loop_env = env;

            if (step > 0) {
                for (double i = start; i < end; i += step) {
                    loop_env.Set(for_loop->var_name, i);
                    try {
                        Exec(for_loop->body, loop_env);
                    } catch (const BreakException&) {
                        break;
                    } catch (const ContinueException&) {
                        continue;
                    }
                }
            } else {
                for (double i = start; i > end; i += step) {
                    loop_env.Set(for_loop->var_name, i);
                    try {
                        Exec(for_loop->body, loop_env);
                    } catch (const BreakException&) {
                        break;
                    } catch (const ContinueException&) {
                        continue;
                    }
                }
            }

            return "";
        } else if (auto func_decl =
                       std::dynamic_pointer_cast<FunctionDeclaration>(node)) {
            if (!func_decl->name.empty()) {
                env.Set(func_decl->name, func_decl);
            }
            return "";
        } else {
            EvalValue(node, env);
            return "";
        }
    }
};

inline std::shared_ptr<ASTNode> ParseCode(const std::string& code) {
    Lexer lexer(code);
    auto tokens = lexer.Tokenize();
    Parser parser(tokens);
    return parser.Parse();
}

inline std::shared_ptr<ASTNode> ParseCode(std::istream& input) {
    std::string code((std::istreambuf_iterator<char>(input)),
                     std::istreambuf_iterator<char>());
    return ParseCode(code);
}

inline bool Interpret(std::istream& input, std::ostream& output) {
    auto ast = ParseCode(input);
    if (!ast) return false;

    Interpreter interpreter(output);
    try {
        interpreter.Interpret(ast);
        return true;
    } catch (const std::exception& e) {
        output << "Error: " << e.what() << std::endl;
        return false;
    }
}