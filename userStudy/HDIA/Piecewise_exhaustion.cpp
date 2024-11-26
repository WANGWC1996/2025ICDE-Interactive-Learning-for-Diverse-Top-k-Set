#include "Piecewise_exhaustion.h"
#include "Partition.h"
#include "halfspace_tree.h"
#include "interaction_tree.h"
#define eps 0.2
#include <thread>
#include <map>
#include "libs/boost_1_82_0/boost/asio.hpp"
#include "libs/websocketpp/config/asio_no_tls.hpp"
#include "libs/websocketpp/server.hpp"
#include <mutex>
#include "structure/data_struct.h"
typedef websocketpp::server<websocketpp::config::asio> server;


/*

void find_shortest_tree(hyperplane_set* crt, hyperplane_set *rem, std::vector<point_t*> partitionPt, int &curMinHeight)
{
    int M = rem->hyperplanes.size();
    if(M == 0)
        return;
    else
    {
        for (int i = 0; i < M; ++i)
        {
            hyperplane *h = rem->hyperplanes[i];
            crt->hyperplanes.push_back(h);
            rem->hyperplanes.erase(rem->hyperplanes.begin() + i);

            find_shortest_tree(crt, rem, partitionPt, curMinHeight);

            crt->hyperplanes.pop_back();
            rem->hyperplanes.insert(rem->hyperplanes.begin() + i, h);
        }
    }
}
*/

int Piecewise_exhaustion(point_set *pset, point_set *realDataset, point_t *u, int k, point_set *realResult, std::vector<std::string> &category,
                          std::vector<std::vector<double>> &bound, server* s, const websocketpp::connection_hdl& hdl,
                          std::map<websocketpp::connection_hdl, client_data, std::owner_less<websocketpp::connection_hdl>> &client_info,
                         std::ofstream &out_cp, bool EnglishChinese)
{
    timeval t1;
    gettimeofday(&t1, 0);

    int dim = pset->points[0]->dim;
    int piece_size = 30;
    Partition *R = new Partition(dim);
    point_set *resultSet = new point_set();
    int numOfQuestion = 0;

    while(1)
    {
        //randomly build some hyper-planes based on the points without overlapping
        hyperplane_set *hset = new hyperplane_set();
        int count = 0;
        while (hset->hyperplanes.size() < piece_size && count < pset->points.size() * 10)
        {
            count++;
            int pindex1 = rand() % pset->points.size();
            int pindex2 = rand() % pset->points.size();
            while (pindex1 == pindex2)
                pindex2 = rand() % pset->points.size();
            hyperplane *h = new hyperplane(pset->points[pindex1], pset->points[pindex2]);

            if (R->check_relation(h) == 0 && !hset->is_hyperplane_exist(h))
                hset->hyperplanes.push_back(h);
        }

        //obtain the partitions divided by the hyper-planes
        halfspace_tree *hTree = new halfspace_tree(R);
        for(int i = 0; i < hset->hyperplanes.size(); ++i)
        {
            std::cout << i << "\n";
            hTree->insert(hset->hyperplanes[i]);
            //hTree->print();
        }
        //hTree->print_leaves();

        //find the shortest tree based on the partitions
        std::vector<point_t*> partitionPt;
        int currentMinRound = hTree->find_leafPt_height(partitionPt);

        if(partitionPt.size() <= 1)
        {
            point_set* rankingSet1 = new point_set(), *resultSet1 = new point_set();
            pset->findRanking(R->ext_pts[0], rankingSet1);
            rankingSet1->fairTopk(k, category, bound,resultSet1);
            resultSet1->resultEvaluation(realDataset, "HDIA", numOfQuestion, s, hdl,
                                         client_info, out_cp, EnglishChinese);
            return numOfQuestion;
        }

        while(partitionPt.size() > 1)
        {
            int partitionLeft = partitionPt.size() / 6;
            if(partitionLeft < 1)
                partitionLeft = 1;
            it_tree *itTree = new it_tree(hset, partitionPt, partitionLeft);
            //itTree->print();

            //interaction based on the tree
            it_node *curNode = itTree->root;
            if (curNode == NULL)
                break;
            if(curNode->bestpt == NULL || curNode->bestng == NULL)
                break;
            while (curNode->bestpt != NULL && curNode->bestng != NULL) //if it is an internal node
            {
                numOfQuestion++;
                double v1 = curNode->bestdivideHyper->p_1->dot_product(u);
                double v2 = curNode->bestdivideHyper->p_2->dot_product(u);
                point_t *p1 = realDataset->points[curNode->bestdivideHyper->p_1->id];
                point_t *p2 = realDataset->points[curNode->bestdivideHyper->p_2->id];
                int compareResult = p1->interaction_compare(p1, p2, s, hdl, client_info, out_cp, EnglishChinese);
                if (compareResult == 888)
                    return 888;
                if(compareResult == 1)
                {
                    hyperplane *h = new hyperplane(curNode->bestdivideHyper->p_2, curNode->bestdivideHyper->p_1);
                    R->hyperplanes.push_back(h);
                    curNode = curNode->bestpt;
                }
                else
                {
                    hyperplane *h = new hyperplane(curNode->bestdivideHyper->p_1, curNode->bestdivideHyper->p_2);
                    R->hyperplanes.push_back(h);
                    curNode = curNode->bestng;
                }
                R->set_ext_pts();
                R->center = R->average_point();

                //stopping condition
                point_set *rankingSet1 = new point_set(), *resultSet1 = new point_set();
                pset->findRanking(R->ext_pts[0], rankingSet1);
                rankingSet1->fairTopk(k, category, bound, resultSet1);
                bool flag = true;
                for (int i = 1; i < R->ext_pts.size(); ++i)
                {
                    point_set *rankingSet2 = new point_set(), *resultSet2 = new point_set();
                    pset->findRanking(R->ext_pts[i], rankingSet2);
                    rankingSet2->fairTopk(k, category, bound, resultSet2);
                    if (!resultSet1->isSame(resultSet2))
                    {
                        flag = false;
                        break;
                    }
                }
                if (flag)
                {
                    resultSet = resultSet1;
                    resultSet->resultEvaluation(realDataset, "HDIA", numOfQuestion, s, hdl,
                                                client_info, out_cp, EnglishChinese);
                    return numOfQuestion;
                }
            }

            partitionPt = curNode->ptSet;
        }
    }

}


int Piecewise_exhaustion_diveristy_results(point_set *pset, point_set *realDataset, point_t *u, int k, point_set *realResult,
                                            std::vector<std::string> &category, server* s, const websocketpp::connection_hdl& hdl,
                                            std::map<websocketpp::connection_hdl, client_data,
                                            std::owner_less<websocketpp::connection_hdl>> &client_info, std::ofstream &out_cp, bool EnglishChinese)
{
    timeval t1;
    gettimeofday(&t1, 0);

    int dim = pset->points[0]->dim;
    int piece_size = 40;
    Partition *R = new Partition(dim);
    point_set *resultSet = new point_set();
    int numOfQuestion = 0;

    while(1)
    {
        //randomly build some hyper-planes based on the points without overlapping
        hyperplane_set *hset = new hyperplane_set();
        int count = 0;
        while (hset->hyperplanes.size() < piece_size && count < pset->points.size() * 10)
        {
            count++;
            int pindex1 = rand() % pset->points.size();
            int pindex2 = rand() % pset->points.size();
            while (pindex1 == pindex2)
                pindex2 = rand() % pset->points.size();
            hyperplane *h = new hyperplane(pset->points[pindex1], pset->points[pindex2]);

            if (R->check_relation(h) == 0 && !hset->is_hyperplane_exist(h))
                hset->hyperplanes.push_back(h);
        }

        //obtain the partitions divided by the hyper-planes
        halfspace_tree *hTree = new halfspace_tree(R);
        for(int i = 0; i < hset->hyperplanes.size(); ++i)
        {
            std::cout << i << "\n";
            hTree->insert(hset->hyperplanes[i]);
            //hTree->print();
        }
        //hTree->print_leaves();

        //find the shortest tree based on the partitions
        std::vector<point_t*> partitionPt;
        int currentMinRound = hTree->find_leafPt_height(partitionPt);

        if(partitionPt.size() <= 1)
        {
            point_set* rankingSet1 = new point_set(), *resultSet1 = new point_set();
            pset->findRanking(R->ext_pts[0], rankingSet1);
            rankingSet1->fairTopk_diverse_results(k, category, resultSet1);
            resultSet1->resultEvaluation(realDataset, "HDIA(Diverse)", numOfQuestion, s, hdl,
                                         client_info, out_cp, EnglishChinese);
            return numOfQuestion;
        }


        while(partitionPt.size() > 1)
        {
            int partitionLeft = partitionPt.size() / 10;
            if(partitionLeft < 1)
                partitionLeft = 1;
            it_tree *itTree = new it_tree(hset, partitionPt, partitionLeft);
            //itTree->print();

            //interaction based on the tree
            it_node *curNode = itTree->root;
            if(curNode->bestpt == NULL || curNode->bestng == NULL)
                break;
            while (curNode->bestpt != NULL && curNode->bestng != NULL) //if it is an internal node
            {
                numOfQuestion++;
                double v1 = curNode->bestdivideHyper->p_1->dot_product(u);
                double v2 = curNode->bestdivideHyper->p_2->dot_product(u);
                point_t *p1 = realDataset->points[curNode->bestdivideHyper->p_1->id];
                point_t *p2 = realDataset->points[curNode->bestdivideHyper->p_2->id];
                int compareResult = p1->interaction_compare(p1, p2, s, hdl, client_info, out_cp, EnglishChinese);
                if (compareResult == 888)
                    return 888;
                if(compareResult == 1)
                {
                    hyperplane *h = new hyperplane(curNode->bestdivideHyper->p_2, curNode->bestdivideHyper->p_1);
                    R->hyperplanes.push_back(h);
                    curNode = curNode->bestpt;
                }
                else
                {
                    hyperplane *h = new hyperplane(curNode->bestdivideHyper->p_1, curNode->bestdivideHyper->p_2);
                    R->hyperplanes.push_back(h);
                    curNode = curNode->bestng;
                }
                R->set_ext_pts();
                R->center = R->average_point();

                //stopping condition
                point_set *rankingSet1 = new point_set(), *resultSet1 = new point_set();
                pset->findRanking(R->ext_pts[0], rankingSet1);
                rankingSet1->fairTopk_diverse_results(k, category, resultSet1);
                bool flag = true;
                for (int i = 1; i < R->ext_pts.size(); ++i)
                {
                    point_set *rankingSet2 = new point_set(), *resultSet2 = new point_set();
                    pset->findRanking(R->ext_pts[i], rankingSet2);
                    rankingSet2->fairTopk_diverse_results(k, category, resultSet2);
                    if (!resultSet1->isSame(resultSet2))
                    {
                        flag = false;
                        break;
                    }
                }
                if (flag)
                {
                    resultSet = resultSet1;
                    resultSet->resultEvaluation(realDataset, "HDIA(Diverse)", numOfQuestion, s, hdl,
                                                client_info, out_cp, EnglishChinese);
                    return numOfQuestion;
                }
            }

            partitionPt = curNode->ptSet;
        }
    }

}


int Piecewise_exhaustion_bound_diveristy(point_set *pset, point_set *realDataset, point_t *u, int k, point_set *realResult,
                                          std::vector<std::string> &category, server* s, const websocketpp::connection_hdl& hdl,
                                          std::map<websocketpp::connection_hdl,
                                          client_data, std::owner_less<websocketpp::connection_hdl>> &client_info,
                                          std::ofstream &out_cp, bool EnglishChinese)
{
    timeval t1;
    gettimeofday(&t1, 0);

    int dim = pset->points[0]->dim;
    int piece_size = 40;
    Partition *R = new Partition(dim);
    point_set *resultSet = new point_set();
    int numOfQuestion = 0;

    while(1)
    {
        //randomly build some hyper-planes based on the points without overlapping
        hyperplane_set *hset = new hyperplane_set();
        int count = 0;
        while (hset->hyperplanes.size() < piece_size && count < pset->points.size() * 10)
        {
            count++;
            int pindex1 = rand() % pset->points.size();
            int pindex2 = rand() % pset->points.size();
            while (pindex1 == pindex2)
                pindex2 = rand() % pset->points.size();
            hyperplane *h = new hyperplane(pset->points[pindex1], pset->points[pindex2]);

            if (R->check_relation(h) == 0 && !hset->is_hyperplane_exist(h))
                hset->hyperplanes.push_back(h);
        }

        //obtain the partitions divided by the hyper-planes
        halfspace_tree *hTree = new halfspace_tree(R);
        for(int i = 0; i < hset->hyperplanes.size(); ++i)
        {
            std::cout << i << "\n";
            hTree->insert(hset->hyperplanes[i]);
            //hTree->print();
        }
        //hTree->print_leaves();

        //find the shortest tree based on the partitions
        std::vector<point_t*> partitionPt;
        int currentMinRound = hTree->find_leafPt_height(partitionPt);

        if(partitionPt.size() <= 1)
        {
            point_set* rankingSet1 = new point_set(), *resultSet1 = new point_set();
            pset->findRanking(R->ext_pts[0], rankingSet1);
            rankingSet1->fairTopk_bound_diverse(k, category, R->ext_pts[0], resultSet1);
            resultSet1->resultEvaluation(realDataset, "HDIA(bound)", numOfQuestion, s, hdl,
                                         client_info, out_cp, EnglishChinese);
            return numOfQuestion;
        }


        while(partitionPt.size() > 1)
        {
            int partitionLeft = partitionPt.size() / 10;
            if(partitionLeft < 1)
                partitionLeft = 1;
            it_tree *itTree = new it_tree(hset, partitionPt, partitionLeft);
            //itTree->print();

            //interaction based on the tree
            it_node *curNode = itTree->root;
            if(curNode->bestpt == NULL || curNode->bestng == NULL)
                break;
            while (curNode->bestpt != NULL && curNode->bestng != NULL) //if it is an internal node
            {
                numOfQuestion++;
                double v1 = curNode->bestdivideHyper->p_1->dot_product(u);
                double v2 = curNode->bestdivideHyper->p_2->dot_product(u);
                point_t *p1 = realDataset->points[curNode->bestdivideHyper->p_1->id];
                point_t *p2 = realDataset->points[curNode->bestdivideHyper->p_2->id];
                int compareResult = p1->interaction_compare(p1, p2, s, hdl, client_info, out_cp, EnglishChinese);
                if (compareResult == 888)
                    return 888;
                if(compareResult == 1)
                {
                    hyperplane *h = new hyperplane(curNode->bestdivideHyper->p_2, curNode->bestdivideHyper->p_1);
                    R->hyperplanes.push_back(h);
                    curNode = curNode->bestpt;
                }
                else
                {
                    hyperplane *h = new hyperplane(curNode->bestdivideHyper->p_1, curNode->bestdivideHyper->p_2);
                    R->hyperplanes.push_back(h);
                    curNode = curNode->bestng;
                }
                R->set_ext_pts();
                R->center = R->average_point();

                //stopping condition
                point_set *rankingSet1 = new point_set(), *resultSet1 = new point_set();
                pset->findRanking(R->ext_pts[0], rankingSet1);
                rankingSet1->fairTopk_bound_diverse(k, category, R->ext_pts[0], resultSet1);
                bool flag = true;
                for (int i = 1; i < R->ext_pts.size(); ++i)
                {
                    point_set *rankingSet2 = new point_set(), *resultSet2 = new point_set();
                    pset->findRanking(R->ext_pts[i], rankingSet2);
                    rankingSet2->fairTopk_bound_diverse(k, category, R->ext_pts[i], resultSet2);
                    if (!resultSet1->isSame(resultSet2))
                    {
                        flag = false;
                        break;
                    }
                }
                if (flag)
                {
                    resultSet = resultSet1;
                    resultSet->resultEvaluation(realDataset, "HDIA(bound)", numOfQuestion, s, hdl,
                                                client_info, out_cp, EnglishChinese);
                    return numOfQuestion;
                }
            }

            partitionPt = curNode->ptSet;
        }
    }

}
