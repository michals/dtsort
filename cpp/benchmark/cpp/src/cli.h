#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>

/**
 * @class CommandLine
 * @brief Minimal key-value command line parser for benchmark binaries.
 * Supports: --key=value, --key (as flag).
 */
class CommandLine {
public:
    CommandLine(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg.find("--") == 0) {
                size_t eq = arg.find('=');
                if (eq != std::string::npos) {
                    args_[arg.substr(2, eq - 2)] = arg.substr(eq + 1);
                } else {
                    args_[arg.substr(2)] = "true";
                }
            } else {
                positional_.push_back(arg);
            }
        }
    }

    bool has(const std::string& key) const {
        return args_.count(key) > 0;
    }

    std::string get(const std::string& key, const std::string& default_val = "") const {
        auto it = args_.find(key);
        if (it != args_.end()) return it->second;
        return default_val;
    }

    std::vector<std::string> get_list(const std::string& key) const {
        std::vector<std::string> result;
        std::string val = get(key);
        if (val.empty()) return result;

        std::stringstream ss(val);
        std::string item;
        while (std::getline(ss, item, ',')) {
            if (!item.empty()) result.push_back(item);
        }
        return result;
    }

    const std::vector<std::string>& positional() const {
        return positional_;
    }

private:
    std::map<std::string, std::string> args_;
    std::vector<std::string> positional_;
};
