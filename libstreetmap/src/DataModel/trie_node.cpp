#include "trie_node.h"
//constructor and destructor
trie_node::trie_node() {
    end_of_word = false;
    for(int i = 0; i < alpha_size; i++)
        children[i] = NULL;
}
trie_node::~trie_node(){
    recursive_destroy(this);
}
void trie_node::recursive_destroy(trie_node* root){
    if(root == NULL)
        return;
    for(int i = 0; i<alpha_size; i++){
        recursive_destroy(root->children[i]);
    }
    delete root;
}

//mutators
void trie_node::set_trie_node(char value){
    children[value - 'a'] = new trie_node();
}
void trie_node::set_end_of_word(bool value){
    end_of_word = value;
}

//accessors
trie_node* trie_node::get_trie_node(char value){
    return children[value-'a'];
}
bool trie_node::get_end_of_word(){
    return end_of_word;
}

bool trie_node::check_null_children(char value){
    if (children[value-'a']==NULL)
        return true;
    else
        return false;
}

//other useful member functions
void trie_node::insert(trie_node* root, std::string word){
    trie_node* current = root;
    //when the trie tree is empty
    if(!current)
        current = new trie_node();
    
    for(int i = 0; i<(int)word.length();i++){
        if(current->check_null_children(word[i]))
            current->set_trie_node(word[i]);
        current = current->get_trie_node(word[i]);
    }
    current->set_end_of_word(true);
}
bool trie_node::search(trie_node* root, std::string word){
    trie_node* current = root;
    for (int i = 0; i<(int)word.length(); i++){
        if(current->check_null_children(word[i]))
            return false;
        current = current->get_trie_node(word[i]);
    }
    return current->get_end_of_word();
}
bool trie_node::is_last_node(trie_node* root){
    for(int i = 0; i<alpha_size; i++)
        if(root->children[i]!=NULL)
            return false;
    return true;
}
void trie_node::partial_print_helper(trie_node* root, std::string prefix) 
{ 
    int length = prefix.length();
    // found a string in trie that matches prefix
    if (root->get_end_of_word()) 
        std::cout << prefix << std::endl; 

    for (int i = 0; i < alpha_size; i++) { 
        if (root->children[i]) { 
            // append current character to prefix string 
            prefix.push_back('a' + i); 
            // recursive call to loop over the rest nodes
            partial_print_helper(root->children[i], prefix); 
            //reset prefix back to the original
            prefix = prefix.substr(0,length);
        } 
    } 
} 
// return 0 = no string with query and no query in trie, -1 = only query exist in trie, 1 = proceed to partial_print_helper
int trie_node::partial_print(trie_node* root, std::string query) { 
    trie_node* current = root; 
    int length = query.length(); 
    
    for (int level = 0; level < length; level++) { 
        int index = query[level]-'a'; 
        // no string in the Trie has this prefix 
        if (!current->children[index]) 
            return 0; 
        current = current->children[index]; 
    } 
  
    bool is_word = (current->get_end_of_word()); 
    bool is_last = current->is_last_node(current); 
    //only query exists in trie
    if (is_word && is_last) { 
        std::cout << query << std::endl; 
        return -1; 
    } 
    // If there are more node
    if (!is_last) { 
        std::string prefix = query; 
        partial_print_helper(current, query); 
        return 1; 
    } 
    return 2;
} 