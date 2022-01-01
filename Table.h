//
//  Table.h
//  Project 4
//
//  Created by Jordan Golan on 5/30/21.
//

#ifndef Table_h
#define Table_h

#include <stdio.h>
#include <functional>
#include <cstdlib>
#include <string>
#include <vector>
#include <list>
using namespace std;

class HashTable
{
public:
    HashTable();
    vector<vector<string>> search(const string& key) const;
    void add(const vector<string>& s);
    unsigned int hashFunc(const vector<string>& hash) const;
    vector<vector<string>> querySearch(const int& index, const string& op, const string& token) const;
    vector<vector<string>> querySearch(const int& index, const string& op, const double& token) const;
private:
    struct Node
    {
        vector<string> m_row;
    };
    
    vector<list<Node>> Buckets;
};

class Table
{
public:
    Table(std::string keyColumn, const std::vector<std::string>& columns);
    ~Table();
    bool good() const;
    bool insert(const std::string& recordString);
    void find(std::string key, std::vector<std::vector<std::string>>& records) const;
    int select(std::string query, std::vector<std::vector<std::string>>& records) const;

        // We prevent a Table object from being copied or assigned by
        // making the copy constructor and assignment operator unavailable.
    Table(const Table&) = delete;
    Table& operator=(const Table&) = delete;
private:
    vector<string> m_cols;
    string m_keyField;
    int m_index;
    HashTable m_hashTable;
    
};

class StringParser
{
public:
    StringParser(std::string text = "")
    {
        setString(text);
    }

    void setString(std::string text)
    {
        m_text = text;
        m_start = 0;
    }

    bool getNextField(std::string& field);

private:
    std::string m_text;
    size_t m_start;
};

#endif /* Table_h */
