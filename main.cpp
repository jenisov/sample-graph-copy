#include <iostream>
#include <cassert>
#include <unordered_map>
#include <random>

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

    list_node<T> *m_next;
    T m_data;

public:
    list_node() : m_data(), m_next(nullptr) {}

    ~list_node() = default;
};

template<class T>
class list {
public:
    class iterator;
protected:
    size_t m_size;

    list_node<T> *m_head;
    list_node<T> *m_end;

public:
    class iterator {
    private:
        list_node<T> *m_ptr;

    public:
        iterator() : m_ptr(nullptr) {}

        explicit iterator(list_node<T> *ptr) : m_ptr(ptr) {}

        ~iterator() = default;

        T& operator*() const {
            return this->m_ptr->m_data;
        }

        const list_node<T>* operator->() const {
            return this->m_ptr;
        }

        const list<T>::iterator& operator++() {
            if (!this->m_ptr) {
                throw std::exception("Iterator not incrementable.");
            }
            this->m_ptr = this->m_ptr->m_next;
            return *this;
        }

        const list<T>::iterator operator++(int) {
            list<T>::iterator old;
            old.m_ptr = this->m_ptr;
            this->m_ptr = this->m_ptr->m_next;
            return old;
        }

        bool operator==(const list<T>::iterator &right) {
            return this->m_ptr == right.m_ptr;
        }

        bool operator!=(const list<T>::iterator &right) {
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
            this->m_head = new list_node<T>;
            this->m_head->m_data = arg;
            ++this->m_size;
            this->m_end = this->m_head;
            return;
        }
        this->m_end->m_next = new list_node<T>;
        this->m_end->m_next->m_data = arg;
        this->m_end = this->m_end->m_next;
        ++this->m_size;
    }

    T& operator[] (const int& pos) const {
        assert(pos > -1 && pos < this->m_size);
        list_node<T> *ptr = this->m_head;
        for (int i = 0; i < pos; i++) {
            ptr = ptr->m_next;
        }

        return ptr->m_data;
    }

    void clear() {
        if (!this->m_head) {
            return;
        }
        list_node<T> *ptr = this->m_head;
        list_node<T> *next;
        while (ptr) {
            next = ptr->m_next;
            delete[] ptr;
            ptr = next;
        }
        this->m_head = nullptr;
        this->m_size = 0;
    }

    list<T>::iterator begin() {
        return list<T>::iterator(this->m_head);
    }

    list<T>::iterator end() {
        return list<T>::iterator(nullptr);
    }

    list<T>::iterator pre_end() {
        return list<T>::iterator(this->m_end);
    }

    friend std::ostream& operator<< (std::ostream& out, const list<T>& l) {
        list_node<T> *current = l.m_head;
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
        i = (void*)lst.pre_end().operator->();
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