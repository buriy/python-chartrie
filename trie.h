#pragma once
/*
choices[5] = "abcde"
choice_links[5] = {130, 6, 16, 2483, 134}
values[5] = NULL // if this word is illegal
values[6] = VALUE // if this word is legal
choices[6] = "#ing" // "patricia trie" optimization
choice_links[6] = {9641}
values[6] = VALUE
*/
typedef struct Node{
	char *choices;
	struct Node* children;
	int value;
} Node;

typedef struct Trie{
	Node* root;
	Node* all_nodes; // length() = node_count
	char* all_choices; // length() = 2 * node_count - 1
	unsigned int node_count;
} Trie;

Trie* trie_create(int required);
int trie_find_word(Node *trie, const char* str);
Node* trie_add_word(Node *trie, const char* str);
int trie_size(Trie* trie);
void trie_compact(Trie *trie);
void trie_destroy(Trie *trie);
void trie_print(Trie* trie);
