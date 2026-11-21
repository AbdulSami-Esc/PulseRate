#ifndef TRIE_H
#define TRIE_H

#include <string>
#include "DynamicArray.h"

class Trie {
private:
    struct TrieNode {
        TrieNode* child[128];
        bool isWord;
        TrieNode() : isWord(false) {
            for (int i = 0; i < 128; i++) child[i] = nullptr;
        }
    };

    TrieNode* root;

    void destroy(TrieNode* node) {
        if (node == nullptr) return;
        for (int i = 0; i < 128; i++) destroy(node->child[i]);
        delete node;
    }

    // follows the letters of text, returns nullptr if the path does not exist
    TrieNode* walk(const std::string& text) const {
        TrieNode* cur = root;
        for (size_t i = 0; i < text.size(); i++) {
            unsigned char c = text[i];
            if (c >= 128 || cur->child[c] == nullptr) return nullptr;
            cur = cur->child[c];
        }
        return cur;
    }

    // recursively gathers every word below a node
    void collect(TrieNode* node, std::string& word, DynamicArray<std::string>& out) const {
        if (node->isWord) out.push_back(word);
        for (int i = 0; i < 128; i++) {
            if (node->child[i] != nullptr) {
                word.push_back((char)i);
                collect(node->child[i], word, out);
                word.pop_back();
            }
        }
    }

public:
    Trie() : root(new TrieNode()) {}
    ~Trie() { destroy(root); }
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;

    void insert(const std::string& word) {
        TrieNode* cur = root;
        for (size_t i = 0; i < word.size(); i++) {
            unsigned char c = word[i];
            if (c >= 128) return;
            if (cur->child[c] == nullptr) cur->child[c] = new TrieNode();
            cur = cur->child[c];
        }
        cur->isWord = true;
    }

    bool contains(const std::string& word) const {
        TrieNode* node = walk(word);
        return node != nullptr && node->isWord;
    }

    // all stored words that start with prefix, in alphabetical order
    DynamicArray<std::string> suggest(const std::string& prefix) const {
        DynamicArray<std::string> out;
        TrieNode* start = walk(prefix);
        if (start != nullptr) {
            std::string word = prefix;
            collect(start, word, out);
        }
        return out;
    }
};

#endif