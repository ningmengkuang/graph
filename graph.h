//------------------------------------------------------------------------------
// Code block: Header Guard
//------------------------------------------------------------------------------
#ifndef GRAPH_H     //#pragma once
#define GRAPH_H

//------------------------------------------------------------------------------
// Code block: Includes
//------------------------------------------------------------------------------
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <stack>
#include <vector>

//------------------------------------------------------------------------------
// Code block: Constants & Typedef
//------------------------------------------------------------------------------
/* 顶点编号的合法上界：顶点编号 v 满足 0 < v <= MAX_VERTEX_ID */
#define MAX_VERTEX_ID ((int)(2e5) + 3)

/* 下标与计数统一使用的整数类型别名 */
typedef int size_type;

/*
全局约定：
1. 类内成员变量一律以下划线开头；
2. vertex_induced_subgraph / edge_induced_subgraph 不保留原始边的编号；
3. 顶点编号 v 的取值范围是 [1,N)，邻接表 _g 的第 0 号下标空置。
*/

//------------------------------------------------------------------------------
// Code block: Class Definitions
//------------------------------------------------------------------------------
//template<typename weight_type=int>
class Graph
{
protected:
    //--------------------------------------------------------------------------
    // Code block: Data Members
    //--------------------------------------------------------------------------
    /* 顶点集：当前图中所有顶点的编号 */
    std::set<int> _v;
    /* 边集：无向边用一对反平行有向边存储，键为 (u,v)，值为从 1 开始的边编号 */
    std::multimap<std::pair<int, int>, int> _e;
    /* 邻接表：_g[u] 列出与 u 相邻的所有顶点，重边对应重复元素 */
    std::vector<std::vector<int>> _g;

    /* 判断顶点编号 x 是否落在邻接表 _g 的合法下标范围内 */
    #define IS_IN_RANGE(x) ((0 < (x) && (x) < _g.size()))

    /*
        Tarjan 割点算法的递归搜索过程：从 u 出发做 DFS，标记以 u 为根的搜索子树中的割点。
        为了代码易于编写，暂时使用递归来写，不用栈模拟。
        @param u 当前搜索到的顶点
        @param f DFS 树上 u 的父节点（根节点固定传 0）
        @param low 各顶点经自身子树内的反向边能回溯到的最小 dfn
        @param dfn 各顶点的 DFS 时间戳，兼作访问标记（0 表示尚未访问）
        @param dfs_time DFS 时间戳计数器
        @param is_cut_vertex 输出数组，is_cut_vertex[v] 标记 v 是否为割点
        @returns 无（割点结果通过 is_cut_vertex 传出）
    */
    void _find_cut_vertex(int u, int f, std::vector<int> &low, std::vector<int> &dfn,
                          int &dfs_time, std::vector<bool> &is_cut_vertex) const;

    /*
        求无向图割边（桥）集合的递归搜索过程。
        @param u 当前搜索到的顶点
        @param f DFS 树上 u 的父节点（根节点固定传 0）
        @returns 无（割边结果追加到 _cut_edge）
        注意：实现体当前尚未完成，声明先保留以便后续补全。
    */
    void _find_cut_edge(int u, int f);

public:
    //--------------------------------------------------------------------------
    // Code block: Construction & Modification
    //--------------------------------------------------------------------------
    /*
        构造一张空图（无顶点、无边、邻接表为空）。
        @returns 无
    */
    Graph();
    /*
        析构函数：清空顶点集、边集与邻接表并释放其占用的空间。
        @returns 无
    */
    ~Graph();

    /*
        判断顶点 x 是否存在于当前图中。
        @param x 待查询的顶点编号
        @returns 存在返回 true，否则返回 false
    */
    bool is_vertex_in_v(int x) const { return _v.find(x) != _v.end(); }
    /*
        尝试向图中加入编号为 x 的顶点；若该顶点已经存在，则图不发生变化。
        @param x 待加入的顶点编号，需满足 0 < x <= MAX_VERTEX_ID
        @returns 调用后的顶点总数
    */
    int add_vertex(int x);
    /*
        尝试加入一条连接 u、v 的无向边：仅当 u、v 均存在且不是自环时才生效，
        否则忽略此次调用。成功的边按加入顺序获得从 1 开始的编号，
        并在边集 _e 与邻接表 _g 中各记录一次（一对反平行有向边）。
        @param u 第一个端点的编号
        @param v 第二个端点的编号
        @returns 无
    */
    void add_edge(int u, int v);

    //--------------------------------------------------------------------------
    // Code block: Queries
    //--------------------------------------------------------------------------
    /*
        查询当前图的顶点数。
        @returns 顶点总数
    */
    int vertex_count() const { return _v.size(); };
    /*
        查询当前图的边数；重边需要分别计数。
        @returns 边总数（每条无向边在 _e 中占两项，故取其大小的一半）
    */
    int edge_count() const { return _e.size() / 2; };
    /*
        查询顶点 x 的度。
        @param x 顶点编号（调用时保证该顶点存在）
        @returns x 的度数（重边多次计数）
    */
    int degree(int x) const
    {
        assert(IS_IN_RANGE(x));
        return (int)_g[x].size();
    }
    /*
        查询端点为 u、v 的边的重数。
        @param u 第一个端点的编号
        @param v 第二个端点的编号
        @returns 端点恰为 u、v 的边数（无向边的端点顺序不影响结果）
    */
    int edge_multiplicity(int u, int v) const
    {
        assert(IS_IN_RANGE(u));
        assert(IS_IN_RANGE(v));
        return (int)_e.count({u, v});
    }

    //--------------------------------------------------------------------------
    // Code block: Induced Subgraphs
    //--------------------------------------------------------------------------
    /*
        求由给定顶点集导出的点导出子图。
        @param vertices 导出子图包含的顶点编号列表
        @returns 一个新的 Graph 对象：顶点集为 vertices，边集为原图中两端都在 vertices 内的边
                 （不保留原始边的编号）
    */
    Graph vertex_induced_subgraph(const std::vector<int> &vertices) const;
    /*
        求由给定边集导出的边导出子图。
        @param edge_ids 导出子图包含的边编号列表
        @returns 一个新的 Graph 对象：边集为指定的这些边，顶点集为其端点的并集
                 （不保留原始边的编号）
    */
    Graph edge_induced_subgraph(const std::vector<int> &edge_ids) const;

    //--------------------------------------------------------------------------
    // Code block: Debug Output
    //--------------------------------------------------------------------------
    /*
        调试用输出：把顶点集、边集与邻接表打印到标准错误流。
        @returns 无
    */
    void print() const;

    //--------------------------------------------------------------------------
    // Code block: Connectivity
    //--------------------------------------------------------------------------
    /*
        求无向图的全部割点（Tarjan 算法）。
        @returns 割点的编号列表；无割点时返回空向量
    */
    std::vector<int> find_cut_vertex() const;
    /*
        求无向图的全部割边（桥）。
        @returns 每条割边用其中一个有向边 (u,v) 表示，组成列表；无割边时返回空向量
        注意：实现体当前尚未完成。
    */
    std::vector<std::pair<int, int>> find_cut_edge();
};

//------------------------------------------------------------------------------
// Code block: Struct & Enum
//------------------------------------------------------------------------------
/*
    二分图判定中顶点的染色状态。
    UNVIS：尚未访问；SAME / DIF：两种不同的颜色，相邻顶点必须取不同颜色。
*/
typedef enum
{
    UNVIS = 0,
    SAME = 1,
    DIF = 2
} vis_type;

/*
    GraphAlgorithms：在 Graph 的基础上提供具体图算法
    （割点、二分图判定、欧拉迹、点双连通分量分解等）。
*/
class GraphAlgorithms : public Graph
{
    /*
        Hierholzer 算法走欧拉迹的递归过程：从 u 出发沿着尚未使用的边深入，
        回溯时把顶点插入序列前端，从而得到经过这些边的一条顶点序列。
        @param u 当前所在的顶点
        @param edge_vis 已使用边的计数集合，用于判断某条边是否还能走
        @param cur 各顶点在邻接表中的扫描游标，指向下一条待检查的出边
        @returns 从 u 出发走出的顶点序列（list，首元素为 u）
    */
    std::list<int> _eular_trail(int u, std::multiset<std::pair<int,int> > &edge_vis,
                                std::vector<int> &cur) const;
    /*
        二分图判定的递归染色过程：给尚未染色的邻居染上与 u 相反的颜色，
        若遇到与 u 同色的邻居则说明存在奇环，判定失败。
        @param u 当前搜索到的顶点
        @param vis 各顶点的染色状态（UNVIS / SAME / DIF）
        @returns 以 u 为根的搜索子树是否满足二分性
    */
    bool _is_bipartite(int u, std::vector<vis_type> &vis) const;
    /*
        求点双连通分量（块）。
        @returns 无
        注意：当前尚未实现（无定义、无调用），保留以备后续补全。
    */
    void get_block();
public:
    //--------------------------------------------------------------------------
    // Code block: Construction
    //--------------------------------------------------------------------------
    /*
        用 n 个顶点（编号依次为 1,2,...,n）和给定边表构造一张图，并加入所有边。
        @param n 顶点个数
        @param edges 边表，每一项是一对端点编号
        @returns 无
    */
    GraphAlgorithms(
        int n,
        const std::vector<std::pair<int, int>> &edges);

    //--------------------------------------------------------------------------
    // Code block: Algorithms
    //--------------------------------------------------------------------------
    /*
        求无向图的所有割点。
        @returns 割点的编号列表（转发到 Graph::find_cut_vertex）
    */
    std::vector<int> articulation_points() const { return find_cut_vertex(); }
    /*
        判断无向图是否为二分图。
        @returns 是二分图返回 true，否则返回 false
    */
    bool is_bipartite() const;
    /*
        求一条欧拉迹（经过每条边恰好一次的一条通路）。
        @returns 依次经过的顶点编号序列；不存在欧拉迹时返回空向量；
                 当 m = 0 时返回仅含一个合法顶点的序列
    */
    std::vector<int> euler_trail() const;
    /*
        求全部点双连通分量（块）。
        @returns 每个块用其边集（(u,v) 的列表）表示
        注意：当前返回空向量，尚未实现。
    */
    std::vector<std::vector<std::pair<int,int> > > blocks() const;
};

#endif  // GRAPH_H