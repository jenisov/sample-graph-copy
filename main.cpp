#include <iostream>
#include <cassert>
#include <unordered_map>
#include <random>

#define LIST_SIZE 10

using std::unordered_map;
using std::cout;
using std::mt19937;

class graph;

class graph_node {
private:
    friend class graph;

    graph_node *m_next;
    graph_node *m_link;

    size_t m_data;

public:
    graph_node() : m_next(nullptr), m_link(nullptr), m_data(0) {}

    ~graph_node() = default;

    void bind(graph_node *link) {
        this->m_link = link;
    }
};

class graph {
public:
    class iterator;
private:
    size_t m_size;

    graph_node *m_head;
    graph_node *m_end;

public:
    class iterator {
    private:
        graph_node *m_ptr;

    public:
        iterator() : m_ptr(nullptr) {}

        explicit iterator(graph_node *ptr) : m_ptr(ptr) {}

        ~iterator() = default;

        size_t& operator*() const {
            return this->m_ptr->m_data;
        }

        graph_node *operator->() const {
            return this->m_ptr;
        }

        const graph::iterator& operator++() {
            if (!this->m_ptr) {
                throw std::exception("Iterator not incrementable.");
            }
            this->m_ptr = this->m_ptr->m_next;
            return *this;
        }

        const graph::iterator operator++(int) {
            graph::iterator old;
            old.m_ptr = this->m_ptr;
            this->m_ptr = this->m_ptr->m_next;
            return old;
        }

        bool operator==(const graph::iterator &right) {
            return this->m_ptr == right.m_ptr;
        }

        bool operator!=(const graph::iterator &right) {
            return this->m_ptr != right.m_ptr;
        }

        explicit operator bool() {
            return (bool)this->m_ptr;
        }
    };

    graph() : m_head(nullptr), m_end(nullptr), m_size(0) {}

    graph(const graph& right) : m_head(nullptr), m_end(nullptr), m_size(0) {
        std::unordered_map<graph_node*, graph_node*> vec;

        int i = 0;
        for (auto ptr = right.m_head; ptr; ptr = ptr->m_next, i++) {
            this->push_back(ptr->m_data, ptr->m_link);
            vec.emplace(ptr, this->m_end);
        }

        for (auto ptr = this->m_head; ptr; ptr = ptr->m_next) {
            if (!ptr->m_link) {
                continue;
            }
            ptr->m_link = vec[ptr->m_link];
        }
    }

    // более медленное копирование за один проход
/*    graph(const graph &right) {
        std::unordered_map<void*, void*> vec;
        this->clear();
        this->push_back(0);
        vec.emplace(right.m_head, this->m_head);
        for (auto ptr = right.m_head; ptr; ptr = ptr->m_next) {
            list_node<T> *data = nullptr;
            list_node<T> *next = nullptr;
            if (ptr->m_data) {
                if (vec.find(ptr->m_data) == vec.end()) {
                    data = new list_node<T>;
                    vec.emplace(ptr->m_data, data);
                } else {
                    data = (list_node<T> *)vec[ptr->m_data];
                }
            }
            if (ptr->m_next) {
                if (vec.find(ptr->m_next) == vec.end()) {
                    next = new list_node<T>;
                    vec.emplace(ptr->m_next, next);
                } else {
                    next = (list_node<T> *)vec[ptr->m_next];
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

    ~graph() {
        this->clear();
    }

    void clear() {
        if (!this->m_head) {
            this->m_end = nullptr;
            this->m_size = 0;
            return;
        }
        graph_node *ptr = this->m_head;
        graph_node *next;
        while (ptr) {
            next = ptr->m_next;
            delete[] ptr;
            ptr = next;
        }
        this->m_head = nullptr;
        this->m_end = nullptr;
        this->m_size = 0;
    }

    void push_back(const size_t& val, graph_node *link = nullptr) {
        if (!this->m_head) {
            this->m_head = new graph_node;
            this->m_head->m_data = val;
            this->m_head->m_link = link;
            ++this->m_size;
            this->m_end = this->m_head;
            return;
        }
        this->m_end->m_next = new graph_node;
        this->m_end->m_next->m_data = val;
        this->m_end->m_next->m_link = link;
        this->m_end = this->m_end->m_next;
        ++this->m_size;
    }

    graph::iterator begin() {
        return graph::iterator(this->m_head);
    }

    graph::iterator end() {
        return graph::iterator(nullptr);
    }

    graph::iterator pre_end() {
        return graph::iterator(this->m_end);
    }

    void print() {
        unordered_map<void*, int> vec;
        int i = 1;
        for (auto ptr = this->m_head; ptr; ptr = ptr->m_next) {
            vec.emplace(ptr, i++);
        }
        i = 1;
        for (auto ptr = this->m_head; ptr; ptr = ptr->m_next) {
            std::cout << i++ << " -> " << vec[ptr->m_link] << "\n";
            if (ptr->m_next) {
                cout << "|\n";
            }
        }
    }
};

int main() {
    graph lst;
    mt19937 rg;
    graph_node* vec[LIST_SIZE];
    for (auto &i : vec) {
        lst.push_back(0);
        i = lst.pre_end().operator->();
    }

    for (auto it = lst.begin(); it != lst.end(); it++) {
        it->bind(vec[rg() % LIST_SIZE]);
    }

    graph lst1(lst);

    lst.print();
    std::cout << "\n\n";
    lst1.print();
    return 0;
}