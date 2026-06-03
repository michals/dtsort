#pragma once
#include "../DecisionTree.hh"
#include "Generator.hh"
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>

template <N_t N> class DTGenerator : public Generator<N> {
public:
  // Generator interface implementation
  void generate(std::ostream &out, const DTNode<N> &root,
                const Solutions<N> &solutions) override {
    out << "# dtsort decision tree for N=" << (int)N << "\n";
    out << "# Format: NODE ID ROW COL TRUE_ID FALSE_ID SOLUTIONS VARIATIONS\n";
    out << "# Format: LEAF ID PERM_IDX PERM CYCLE VARIATIONS\n";

    size_t nextId = 0;
    emitNode(out, root, nextId, solutions);
  }

  // Static helper for loading (Parser logic)
  static std::unique_ptr<DTNode<N>> load(const std::string &path) {
    std::ifstream in(path);
    if (!in.is_open()) {
      std::cerr << "Error: Could not open " << path << " for reading.\n";
      return nullptr;
    }

    std::string line;

    // Let's parse all into a vector or map first.
    // The save format I proposed: NODE ID ROW COL TRUE_ID FALSE_ID ...
    // We can load all nodes into a `map<int, DTNode<N>*>` then link them.

    // Reset file stream just in case
    in.clear();
    in.seekg(0);

    std::map<int, std::unique_ptr<DTNode<N>>> nodesById;
    std::map<int, std::pair<int, int>> links; // id -> {trueId, falseId}

    while (std::getline(in, line)) {
      if (line.empty() || line[0] == '#')
        continue;

      // Find [ and ] to parse/skip matching solutions
      size_t bracketOpen = line.find('[');
      size_t bracketClose = line.find(']');

      std::string mainPart = (bracketOpen != std::string::npos)
                                 ? line.substr(0, bracketOpen)
                                 : line;
      std::string varPart = (bracketClose != std::string::npos)
                                ? line.substr(bracketClose + 1)
                                : "";

      std::stringstream ss(mainPart);
      std::string type;
      int id;
      ss >> type >> id;

      auto node = std::unique_ptr<DTNode<N>>(new DTNode<N>());

      if (type == "NODE") {
        int row, col, trueId, falseId;
        ss >> row >> col >> trueId >> falseId;
        node->row = row;
        node->col = col;
        links[id] = {trueId, falseId};
      } else if (type == "LEAF") {
        int permIdx;
        ss >> permIdx; // PermID
        // Skip PERM string and CYCLE string
        std::string permStr, cycleStr;
        ss >> permStr;
        ss >> cycleStr;

        node->matchingSolutions.push_back(permIdx);
      }

      // Parse variation count if available
      if (!varPart.empty()) {
        std::stringstream ssVar(varPart);
        ssVar >> node->variationCount;
      } else {
        // Try reading from ss if no brackets (Leaf)
        ss >> node->variationCount;
      }

      nodesById[id] = std::move(node);
    }

    // Linking phase
    if (nodesById.find(0) == nodesById.end())
      return nullptr;

    return reconstruct(0, nodesById, links);
  }

private:
  static std::unique_ptr<DTNode<N>>
  reconstruct(int id, std::map<int, std::unique_ptr<DTNode<N>>> &nodes,
              const std::map<int, std::pair<int, int>> &links) {
    auto it = nodes.find(id);
    if (it == nodes.end())
      return nullptr;

    std::unique_ptr<DTNode<N>> node = std::move(it->second);
    nodes.erase(it); // Remove from map as we moved ownership

    if (links.count(id)) {
      auto children = links.at(id);
      node->trueChild = reconstruct(children.first, nodes, links);
      node->falseChild = reconstruct(children.second, nodes, links);
    }

    return node;
  }

  static void emitNode(std::ostream &out, const DTNode<N> &node, size_t &nextId,
                       const Solutions<N> &solutions) {
    size_t myId = nextId++;

    if (node.isLeaf()) {
      size_t permIdx = node.matchingSolutions[0];
      const auto &mat = solutions.all[permIdx];
      out << "LEAF " << myId << " " << permIdx << " " << mat.perm << " "
          << mat.perm.decomposedPermutation() << " " << node.variationCount
          << "\n";
    } else {
      size_t trueId = myId + 1;
      size_t leftSize = node.trueChild ? node.trueChild->countNodes() : 0;
      size_t falseId = trueId + leftSize;

      out << "NODE " << myId << " " << (int)node.row << " " << (int)node.col
          << " " << trueId << " " << falseId << " [";

      for (size_t i = 0; i < std::min((size_t)3, node.matchingSolutions.size());
           ++i) {
        out << node.matchingSolutions[i]
            << (i + 1 < node.matchingSolutions.size() ? "," : "");
      }
      if (node.matchingSolutions.size() > 3)
        out << "...";
      out << "] " << node.variationCount << "\n";

      if (node.trueChild)
        emitNode(out, *node.trueChild, nextId, solutions);
      if (node.falseChild)
        emitNode(out, *node.falseChild, nextId, solutions);
    }
  }
};
