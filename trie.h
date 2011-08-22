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
typedef struct Node {
	char *choices;
	struct Node* children;
	int value;
} Node;

typedef struct Trie {
	Node* root;
} Trie;

typedef struct SerialTrie {
	Node* root;
	char* stream;
	unsigned int nodes;
	unsigned int chars;
	unsigned int size; // == len(stream)
} SerialTrie;

typedef struct FrozenTrie {
	Node* nodes;
	char* chars;
	unsigned int node_count; // == len(nodes)
	unsigned int char_count; // == len(chars)
} FrozenTrie;

Trie* trie_create();
int trie_find_word(Node *trie, const char* str);
Node* trie_add_word(Node *trie, const char* str);
int trie_size(Node* trie);
void trie_destroy(Trie *trie);
void trie_print(Node* trie);
SerialTrie* trie_save(Node* trie);
FrozenTrie* trie_load(char* stream);
