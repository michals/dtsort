#pragma once
#include "../DecisionTree.hh"
#include "../Solutions.hh"
#include <iostream>

template <N_t N> class Generator {
public:
  virtual ~Generator() = default;
  virtual void generate(std::ostream &out, const DTNode<N> &root,
                        const Solutions<N> &solutions) = 0;
};
