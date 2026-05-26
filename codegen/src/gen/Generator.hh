#pragma once
#include <iostream>
#include "../DecisionTree.hh"
#include "../Solutions.hh"

template<N_t N>
class Generator {
public:
    virtual ~Generator() = default;
    virtual void generate(std::ostream& out, const DTNode<N>& root, const Solutions<N>& solutions) = 0;
};
