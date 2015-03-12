/*
 Copyright (C) 2013-2014 Kristian Duske
 
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

#ifndef __Tippi__StronglyConnectedComponents__
#define __Tippi__StronglyConnectedComponents__

#include "GraphNode.h"
#include "GraphEdge.h"

#include <map>
#include <stack>
#include <vector>

namespace Tippi {
    template <class CommonNode>
    class StronglyConnectedComponents {
    public:
        typedef std::vector<CommonNode*> Component;
        typedef std::vector<Component> ComponentList;
    private:
        typedef size_t Index;
        
        struct NodeInfo {
            Index index;
            Index lowLink;
            bool onStack;
            
            NodeInfo() : index(0), lowLink(0), onStack(false) {}
            NodeInfo(const Index i_index) : index(i_index), lowLink(i_index), onStack(true) {}
            
            bool isRootNode() const {
                return lowLink == index;
            }
            
            void updateLowLink(const Index i_index) {
                lowLink = std::min(lowLink, i_index);
            }
        };
        
        typedef std::map<CommonNode*, NodeInfo> InfoMap;
        InfoMap m_infos;
        
        typedef std::stack<CommonNode*> NodeStack;
        NodeStack m_stack;
        
        Index m_index;
        ComponentList m_components;
    public:
        StronglyConnectedComponents() : m_index(0) {}
        
        const ComponentList& components() const {
            return m_components;
        }
        
        template <typename Node>
        NodeInfo& visitNode(Node* node) {
            assert(m_infos.count(node) == 0);
            typename InfoMap::iterator nIt = m_infos.insert(std::make_pair(node, NodeInfo(m_index++))).first;
            NodeInfo& nodeInfo = nIt->second;
            m_stack.push(node);
            
            typedef typename Node::Outgoing::List OutList;
            const OutList& edges = node->getOutgoing();
            typename OutList::const_iterator eIt, eEnd;
            for (eIt = edges.begin(), eEnd = edges.end(); eIt != eEnd; ++eIt) {
                typename Node::Outgoing* edge = *eIt;
                
                typename InfoMap::iterator iIt = m_infos.find(edge->getTarget());
                if (iIt == m_infos.end()) {
                    const NodeInfo& targetInfo = visitNode(edge->getTarget());
                    nodeInfo.updateLowLink(targetInfo.lowLink);
                } else {
                    const NodeInfo& targetInfo = iIt->second;
                    if (targetInfo.onStack)
                        nodeInfo.updateLowLink(targetInfo.index);
                }
            }
            
            if (nodeInfo.isRootNode()) {
                Component component;
                CommonNode* top;
                do {
                    top = m_stack.top(); m_stack.pop();
                    m_infos[top].onStack = false;
                    component.push_back(top);
                } while (top != node);
                
                m_components.push_back(component);
            }
            
            return nodeInfo;
        }
    };
}

#endif /* defined(__Tippi__StronglyConnectedComponents__) */
