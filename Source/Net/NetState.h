//
//  NetState.h
//  Tippi
//
//  Created by Kristian Duske on 21.09.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__NetState__
#define __Tippi__NetState__

#include "PlaceMarking.h"
#include "TimeMarking.h"

#include <set>
#include <sstream>
#include <string>

namespace Tippi {
    class NetState {
    private:
        PlaceMarking m_placeMarking;
        TimeMarking m_timeMarking;
    public:
        NetState(const PlaceMarking& placeMarking, const TimeMarking& timeMarking) :
        m_placeMarking(placeMarking),
        m_timeMarking(timeMarking) {}
        
        inline const PlaceMarking& placeMarking() const {
            return m_placeMarking;
        }
        
        inline PlaceMarking& placeMarking() {
            return m_placeMarking;
        }
        
        inline const TimeMarking& timeMarking() const {
            return m_timeMarking;
        }
        
        inline TimeMarking& timeMarking() {
            return m_timeMarking;
        }
        
        inline std::string asString() const {
            std::stringstream buffer;
            buffer << "(" << m_placeMarking.asString() << ", " << m_timeMarking.asString() << ")";
            return buffer.str();
        }
    };
    
    class StateComparator {
    public:
        inline bool operator() (const NetState& left, const NetState& right) const {
            const PlaceMarking& leftPlaceMarking = left.placeMarking();
            const PlaceMarking& rightPlaceMarking = right.placeMarking();
            assert(leftPlaceMarking.size() == rightPlaceMarking.size());
            
            for (size_t i = 0; i < leftPlaceMarking.size(); i++) {
                if (leftPlaceMarking[i] < rightPlaceMarking[i])
                    return true;
                if (leftPlaceMarking[i] > rightPlaceMarking[i])
                    return false;
            }
            
            const TimeMarking& leftTimeMarking = left.timeMarking();
            const TimeMarking& rightTimeMarking = right.timeMarking();
            assert(leftTimeMarking.size() == rightTimeMarking.size());
            
            for (size_t i = 0; i < leftTimeMarking.size(); i++) {
                if (leftTimeMarking[i] < rightTimeMarking[i])
                    return true;
                if (leftTimeMarking[i] > rightTimeMarking[i])
                    return false;
            }
            
            // both are equal
            return false;
        }
    };
    
    typedef std::set<NetState, StateComparator> NetStateSet;
}

#endif /* defined(__Tippi__State__) */
