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

#include "SimpleAutomatonParser.h"
#include "SimpleAutomaton.h"
#include "StringUtils.h"

#include <gtest/gtest.h>

namespace Tippi {
    TEST(SimpleAutomatonParserTest, testEmpty) {
        ASSERT_TRUE(SimpleAutomatonParser("").parse() == NULL);
        
        const SimpleAutomaton* automaton = SimpleAutomatonParser("AUTOMATON").parse();
        ASSERT_TRUE(automaton != NULL);
        ASSERT_TRUE(automaton->getStates().empty());
        ASSERT_TRUE(automaton->getEdges().empty());
        ASSERT_TRUE(automaton->getInitialState() == NULL);
        ASSERT_TRUE(automaton->getFinalStates().empty());
        
        automaton = SimpleAutomatonParser(" \n  \n AUTOMATON     \n\n").parse();
        ASSERT_TRUE(automaton != NULL);
        ASSERT_TRUE(automaton->getStates().empty());
        ASSERT_TRUE(automaton->getEdges().empty());
        ASSERT_TRUE(automaton->getInitialState() == NULL);
        ASSERT_TRUE(automaton->getFinalStates().empty());
    }
    
    TEST(SimpleAutomatonParserTest, testStates) {
        ASSERT_THROW(SimpleAutomatonParser("AUTOMATON\n"
                                           "STATES\n").parse(), ParserException);
        const SimpleAutomaton* automaton = SimpleAutomatonParser("AUTOMATON\n"
                                                                 "STATES;\n").parse();
        ASSERT_TRUE(automaton != NULL);
        ASSERT_TRUE(automaton->getStates().empty());
        ASSERT_TRUE(automaton->getEdges().empty());
        ASSERT_TRUE(automaton->getInitialState() == NULL);
        ASSERT_TRUE(automaton->getFinalStates().empty());
        
        automaton = SimpleAutomatonParser("AUTOMATON\n"
                                          "STATES 1,2,3,asdf,_bleh;\n").parse();
        ASSERT_TRUE(automaton != NULL);
        ASSERT_EQ(5u, automaton->getStates().size());
        
        const SimpleAutomatonState* state1 = automaton->findState("1");
        const SimpleAutomatonState* state2 = automaton->findState("2");
        const SimpleAutomatonState* state3 = automaton->findState("3");
        const SimpleAutomatonState* state4 = automaton->findState("asdf");
        const SimpleAutomatonState* state5 = automaton->findState("_bleh");
        
        ASSERT_TRUE(state1 != NULL);
        ASSERT_EQ("1", state1->getName());
        ASSERT_FALSE(state1->isFinal());
        ASSERT_TRUE(state2 != NULL);
        ASSERT_EQ("2", state2->getName());
        ASSERT_FALSE(state2->isFinal());
        ASSERT_TRUE(state3 != NULL);
        ASSERT_EQ("3", state3->getName());
        ASSERT_FALSE(state3->isFinal());
        ASSERT_TRUE(state4 != NULL);
        ASSERT_EQ("asdf", state4->getName());
        ASSERT_FALSE(state4->isFinal());
        ASSERT_TRUE(state5 != NULL);
        ASSERT_EQ("_bleh", state5->getName());
        ASSERT_FALSE(state5->isFinal());
        
        ASSERT_TRUE(automaton->getEdges().empty());
        ASSERT_TRUE(automaton->getInitialState() == NULL);
        ASSERT_TRUE(automaton->getFinalStates().empty());
    }
    
    TEST(SimpleAutomatonParserTest, testAutomaton) {
        const SimpleAutomaton* automaton = SimpleAutomatonParser("AUTOMATON\n"
                                                                 "STATES 1,2,3,4;\n"
                                                                 "TRANSITION 1; FROM 1; TO 1;\n"
                                                                 "TRANSITION a; FROM 1; TO 2;\n"
                                                                 "TRANSITION b; FROM 1; TO 3;\n"
                                                                 "TRANSITION b; FROM 1; TO 4;\n"
                                                                 "TRANSITION c; FROM 3; TO 1;\n"
                                                                 "INITIALSTATE 1;\n"
                                                                 "FINALSTATES 2,3;\n").parse();
        ASSERT_TRUE(automaton != NULL);
        ASSERT_EQ(4u, automaton->getStates().size());
        
        SimpleAutomatonState* state1 = automaton->findState("1");
        SimpleAutomatonState* state2 = automaton->findState("2");
        SimpleAutomatonState* state3 = automaton->findState("3");
        SimpleAutomatonState* state4 = automaton->findState("4");

        ASSERT_TRUE(state1 != NULL);
        ASSERT_TRUE(state2 != NULL);
        ASSERT_TRUE(state3 != NULL);
        ASSERT_TRUE(state4 != NULL);
        
        ASSERT_EQ(5u, automaton->getEdges().size());
        
        ASSERT_EQ(4u, state1->getOutgoing().size());
        ASSERT_EQ(1u, state1->getDirectSuccessors("1").size());
        ASSERT_EQ(state1, state1->findDirectSuccessor("1"));
        ASSERT_EQ(1u, state1->getDirectSuccessors("a").size());
        ASSERT_EQ(state2, state1->findDirectSuccessor("a"));
        ASSERT_EQ(2u, state1->getDirectSuccessors("b").size());
        ASSERT_TRUE(VectorUtils::contains(state1->getDirectSuccessors("b"), state3));
        ASSERT_TRUE(VectorUtils::contains(state1->getDirectSuccessors("b"), state4));
        ASSERT_EQ(2u, state1->getIncoming().size());
        
        ASSERT_TRUE(state2->getOutgoing().empty());
        ASSERT_EQ(1u, state2->getIncoming().size());
        
        ASSERT_EQ(1u, state3->getOutgoing().size());
        ASSERT_EQ(1u, state3->getDirectSuccessors("c").size());
        ASSERT_EQ(state1, state3->findDirectSuccessor("c"));
        ASSERT_EQ(1u, state3->getIncoming().size());
        
        ASSERT_TRUE(state4->getOutgoing().empty());
        ASSERT_EQ(1u, state4->getIncoming().size());
        
        ASSERT_TRUE(automaton->getInitialState() == state1);
        ASSERT_EQ(2u, automaton->getFinalStates().size());
        ASSERT_TRUE(automaton->getFinalStates().count(state2) > 0);
        ASSERT_TRUE(automaton->getFinalStates().count(state3) > 0);
    }
}
