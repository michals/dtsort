// Copyright (C) 2018 Michał Šrajer <michal@post.pl>
// This software is licensed under (one of your choice):
// - MIT
// - Apache-2.0
// - LGPL-2.1
// See LICENSE-*.txt for details.

#include <iostream>
#include <vector>
#include <ctime>
#include "debug.h"
#include "stl_algo.h"

#define Out(x) std::cout << x

#define Outn(x) std::cout << x << std::endl

struct {
    size_t swap=0;
    size_t cmp=0;
    size_t defaultctor=0;
    size_t argctor=0;
    size_t copyctor=0;
    size_t assign=0;
    size_t move=0;
    void clear() {
        swap = cmp = defaultctor = argctor = copyctor = assign = move = 0;
    }
} cnt;


class Element {
public:
    Element(long f, long s): first(f), second(s) {
        cnt.argctor++;
    }
    Element(const Element& rhs) {
        cnt.copyctor++;
        first = rhs.first;
        second = rhs.second;
    }
    Element(Element&& rhs) {
        cnt.move++;
        first = rhs.first;
        second = rhs.second;
    }
    Element& operator=(const Element& rhs) {
        cnt.copyctor++;
        if (this==&rhs) return *this;
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
    Element& operator=(const Element&& rhs) {
        cnt.move++;
        if (this==&rhs) return *this;
        first = rhs.first;
        second = rhs.second;
        return *this;
    }
    bool operator==(const Element& other) const {
        cnt.cmp++;
        return first==other.first;
    }
    bool operator<(const Element& other) const {
        cnt.cmp++;
        return first < other.first;
    }
    long first;
    long second;
};


template <typename T>
void print(const std::vector<T>& vec) {
    for(auto& i: vec) {
        Debug(i.first << ':' << i.second << ' ');
    }
    Debug("\n");
}

size_t std_clock = 0;
size_t dts_clock = 0;

bool chack_stability(std::vector<Element> marked) {
    size_t i, j;
    bool out = true;
    for(i=0; i<marked.size()-1; ++i) {
        for(j=i+1; j<marked.size(); ++j) {
            Element &mi = marked[i];
            Element &mj = marked[j];
//            std::cout << "i=" << i << " j=" << j << "\n";
            if ((mi.first == mj.first) && !(mi.second < mj.second)) {
                out = false;
                break;
            }
        }
    }
    return out;
}

bool check(size_t n) {
    std::vector<Element> vec;
    for(long i=0; i<n; ++i) {
        Element el = Element(rand() % n, i);
        vec.push_back(el); 
    }
        
    size_t comp_std = 0;
    size_t comp_dts = 0;
    size_t move_std = 0;
    size_t move_dts = 0;
    size_t copy_std = 0;
    size_t copy_dts = 0;
    clock_t begin;
    
    std::vector<Element> copy = vec;

    Debugn("stable_sort():");
    Debug("BEFORE: "); print(vec);
    cnt.clear(); begin = clock();
    std::stable_sort(vec.begin(), vec.end());
//            [&comp_std](auto a, auto b){++comp_std;return a.first < b.first;});
    std_clock += clock() - begin;
    comp_std = cnt.cmp;
    move_std = cnt.move;
    copy_std = cnt.copyctor;
    Debug("AFTER:  "); print(vec);
    Debug((chack_stability(vec)? "STABLE ": "!!!!NON-STABLE!!!! "));

    Debugn("dt_stable_sort():");
    Debug("BEFORE: "); print(copy);
    cnt.clear();
    begin = clock();
    std::dt_stable_sort(copy.begin(), copy.end());
//            [&comp_dts](auto a, auto b){++comp_dts;return a.first < b.first;});
    dts_clock += clock() - begin;
    comp_dts = cnt.cmp;
    move_dts = cnt.move;
    copy_dts = cnt.copyctor;
    Debug("AFTER:  "); print(copy);
    Outn((chack_stability(copy)? "STABLE ": "!!!!NON-STABLE!!!! "));
    

    Debug("validate st_stable_sort():");
    for(auto i=0; i<n; ++i) {
        if (!(vec[i].first == copy[i].first && vec[i].second == copy[i].second)) {
            Debug(" !ERR! ");
            return false;
        } else Debug(". ");
    }
    
    
    Outn("");
    std::cout.precision(3);
    Outn("   stable_sort compare: " << comp_std);
    Outn("dt_stable_sort compare: " << comp_dts);
    Outn("   stable_sort move: " << move_std);
    Outn("dt_stable_sort move: " << move_dts);
    Outn("   stable_sort copy: " << copy_std);
    Outn("dt_stable_sort copy: " << copy_dts);
    Outn("   stable_sort clock: " << std_clock);
    Outn("dt_stable_sort clock: " << dts_clock);
    Outn("compare ratio: "
            << ((comp_dts*1.0f)/comp_std)
            << "     \t move ratio: "
            << ((move_dts*1.0f)/move_std)
            << "     \t copy ratio: "
            << copy_dts << '/' << copy_std);

    return true;
}


int main(int argc, char**argv) {
    size_t len = 24;
    size_t times = 100;
    if (argc >= 2) {
        len = std::atol(argv[1]);        
    }
    if (argc >= 3) {
        times = std::atol(argv[2]);        
    }
    srand(time(0));
      
    for(size_t i=0; i<times; ++i) {
        check(len);
    }
    Outn("len="<<len << " times="<<times);
    Outn("clock RATIO: " << ((dts_clock * 1.0f)/std_clock));
    
    return 0;
}
