/*

choices[5] = "abcde"
children[5] = {130, 6, 16, 2483, 134}
values[5] = NULL // this word is illegal
choices[6] = "#ing" // "patricia trie" optimization -- TODO
children[6] = {9641}
values[6] = VALUE // this word is legal
*/
#include "stdafx.h"
#include "trie.h"

#define EMPTY_VALUE -1

Node* _add_root_node(){
	Node* trie = (Node*)malloc(sizeof(Node));
	trie->choices = NULL;
	trie->children = NULL;
	trie->value = EMPTY_VALUE;
	return trie;
}

int _add_choice(Node* node, const char chr){
	int insert_position;
	char* choices = node->choices;
	if(choices != NULL){
		insert_position = strlen(choices);
		choices = (char*)realloc(choices, (insert_position+2) * sizeof(*choices));
	}else{
		insert_position = 0;
		choices = (char*)malloc(2 * sizeof(*choices));
	}
	choices[insert_position] = chr;
	choices[insert_position+1] = 0;
	node->choices = choices;
	node->children = (Node*)realloc(node->children, (insert_position+1) * sizeof(*node->children));
	node->children[insert_position].choices = NULL;
	node->children[insert_position].children = NULL;
	node->children[insert_position].value = EMPTY_VALUE;
	return insert_position;
}

//int _trie_update_node(Node* trie, const char chr){
//	char* choices = trie->choices;
//	Node** children = trie->children;
//	int item_size = strlen(choices);
//	choices = (char*)realloc(choices, (item_size+2) * sizeof(*choices));
//	choices[item_size] = chr;
//	choices[item_size+1] = 0;
//	children = (Node**)realloc(children, (item_size+1) * sizeof(*children));
//	children[item_size] = EMPTY_NODE;
//	trie->choices = choices;
//	trie->children = children;
//	return item_size; // added branch
//}

Trie* trie_create(int required){
	Trie* trie = (Trie*)malloc(sizeof(Trie));
	trie->root = _add_root_node();
	return trie;
}

int trie_find_word(Node *trie, const char* str){
	int last_branch=0, last_match=0;
	Node *next_pos = trie;
	const char* ptr;
	for(ptr = str; *ptr; ptr++){
		if(!next_pos || !next_pos->choices){
			return ~(ptr-str);
		}else{ // trying prefixes
			char* choices = next_pos->choices;
			char* found = strchr(choices, *ptr);
			if(found == NULL){
				return ~(ptr-str);
			}else{
				last_branch = (found - choices) / sizeof(*choices);
				next_pos = next_pos->children + last_branch;
			}
		}
	}
	if(next_pos && next_pos->value != EMPTY_VALUE){
		return next_pos->value;
	}else{
		return ~(ptr-str); // not found, return processed length
	}
}

Node* trie_add_word(Node *trie, const char* str){
	int last_branch=0;
	Node *next_pos = trie;
	const char* ptr;
	for(ptr = str; *ptr; ptr++){
		char* choices = next_pos->choices;
		char* found = NULL;
		if(choices != NULL){
			found = strchr(choices, *ptr);
		}
		if(found == NULL){
			last_branch = _add_choice(next_pos, *ptr);
		}else{
			last_branch = (found - choices) / sizeof(*choices);
		}
		next_pos = next_pos->children+last_branch;
	}
	return next_pos;
}

void _trie_print(Node* trie, int depth){
	if(depth == 0){
		printf("Trie:\n");
	}

	if(trie && trie->choices){
		char* c;
		int i = 0, j;
		for(c = trie->choices; *c; c++, i++){
			for(j=0; j<depth; j++){
				printf("- ");
			}
			printf("%c:\n", *c);
			_trie_print(trie->children+i, depth+1);
		}
		for(j=0; j<depth; j++){
			printf("- ");
		}
		printf("*\n", *c);
	}else{
		int i;
		for(i=0; i<depth; i++){
			printf("- ");
		}
		printf("(empty)\n");
	}
}

void trie_print(Trie* trie){
	_trie_print(trie->root, 0);
}

void _trie_remove_node(Node* node){
	free(node->choices);
	free(node->children);
}

void trie_delete(Node* node){
	if(node){
		Node* next = node->children;
		char* c = node->choices;
		if(c){
			for(; *c; c++, next++){
				if(next->choices != NULL){
					trie_delete(next);
				}
			}
			_trie_remove_node(node);
		}
	}
}

void trie_destroy(Trie* trie){
	trie_delete(trie->root);
	free(trie->root);
	trie->root = NULL;
}

int _node_is_compacted(Trie* trie, Node* node){
	int pos = node - trie->all_nodes;
	if(node - trie->all_nodes < 0){
		return 0;
	}else{
		int end = sizeof(Node) * trie->node_count;
		return pos < end;
	}
}

void _trie_compact_node(Trie* node, Node* trie){
	//...
}

int _trie_size(Node* node){
	if(node->choices){
		int i, len_choices=strlen(node->choices), sum=1;
		//printf("Node: %d choices: %s\n", len_choices+1, node->choices);		
		for(i=0; i<len_choices; i++){
			Node* next = node->children + i;
			if(next){
				sum += _trie_size(next);
			}
		}
		return sum;
	}
	return node != NULL;
}

int trie_size(Trie* trie){
	return _trie_size(trie->root);
}

void trie_compact(Trie* trie){
	int size = trie_size(trie);
}

void trie_fsck(Trie* trie){
	trie_size(trie);
}

void test_create(){
	Trie *trie = trie_create(0);
	trie_fsck(trie);
}

void assert_not_found(Trie* trie, char* word, int expected_len){
	int actual, found = trie_find_word(trie->root, word);
	assert(found < 0);
	actual = ~found;
	printf("Expected: max prefix length %d for word %s, actual %d ... ", expected_len, word, actual);
	if(actual != expected_len){
		printf("Failed.\n");
	}else{
		printf("Passed.\n");
	}
	assert(actual == expected_len);
}

void assert_found(Trie* trie, char* word, int correct){
	int position;
	printf("Expected: word %s in trie, got ", word);
	position = trie_find_word(trie->root, word);
	if(position >= 0){
		printf("position: %d\n", position);
	}else{
		printf("error: %d\n", ~position);
	}
	assert(position == correct);
}

int _tmain(int argc, _TCHAR* argv[])
{
	char * words[100] = {
		"abstract", "boolean", "break", "byte", "case", "catch", "char", "class",
		"const", "continue", "debugger", "default", "delete", "do", "double", "else", 
		"enum", "export", "extends", "false", "finally", "final", "float", "for", 
		"function", "goto", "if", "implements", "import", "in", "instanceof", "int", 
		"interface", "long", "native", "new", "null", "package", "private", "protected", 
		"public", "return", "short", "static", "super", "switch", "synchronized", "this",
		"throw", "throws", "transient", "true", "try", "typeof", "var", "void",
		"volatile", "while", "with", 
		"a", "ab", "abst", "234", 0
	};
	int i;
	Node* id;
	Trie *trie = trie_create(0);
	trie_fsck(trie);
	//for(i=0; words[i]; i++){
	for(i=0; words[i]; i++){
		char* rev = words[i];
		//rev = _strdup(words[i]);
		//_strrev(rev);
		id = trie_add_word(trie->root, rev);
		id->value = i;
	}
	id = trie_add_word(trie->root, "class");
	assert(id->value == 7);

	trie_fsck(trie);
	//print_trie(trie, 0, 0);

	assert_not_found(trie, "abs", 3);
	assert_not_found(trie, "apple", 1);
	assert_not_found(trie, "breaks", 5);
	assert_not_found(trie, "finall", 6);
	assert_not_found(trie, "123", 0);

	for(i=0; words[i]; i++){
		assert_found(trie, words[i], i);
	}

	return 0;
}
