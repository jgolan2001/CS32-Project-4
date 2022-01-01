//
//  Table.cpp
//  Project 4
//
//  Created by Jordan Golan on 5/30/21.
//

#include "Table.h"

//helper function
//convert string to double
bool stringToDouble(string s, double& d)
{
    char* end;
    d = std::strtof(s.c_str(), &end);
    
    bool result = false;
    
    if (end == s.c_str() + s.size() && !s.empty())
    {
        result = true;
    }
    return result;
}


Table::Table(std::string keyColumn, const std::vector<std::string>& columns)
{
    m_keyField = keyColumn;
    m_cols = columns;
    
    // find index of the keyfield
    for (int i = 0; i < m_cols.size(); i++)
    {
        if (m_cols[i] == m_keyField)
        {
            m_index = i;
            break;
        }
    }
}

bool Table::good() const
{
    bool result = false;
    
    if (m_cols.size() == 0) // if columns vector is empty
        return false;
    
    if (m_keyField == "") // if keyis empty string
        return false;
    
    for (int i = 0; i < m_cols.size(); i++) // look to seee if any elemeent is empty string
    {
        if (m_cols[i] == "")
            return false;
        
        if (m_cols[i] == m_keyField)
            result = true;
    }
    
    if (result == false) //if key is not in column vector
        return false;
    
    for (int i = 0; i < m_cols.size(); i++) // look for duplicates.
    {
        for (int j = i+1; j < m_cols.size(); j++)
        {
            if (m_cols[i] == m_cols[j])
                return false;
        }
    }
    return true;
}

Table::~Table()
{
    
}

bool Table::insert(const std::string& recordString)
{
    if (!good())
        return false;
    
    string input = recordString;
    StringParser parser(input);
    vector<string> row;
    string s = "";
    int i = 0;
    
    while (parser.getNextField(s)) // each parsed string is an input in vector
    {
        row.push_back(s);
        i++;
    }
    
    if (i != m_cols.size()) // if cols in recordstring not equal to columns of table return false
        return false;
    
    m_hashTable.add(row); // insert vector into hashtable
    
    return true;
}

void Table::find(std::string key, std::vector<std::vector<std::string>>& records) const
{
    records.clear(); // clears final vector
    vector<vector<string>> result; // create result vector
    
    result = m_hashTable.search(key); // add every record that contains key into list
    
    for (int i = 0; i < result.size(); i++) //iterate thru each vector
    {
        for (int j = 0; j < result[i].size(); j++) // iterate thru each element of each vector
        {
            if (j == m_index && result[i][j] == key) // only add vector to records if the key is in correct index
            {
                records.push_back(result[i]);
            }
        }
    }
}

int Table::select(std::string query, std::vector<std::vector<std::string>>& records) const
{
    records.clear(); // clear final vector
    
    StringParser parser(query); //convert query into vector
    vector<string> parsedQuery;
    string s = "";
    while(parser.getNextField(s))
    {
        parsedQuery.push_back(s);
    }
    
    if (parsedQuery.size() != 3) //if not 3 elements, invalid
        return -1;
    
    for (int i = 0; i < parsedQuery.size(); i++) // if any empty strings, invalid
    {
        if (parsedQuery[i] == "")
            return -1;
    }
    
    int count = 0;
    int cIndex = -1;
    
    for (int i = 0; i < m_cols.size(); i++)
    {
        if (parsedQuery[0] == m_cols[i])
        {
            count++;
            cIndex = i; // store index of correct column
        }
    }
    
    if (count != 1) // if duplicate values, invalid
        return -1;
    
    for (auto & c : parsedQuery[1]) //convert to lowercase
    {
        c = tolower(c);
    }
    // if second element isnt valid operator, invalid
    if (parsedQuery[1] != "<=" && parsedQuery[1] != "<" && parsedQuery[1] != ">=" && parsedQuery[1] != ">" && parsedQuery[1] != "!=" && parsedQuery[1] != "==" && parsedQuery[1] != "=" && parsedQuery[1] != "lt" && parsedQuery[1] != "le" && parsedQuery[1] != "gt" && parsedQuery[1] != "ge" && parsedQuery[1] != "ne" && parsedQuery[1] != "eq")
    {
        return -1;
    }
    
    double var;
    vector<vector<string>> result;
    
    if (parsedQuery[1] == "lt" || parsedQuery[1] == "le" || parsedQuery[1] == "gt" || parsedQuery[1] == "ge" || parsedQuery[1] == "ne" || parsedQuery[1] == "eq")
    {
        if (!stringToDouble(parsedQuery[2], var)) // if the operator is for numbers, third element must be number
        {
            return -1;
        }
        
        result = m_hashTable.querySearch(cIndex, parsedQuery[1], var); //do query search
    }
    else
    {
        result = m_hashTable.querySearch(cIndex, parsedQuery[1], parsedQuery[2]); // query searh for strings
    }
    
    int resultCount = 0;
    
    for (int i = 0; i < result.size(); i++) // iterate thru each vector in result
    {
        if (result[i].empty())
        {
            resultCount++;
            continue;
        }
        else
        {
            records.push_back(result[i]); //push vector into records
        }
    }
    return resultCount;
}


bool StringParser::getNextField(std::string& fieldText)
{
    m_start = m_text.find_first_not_of(" \t\r\n", m_start);
    if (m_start == std::string::npos)
    {
        m_start = m_text.size();
        fieldText = "";
        return false;
    }
    if (m_text[m_start] != '\'')
    {
        size_t end = m_text.find_first_of(" \t\r\n", m_start+1);
        fieldText = m_text.substr(m_start, end-m_start);
        m_start = end;
        return true;
    }
    fieldText = "";
    for (;;)
    {
        m_start++;
        size_t end = m_text.find('\'', m_start);
        fieldText += m_text.substr(m_start, end-m_start);
        m_start = (end != std::string::npos ? end+1 : m_text.size());
        if (m_start == m_text.size()  ||  m_text[m_start] != '\'')
            break;
        fieldText += '\'';
    }
    return true;
}

// HashTable function immplementation

HashTable::HashTable()
{
    list<Node> nodes; // create hashtable
    
    for (int i = 0; i < 999; i++)
    {
        Buckets.push_back(nodes);
    }
}

vector<vector<string>> HashTable::search(const string& key) const
{
    vector<vector<string>> result; // create result vector
    
    for (int i = 0; i < 999; i++) // itereate thru every bucket
    {
        list<Node> current = Buckets[i];
        
        if (current.empty()) // if empty skip to next bucket
            continue;;
        
        // iterate thru list of vectors in each bucket
        for (list<Node>::iterator iter = current.begin(); iter != current.end(); iter++)
        {
            for (int j = 0; j < iter -> m_row.size(); j++) // iterate thru each element in the vector
            {
                if (iter -> m_row[j] == key) // if vector contains key, add to result
                {
                    result.push_back(iter -> m_row);
                }
            }
        }
    }
    return result;
}

void HashTable::add(const vector<string>& s)
{
    list<Node> current = Buckets[hashFunc(s)]; //use hashFunc to find correct bucket to add to
    
    Node newNode; //create new node
    newNode.m_row = s;
    current.push_back(newNode); //add new node to list
    Buckets[hashFunc(s)] = current; //replace old bucket w new one
    
}

unsigned int HashTable::hashFunc(const vector<string>& hash) const
{
    unsigned int value = std::hash<string>()(hash[0]); // hash first element in vector
    
    for (int i = 1; i < hash.size(); i++) // loop thru every element in vector
    {
        value = value^std::hash<string>()(hash[i]);
    }
    
    unsigned int bucket = value % 999; // find correct bucket
    return bucket;
}
// for letter operators
vector<vector<string>> HashTable::querySearch(const int& index, const string& op, const string& token) const
{
    vector<vector<string>> result; // create result vector
    
    for (int i = 0; i < 999; i++) //iterate thru every bucket
    {
        list<Node> current = Buckets[i];
        
        if (current.empty()) // if bucket empty, skip
            continue;
        // iterate thru list of vectors in each bucket
        for (list<Node>::iterator iter = current.begin(); iter != current.end(); iter++)
        {
            for (int j = 0; j < iter -> m_row.size(); j++) // iterate thru every element in vector
            {
                if (j == index) //check element at index
                {
                    // push element if it passes operation
                    if (op == "<")
                        if (iter->m_row[j] < token)
                            result.push_back(iter->m_row);
                    if (op == "<=")
                        if (iter->m_row[j] <= token)
                            result.push_back(iter->m_row);
                    if (op == ">")
                        if (iter->m_row[j] > token)
                            result.push_back(iter->m_row);
                    if (op == ">=")
                        if (iter->m_row[j] >= token)
                            result.push_back(iter->m_row);
                    if (op == "!=")
                        if (iter->m_row[j] != token)
                            result.push_back(iter->m_row);
                    if (op == "==")
                        if (iter->m_row[j] == token)
                            result.push_back(iter->m_row);
                    if (op == "=")
                        if (iter->m_row[j] == token)
                            result.push_back(iter->m_row);
                }
            }
        }
    }
    return result;
}
// for symbol operators
vector<vector<string>> HashTable::querySearch(const int& index, const string& op, const double& token) const
{
    vector<vector<string>> result; // create empty result vector
    for (int i = 0; i < 999; i++)
    {
        list<Node> current = Buckets[i];
        
        if (current.empty())
            continue;
        
        for (list<Node>::iterator iter = current.begin(); iter != current.end(); iter++)
        {
            for (int j = 0; j < iter->m_row.size(); j++)
            {
                double var;
                if (j==index && stringToDouble(iter->m_row[j], var))
                {
                    if (op == "lt")
                        if (var < token)
                            result.push_back(iter->m_row);
                    if (op == "le")
                        if (var <= token)
                            result.push_back(iter->m_row);
                    if (op == "gt")
                        if (var > token)
                            result.push_back(iter->m_row);
                    if (op == "ge")
                        if (var >= token)
                            result.push_back(iter->m_row);
                    if (op == "ne")
                        if (var != token)
                            result.push_back(iter->m_row);
                    if (op == "eq")
                        if (var == token)
                            result.push_back(iter->m_row);
                }
                
                else if (j == index && !stringToDouble(iter->m_row[j], var))
                {
                    vector<string> empty;
                    result.push_back(empty);
                }
            }
        }
    }
    return result;
}
