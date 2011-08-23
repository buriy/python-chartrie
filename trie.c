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
//#define _DEBUG
//#undef _DEBUG

Node* _add_root_node(){
    Node* trie = malloc(sizeof(Node));
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
        choices = realloc(choices, (insert_position+2) * sizeof(*choices));
    }else{
        insert_position = 0;
        choices = malloc(2 * sizeof(*choices));
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

Trie* trie_create(){
    Trie* trie = malloc(sizeof(Trie));
    trie->root = _add_root_node();
    return trie;
}

int trie_find_word(Node *trie, const char* str){
    int last_branch=0, last_match=0;
    Node *next_pos = trie;
    const char* ptr = str;
    for(; *ptr; ptr++){
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
    if(trie && trie->choices){
        char* c;
        int i = 0, j;
        for(c = trie->choices; *c; c++, i++){
            if(i){
                for(j=0; j<depth; j++){
                    printf(".");
                }
            }
            printf("%c", *c);
            _trie_print(trie->children+i, depth+1);
        }
    }else{
        printf("\n");
    }
}

void trie_print(Node* node){
    _trie_print(node, 0);
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
            free(node->choices);
            free(node->children);
        }
    }
}

void trie_destroy(Trie* trie){
    trie_delete(trie->root);
    free(trie->root);
    trie->root = NULL;
}

int node_size(Node* node){
    if(node->choices){
        int i, len_choices=strlen(node->choices), sum=1;
        if(node->children){
            for(i=0; i<len_choices; i++){
                Node* next = node->children + i;
                sum += node_size(next);
            }
        }
        return sum;
    }
    return node != NULL;
}

int node_save(Node* node, char* stream){ // returns offset
    //Order: VALUE, choices, [save(c) for c in children]
    int offset = 0;
    if(node){
        if(stream){
            memcpy(stream+offset, &node->value, sizeof(node->value));
        }
        offset += sizeof(node->value);
        if(node->choices){
            int i, len = strlen(node->choices);
            if(stream){
                memcpy(stream+offset, node->choices, len+1);
            }
            offset += len + 1;
            for(i=0; i<len; i++){
                Node* subnode = NULL;
                if(node->children){
                    subnode = node->children + i;
                }else{
                }
                if(stream){
                    offset += node_save(subnode, stream + offset);
                }else{
                    offset += node_save(subnode, NULL);
                }
            }
        }else{
            if(stream){
                *(stream+offset) = 0;
            }
            offset += 1;
        }
    }else{
#ifdef _DEBUG
        printf("Saving NULL node!\n");
        fflush(stdout);
#endif
    }
    return offset;
}

SerialTrie* trie_save(Node* root){
    SerialTrie* st = malloc(sizeof(SerialTrie));
    char* stream;
    int actual;
    st->nodes = node_size(root);
    st->size = node_save(root, NULL);
    st->chars = st->size - st->nodes * sizeof(int); // st->chars == st->nodes * 2 - 1 by now
    //printf("Saving: %d bytes, %d chars, %d nodes\n", st->size, st->chars, st->nodes);
    st->size += 3 * sizeof(int);
    st->stream = malloc(st->size);
    stream = st->stream;
    memcpy(stream, &st->size , sizeof(int)); stream += sizeof(int);
    memcpy(stream, &st->nodes, sizeof(int)); stream += sizeof(int);
    memcpy(stream, &st->chars, sizeof(int)); stream += sizeof(int);
    actual = node_save(root, stream) + 3 * sizeof(int);
#ifdef _DEBUG
    printf("Expected: %d, Actual: %d\n", st->size, actual);
#endif
    return st;
}

typedef struct LoadPosition{
    char* chars;
    Node* nodes;
} LoadPosition;

int node_load(Node* node, LoadPosition *position, char* stream){ // returns offset
    int offset = 0;
    memcpy(&node->value, stream + offset, sizeof(node->value));
    offset += sizeof(node->value);
    if(*(stream+offset)){
        int i, len = strlen(stream + offset);
        memcpy(position->chars, stream + offset, len + 1);
        offset += len + 1;
        
        node->choices = position->chars;
        node->children = position->nodes;

        position->chars += len + 1;
        position->nodes += len;
        
        for(i=0; i<len; i++){
            Node* child = node->children+i;
            offset += node_load(child, position, stream+offset);
        }
    }else{
        node->children = NULL;
        node->choices = NULL;
        offset += 1;
    }
    return offset;
}

FrozenTrie* trie_load(char* stream){
    LoadPosition lp;
    Node* root;
    FrozenTrie* ftrie = malloc(sizeof(FrozenTrie));
    unsigned int node_offset, char_offset;
    int size          = *(int*)stream; stream += sizeof(int);
    ftrie->node_count = *(int*)stream; stream += sizeof(int);
    ftrie->char_count = *(int*)stream; stream += sizeof(int);
#ifdef _DEBUG
    printf("Loading: %d bytes, %d chars, %d nodes\n", size, ftrie->char_count, ftrie->node_count);
#endif
    fflush(stdout);
    ftrie->nodes = malloc(ftrie->node_count * sizeof(Node));
    ftrie->chars = malloc(ftrie->char_count);
    lp.nodes = ftrie->nodes;
    lp.chars = ftrie->chars;
    root = lp.nodes++;
    size -= node_load(root, &lp, stream);
    assert(size == 12);
    node_offset = lp.nodes - ftrie->nodes;
    char_offset = lp.chars - ftrie->chars;
    assert(ftrie->char_count >= char_offset);
    assert(ftrie->node_count == node_offset);
    return ftrie;
}

int trie_size(Node* root){
    return node_size(root) - 1;
}

void trie_fsck(Trie* trie){
    node_size(trie->root);
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
    trie_print(trie->root);

    assert_not_found(trie, "abs", 3);
    assert_not_found(trie, "apple", 1);
    assert_not_found(trie, "breaks", 5);
    assert_not_found(trie, "finall", 6);
    assert_not_found(trie, "123", 0);

    for(i=0; words[i]; i++){
        assert_found(trie, words[i], i);
    }

    {
        SerialTrie* strie = trie_save(trie->root);
        FrozenTrie* trie = trie_load(strie->stream);
    }

    return 0;
}
