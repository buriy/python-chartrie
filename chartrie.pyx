cdef extern from "trie.h":
    cdef struct Trie:
        Node* root
    cdef struct Node:
        int value

    Trie* trie_create(int required)
    int trie_find_word(Node *trie, char* str)
    Node* trie_add_word(Node *trie, char* str)
    int trie_size(Trie* trie)
    char* trie_compact(Trie *trie)
    void trie_destroy(Trie *trie)
    void trie_print(Trie* trie)

cdef class CharTrie:
    cdef Trie *trie
    def __cinit__(self):
        self.trie = trie_create(0)

    def __dealloc__(self):
        if self.trie is not NULL:
            trie_destroy(self.trie)
    def __len__(self):
        return trie_size(self.trie)

    def __getitem__(self, key):
        node = trie_find_word(self.trie.root, key)
        if node < 0:
            return None
        return node

    def __setitem__(self, key, value):
        node = trie_add_word(self.trie.root, key)
        node.value = value

    #def save(self):
    #    return trie_serialize(self.trie)

    #def load(self, stream):
    #    return trie_deserialize(self.trie)

    def debug_print(self):
        trie_print(self.trie)

    def find(self, key):
        cdef Trie* trie = self.trie
        node = trie_find_word(trie.root, key)
        return node
    