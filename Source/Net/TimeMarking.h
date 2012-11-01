//
//  TimeMarking.h
//  Tippi
//
//  Created by Kristian Duske on 17.09.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef __Tippi__TimeMarking__
#define __Tippi__TimeMarking__

#include "Transition.h"
#include "NetTypes.h"

#include <cassert>
#include <map>
#include <sstream>
#include <string>

namespace Tippi {
    class Transition;
    
    class TimeMarking {
    public:
        static const unsigned int Disabled;
    private:
        typedef unsigned int* Markings;
        typedef Transition const** Transitions;
        
        size_t m_size;
        Markings m_markings;
        Transitions m_transitions;
    public:
        TimeMarking() :
        m_size(0),
        m_markings(NULL),
        m_transitions(NULL) {}
        
        TimeMarking(const TransitionList& transitions) :
        m_size(transitions.size()),
        m_markings(new unsigned int[m_size]),
        m_transitions(new Transition const *[m_size]) {
            for (unsigned int i = 0; i < transitions.size(); i++) {
                size_t index = transitions[i]->index();
                m_markings[index] = Disabled;
                m_transitions[index] = transitions[i];
            }
        }
        
        TimeMarking(const TimeMarking& other) :
        m_size(other.size()),
        m_markings(new unsigned int[m_size]),
        m_transitions(new Transition const *[m_size]) {
            for (unsigned int i = 0; i < m_size; i++) {
                m_markings[i] = other.m_markings[i];
                m_transitions[i] = other.m_transitions[i];
            }
        }
        
        const TimeMarking& operator= (const TimeMarking& other) {
            if (m_size != other.size()) {
                if (m_markings != NULL)
                    delete [] m_markings;
                if (m_transitions != NULL)
                    delete [] m_transitions;
                m_size = other.size();
                m_markings = new unsigned int[m_size];
                m_transitions = new Transition const *[m_size];
            }
            for (unsigned int i = 0; i < m_size; i++) {
                m_markings[i] = other.m_markings[i];
                m_transitions[i] = other.m_transitions[i];
            }
            return *this;
        }
        
        ~TimeMarking() {
            if (m_markings != NULL) {
                delete [] m_markings;
                m_markings = NULL;
            }
            if (m_transitions != NULL) {
                delete [] m_transitions;
                m_transitions = NULL;
            }
        }
        
        inline size_t size() const {
            return m_size;
        }
        
        bool operator== (const TimeMarking& other) const {
            if (m_size != other.m_size)
                return false;
            for (unsigned int i = 0; i < m_size; i++) {
                if (m_transitions[i] != other.m_transitions[i])
                    return false;
                if (m_markings[i] != other.m_markings[i])
                    return false;
            }
            return true;
        }
        
        const unsigned int& operator[] (size_t index) const {
            assert(index < m_size);
            return m_markings[index];
        }
        
        const unsigned int& operator[] (const Transition& transition) const {
            size_t index = transition.index();
            assert(index < m_size);
            assert(m_transitions[index] == &transition);
            return m_markings[index];
        }
        
        unsigned int& operator[] (const Transition& transition) {
            size_t index = transition.index();
            assert(index < m_size);
            assert(m_transitions[index] == &transition);
            return m_markings[index];
        }

        inline std::string asString() const {
            std::stringstream buffer;
            
            buffer << "[";
            for (unsigned int i = 0; i < m_size; i++) {
                const Transition& transition = *m_transitions[i];
                buffer << transition.name() << ":";
                if ((*this)[transition] == Disabled)
                    buffer << "#";
                else
                    buffer << (*this)[transition];
                if (i < m_size - 1)
                    buffer << ",";
            }
            buffer << "]";
            
            return buffer.str();
        }
        
        inline std::string asShortString() const {
            std::stringstream buffer;
            
            buffer << "[";
            for (unsigned int i = 0; i < m_size; i++) {
                const Transition& transition = *m_transitions[i];
                if ((*this)[transition] == Disabled)
                    buffer << "#";
                else
                    buffer << (*this)[transition];
                if (i < m_size - 1)
                    buffer << ",";
            }
            buffer << "]";
            
            return buffer.str();
        }
    };
}

#endif /* defined(__Tippi__TimeMarking__) */
