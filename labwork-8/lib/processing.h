#pragma once
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <vector>

template <typename Key, typename Value>
struct KV {
    Key key;
    Value value;
    using value_type = Value;
};

template <typename Base, typename Joined>
struct JoinResult {
    Base base;
    std::optional<Joined> joined;

    bool operator==(const JoinResult& other) const {
        return base == other.base && joined == other.joined;
    }
};

//----------
class Dir {
    std::filesystem::path path;
    bool recursive;

   public:
    Dir(const char* p, bool r) : path(p), recursive(r) {
        if (!std::filesystem::is_directory(path)) {
            throw std::runtime_error("Not a directory!");
        }
    }

    class Iterator {
       public:
        using value_type = std::filesystem::directory_entry;
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;
        std::filesystem::directory_iterator dir_it;
        std::filesystem::recursive_directory_iterator rec_it;
        bool is_recursive;

        Iterator(bool recursive, const std::filesystem::path& p)
            : is_recursive(recursive) {
            if (is_recursive) {
                rec_it = std::filesystem::recursive_directory_iterator(p);
            } else {
                dir_it = std::filesystem::directory_iterator(p);
            }
        }

        Iterator(bool recursive) : is_recursive(recursive) {
            if (is_recursive) {
                rec_it = std::filesystem::recursive_directory_iterator();
            } else {
                dir_it = std::filesystem::directory_iterator();
            }
        }

        Iterator& operator++() {
            if (is_recursive)
                ++rec_it;
            else
                ++dir_it;
            return *this;
        }

        auto operator*() const {
            if (is_recursive) return *rec_it;
            return *dir_it;
        }

        bool operator==(const Iterator& other) const {
            if (is_recursive) return rec_it == other.rec_it;
            return dir_it == other.dir_it;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };

    Iterator begin() const { return Iterator(recursive, path); }
    Iterator end() const { return Iterator(recursive); }
};

//----------

template <typename Iter, typename Pred>
class FilterAdapter {
    Iter begin_;
    Iter end_;
    Pred pred_;

   public:
    using value_type = typename std::iterator_traits<Iter>::value_type;

    FilterAdapter(Iter begin, Iter end, Pred pred)
        : begin_(begin), end_(end), pred_(pred) {}

    class Iterator {
        Iter current;
        Iter end;
        Pred pred;

        void skip() {
            while (current != end && !pred(*current)) ++current;
        }

       public:
        using iterator_category = std::input_iterator_tag;
        using value_type = typename std::iterator_traits<Iter>::value_type;
        using difference_type =
            typename std::iterator_traits<Iter>::difference_type;
        using pointer = typename std::iterator_traits<Iter>::pointer;
        using reference = typename std::iterator_traits<Iter>::reference;

        Iterator(Iter it, Iter end_it, Pred p)
            : current(it), end(end_it), pred(p) {
            skip();
        }

        Iterator& operator++() {
            ++current;
            skip();
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        auto operator*() const { return *current; }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };

    Iterator begin() { return Iterator(begin_, end_, pred_); }
    Iterator end() { return Iterator(end_, end_, pred_); }
};

inline auto Filter(auto pred) {
    return [=](auto&& range) {
        return FilterAdapter(range.begin(), range.end(), pred);
    };
}
//----------
template <typename T, typename F>
auto operator|(T&& value, F&& func) {
    return func(std::forward<T>(value));
}
//----------
template <typename Container>
class AsDataFlowAdapter {
    const Container& container;

   public:
    using value_type = typename Container::value_type;

    explicit AsDataFlowAdapter(const Container& c) : container(c) {}
    /* -why explicit?
    struct Example {
        explicit Example(int x) {}
    };

    Example ex1 = 42; // Error! (неявный вызов конструктора)
    Example ex2(42); // ОК! (явный вызов конструктора)
    */

    auto begin() const { return container.begin(); }
    auto end() const { return container.end(); }
};

inline auto AsDataFlow(auto&& container) {
    return AsDataFlowAdapter<std::remove_reference_t<decltype(container)>>(
        std::forward<decltype(container)>(container));  
}
//----------
inline auto AsVector() {
    return [](auto&& range) {
        using value_type = typename std::decay_t<decltype(*std::begin(range))>;
        std::vector<value_type> result;
        for (auto&& item : range) {
            result.push_back(item);
        }
        return result;
    };
}
//----------

template <typename Stream, typename Delim>
class WriteAdapter {
    Stream& stream;
    Delim delim;

   public:
    WriteAdapter(Stream& s, Delim d) : stream(s), delim(d) {}

    template <typename Range>
    void operator()(Range&& range) {
        bool first = true;  // Separation flag
        for (auto&& item : range) {
            if (!first) {
                stream << delim;
            }
            stream << item;
            first = false;
        }
        if (!first) {
            stream << delim;
        }
    }
};

inline auto Write(auto& stream, auto delim) {
    return [&stream, delim](auto&& range) {
        WriteAdapter adapter(stream, delim);
        adapter(range);
        return range;
    };
}
//----------

template <typename Iter>
class DropNulloptAdapter {
    Iter begin_;
    Iter end_;

   public:
    DropNulloptAdapter(Iter begin, Iter end) : begin_(begin), end_(end) {}

    class Iterator {
        Iter current;
        Iter end;

        void skip() {
            while (current != end && !current->has_value()) ++current;
        }

       public:
        Iterator(Iter it, Iter end_it) : current(it), end(end_it) { skip(); }

        Iterator& operator++() {
            ++current;
            skip();
            return *this;
        }

        auto operator*() const { return **current; }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };

    Iterator begin() { return Iterator(begin_, end_); }
    Iterator end() { return Iterator(end_, end_); }
};

inline auto DropNullopt() {
    return [](auto&& range) {
        return DropNulloptAdapter(range.begin(), range.end());
    };
}
// ----------

template <typename Iter, typename Delim>
class SplitAdapter {
    Iter begin_;
    Iter end_;
    std::string delim_;

   public:
    using value_type = std::string;

    SplitAdapter(Iter begin, Iter end, Delim delim)
        : begin_(begin), end_(end), delim_(delim) {}

    class Iterator {
        Iter current;
        Iter end;
        std::string delim;
        std::string buffer;
        std::vector<std::string> chunks;
        size_t chunk_pos = 0;

        void fill_buffer() {
            if (current == end) return;

            buffer.clear();
            std::string line;
            bool has_content = false;

            std::stringstream temp_stream;
            temp_stream << (*current).rdbuf();
            while (std::getline(temp_stream, line)) {
                buffer += line;
                if (!temp_stream.eof()) {
                    buffer += '\n';
                }
                has_content = true;
            }

            if (!has_content) {
                current = end;
            }
        }

        void split_buffer() {
            chunks.clear();
            size_t start = 0;
            size_t end_pos = buffer.find(delim);

            while (end_pos != std::string::npos) {
                chunks.push_back(buffer.substr(start, end_pos - start));
                start = end_pos + delim.length();
                end_pos = buffer.find(delim, start);
            }
            chunks.push_back(buffer.substr(start));
        }

       public:
        Iterator(Iter it, Iter end_it, const std::string& d)
            : current(it), end(end_it), delim(d) {
            if (current != end) {
                fill_buffer();
                split_buffer();
            }
        }

        Iterator& operator++() {
            ++chunk_pos;
            if (chunk_pos >= chunks.size()) {
                ++current;
                chunk_pos = 0;
                if (current != end) {
                    fill_buffer();
                    split_buffer();
                }
            }
            return *this;
        }

        std::string operator*() const { return chunks[chunk_pos]; }

        bool operator!=(const Iterator& other) const {
            return current != other.current || chunk_pos != other.chunk_pos;
        }
    };

    Iterator begin() { return Iterator(begin_, end_, delim_); }
    Iterator end() { return Iterator(end_, end_, delim_); }
};

inline auto Split(const auto& delim) {
    return [delim](auto&& range) {
        using DelimType = std::decay_t<decltype(delim)>;
        return SplitAdapter<decltype(range.begin()), DelimType>(
            range.begin(), range.end(), delim);
    };
}

//----------

template <typename Iter, typename Func>
class TransformAdapter {
    Iter begin_;
    Iter end_;
    Func func_;

   public:
    TransformAdapter(Iter begin, Iter end, Func func)
        : begin_(begin), end_(end), func_(func) {}

    class Iterator {
        Iter current;
        Iter end;
        Func func;

       public:
        Iterator(Iter it, Iter end_it, Func f)
            : current(it), end(end_it), func(f) {}

        Iterator& operator++() {
            ++current;
            return *this;
        }

        auto operator*() const { return func(*current); }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };

    Iterator begin() { return Iterator(begin_, end_, func_); }
    Iterator end() { return Iterator(end_, end_, func_); }
};

inline auto Transform(auto func) {
    return [=](auto&& range) {
        return TransformAdapter(range.begin(), range.end(), func);
    };
}

// ----------
template <typename Iter, typename Init, typename Accumulator,
          typename KeyExtractor>
class AggregateByKeyAdapter {
    std::vector<
        std::pair<decltype(std::declval<KeyExtractor>()(*std::declval<Iter>())),
                  std::decay_t<Init>>>
        result_;

   public:
    AggregateByKeyAdapter(Iter begin, Iter end, Init init, Accumulator acc,
                          KeyExtractor key_ext) {
        using KeyType = decltype(key_ext(*begin));
        using ValueType = std::decay_t<Init>;

        std::vector<std::pair<KeyType, ValueType>> ordered_result;
        std::unordered_map<KeyType, ValueType> aggregates;

        for (auto it = begin; it != end; ++it) {
            auto key = key_ext(*it);
            if (aggregates.find(key) == aggregates.end()) {
                aggregates[key] = init;
                ordered_result.emplace_back(key, init);
            }
            acc(*it, aggregates[key]);
        }

        for (auto& pair : ordered_result) {
            pair.second = aggregates[pair.first];
        }

        result_ = std::move(ordered_result);
    }

    auto begin() const { return result_.begin(); }
    auto end() const { return result_.end(); }
};

inline auto AggregateByKey(auto init, auto accumulator, auto key_extractor) {
    return [=](auto&& range) {
        return AggregateByKeyAdapter(range.begin(), range.end(), init,
                                     accumulator, key_extractor);
    };
}
// ----------

template <typename Iter>
class SplitExpectedAdapter {
    Iter begin_;
    Iter end_;

   public:
    struct UnexpectedFlow {
        Iter begin_, end_;

        class Iterator {
            Iter current;
            Iter end;

            void skip() {
                while (current != end && (*current).has_value()) ++current;
            }

           public:
            Iterator(Iter it, Iter end_it) : current(it), end(end_it) {
                skip();
            }

            Iterator& operator++() {
                ++current;
                skip();
                return *this;
            }

            auto operator*() const { return (*current).error(); }

            bool operator!=(const Iterator& other) const {
                return current != other.current;
            }
        };

        Iterator begin() { return Iterator(begin_, end_); }
        Iterator end() { return Iterator(end_, end_); }
    };

    struct ExpectedFlow {
        Iter begin_, end_;

        class Iterator {
            Iter current;
            Iter end;

            void skip() {
                while (current != end && !(*current).has_value()) ++current;
            }

           public:
            Iterator(Iter it, Iter end_it) : current(it), end(end_it) {
                skip();
            }

            Iterator& operator++() {
                ++current;
                skip();
                return *this;
            }

            auto operator*() const { return (*current).value(); }

            bool operator!=(const Iterator& other) const {
                return current != other.current;
            }
        };

        Iterator begin() { return Iterator(begin_, end_); }
        Iterator end() { return Iterator(end_, end_); }
    };

    SplitExpectedAdapter(Iter begin, Iter end) : begin_(begin), end_(end) {}

    auto Split() {
        return std::make_pair(UnexpectedFlow{begin_, end_},
                              ExpectedFlow{begin_, end_});
    }
};

// Version without an argument
inline auto SplitExpected() {
    return [](auto&& range) {
        return SplitExpectedAdapter<decltype(range.begin())>(range.begin(),
                                                             range.end())
            .Split();
    };
}

// Version with an argument
inline auto SplitExpected(auto func) {
    return [](auto&& range) {
        return SplitExpectedAdapter<decltype(range.begin())>(range.begin(),
                                                             range.end())
            .Split();
    };
}

//-----------
template <typename LeftIter, typename RightIter, typename LeftKey,
          typename RightKey>
class JoinAdapter {
    LeftIter left_begin_;
    LeftIter left_end_;
    RightIter right_begin_;
    RightIter right_end_;
    LeftKey left_key_;
    RightKey right_key_;

    using LeftType = typename std::iterator_traits<LeftIter>::value_type;
    using RightType = typename std::iterator_traits<RightIter>::value_type;
    using ResultType = JoinResult<LeftType, RightType>;

    std::vector<ResultType> result_;

   public:
    JoinAdapter(LeftIter left_begin, LeftIter left_end, RightIter right_begin,
                RightIter right_end, LeftKey left_key, RightKey right_key)
        : left_begin_(left_begin),
          left_end_(left_end),
          right_begin_(right_begin),
          right_end_(right_end),
          left_key_(left_key),
          right_key_(right_key) {
        std::unordered_multimap<decltype(right_key_(*right_begin_)), RightType>
            right_map;
        for (auto it = right_begin_; it != right_end_; ++it) {
            right_map.emplace(right_key_(*it), *it);
        }

        for (auto left_it = left_begin_; left_it != left_end_; ++left_it) {
            auto left_val = *left_it;
            auto key = left_key_(left_val);
            auto range = right_map.equal_range(key);

            if (range.first != range.second) {
                for (auto right_it = range.first; right_it != range.second;
                     ++right_it) {
                    result_.emplace_back(
                        ResultType{left_val, right_it->second});
                }
            } else {
                result_.emplace_back(ResultType{left_val, std::nullopt});
            }
        }
    }

    auto begin() const { return result_.begin(); }
    auto end() const { return result_.end(); }
};

template <typename LeftIter, typename RightIter>
class JoinKVAdapter {
    LeftIter left_begin_;
    LeftIter left_end_;
    RightIter right_begin_;
    RightIter right_end_;

    using LeftKV = typename std::iterator_traits<LeftIter>::value_type;
    using RightKV = typename std::iterator_traits<RightIter>::value_type;
    using ResultType =
        JoinResult<typename LeftKV::value_type, typename RightKV::value_type>;

    std::vector<ResultType> result_;

   public:
    JoinKVAdapter(LeftIter left_begin, LeftIter left_end, RightIter right_begin,
                  RightIter right_end)
        : left_begin_(left_begin),
          left_end_(left_end),
          right_begin_(right_begin),
          right_end_(right_end) {
        std::unordered_multimap<decltype(RightKV::key),
                                typename RightKV::value_type>
            right_map;
        for (auto it = right_begin_; it != right_end_; ++it) {
            right_map.emplace(it->key, it->value);
        }

        for (auto left_it = left_begin_; left_it != left_end_; ++left_it) {
            auto left_val = left_it->value;
            auto key = left_it->key;
            auto range = right_map.equal_range(key);

            if (range.first != range.second) {
                for (auto right_it = range.first; right_it != range.second;
                     ++right_it) {
                    result_.emplace_back(
                        ResultType{left_val, right_it->second});
                }
            } else {
                result_.emplace_back(ResultType{left_val, std::nullopt});
            }
        }
    }

    auto begin() const { return result_.begin(); }
    auto end() const { return result_.end(); }
};

inline auto Join(auto&& right_range) {
    return [right_range = std::forward<decltype(right_range)>(right_range)](
               auto&& left_range) {
        using LeftIter = decltype(left_range.begin());
        using RightIter = decltype(right_range.begin());
        return JoinKVAdapter<LeftIter, RightIter>(
            left_range.begin(), left_range.end(), right_range.begin(),
            right_range.end());
    };
}

template <typename RightRange, typename LeftKey, typename RightKey>
auto Join(RightRange&& right_range, LeftKey left_key, RightKey right_key) {
    return [right_range = std::forward<RightRange>(right_range),
            left_key = std::move(left_key),
            right_key = std::move(right_key)](auto&& left_range) {
        using LeftIter = decltype(left_range.begin());
        using RightIter = decltype(right_range.begin());
        return JoinAdapter<LeftIter, RightIter, LeftKey, RightKey>(
            left_range.begin(), left_range.end(), right_range.begin(),
            right_range.end(), left_key, right_key);
    };
}

//----------
template <typename Iter>
class OpenFilesAdapter {
    Iter begin_;
    Iter end_;

   public:
    OpenFilesAdapter(Iter begin, Iter end) : begin_(begin), end_(end) {}

    class Iterator {
        Iter current;
        Iter end;

       public:
        Iterator(Iter it, Iter end_it) : current(it), end(end_it) {}

        Iterator& operator++() {
            ++current;
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return current == other.current;
        }

        std::ifstream operator*() const {
            return std::ifstream((*current).path());
        }

        bool operator!=(const Iterator& other) const {
            return current != other.current;
        }
    };

    Iterator begin() { return Iterator(begin_, end_); }
    Iterator end() { return Iterator(end_, end_); }
};

inline auto OpenFiles() {
    return [](auto&& range) {
        return OpenFilesAdapter(range.begin(), range.end());
    };
}
//----------
template <typename Stream>
class OutAdapter {
    Stream& stream;
    const char* delim;

   public:
    OutAdapter(Stream& s, const char* d = " ") : stream(s), delim(d) {}

    template <typename Range>
    void operator()(Range&& range) {
        bool first = true;
        for (auto&& item : range) {
            if (!first) {
                stream << delim;
            }
            stream << item;
            first = false;
        }
        stream << std::endl;
    }
};

inline auto Out(auto& stream, const char* delim) {
    return [&stream, delim](auto&& range) {
        OutAdapter adapter(stream, delim);
        adapter(range);
        return range;
    };
}

inline auto Out(auto& stream) { return Out(stream, " "); }
//----------
