#include <deque>
#include <iostream>
#include <stack>

class magic {
    std::deque<int>* container;

   public:
    magic(std::stack<int>& s) {
        container = reinterpret_cast<std::deque<int>*>(&s);
    }

    std::deque<int>::const_iterator begin() const { return container->begin(); }

    std::deque<int>::const_iterator end() const { return container->end(); }
};

int main() {
    std::stack<int> a;
    a.push(1);
    a.push(2);
    a.push(3);
    a.push(4);
    a.push(5);

    magic b(a);
    for (int i : b) {
        std::cout << i << std::endl;
    }
    return 0;
}
