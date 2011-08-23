cdef extern from "malloc.h":
    void free(void* ptr)

cdef extern from "trie.h":
    cdef struct Trie:
        Node* root
    cdef struct Node:
        int value
    cdef struct SerialTrie:
        Node* root
        char* stream
        unsigned int nodes
        unsigned int chars
        unsigned int size

    cdef struct FrozenTrie:
        Node* nodes
        char* chars
        unsigned int node_count
        unsigned int char_count

    Trie* trie_create()
    int trie_find_word(Node *trie, char* word)
    Node* trie_add_word(Node *trie, char* word)
    int trie_size(Node* trie)
    void trie_destroy(Trie *trie)
    void trie_print(Node* trie)
    SerialTrie* trie_save(Node* trie)
    FrozenTrie *trie_load(char* stream)

cdef class CharTrie:
    cdef Trie *trie
    def __cinit__(self):
        self.trie = trie_create()

    def __dealloc__(self):
        if self.trie is not NULL:
            trie_destroy(self.trie)
            free(self.trie)
            self.trie = NULL
            
    def __len__(self):
        return trie_size(self.trie.root)

    def __getitem__(self, key):
        node = trie_find_word(self.trie.root, key)
        if node < 0:
            return None
        return node

    def __setitem__(self, key, value):
        assert value>=0, "Value should be >= 0"
        node = trie_add_word(self.trie.root, key)
        node.value = value

    def dumps(self):
        cdef SerialTrie* buf = trie_save(self.trie.root)
        try:
            result = buf.stream[:buf.size]
        finally:
            free(buf.stream)
            free(buf)
        return result

    def debug_print(self):
        trie_print(self.trie.root)

    def find(self, key):
        node = trie_find_word(self.trie.root, key)
        return node

cdef class FrozenCharTrie:
    cdef FrozenTrie *trie
    def __cinit__(self):
        self.trie = NULL

    def __dealloc__(self):
        if self.trie is not NULL:
            free(self.trie.nodes)
            free(self.trie.chars)
            free(self.trie)
            self.trie = NULL
            
    def __len__(self):
        return self.trie.node_count - 1

    def __getitem__(self, key):
        node = trie_find_word(self.trie.nodes, key)
        if node < 0:
            return None
        return node

    def loads(self, stream):
        self.trie = trie_load(stream)

    def debug_print(self):
        trie_print(self.trie.nodes)

    def find(self, key):
        node = trie_find_word(self.trie.nodes, key)
        return node
