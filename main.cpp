#include <iostream>
#include <cassert>
#include <unordered_map>
#include <random>
#include <string>

#define LIST_SIZE 10

using std::unordered_map;
using std::cout;
using std::mt19937;

template<class T>
class list;

class graph;

template<class T>
class list_node {
private:
    friend class list<T>;
    friend class graph;

    using this_ptr = list_node<T>*;
    using data_type = T;

    this_ptr m_next;
    data_type m_data;

public:
    list_node() : m_data(), m_next(nullptr) {}

    ~list_node() = default;
};

template<class T>
class list {
public:
    class iterator;
protected:
    using iter_type = list<T>::iterator;
    using node_type = list_node<T>;
    using node_ptr = list_node<T>*;
    using data_ref = T&;

    size_t m_size;

    node_ptr m_head;
    node_ptr m_end;

public:
    class iterator {
    private:
        using this_type = list<T>::iterator;
        using this_ref = list<T>::iterator&;
        using ptr_type = node_ptr;

        ptr_type m_ptr;

    public:
        iterator() : m_ptr(nullptr) {}

        explicit iterator(const ptr_type& ptr) : m_ptr(ptr) {}

        ~iterator() = default;

        data_ref operator*() const {
            return this->m_ptr->m_data;
        }

        const ptr_type operator->() const {
            return this->m_ptr;
        }

        const this_ref operator++() {
            if (!this->m_ptr) {
                throw std::exception("Iterator not incrementable.");
            }
            this->m_ptr = this->m_ptr->m_next;
            return *this;
        }

        const this_type operator++(int) {
            this_type old;
            old.m_ptr = this->m_ptr;
            this->m_ptr = this->m_ptr->m_next;
            return old;
        }

        bool operator==(const this_ref right) {
            return this->m_ptr == right.m_ptr;
        }

        bool operator!=(const this_ref right) {
            return this->m_ptr != right.m_ptr;
        }

        explicit operator bool() {
            return (bool)this->m_ptr;
        }
    };

    list() : m_head(nullptr), m_end(nullptr), m_size(0) {};

    ~list() {
        this->clear();
    }

    void push_back(const T& arg) {
        if (!this->m_head) {
            this->m_head = new node_type;
            this->m_head->m_data = arg;
            ++this->m_size;
            this->m_end = this->m_head;
            return;
        }
        this->m_end->m_next = new node_type;
        this->m_end->m_next->m_data = arg;
        this->m_end = this->m_end->m_next;
        ++this->m_size;
    }

    data_ref operator[] (const int& pos) const {
        assert(pos > -1 && pos < this->m_size);
        node_ptr ptr = this->m_head;
        for (int i = 0; i < pos; i++) {
            ptr = ptr->m_next;
        }

        return ptr->m_data;
    }

    void clear() {
        if (!this->m_head) {
            return;
        }
        node_ptr ptr = this->m_head;
        node_ptr next;
        while (ptr) {
            next = ptr->m_next;
            delete[] ptr;
            ptr = next;
        }
        this->m_head = nullptr;
        this->m_size = 0;
    }

    iter_type begin() {
        return list<T>::iterator(this->m_head);
    }

    iter_type end() {
        return list<T>::iterator(nullptr);
    }

    iter_type pre_end() {
        return list<T>::iterator(this->m_end);
    }

    friend std::ostream& operator<< (std::ostream& out, const list<T>& l) {
        node_ptr current = l.m_head;
        for (current; current; current = current->m_next) {
            out << current->m_data << " ";
        }
        return out;
    }
};

class graph final : public list<void*> {
public:
    graph() : list<void*>() {}

    // копирование за два прохода (исходного и результирующего списков)
    graph(const graph& right) {
        std::unordered_map<void*, void*> vec;

        this->clear();
        int i = 0;
        for (auto ptr = right.m_head; ptr; ptr = ptr->m_next, i++) {
            this->push_back(ptr->m_data);
            vec.emplace(ptr, this->m_end);
        }

        for (auto ptr = this->m_head; ptr; ptr = ptr->m_next) {
            if (!ptr->m_data) {
                continue;
            }
            ptr->m_data = vec[ptr->m_data];
        }
    }

    // более медленное копирование за один проход
/*    graph(const graph &right) {
        std::unordered_map<void*, void*> vec;
        this->clear();
        this->push_back(0);
        vec.emplace(right.m_head, this->m_head);
        for (auto ptr = right.m_head; ptr; ptr = ptr->m_next) {
            node_ptr data = nullptr;
            node_ptr next = nullptr;
            if (ptr->m_data) {
                if (vec.find(ptr->m_data) == vec.end()) {
                    data = new node_type;
                    vec.emplace(ptr->m_data, data);
                } else {
                    data = (node_ptr)vec[ptr->m_data];
                }
            }
            if (ptr->m_next) {
                if (vec.find(ptr->m_next) == vec.end()) {
                    next = new node_type;
                    vec.emplace(ptr->m_next, next);
                } else {
                    next = (node_ptr)vec[ptr->m_next];
                }
            }
            this->m_end->m_next = next;
            this->m_end->m_data = data;
            if (next) {
                this->m_end = next;
                this->m_size++;
            }
        }
    }*/

    ~graph() = default;

    void print() {
        unordered_map<void*, int> vec;
        int i = 1;
        for (auto ptr = this->m_head; ptr; ptr = ptr->m_next) {
            vec.emplace(ptr, i++);
        }
        i = 1;
        for (auto ptr = this->m_head; ptr; ptr = ptr->m_next) {
            std::cout << i++ << " -> " << vec[ptr->m_data] << "\n";
            if (ptr->m_next) {
                cout << "|\n";
            }
        }
    }
};

int main()
{
    graph lst;
    mt19937 rg;
    void* vec[LIST_SIZE];
    for (auto &i : vec) {
        lst.push_back(0);
        i = lst.pre_end().operator->();
    }

    for (auto &el : lst) {
        el = vec[rg() % LIST_SIZE];
    }

    lst[rg() % LIST_SIZE] = 0;

    graph lst1(lst);

    lst.print();
    std::cout << "\n\n";
    lst1.print();
    return 0;
}