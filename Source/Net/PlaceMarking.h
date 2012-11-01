//
//  PlaceMarking.h
//  Tippi
//
//  Created by Kristian Duske on 23.10.12.
//  Copyright (c) 2012 TU Berlin. All rights reserved.
//

#ifndef Tippi_PlaceMarking_h
#define Tippi_PlaceMarking_h

#include "Place.h"

#include "NetTypes.h"

#include <cassert>
#include <map>
#include <sstream>
#include <string>

namespace Tippi {
    class Place;
    
    class PlaceMarking {
    private:
        typedef unsigned int* Markings;
        typedef Place** Places;
        
        size_t m_size;
        Markings m_markings;
        Places m_places;
    public:
        PlaceMarking() :
        m_size(0),
        m_markings(NULL),
        m_places(NULL) {}
        
        PlaceMarking(const PlaceList& places) :
        m_size(places.size()),
        m_markings(new unsigned int[m_size]),
        m_places(new Place*[m_size]) {
            for (unsigned int i = 0; i < places.size(); i++) {
                size_t index = places[i]->index();
                m_markings[index] = 0;
                m_places[index] = places[i];
            }
        }
        
        PlaceMarking(const PlaceMarking& other) :
        m_size(other.size()),
        m_markings(new unsigned int[m_size]),
        m_places(new Place*[m_size]) {
            for (unsigned int i = 0; i < m_size; i++) {
                m_markings[i] = other.m_markings[i];
                m_places[i] = other.m_places[i];
            }
        }
        
        ~PlaceMarking() {
            if (m_markings != NULL) {
                delete [] m_markings;
                m_markings = NULL;
            }
            if (m_places != NULL) {
                delete [] m_places;
                m_places = NULL;
            }
        }
        
        bool operator== (const PlaceMarking& other) const {
            if (m_size != other.m_size)
                return false;
            for (unsigned int i = 0; i < m_size; i++) {
                if (m_places[i] != other.m_places[i])
                    return false;
                if (m_markings[i] != other.m_markings[i])
                    return false;
            }
            return true;
        }
        
        bool operator< (const PlaceMarking& other) const {
            assert(m_size == other.m_size);
            for (unsigned int i = 0; i < m_size; i++) {
                if (m_places[i] > other.m_places[i])
                    return false;
            }
            return true;
        }
        
        const PlaceMarking& operator= (const PlaceMarking& other) {
            if (m_size != other.size()) {
                if (m_markings != NULL)
                    delete [] m_markings;
                if (m_places != NULL)
                    delete [] m_places;
                m_size = other.size();
                m_markings = new unsigned int[m_size];
                m_places = new Place*[m_size];
            }
            
            for (unsigned int i = 0; i < m_size; i++) {
                m_markings[i] = other.m_markings[i];
                m_places[i] = other.m_places[i];
            }
            return *this;
        }
        
        inline size_t size() const {
            return m_size;
        }
        
        inline const unsigned int& operator[] (size_t index) const {
            assert(index < m_size);
            return m_markings[index];
        }
        
        inline const unsigned int& operator[] (const Place& place) const {
            size_t index = place.index();
            assert(index < m_size);
            assert(m_places[index] == &place);
            return m_markings[index];
        }
        
        inline unsigned int& operator[] (const Place& place) {
            size_t index = place.index();
            assert(index < m_size);
            assert(m_places[index] == &place);
            return m_markings[index];
        }
        
        inline bool violatesBound(const Place& place) const {
            return place.bound() != Place::Unbounded && (*this)[place] > place.bound();
        }
        
        inline bool violatesBound() const {
            for (unsigned int i = 0; i < m_size; i++)
                if (violatesBound(*m_places[i]))
                    return true;
            return false;
        }
        
        inline std::string asString() const {
            std::stringstream buffer;
            buffer << "[";
            for (unsigned int i = 0; i < m_size; i++) {
                const Place& place = *m_places[i];
                buffer << place.name() << ":" << (*this)[place];
                if (i < m_size - 1)
                    buffer << ",";
            }
            buffer << "]";
            
            return buffer.str();
        }
        
        inline std::string asShortString() const {
            std::stringstream buffer;
            for (unsigned int i = 0; i < m_size; i++) {
                const Place& place = *m_places[i];
                unsigned int marking = m_markings[i];
                for (unsigned int j = 0; j < marking; j++)
                    buffer << place.name();
            }
            
            return buffer.str();
        }
    };
}


#endif
