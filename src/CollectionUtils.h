/*
 Copyright (C) 2013 Kristian Duske
 
 This file is part of Tippi.
 
 Tippi is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 Tippi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Tippi. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Tippi_CollectionUtils_h
#define Tippi_CollectionUtils_h

#include <algorithm>
#include <iterator>
#include <limits>
#include <map>
#include <set>
#include <vector>

namespace Utils {
    template <typename T>
    struct Deleter {
        void operator()(const T* ptr) const {
            delete ptr;
        }
    };

    template <typename T, typename Comparator = std::less<T> >
    struct UniCmp {
    private:
        Comparator m_cmp;
    public:
        bool operator()(const T* lhs, const T* rhs) const {
            return m_cmp(*lhs, *rhs);
        }
        bool operator()(const T& lhs, const T& rhs) const {
            return m_cmp(lhs, rhs);
        }
    };
}

namespace VectorUtils {
    template <typename T>
    void shiftLeft(std::vector<T>& vec, const size_t offset) {
        if (vec.empty() || offset == 0)
            return;

        // (offset > 0) is used to silence a compiler warning
        typedef typename std::vector<T>::iterator::difference_type DiffType;
        const DiffType modOffset = static_cast<DiffType>(offset) % static_cast<DiffType>(vec.size());
        if (modOffset == 0)
            return;
            
        std::rotate(vec.begin(), vec.begin() + modOffset, vec.end());
    }
    
    template <typename T>
    void shiftRight(std::vector<T>& vec, const size_t offset) {
        if (vec.empty() || offset == 0)
            return;
        
        typedef typename std::vector<T>::iterator::difference_type DiffType;
        const DiffType modOffset = static_cast<DiffType>(offset) % static_cast<DiffType>(vec.size());
		shiftLeft(vec, static_cast<size_t>(vec.size()) - modOffset);
    }
    
    template <typename T>
    void eraseAndDelete(std::vector<T*>& vec, typename std::vector<T*>::iterator first, typename std::vector<T*>::iterator last) {
        std::for_each(first, last, Utils::Deleter<T>());
        vec.erase(first, last);
    }
    
    template <typename T>
    void eraseAndDelete(std::vector<T*>& vec, typename std::vector<T*>::iterator first) {
        eraseAndDelete(vec, first, vec.end());
    }
    
    template <typename T>
    void clearAndDelete(std::vector<T*>& vec) {
        std::for_each(vec.begin(), vec.end(), Utils::Deleter<T>());
        vec.clear();
    }
    
    template <typename T>
    void deleteAll(const std::vector<T*>& vec) {
        std::for_each(vec.begin(), vec.end(), Utils::Deleter<T>());
    }
    
    template <typename T, typename I>
    void remove(std::vector<T>& vec, const I& item) {
        vec.erase(std::remove(vec.begin(), vec.end(), item), vec.end());
    }
    
    template <typename T, typename I>
    void remove(std::vector<T*>& vec, const I* item) {
        vec.erase(std::remove(vec.begin(), vec.end(), item), vec.end());
    }

    template <typename T, typename I>
    void removeAndDelete(std::vector<T*>& vec, const I* item) {
        remove(vec, item);
        delete item;
    }
    
    template <typename T>
    typename std::vector<T>::const_iterator find(const std::vector<T>& vec, const T& item) {
        return std::find(vec.begin(), vec.end(), item);
    }
    
    template <typename T>
    typename std::vector<T>::iterator find(std::vector<T>& vec, const T& item) {
        return std::find(vec.begin(), vec.end(), item);
    }
    
    template <typename T, class P>
    T* findIf(const std::vector<T*>& vec, const P& predicate) {
        typename std::vector<T*>::const_iterator it = std::find_if(vec.begin(), vec.end(), predicate);
        if (it == vec.end())
            return NULL;
        return *it;
    }
    
    template <typename T, class P>
    const T* findIf(const std::vector<T>& vec, const P& predicate) {
        typename std::vector<T>::const_iterator it = std::find_if(vec.begin(), vec.end(), predicate);
        if (it == vec.end())
            return NULL;
        return &(*it);
    }
    
    template <typename T>
    bool contains(const std::vector<T>& vec, const T& item) {
        return std::find(vec.begin(), vec.end(), item) != vec.end();
    }
    
    template <typename T>
    bool contains(std::vector<T*>& vec, const T* item) {
        typedef std::vector<T*> VecType;
        typedef typename VecType::const_iterator VecIter;
        
        VecIter first = vec.begin();
        const VecIter last = vec.end();
        while (first != last)
            if (**(first++) == *item)
                return true;
        return false;
    }
    
    template <typename T>
    size_t indexOf(const std::vector<T>& vec, const T& item) {
        for (size_t i = 0; i < vec.size(); ++i)
            if (vec[i] == item)
                return i;
        return vec.size();
    }
    
    template <typename T1, typename T2, typename R>
    void concatenate(const std::vector<T1>& vec1, const std::vector<T2>& vec2, std::vector<R>& result) {
        result.clear();
        result.reserve(vec1.size() + vec2.size());
        result.insert(result.end(), vec1.begin(), vec1.end());
        result.insert(result.end(), vec2.begin(), vec2.end());
    }
    
    template <typename T>
    std::vector<T> concatenate(const std::vector<T>& vec1, const std::vector<T>& vec2) {
        std::vector<T> result;
        concatenate(vec1, vec2, result);
        return result;
    }
    
    template <typename T1, typename T2>
    void append(std::vector<T1>& vec1, const std::vector<T2>& vec2) {
        vec1.insert(vec1.end(), vec2.begin(), vec2.end());
    }
    
    template <typename T>
    std::vector<T> difference(const std::vector<T>& vec1, const std::vector<T>& vec2) {
        typedef std::vector<T> VecType;
        typedef typename VecType::const_iterator VecIter;
        
        VecType result;
        VecIter it, end;
        for (it = vec1.begin(), end = vec1.end(); it != end; ++it) {
            T elem = *it;
            if (!VectorUtils::contains(vec2, elem))
                result.push_back(elem);
        }
        return result;
    }
    
    template <typename O, typename I>
    std::vector<O> cast(const std::vector<I>& input) {
        std::vector<O> output;
        typename std::vector<I>::const_iterator it, end;
        for (it = input.begin(), end = input.end(); it != end; ++it)
            output.push_back(static_cast<O>(*it));
        return output;
    }
    
    template <typename T, typename Compare>
    void insertOrdered(std::vector<T>& vec, T& object) {
        typename std::vector<T>::iterator it = std::lower_bound(vec.begin(), vec.end(), object, Compare());
        if (it == vec.end())
            vec.push_back(object);
        else
            vec.insert(it, object);
    }
    
    template <typename T, typename I, typename Compare>
    void insertOrdered(std::vector<T>& vec, I cur, const I end) {
        Compare cmp;
        while (cur != end) {
            typename std::vector<T>::iterator it = std::lower_bound(vec.begin(), vec.end(), *cur, cmp);
            if (it == vec.end())
                vec.push_back(*cur);
            else
                vec.insert(it, *cur);
            ++cur;
        }
    }
    
    template <typename T, typename Compare>
    void removeOrdered(std::vector<T>& vec, T& object) {
        typename std::vector<T>::iterator it = std::lower_bound(vec.begin(), vec.end(), object, Compare());
        if (it != vec.end() && *it == object)
            vec.erase(it);
    }

    template <typename T, typename I, typename Compare>
    void removeOrdered(std::vector<T>& vec, I cur, const I end) {
        Compare cmp;
        while (cur != end) {
            typename std::vector<T>::iterator it = std::lower_bound(vec.begin(), vec.end(), *cur, cmp);
            if (it != vec.end() && *it == *cur)
                vec.erase(it);
            ++cur;
        }
    }

    template <typename T>
    void insertOrdered(std::vector<T>& vec, T& object) {
        insertOrdered<T, std::less<T> >(vec, object);
    }
    
    template <typename T, typename I>
    void insertOrdered(std::vector<T>& vec, I cur, const I end) {
        insertOrdered<T, I, std::less<T> >(vec, cur, end);
    }
    
    template <typename T>
    void removeOrdered(std::vector<T>& vec, T& object) {
        removeOrdered<T, std::less<T> >(vec, object);
    }
    
    template <typename T, typename I>
    void removeOrdered(std::vector<T>& vec, I cur, const I end) {
        removeOrdered<T, I, std::less<T> >(vec, cur, end);
    }

    template <typename T, typename K, typename Compare>
    std::pair<typename std::vector<T>::const_iterator, bool> setFind(const std::vector<T>& vec, const K& object) {
        Compare cmp;
        typename std::vector<T>::const_iterator it = std::lower_bound(vec.begin(), vec.end(), object, cmp);
        const bool exists = (it != vec.end() && !cmp(*it, object) && !cmp(object, *it));
        return std::make_pair(it, exists);
    }
 
    template <typename T>
    std::pair<typename std::vector<T>::const_iterator, bool> setFind(const std::vector<T>& vec, const T& object) {
        return setFind<T, T, std::less<T> >(vec, object);
    }
    
    template <typename T, typename K, typename Compare>
    std::pair<typename std::vector<T>::iterator, bool> setFind(std::vector<T>& vec, const K& object) {
        Compare cmp;
        typename std::vector<T>::iterator it = std::lower_bound(vec.begin(), vec.end(), object, cmp);
        const bool exists = (it != vec.end() && !cmp(*it, object) && !cmp(object, *it));
        return std::make_pair(it, exists);
    }
    
    template <typename T>
    std::pair<typename std::vector<T>::iterator, bool> setFind(std::vector<T>& vec, const T& object) {
        return setFind<T, T, std::less<T> >(vec, object);
    }
    
    template <typename T, typename K, typename Compare>
    bool setContains(const std::vector<T>& vec, const K& object) {
        return setFind<T, K, Compare>(vec, object).second;
    }
    
    template <typename T, typename K>
    bool setContains(const std::vector<T>& vec, const K& object) {
        return setFind<T, K, std::less<T> >(vec, object).second;
    }
    
    template <typename T>
    bool setInsert(std::vector<T>& vec, const T& object, std::pair<typename std::vector<T>::iterator, bool> find) {
        if (find.second)
            return false;
        if (find.first == vec.end())
            vec.push_back(object);
        else
            vec.insert(find.first, object);
        return true;
    }
    
    template <typename T, typename Compare>
    bool setInsert(std::vector<T>& vec, const T& object) {
        return setInsert(vec, object, setFind<T, T, Compare>(vec, object));
    }
    
    template <typename T, typename Compare>
    void setInsertOrReplace(std::vector<T>& vec, const T& object) {
        typedef std::pair<typename std::vector<T>::iterator, bool> FindResult;
        FindResult find = setFind<T, T, Compare>(vec, object);
        
        if (find.second)
            *find.first = object;
        else if (find.first == vec.end())
            vec.push_back(object);
        else
            vec.insert(find.first, object);
    }
    
    template <typename T, typename I, typename Compare>
    void setInsert(std::vector<T>& vec, I cur, const I end) {
        while (cur != end) {
            setInsert<T, Compare>(vec, *cur);
            ++cur;
        }
    }
    
    template <typename T>
    bool setInsert(std::vector<T>& vec, const T& object) {
        return setInsert<T, std::less<T> >(vec, object);
    }
    
    template <typename T, typename I, typename Compare>
    void setInsertOrReplace(std::vector<T>& vec, I cur, const I end) {
        while (cur != end) {
            setInsertOrReplace<T, Compare>(vec, *cur);
            ++cur;
        }
    }
    
    template <typename T, typename Compare>
    bool setRemove(std::vector<T>& vec, const T& object) {
        typedef std::pair<typename std::vector<T>::iterator, bool> FindResult;
        FindResult find = setFind<T, T, Compare>(vec, object);
        if (find.second) {
            vec.erase(find.first);
            return true;
        }
        return false;
    }
    
    template <typename T, typename I, typename Compare>
    void setRemove(std::vector<T>& vec, I cur, const I end) {
        typedef std::pair<typename std::vector<T>::iterator, bool> FindResult;
        while (cur != end) {
            FindResult find = setFind<T, T, Compare>(vec, *cur);
            if (find.second)
                vec.erase(find.first);
            ++cur;
        }
    }

    template <typename T, typename Compare>
    bool setRemoveAndDelete(std::vector<T*>& vec, const T* object) {
        typedef std::pair<typename std::vector<T*>::iterator, bool> FindResult;
        FindResult find = setFind<T*, T*, Compare>(vec, object);
        if (find.second) {
            vec.erase(find.first);
            delete *find.first;
            return true;
        }
        return false;
    }
    
    template <typename T, typename I, typename Compare>
    void setRemoveAndDelete(std::vector<T*>& vec, I cur, const I end) {
        typedef std::pair<typename std::vector<T*>::iterator, bool> FindResult;
        while (cur != end) {
            while (cur != end) {
                FindResult find = setFind<T*, T*, Compare>(vec, *cur);
                if (find.second) {
                    vec.erase(find.first);
                    delete *find.first;
                }
                ++cur;
            }
        }
    }
}

namespace SetUtils {
    template <typename T, typename Cmp>
    bool equals(const std::set<T, Cmp>& set, const T& lhs, const T& rhs) {
        return !set.key_comp()(lhs, rhs) && !set.key_comp()(rhs, lhs);
    }
    
    template <typename T>
    std::set<T> minus(const std::set<T>& lhs, const std::set<T>& rhs) {
        std::set<T> result;
        std::set_difference(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), std::insert_iterator<std::set<T> >(result, result.end()));
        return result;
    }
    
    template <typename T, typename Cmp>
    void clearAndDelete(std::set<T*, Cmp>& set) {
        std::for_each(set.begin(), set.end(), Utils::Deleter<T>());
        set.clear();
    }

    template <typename T, typename I, typename Cmp>
    void remove(std::set<T*, Cmp>& set, I* item) {
        typename std::set<T*, Cmp>::iterator it = set.find(item);
        if (it != set.end())
            set.erase(it);
    }
    
    template <typename T, typename I, typename Cmp>
    void removeAndDelete(std::set<T*, Cmp>& set, I* item) {
        remove(set, item);
        delete item;
    }
}

namespace MapUtils {
    template <typename K, typename V>
    struct Deleter {
    public:
        void operator()(std::pair<K, V*>& entry) {
            delete entry.second;
        }

        void operator()(std::pair<const K, V*>& entry) {
            delete entry.second;
        }
    };

    template <typename K, typename V>
    struct VectorDeleter {
    public:
        void operator()(std::pair<K, std::vector<V*> >& entry) {
            VectorUtils::clearAndDelete(entry.second);
        }

        void operator()(std::pair<const K, std::vector<V*> >& entry) {
            VectorUtils::clearAndDelete(entry.second);
        }
    };
    
    template <typename K, typename V>
    V* findValue(std::map<K, V*>& map, const K& key) {
        typedef std::map<K, V*> Map;
        typename Map::iterator it = map.find(key);
        if (it == map.end())
            return NULL;
        return it->second;
    }
    
    template <typename K, typename V>
    const V* findValue(const std::map<K, V*>& map, const K& key) {
        typedef std::map<K, V*> Map;
        typename Map::const_iterator it = map.find(key);
        if (it == map.end())
            return NULL;
        return it->second;
    }
    
    template <typename K, typename V>
    std::pair<typename std::map<K, V>::iterator, bool> findInsertPos(std::map<K, V>& map, const K& key) {
        typedef std::map<K, V> Map;
        typename Map::key_compare compare = map.key_comp();
        typename Map::iterator insertPos = map.lower_bound(key);
        if (insertPos == map.end() || compare(key, insertPos->first))
            return std::make_pair(insertPos, false); // map does not contain key, the two keys are not equal (key is less than insertPos' key)
        return std::make_pair(insertPos, true); // map does contain key
    }

    template <typename K, typename V>
    typename std::map<K, V>::iterator findOrInsert(std::map<K, V>& map, const K& key) {
        typedef std::map<K, V> Map;
        const std::pair<typename Map::iterator, bool> insert = findInsertPos(map, key);
        if (!insert.second)
            return map.insert(insert.first, std::make_pair(key, V()));
        return insert.first;
    }

    template <typename K, typename V>
    void insertOrReplace(std::map<K, V>& map, const K& key, V& value) {
        typedef std::map<K, V> Map;
        std::pair<typename Map::iterator, bool> insert = findInsertPos(map, key);
        if (!insert.second)
            map.insert(insert.first, std::make_pair(key, value));
        else
            insert.first->second = value;
    }

    template <typename K, typename V>
    void insertOrReplace(std::map<K, V*>& map, const K& key, V* value) {
        typedef std::map<K, V*> Map;
        std::pair<typename Map::iterator, bool> insert = findInsertPos(map, key);
        if (!insert.second) {
            map.insert(insert.first, std::make_pair(key, value));
        } else {
            delete insert.first->second;
            insert.first->second = value;
        }
    }

    template <typename K, typename V>
    void clearAndDelete(std::map<K, V*>& map) {
        Deleter<K,V> deleter; // need separate instance because for_each only allows modification of the items if the function is not const
        std::for_each(map.begin(), map.end(), deleter);
        map.clear();
    }

    template <typename K, typename V>
    void clearAndDelete(std::map<K, std::vector<V*> >& map) {
        VectorDeleter<K,V> deleter; // need separate instance because for_each only allows modification of the items if the function is not const
        std::for_each(map.begin(), map.end(), deleter);
        map.clear();
    }
}

#endif
