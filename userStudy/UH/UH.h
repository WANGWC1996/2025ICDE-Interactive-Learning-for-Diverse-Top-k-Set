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

#include <thread>
#include <map>
#include "libs/boost_1_82_0/boost/asio.hpp"
#include "libs/websocketpp/config/asio_no_tls.hpp"
#include "libs/websocketpp/server.hpp"
#include <mutex>
#include "structure/data_struct.h"
typedef websocketpp::server<websocketpp::config::asio> server;


using namespace std;

int UHRandom(point_set *originalSet, point_t *u, int k, std::vector<std::string> &category, std::vector<std::vector<double>> &bound);

int max_utility(point_set *pSet, point_set *realDataset, point_t *u, int k, point_set *realResult, std::vector<std::string> &category,
                std::vector<std::vector<double>> &bound, int s, int cmp_option, bool error, server* ss,
                const websocketpp::connection_hdl& hdl, std::map<websocketpp::connection_hdl, client_data,
                std::owner_less<websocketpp::connection_hdl>> &client_info, std::ofstream &out_cp, bool EnglishChinese);

int UHSimplex(point_set *originalSet, point_t *u, int k, int s);

#endif