#ifndef ANALYZE_HPP
#define ANALYZE_HPP

#include <functional>
#include "predicate.hpp"
#include "world.hpp"

void analyze(const world &w, int x, int y, int z, vector<int> &result);

const int wide_len = 25;
const int wide_dx[] = {-2, -1, -1, -1, -1, -1,  0,  0,  0,  0,  0,  0, 0, 0,  0, 0, 0, 0,  1,  1, 1, 1, 1, 2};
const int wide_dy[] = { 0, -1,  0,  0,  0,  1, -2, -1, -1, -1,  0,  0, 0, 0,  1, 1, 1, 2, -1,  0, 0, 0, 1, 0};
const int wide_dz[] = { 0,  0, -1,  0,  1,  0,  0, -1,  0,  1, -2, -1, 1, 2, -1, 0, 1, 0,  0, -1, 0, 1, 0, 0};

const int narrow_len = 6;
const int narrow_dx[] = {-1,  0,  0, 0, 0, 1};
const int narrow_dy[] = { 0, -1,  0, 0, 1, 0};
const int narrow_dz[] = { 0,  0, -1, 1, 0, 0};

#endif
