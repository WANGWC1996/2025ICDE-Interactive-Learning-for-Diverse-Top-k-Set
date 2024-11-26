#ifndef POINT_T_H
#define POINT_T_H
#include "define.h"
#include "structure/data_struct.h"
#include <thread>
#include <map>
#include "libs/boost_1_82_0/boost/asio.hpp"
#include "libs/websocketpp/config/asio_no_tls.hpp"
#include "libs/websocketpp/server.hpp"
#include <mutex>
typedef websocketpp::server<websocketpp::config::asio> server;

class point_t
{
public:
    int	id, index, dim, place, clusterID;
    double *attr, value;
    std::string cateAttr;
    std::vector<point_t*> surpass;
    std::vector<double> surpassVector;
    int topk, result;
    std::vector<int> notRdominateID;
    int count, currentScanID;


    explicit point_t(int dim);
    explicit point_t(int dim, int id);
    explicit point_t(point_t *p);
    point_t(point_t *p1, point_t *p2);
    ~point_t();
    void print(); //print the point

    bool is_same(point_t *p);//check whether the point is the same as p
    bool is_zero();
    bool is_positive();
    double dot_product(point_t* p);
    double dot_product(double *v);
    point_t* sub(point_t* p);
    point_t* add(point_t* p);
    point_t* scale(double c);
    double cal_len();
    double distance(point_t* p);
    double distance(double *p);
    bool dominate(point_t *p);
    double bound(point_t* p, double y);
    bool is_changed(point_t *p);
    int countPassed(double ut);
    double tranBound(point_t *p);
    void printResult(char *name, int Qcount, timeval t1);
    void printResult(char *name, int Qcount, timeval t1, double preTime, long mem_baseline);
    int interaction_compare(point_t *p1, point_t *p2, server* s, websocketpp::connection_hdl hdl,
                            std::map<websocketpp::connection_hdl, client_data, std::owner_less<websocketpp::connection_hdl>> &client_info,
                            std::ofstream &out_cp, bool EnglishChinese);
};










#endif //U_2_POINT_T_H
