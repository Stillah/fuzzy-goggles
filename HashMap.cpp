#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

template <typename K, typename V>
class Map
{
    virtual int getSize() = 0;
    virtual void put(K key, V value) = 0;
    virtual V get(K key) = 0;
    virtual std::vector<pair<K, V>> entrySet() = 0;
};

template <typename K, typename V>
class HashMap : Map<K, V> {
    int mapSize, capacity;
    const int INITIAL_CAPACITY = 64, MAX_LOAD_FACTOR = 5;
    vector<list<pair<K, V>>> hashTable;

    int getHash(K key) { return hash<K>{}(key) % capacity;} //hopefully this works

public:
    HashMap() {
        mapSize = 0;
        capacity = INITIAL_CAPACITY;
        vector<list<pair<K, V>>> fix(INITIAL_CAPACITY);
        hashTable = fix;
    }

    void put(K key, V value) override {
        if (MAX_LOAD_FACTOR * capacity <= mapSize) {
            capacity = MAX_LOAD_FACTOR * mapSize;
            vector<list<pair<K, V>>> newHashTable(capacity);

            for (const auto& lst: hashTable) {
                for (const auto& it: lst)
                    newHashTable[getHash(it.first)].emplace_back(it);
            }
            hashTable = newHashTable;
        }

        if (!isEmpty()) {
            auto it = hashTable[getHash(key)].begin();
            for (int i = 0; i < hashTable[getHash(key)].size(); i++, it++) { // it doesnt change the actual object
                if ((*it).first == key) {
                    (*it).second = value;
                    return;
                }
            }
        }
        hashTable[getHash(key)].emplace_back(key, value);
        mapSize++;

    }

    V get(K key) override {
        if (!isEmpty()) {
            for (const pair<K, V> &it: hashTable[getHash(key)]) {
                if (it.first == key)
                    return it.second;
            }
        }
        return 0;
    }

    vector<pair<K, V>> entrySet() override {
        if (isEmpty()) throw runtime_error("The map is empty"); // might be removable
        vector <pair<K, V>> returnVector(mapSize);
        int i = 0;
        for (const auto& lst: hashTable) {
            for (const auto& it: lst) {
                returnVector[i] = it;
                i++;
            }
        }
        return returnVector;
    }

    int getSize() override {return mapSize; }

    bool isEmpty() {return mapSize == 0; }

};
