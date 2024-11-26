#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include "define.h"
#include <mutex>
#include <condition_variable>

// R-tree related data structures
typedef struct rtree_info_s
{
	int m, M, dim, reinsert_p, no_histogram;
	int extra_level;
} rtree_info;

// R-tree related data structures
typedef struct node {
	R_TYPE *a;
	R_TYPE *b;
	int id;
	int attribute;
	int vacancy;
	struct node *parent;
	struct node **ptr;
	int dim;

} node_type;

// R-tree related data structures
struct nodeCmp
{
	bool operator()(const node_type* lhs, const node_type* rhs) const
	{
		double dist1 = 0, dist2 = 0;
		for (int i = 0; i < lhs->dim; i++)
		{
			dist1 += pow(1 - lhs->b[i], 2);
			dist2 += pow(1 - rhs->b[i], 2);
		}

		return dist1 > dist2;
	}
};

// R-tree related data structures
typedef struct NN {
	double dist;
	int oid;
	struct node *pointer;
	int level;
	struct NN *next;
} NN_type;

// R-tree related data structures
typedef struct BranchArray {
	double min;
	node_type *node;
} ABL;

// R-tree related data structures
typedef struct config_rt {
	int dim;
	int m;
	int M;
	int reinsert_p;
	int no_histogram;
	//char nodefile[FILENAME_MAX];
	//char rootfile[FILENAME_MAX];
	char queryfile[FILENAME_MAX];
	char positionfile[FILENAME_MAX];
	char save_tree_file[FILENAME_MAX];
}   config_type;

// R-tree related data structures
struct setNode_s
{
	int noOfNode;
	node_type **elt;
	int *level;
};

typedef struct setNode_s setNode;

// 客户端交互信息
struct client_data {
    std::string user_input;  // 用户输入的数据
    std::mutex mtx;  // 互斥锁
    std::condition_variable cv;  // 条件变量用于同步
    bool data_ready = false;  // 表示数据是否准备好
    bool should_exit = false;  // 标志位，用于通知线程退出

};
#endif