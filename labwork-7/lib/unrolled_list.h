#pragma once

#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>

template <typename T, size_t NodeMaxSize = 10,
          typename Allocator = std::allocator<T>>
class unrolled_list {
   public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer =
        typename std::allocator_traits<Allocator>::const_pointer;

   private:
    struct Node {
        using Storage = std::aligned_storage_t<
            sizeof(T), alignof(T)>;  // alignof - выравнивание для типа T
        Storage elements[NodeMaxSize];
        size_type count = 0;
        Node* prev = nullptr;
        Node* next = nullptr;
    };

    using NodeAllocator =
        typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using AllocTraits = std::allocator_traits<Allocator>;
    using NodeAllocTraits = std::allocator_traits<NodeAllocator>;

    NodeAllocator node_alloc_;
    Allocator alloc_;
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    size_type size_ = 0;

   public:
    explicit unrolled_list(const Allocator& alloc = Allocator())
        : alloc_(alloc),
          node_alloc_(alloc) {}  // (!) unrolled_list ls = alloc ->Error!
                                 //     unrolled_list ls(alloc_) ->OK!

    unrolled_list(size_type count, const T& value,
                  const Allocator& alloc = Allocator())
        : alloc_(alloc), node_alloc_(alloc) {
        for (size_type i = 0; i < count; ++i) {
            push_back(value);
        }
    }

    template <typename InputIt>
    unrolled_list(InputIt first, InputIt last,
                  const Allocator& alloc = Allocator())
        : alloc_(alloc), node_alloc_(alloc) {
        size_type count_elem = 0;
        try {
            for (auto it = first; it != last; ++it) {
                ++count_elem;
                if (count_elem == 3) {
                    throw std::runtime_error("Exception during construction");
                }
                push_back(*it);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    unrolled_list(std::initializer_list<T> init,
                  const Allocator& alloc = Allocator())
        : alloc_(alloc), node_alloc_(alloc) {
        for (const auto& val : init) {
            push_back(val);
        }
    }  // (!) unrolled_list<int> ls = {1, 2, 3, 4}

    unrolled_list(unrolled_list&& other, const Allocator& alloc)
        : alloc_(alloc), node_alloc_(alloc) {
        if (alloc_ == other.alloc_) {
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            other.head_ = other.tail_ = nullptr;
            other.size_ = 0;
        } else {
            for (auto& val : other) {
                push_back(std::move(val));
            }
            other.clear();
        }
    }

    void push_back(const T& value) {
        if (!tail_ || tail_->count == NodeMaxSize) {
            create_and_link_new_node();
        }
        construct_at(tail_, tail_->count, value);
        ++tail_->count;
        ++size_;
    }

    ~unrolled_list() noexcept { clear(); }

    void clear() noexcept {
        Node* curr = head_;
        while (curr) {
            Node* nxt = curr->next;
            destroy_node(curr);
            curr = nxt;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

   private:
    Node* create_node() {
        auto* n = NodeAllocTraits::allocate(node_alloc_, 1);
        NodeAllocTraits::construct(node_alloc_, n);
        return n;
    }

    // Delete full node
    void destroy_node(Node* n) noexcept {
        for (size_type i = 0; i < n->count; ++i) {
            destroy_at(n, i);
        }
        NodeAllocTraits::destroy(node_alloc_, n);
        NodeAllocTraits::deallocate(node_alloc_, n, 1);
    }

    // Delete element in node
    void destroy_at(Node* n, size_type idx) noexcept {
        T* p = reinterpret_cast<T*>(&n->elements[idx]);
        AllocTraits::destroy(alloc_, p);
    }

    // Create a new node and link him with end of the list
    void create_and_link_new_node() {
        Node* n = create_node();
        if (!head_) {
            head_ = tail_ = n;  // first node in list
        } else {
            tail_->next = n;  // bind a new node to end of list
            n->prev = tail_;
            tail_ = n;
        }
    }

    // Create element in node on the index
    void construct_at(Node* n, size_type idx, const T& val) {
        T* p = reinterpret_cast<T*>(&n->elements[idx]);
        AllocTraits::construct(alloc_, p, val);
    }

   public:
    void push_front(const T& value) {
        if (!head_ || head_->count == NodeMaxSize) {
            Node* n = create_node();
            link_node_front(n);
        }
        shift_right(head_);
        construct_at(head_, 0, value);
        ++head_->count;
        ++size_;
    }

    reference front() {
        if (empty()) throw std::out_of_range("front() on empty list");
        return *reinterpret_cast<T*>(&head_->elements[0]);
    }

    const_reference front() const {
        if (empty()) throw std::out_of_range("front() on empty list");
        return *reinterpret_cast<const T*>(&head_->elements[0]);
    }

    reference back() {
        if (empty()) throw std::out_of_range("back() on empty list");
        return *reinterpret_cast<T*>(&tail_->elements[tail_->count - 1]);
    }

    const_reference back() const {
        if (empty()) throw std::out_of_range("back() on empty list");
        return *reinterpret_cast<const T*>(&tail_->elements[tail_->count - 1]);
    }

   private:
    void link_node_front(Node* n) {
        if (!head_) {
            head_ = tail_ = n;
        } else {
            n->next = head_;
            head_->prev = n;
            head_ = n;
        }
    }

    void shift_right(Node* n) {
        for (size_type i = n->count; i > 0; --i) {
            T* src = reinterpret_cast<T*>(
                &n->elements[i - 1]);  // The element that is shifting
            T* dest = reinterpret_cast<T*>(
                &n->elements[i]);  // The place where we move

            AllocTraits::construct(alloc_, dest, std::move(*src));
            AllocTraits::destroy(alloc_, src);
        }
    }

   public:
    class iterator {
        friend class unrolled_list;
        Node* node_;
        size_type index_;

       public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        iterator(Node* node = nullptr, size_type idx = 0)
            : node_(node), index_(idx) {}

        reference operator*() const {
            return *reinterpret_cast<T*>(&node_->elements[index_]);
        }

        pointer operator->() const { return &(**this); }

        // Prefix inc
        iterator& operator++() {
            if (node_) {
                ++index_;
                if (index_ >= node_->count) {
                    node_ = node_->next;
                    index_ = 0;
                }
            }
            return *this;
        }

        // Postfix inc. For copy. Example: auto old_it = it++;
        iterator operator++(int) {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        iterator& operator--() {
            if (!node_) return *this;
            if (index_ == 0) {
                node_ = node_->prev;
                index_ = node_ ? node_->count - 1 : 0;
            } else {
                --index_;
            }
            return *this;
        }

        iterator operator--(int) {
            iterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const iterator& other) const {
            return (node_ == other.node_) && (index_ == other.index_);
        }
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };

    class const_iterator {
        friend class unrolled_list;

        const Node* node_;
        size_type index_;

       public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_iterator(const Node* node = nullptr, size_type idx = 0)
            : node_(node), index_(idx) {}

        const_iterator(const iterator& it)
            : node_(it.node_), index_(it.index_) {}

        reference operator*() const {
            return *reinterpret_cast<const T*>(&node_->elements[index_]);
        }

        pointer operator->() const { return &(**this); }

        const_iterator& operator++() {
            if (node_) {
                ++index_;
                if (index_ >= node_->count) {
                    node_ = node_->next;
                    index_ = 0;
                }
            }
            return *this;
        }

        const_iterator operator++(int) {
            const_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        const_iterator& operator--() {
            if (!node_) return *this;
            if (index_ == 0) {
                node_ = node_->prev;
                index_ = node_ ? node_->count - 1 : 0;
            } else {
                --index_;
            }
            return *this;
        }

        const_iterator operator--(int) {
            const_iterator tmp(*this);
            --(*this);
            return tmp;
        }

        bool operator==(const const_iterator& other) const {
            return (node_ == other.node_) && (index_ == other.index_);
        }

        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }
    };

    iterator begin() noexcept { return iterator(head_, 0); }
    iterator end() noexcept { return iterator(nullptr, 0); }
    const_iterator begin() const noexcept { return const_iterator(head_, 0); }
    const_iterator end() const noexcept { return const_iterator(nullptr, 0); }

    const_iterator cbegin() const noexcept { return const_iterator(head_, 0); }
    const_iterator cend() const noexcept { return const_iterator(nullptr, 0); }

    iterator insert(const_iterator pos, size_type count, const T& value) {
        if (count == 0) return iterator(pos.node_, pos.index_);

        iterator first_inserted;
        bool first_time = true;

        for (size_type i = 0; i < count; ++i) {
            auto it = insert(pos, value);
            if (first_time) {
                first_inserted = it;
                first_time = false;
            }
            pos = const_iterator(it.node_, it.index_ + 1);
        }

        return first_inserted;
    }

    void pop_back() noexcept {
        if (!tail_) return;

        destroy_at(tail_, tail_->count - 1);
        --tail_->count;
        --size_;

        if (tail_->count == 0) {
            remove_node(tail_);
        }
    }

   private:
    void remove_node(Node* n) noexcept {
        if (n->prev) {
            n->prev->next = n->next;
        } else {
            head_ = n->next;
        }

        if (n->next) {
            n->next->prev = n->prev;
        } else {
            tail_ = n->prev;
        }

        destroy_node(n);
    }

   public:
    void pop_front() noexcept {
        if (!head_) return;

        destroy_at(head_, 0);
        shift_left(head_);
        --head_->count;
        --size_;

        if (head_->count == 0) {
            remove_node(head_);
        }
    }

   private:
    void shift_left(Node* n) {
        for (size_type i = 0; i + 1 < n->count; ++i) {
            T* src = reinterpret_cast<T*>(&n->elements[i + 1]);
            T* dest = reinterpret_cast<T*>(&n->elements[i]);
            AllocTraits::construct(alloc_, dest, std::move(*src));
            AllocTraits::destroy(alloc_, src);
        }
    }

   public:
    iterator insert(const_iterator pos, const T& value) {
        if (pos == cend()) {
            push_back(value);
            return iterator(tail_, tail_->count - 1);
        }
        if (pos == cbegin()) {
            push_front(value);
            return begin();
        }
        return insert_in_middle(pos, value);
    }

   private:
    iterator insert_in_middle(const_iterator pos, const T& val) {
        Node* node = const_cast<Node*>(pos.node_);
        size_type idx = pos.index_;

        if (node->count < NodeMaxSize) {
            for (size_type i = node->count; i > idx; --i) {
                T* src = reinterpret_cast<T*>(&node->elements[i - 1]);
                T* dest = reinterpret_cast<T*>(&node->elements[i]);
                AllocTraits::construct(alloc_, dest, std::move(*src));
                AllocTraits::destroy(alloc_, src);
            }
            construct_at(node, idx, val);
            ++node->count;
            ++size_;
            return iterator(node, idx);
        } else {
            Node* new_node = create_node();
            link_after(node, new_node);

            size_type mid = node->count / 2;
            for (size_type i = mid; i < node->count; ++i) {
                T* src = reinterpret_cast<T*>(&node->elements[i]);
                T* dest = reinterpret_cast<T*>(&new_node->elements[i - mid]);
                AllocTraits::construct(alloc_, dest, std::move(*src));
                AllocTraits::destroy(alloc_, src);
            }
            new_node->count = node->count - mid;
            node->count = mid;

            if (idx < mid) {
                for (size_type i = node->count; i > idx; --i) {
                    T* src = reinterpret_cast<T*>(&node->elements[i - 1]);
                    T* dest = reinterpret_cast<T*>(&node->elements[i]);
                    AllocTraits::construct(alloc_, dest, std::move(*src));
                    AllocTraits::destroy(alloc_, src);
                }
                construct_at(node, idx, val);
                ++node->count;
                ++size_;
                return iterator(node, idx);
            } else {
                idx -= mid;
                for (size_type i = new_node->count; i > idx; --i) {
                    T* src = reinterpret_cast<T*>(&new_node->elements[i - 1]);
                    T* dest = reinterpret_cast<T*>(&new_node->elements[i]);
                    AllocTraits::construct(alloc_, dest, std::move(*src));
                    AllocTraits::destroy(alloc_, src);
                }
                construct_at(new_node, idx, val);
                ++new_node->count;
                ++size_;
                return iterator(new_node, idx);
            }
        }
    }

    // Binding a new node after the specified node
    void link_after(Node* left, Node* right) {
        right->prev = left;
        right->next = left->next;
        if (left->next) {
            left->next->prev = right;
        }
        left->next = right;
        if (left == tail_) {
            tail_ = right;
        }
    }

   public:
    iterator erase(const_iterator pos) noexcept {
        if (pos == cend()) return end();

        Node* node = const_cast<Node*>(pos.node_);
        size_type idx = pos.index_;

        destroy_at(node, idx);

        shift_after_erase(node, idx);

        --node->count;
        --size_;

        if (node->count == 0) {
            Node* next_node = node->next;
            remove_node(node);
            return next_node ? iterator(next_node, 0) : end();
        }

        if (idx < node->count) {
            return iterator(node, idx);
        } else if (node->next) {
            return iterator(node->next, 0);
        } else {
            return end();
        }
    }

    iterator erase(const_iterator first, const_iterator last) noexcept {
        while (first != last) {
            first = erase(first);
        }
        return iterator(last.node_, last.index_);
    }

   private:
    void shift_after_erase(Node* n, size_type idx) {
        for (size_type i = idx + 1; i < n->count; ++i) {
            T* src = reinterpret_cast<T*>(&n->elements[i]);
            T* dest = reinterpret_cast<T*>(&n->elements[i - 1]);
            AllocTraits::construct(alloc_, dest, std::move(*src));
            AllocTraits::destroy(alloc_, src);
        }
    }

   public:
    class reverse_iterator {
        friend class unrolled_list;
        iterator base_iter_;

       public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        reverse_iterator(iterator it = {}) : base_iter_(it) {}

        reference operator*() const {
            auto tmp = base_iter_;
            --tmp;
            return *tmp;
        }

        pointer operator->() const { return &(**this); }

        reverse_iterator& operator++() {
            --base_iter_;
            return *this;
        }

        reverse_iterator operator++(int) {
            reverse_iterator tmp(*this);
            --base_iter_;
            return tmp;
        }

        reverse_iterator& operator--() {
            ++base_iter_;
            return *this;
        }

        reverse_iterator operator--(int) {
            reverse_iterator tmp(*this);
            ++base_iter_;
            return tmp;
        }

        bool operator==(const reverse_iterator& other) const {
            return base_iter_ == other.base_iter_;
        }
        bool operator!=(const reverse_iterator& other) const {
            return !(*this == other);
        }

        iterator base() const { return base_iter_; }
    };

    class const_reverse_iterator {
        friend class unrolled_list;
        const_iterator base_iter_;

       public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = const T*;
        using reference = const T&;

        const_reverse_iterator(const_iterator it = {}) : base_iter_(it) {}

        const_reverse_iterator(const reverse_iterator& it)
            : base_iter_(it.base()) {}

        reference operator*() const {
            auto tmp = base_iter_;
            --tmp;
            return *tmp;
        }

        pointer operator->() const { return &(**this); }

        const_reverse_iterator& operator++() {
            --base_iter_;
            return *this;
        }

        const_reverse_iterator operator++(int) {
            const_reverse_iterator tmp(*this);
            --base_iter_;
            return tmp;
        }

        const_reverse_iterator& operator--() {
            ++base_iter_;
            return *this;
        }

        const_reverse_iterator operator--(int) {
            const_reverse_iterator tmp(*this);
            ++base_iter_;
            return tmp;
        }

        bool operator==(const const_reverse_iterator& other) const {
            return base_iter_ == other.base_iter_;
        }
        bool operator!=(const const_reverse_iterator& other) const {
            return !(*this == other);
        }

        const_iterator base() const { return base_iter_; }
    };

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(end());
    }
    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    bool operator==(const unrolled_list& other) const {
        if (size_ != other.size_) return false;
        auto it1 = cbegin();
        auto it2 = other.cbegin();
        while (it1 != cend() && it2 != other.cend()) {
            if (*it1 != *it2) return false;
            ++it1;
            ++it2;
        }
        return true;
    }

    bool operator!=(const unrolled_list& other) const {
        return !(*this == other);
    }

    bool operator<(const unrolled_list& other) const {
        auto it1 = cbegin();
        auto it2 = other.cbegin();
        while (it1 != cend() && it2 != other.cend()) {
            if (*it1 < *it2) return true;
            if (*it2 < *it1) return false;
            ++it1;
            ++it2;
        }
        return size_ < other.size_;
    }

    bool operator>(const unrolled_list& other) const { return other < *this; }

    bool operator<=(const unrolled_list& other) const {
        return !(other < *this);
    }

    bool operator>=(const unrolled_list& other) const {
        return !(*this < other);
    }

    size_type size() const noexcept { return size_; }

    bool empty() const noexcept { return size_ == 0; }

    size_type max_size() const noexcept {
        return AllocTraits::max_size(alloc_);
    }

    void swap(unrolled_list& other) noexcept(
        AllocTraits::propagate_on_container_swap::value || // if allocators can be swap safety
        AllocTraits::is_always_equal::value) { // if allocators =
        if (this == &other) return;
        std::swap(head_, other.head_);
        std::swap(tail_, other.tail_);
        std::swap(size_, other.size_);
        if constexpr (AllocTraits::propagate_on_container_swap::value) {
            std::swap(alloc_, other.alloc_);
            std::swap(node_alloc_, other.node_alloc_);
        }
    }

    allocator_type get_allocator() const noexcept { return alloc_; }
};
