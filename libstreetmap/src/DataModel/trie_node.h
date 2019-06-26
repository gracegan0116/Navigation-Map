#ifndef TRIE_NODE_H
#define TRIE_NODE_H
#include <string>
#include <iostream>
#define alpha_size 26

class trie_node {
private:
    trie_node* children[alpha_size];
    bool end_of_word;
public:
    //constructor and destructor
    trie_node();
    ~trie_node();
    void recursive_destroy(trie_node* root);
    //mutator
    void set_trie_node(char value);
    void set_end_of_word(bool value);
    //accessor
    trie_node* get_trie_node(char value);
    bool get_end_of_word();
    //other useful member functions
    bool check_null_children(char value);
    void insert(trie_node* root, std::string word);
    bool search(trie_node* root, std::string word);
    bool is_last_node(trie_node* root);
    int partial_print(trie_node* root, std::string query);
    void partial_print_helper(trie_node* root, std::string prefix);
};

#endif /* TRIE_NODE_H */

