//
// Created by Danya Smelskiy on 03.01.2022.
//
#include <stdio.h>
#include <stdlib.h>

size_t data[] = {4, 8, 15, 16, 23, 42};
size_t data_length = (sizeof(data) / sizeof(size_t));

struct Node {
    size_t value;
    struct Node* next;
};

typedef struct Node Node;

void print_int(size_t value) {
    printf("%ld ", value);
    fflush(stdout);
}

size_t p(size_t value) {
    return (value & 1ull);
}

Node* add_element(size_t value, Node* next) {
    Node* cur_node = NULL;
    cur_node = (Node*)(malloc(sizeof(Node)));
    if (cur_node == NULL) {
        // abort
        exit(1);
    }
    cur_node->value = value;
    cur_node->next = next;
    return cur_node;
}

void m(Node* node, void(*print_function)(size_t)) {
    if (node == NULL) {
        return;
    }
    (*print_function)(node->value);
    m(node->next, print_function);
}

Node* f(Node* node, Node* odd_numbers_list, size_t(*predicate)(size_t)) {
    if (node == NULL) {
        return odd_numbers_list;
    }
    if ((*predicate)(node->value)) {
        odd_numbers_list = add_element(node->value, odd_numbers_list);
    }
    return f(node->next, odd_numbers_list, predicate);
}

int main() {
    Node* initial_list = NULL;
    size_t i = data_length;
    while (i--) {
        size_t value = data[i];
        initial_list = add_element(value, initial_list);
    }

    m(initial_list, print_int);
    putc('\n', stdout);

    Node* odd_values_list = NULL;
    odd_values_list = f(initial_list, odd_values_list, p);

    m(odd_values_list, print_int);
    putc('\n', stdout);

    return 0;
}
