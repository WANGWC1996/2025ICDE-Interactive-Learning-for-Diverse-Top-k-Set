#ifndef RUN_RH_H
#define RUN_RH_H
#include "hyperplane_set.h"
#include "Partition.h"
#include "choose_item.h"
#include <thread>
#include <map>
#include "libs/boost_1_82_0/boost/asio.hpp"
#include "libs/websocketpp/config/asio_no_tls.hpp"
#include "libs/websocketpp/server.hpp"
#include <mutex>
#include "structure/data_struct.h"
typedef websocketpp::server<websocketpp::config::asio> server;

int RH(point_set* pset, point_set *realDataset, point_t* u, int k, point_set *realResult, std::vector<std::string> &category,
       std::vector<std::vector<double>> &bound, bool error, server* s, const websocketpp::connection_hdl& hdl,
       std::map<websocketpp::connection_hdl, client_data, std::owner_less<websocketpp::connection_hdl>> &client_info,
       std::ofstream &out_cp, bool EnglishChinese);


#endif //RUN_RH_H
