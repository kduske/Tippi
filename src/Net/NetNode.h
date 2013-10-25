//
//  NetNode.h
//  Tippi
//
//  Created by Kristian Duske on 25.10.13.
//
//

#ifndef Tippi_NetNode_h
#define Tippi_NetNode_h

#include "StringUtils.h"
#include "CollectionUtils.h"

namespace Tippi {
    class NetNode {
    private:
        String m_name;
        size_t m_index;
    public:
        NetNode(const String& name, const size_t index);
        const String& getName() const;
        size_t getIndex() const;
    };
    
    template <class N>
    class NetNodeStore {
    private:
        typedef std::map<String, N*> NameMap;
        typedef std::vector<N*> List;
        
        size_t m_nextIndex;
        List m_nodes;
        NameMap m_nodesByName;
    public:
        NetNodeStore() :
        m_nextIndex(0) {}
        
        ~NetNodeStore() {
            VectorUtils::clearAndDelete(m_nodes);
            m_nodesByName.clear();
        }
        
        size_t getNextIndex() const {
            return m_nextIndex;
        }
        
        const List& getNodes() const {
            return m_nodes;
        }
        
        N* findNode(const String& name) {
            return MapUtils::findValue(m_nodesByName, name);
        }
        
        bool insertNode(N* node) {
            assert(node != NULL);
            
            typedef std::pair<typename NameMap::iterator, bool> InsertPos;
            const InsertPos insert = MapUtils::findInsertPos(m_nodesByName, node->getName());
            if (insert.second)
                return false;
            m_nodes.push_back(node);
            m_nodesByName.insert(insert.first, std::make_pair(node->getName(), node));
            ++m_nextIndex;
            return true;
        }
        
        bool deleteNode(N* node) {
            assert(node != NULL);
            
            if (m_nodesByName.erase(node->getName()) == 0)
                return false;
            
            VectorUtils::removeAndDelete(m_nodes, node);
            return true;
        }
    };
}

#endif
