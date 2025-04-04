#ifndef MAXUTILITY_H
#define MAXUTILITY_H

#include "structure/define.h"
#include "structure/data_struct.h"
#include "structure/data_utility.h"

#include <vector>
#include <algorithm>
#include "structure/rtree.h"
#include "Others/lp.h"
#include "Others/pruning.h"
#include "Others/operation.h"
#include <queue>
#define RANDOM 1
#define SIMPLEX 2

using namespace std;

int UHRandom(point_set *originalSet, point_t *u, int k, std::vector<std::string> &category, std::vector<std::vector<double>> &bound);

int max_utility(point_set *pSet, point_t *u, int k, point_set *realResult, std::vector<std::string> &category,
                std::vector<std::vector<double>> &bound, int s, int cmp_option, bool error);

int UHSimplex(point_set *originalSet, point_t *u, int k, int s);

#endif