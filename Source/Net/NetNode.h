//
//  NetNode.h
//  TPNA
//
//  Created by Kristian Duske on 08.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef TPNA_NetNode_h
#define TPNA_NetNode_h

#include <cassert>
#include <map>
#include <string>
#include <vector>

namespace TPNA {
    class NetNode {
    public:
        static const size_t InvalidIndex;
    protected:
        size_t m_index;
        std::string m_name;
    public:
        NetNode(const std::string& name, size_t index = InvalidIndex) :
        m_name(name),
        m_index(index) {}
        
        inline const std::string& name() const {
            return m_name;
        }
        
        inline const size_t index() const {
            return m_index;
        }
        
        inline void setIndex(size_t index) {
            assert(m_index == InvalidIndex);
            assert(index != InvalidIndex);
            m_index = index;
        }
    };
    
    template <typename NodeType>
    class NodeStore {
    private:
        typedef std::vector<NodeType*> NodeList;
        typedef std::map<std::string, NodeType*> NodeMap;
        
        NodeList m_list;
        NodeMap m_map;
    public:
        ~NodeStore() {
            while (!m_list.empty()) delete m_list.back(), m_list.pop_back();
        }
        
        inline void add(NodeType* item) {
            typename NodeMap::iterator it = m_map.find(item->name());
            if (it != m_map.end()) {
                size_t index = it->second->index();
                delete it->second;
                m_map.erase(it);
                
                item->setIndex(index);
                m_list[index] = item;
                m_map[item->name()] = item;
            } else {
                size_t index = m_map.size();
                item->setIndex(index);
                m_list.push_back(item);
                m_map[item->name()] = item;
            }
        }
        
        inline const NodeType* operator[] (size_t index) const {
            assert(index < m_list.size);
            return m_list[index];
        }
        
        inline NodeType* operator[] (size_t index) {
            assert(index < m_list.size());
            return m_list[index];
        }
        
        inline const NodeType* operator[] (const std::string& name) const {
            typename NodeMap::const_iterator it = m_map.find(name);
            if (it == m_map.end())
                return NULL;
            return it->second;
        }
        
        inline NodeType* operator[] (const std::string& name) {
            typename NodeMap::iterator it = m_map.find(name);
            if (it == m_map.end())
                return NULL;
            return it->second;
        }
        
        inline const NodeList& asList() const {
            return m_list;
        }
        
        inline size_t size() const {
            return m_list.size();
        }
    };
}

#endif
