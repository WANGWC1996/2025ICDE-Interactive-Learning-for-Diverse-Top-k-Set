#ifndef RUN_PIECEWISE_EXHAUSTION_H
#define RUN_PIECEWISE_EXHAUSTION_H
#include "hyperplane_set.h"
#include "u_vector.h"

int Piecewise_exhaustion(point_set *pset, point_set *realDataset, point_t *u, int k, point_set *realResult, std::vector<std::string> &category, std::vector<std::vector<double>> &bound,
                          server* s, const websocketpp::connection_hdl& hdl,
                          std::map<websocketpp::connection_hdl, client_data, std::owner_less<websocketpp::connection_hdl>> &client_info, std::ofstream &out_cp, bool EnglishChinese);

int Piecewise_exhaustion_diveristy_results(point_set *pset, point_set *realDataset, point_t *u, int k, point_set *realResult, std::vector<std::string> &category,
                                            server* s, const websocketpp::connection_hdl& hdl,
                                            std::map<websocketpp::connection_hdl, client_data, std::owner_less<websocketpp::connection_hdl>> &client_info, std::ofstream &out_cp, bool EnglishChinese);

int Piecewise_exhaustion_bound_diveristy(point_set *pset, point_set *realDataset, point_t *u, int k, point_set *realResult, std::vector<std::string> &category,
                                          server* s, const websocketpp::connection_hdl& hdl,
                                          std::map<websocketpp::connection_hdl, client_data, std::owner_less<websocketpp::connection_hdl>> &client_info, std::ofstream &out_cp, bool EnglishChinese);













#endif //RUN_PIECEWISE_EXHAUSTION_H
