//
//  GraphNodeStore.h
//  Tippi
//
//  Created by Kristian Duske on 23.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef Tippi_GraphNodeStore_h
#define Tippi_GraphNodeStore_h

#include "GraphNode.h"

namespace Tippi {
    template <typename NodeType>
    class GraphNodeStore {
    public:
        virtual void add(NodeType* node) = 0;
        virtual void remove(NodeType* node) = 0;
        
        virtual const std::vector<NodeType*>& nodes() const = 0;
    };
}

#endif
