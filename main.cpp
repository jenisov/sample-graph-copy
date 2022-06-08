#include <iostream>
#include <unordered_map>
#include <random>

#define LIST_SIZE 10

using std::unordered_map;
using std::cout;
using std::mt19937;
using std::exception;
using std::array;

class graph;

class graph_node {
private:
    friend class graph;

    graph_node *m_next;
    graph_node *m_link;

    size_t m_data;

public:
    graph_node() : m_next(nullptr), m_link(nullptr), m_data(0) {}

    graph_node(const graph_node& right) : m_next(nullptr), m_link(right.m_link), m_data(right.m_data) {}

    ~graph_node() {
        this->m_link = nullptr;
        this->m_next = nullptr;
        this->m_data = 0;
    }

    void bind(graph_node* link) {
        this->m_link = link;
    }
};

class graph {
private:
    size_t m_size;

    graph_node* m_head;
    graph_node* m_end;

public:
    class iterator {
    private:
        graph_node* m_ptr;

    public:
        iterator() : m_ptr(nullptr) {}

        explicit iterator(graph_node* ptr) : m_ptr(ptr) {}

        iterator(const graph::iterator& right) = default;

        ~iterator() = default;

        size_t& operator*() const {
            if (!this->m_ptr) {
                throw exception("Cannot dereference end graph iterator.");
            }
            return this->m_ptr->m_data;
        }

        graph_node* operator->() const {
            return this->m_ptr;
        }

        const graph::iterator& operator++() {
            if (!this->m_ptr) {
                throw exception("Cannot increment end graph iterator.");
            }
            this->m_ptr = this->m_ptr->m_next;
            return *this;
        }

        const graph::iterator operator++(int) {
            graph::iterator old(*this);
            ++*this;
            return old;
        }

        bool operator==(const graph::iterator& right) const {
            return this->m_ptr == right.m_ptr;
        }

        bool operator!=(const graph::iterator& right) const {
            return this->m_ptr != right.m_ptr;
        }

        explicit operator bool() const {
            return (bool) this->m_ptr;
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

    ~graph() {
        this->clear();
    }

    void clear() {
        if (!this->m_head) {
            this->m_end = nullptr;
            this->m_size = 0;
            return;
        }
        graph_node* ptr = this->m_head;
        graph_node* next;
        while (ptr) {
            next = ptr->m_next;
            delete ptr;
            ptr = next;
        }
        this->m_head = nullptr;
        this->m_end = nullptr;
        this->m_size = 0;
    }

    void push_back(const size_t& val, graph_node* link = nullptr) {
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

    graph::iterator begin() const {
        return graph::iterator(this->m_head);
    }

    graph::iterator end() const {
        return graph::iterator(nullptr);
    }

    graph::iterator pre_end() const {
        return graph::iterator(this->m_end);
    }

    // функция, подготавливающая граф к рекурсивному копированию и вызывающая функцию копирования
    void copy(graph::iterator head) {
        this->clear();
        this->recursive_copy(head);
    }

    void print() const {
        unordered_map<void*, int> vec;
        int i = 1;
        for (auto ptr = this->m_head; ptr; ptr = ptr->m_next) {
            vec.emplace(ptr, i++);
        }
        i = 1;
        for (auto ptr = this->m_head; ptr; ptr = ptr->m_next) {
            std::cout << i++ << " (" << ptr->m_data << ") -> " << vec[ptr->m_link] << "\n";
            if (ptr->m_next) {
                cout << "|\n";
            }
        }
    }

private:
    void recursive_copy(graph::iterator head) {
        // инициализируем вершину графа
        static auto this_ptr = this->m_head = new graph_node(*head.operator->());
        auto current_ptr = this_ptr;
        static unordered_map<graph_node*, graph_node*> vec;
        // добавляем ассоциацию узлов одного графа узлу другого графа
        vec.emplace(head.operator->(), this_ptr);
        if (head->m_next) {
            ++head;
            // инициализация узла
            this_ptr->m_next = new graph_node(*head.operator->());
            this_ptr = this_ptr->m_next;
            this->recursive_copy(head);
        }
        // переопределяем линк узла графа
        if (!current_ptr->m_link) {
            return;
        }
        current_ptr->m_link = vec[current_ptr->m_link];
    }
};

int main() {
    graph lst;
    mt19937 rg;
    // массив, содержащий адресы узлов графа
    graph_node* vec[LIST_SIZE];
    // инициализируем граф
    for (auto& i: vec) {
        lst.push_back(rg() % 100 + 1);
        i = lst.pre_end().operator->();
    }

    // связываем узлы графа со случайными узлами этого графа
    for (auto it = lst.begin(); it != lst.end(); it++) {
        it->bind(vec[rg() % LIST_SIZE]);
    }
    // тест на пустой линк
    lst.begin()->bind(nullptr);
    cout << "\n";

    graph lst1;
    lst1.copy(lst.begin());

    lst.print();
    cout << "\n\n";
    lst1.print();
    lst.clear();
    return 0;
}