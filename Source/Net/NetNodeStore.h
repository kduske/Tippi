//
//  NetNodeStore.h
//  Tippi
//
//  Created by Kristian Duske on 23.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef Tippi_NetNodeStore_h
#define Tippi_NetNodeStore_h

#include "GraphNodeStore.h"

#include <map>
#include <string>
#include <vector>

namespace Tippi {
    template <typename NodeType>
    class NetNodeStore : public GraphNodeStore<NodeType> {
    private:
        typedef std::vector<NodeType*> NodeList;
        typedef std::map<std::string, NodeType*> NodeMap;
        
        NodeList m_list;
        NodeMap m_map;
    public:
        ~NetNodeStore() {
            while (!m_list.empty()) delete m_list.back(), m_list.pop_back();
        }
        
        inline void add(NodeType* node) {
            typename NodeMap::iterator it = m_map.find(node->name());
            if (it != m_map.end()) {
                size_t index = it->second->index();
                delete it->second;
                m_map.erase(it);
                
                node->setIndex(index);
                m_list[index] = node;
                m_map[node->name()] = node;
            } else {
                size_t index = m_map.size();
                node->setIndex(index);
                m_list.push_back(node);
                m_map[node->name()] = node;
            }
        }
        
        inline void remove(NodeType* node) {
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
        
        const NodeList& nodes() const {
            return m_list;
        }
        
        inline size_t size() const {
            return m_list.size();
        }
    };
}

#endif
