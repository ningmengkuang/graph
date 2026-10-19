/*
文件说明：
默认类中的变量都以下划线开头
未完成: 拷贝构造函数（暂时不需要）；
测试相关:测试代码（main函数）在单模块测试时保留,UNFIED_TEST时不参与编译.
*/

//------------------------------------------------------------------------------
// Code block: Includes
//------------------------------------------------------------------------------
#include "./graph.h"

//------------------------------------------------------------------------------
// Code block: Configuration Macros
//------------------------------------------------------------------------------
/* 总开关宏：定义 OFFLINE_TEST 表示本地离线调试模式（保留自测 main 与调试输出）；
   提交整个工程时不定义它，相关调试痕迹即全部失效化。 */
#define OFFLINE_TEST
/* 关闭断言 */
#define NDEBUG
// #define assert(...) true

//------------------------------------------------------------------------------
// Code block: Disabled Code
//------------------------------------------------------------------------------
/* 下面是割边算法的实现，当前尚未完成，整段注释保留以便后续补全。 */
// void _find_cut_edge(int u, int f)
// {
//     low[u] = dfn[u] = ++dfs_time;
//     bool is_cut_vertex = false;
//     int child_cnt = 0;
//     for (int v : _g[u])
//     {
//         if (v == f) // 不回头访问父节点
//             continue;
//         if (!dfn[v]) // 树边
//         {
//             _find_cut_edge(v, u);
//             child_cnt++;
//             if (f != 0 && low[v] == dfn[v]) // 小心啊啊啊
//                 _cut_edge.push_back({u, v});
//             low[u] = std::min(low[u], low[v]);
//         }
//         else
//         { // 后向边
//             low[u] = std::min(low[u], dfn[v]);
//         }
//     }
// }

// std::vector<std::pair<int, int>> find_cut_edge()
// {
//     // 割边集合_cut_edge 无向边用其中一个有向边代表
//     std::vector<std::pair<int, int>> _cut_edge;
//     dfs_time = 0;
//     _cut_edge.clear();
//     dfn.resize(_g.size());
//     low.resize(_g.size());
//     fill(dfn.begin(), dfn.end(), 0);
//     for (int v : _v)
//         if (!dfn[v])
//             _find_cut_edge(v, 0);
//     return _cut_edge;
// }

//------------------------------------------------------------------------------
// Code block: Graph — Implementation
//------------------------------------------------------------------------------
std::vector<int> Graph::find_cut_vertex() const
{
    // DFS及其拓展tarjan算法(find_cut_vertex,find_cut_edge)所用时间戳dfs_time,(访问vis[](使用dfn代替));
    int dfs_time;
    // 时间戳标记dfn[]
    std::vector<int> dfn(_g.size(), 0);
    // 当前节点以及后代节点的反向边能连到的最浅的节点的时间戳
    std::vector<int> low(_g.size());
    // 是否属于割点集合 is_cut_vertex
    std::vector<bool> is_cut_vertex(_g.size(), false);
    dfs_time = 0;
    for (int v : _v)
        if (!dfn[v])
            _find_cut_vertex(v, 0, low, dfn, dfs_time, is_cut_vertex);
    dfn.clear();
    low.clear();
    std::vector<int> cut_vertex;
    for (int v : _v)
    {
        if (is_cut_vertex[v])
        {
            cut_vertex.push_back(v);
        }
    }
    is_cut_vertex.clear();
    return cut_vertex;
}

void Graph::print() const
{
    std::cerr << "Graph:" << std::endl;
    std::cerr << "vertex set:" << std::endl;
    for (int v : _v)
        std::cerr << v << " ";
    std::cerr << std::endl;

    std::cerr << "edge set" << std::endl;
    for (auto e : _e)
        std::cerr << '(' << e.first.first << ',' << e.first.second << ')'
                  << " id:" << e.second << std::endl;
    std::cerr << std::endl;

    std::cerr << "adjacency list" << std::endl;
    for (int u : _v)
    {
        std::cerr << "vertex:" << u << std::endl;
        for (int v : _g[u])
            std::cerr << v << " ";
        std::cerr << std::endl;
    }
    std::cerr << std::endl;
}

void Graph::_find_cut_vertex(int u, int f, std::vector<int> &low, std::vector<int> &dfn,
                             int &dfs_time, std::vector<bool> &is_cut_vertex) const
{
    low[u] = dfn[u] = ++dfs_time;
    is_cut_vertex[u] = false;
    int child_cnt = 0;
    for (int v : _g[u])
    {
        if (v == f) // 不回头访问父节点
            continue;
        if (!dfn[v]) // 树边
        {
            _find_cut_vertex(v, u, low, dfn, dfs_time, is_cut_vertex);
            child_cnt++;
            if (f != 0 && low[v] >= dfn[u] /*小心啊啊啊*/)
                is_cut_vertex[u] = true;
            low[u] = std::min(low[u], low[v]);
        }
        else
        { // 后向边
            low[u] = std::min(low[u], dfn[v]);
        }
    }
    if (f == 0 && child_cnt > 1) // 是根节点且有多个子节点
        is_cut_vertex[u] = true;
}

Graph::Graph()
{
    _g = std::vector<std::vector<int>>();
    _v = std::set<int>();
    _e = std::multimap<std::pair<int, int>, int>();
}

Graph::~Graph()
{
    _g.clear();
    _v.clear();
    _e.clear();
}

Graph Graph::edge_induced_subgraph(const std::vector<int> &edge_ids) const
{
    Graph ans;
    std::set<int> edge_id_set;
    for (int id : edge_ids)
        edge_id_set.insert(id);
    for (auto e : _e)
    {
        int edge_id = e.second;
        if (edge_id_set.count(edge_id))
        {
            int u = e.first.first;
            int v = e.first.second;

            if (v > u /*去掉反平行边*/)
                continue;
            if (!ans.is_vertex_in_v(u))
                ans.add_vertex(u);
            if (!ans.is_vertex_in_v(v))
                ans.add_vertex(v);

            ans.add_edge(u, v);
        }
    }
    return ans;
}

Graph Graph::vertex_induced_subgraph(const std::vector<int> &vertices) const
{
    Graph ans;
    for (int v : vertices)
        ans.add_vertex(v);
    for (int u : _v)
    {
        if (!ans._v.count(u))
            continue;

        for (int v : _g[u])
        {
            if (!ans._v.count(v) || v > u /*去掉反平行边*/)
                continue;

            ans.add_edge(u, v);
        }
    }
    return ans;
}

int Graph::add_vertex(int x)
{
    assert(0 < (x) && (x) <= MAX_VERTEX_ID);
    if (!_v.count(x))
    {
        _v.insert(x);
        if (x >= (int)_g.size() /*小心*/)
        {
            _g.resize(x + 1, std::vector<int>(0));
        }
    }
    return /*小心*/ (int)_v.size();
}

void Graph::add_edge(int u, int v)
{
    assert(IS_IN_RANGE(u) && IS_IN_RANGE(v));
    if (_v.count(u) && _v.count(v))
    {
        _g[u].push_back(v);
        _g[v].push_back(u);
        int edge_id = edge_count() + 1;
        _e.insert({{u, v}, edge_id});
        _e.insert({{v, u}, edge_id});
    }
}

//------------------------------------------------------------------------------
// Code block: GraphAlgorithms — Implementation
//------------------------------------------------------------------------------
std::vector<std::vector<std::pair<int, int>>> GraphAlgorithms::blocks() const
{
    std::vector<std::vector<std::pair<int, int>>> ans;
    return ans;
}

GraphAlgorithms::GraphAlgorithms(
    int n,
    const std::vector<std::pair<int, int>> &edges)
{
    for (int i = 1; i <= n; ++i)
    {
        add_vertex(i);
    }
    for (auto e : edges)
    {
        add_edge(e.first, e.second);
    }
}

std::list<int> GraphAlgorithms::_eular_trail(int u, std::multiset<std::pair<int, int>> &edge_vis,
                                             std::vector<int> &cur) const
{
    std::list<int> ans;
    while (cur[u] < _g[u].size())
    {
        int v = _g[u][cur[u]++];
        if (edge_vis.count({u, v}) + edge_vis.count({v, u}) >= edge_multiplicity(u, v))
            continue;
        edge_vis.insert({u, v});
        if (!ans.size())
            ans = _eular_trail(v, edge_vis, cur);
        else
        {
            std::list<int> new_trail = _eular_trail(v, edge_vis, cur);
            new_trail.splice(new_trail.end(), ans);
            std::swap(ans, new_trail);
        }
    }
    ans.push_front(u);
    return ans;
}

bool GraphAlgorithms::is_bipartite() const
{
    std::vector<vis_type> vis(_g.size(), UNVIS);
    bool flag = true;
    for (const int u : _v)
    {
        if (flag && vis[u] == UNVIS)
        {
            flag = flag && _is_bipartite(u, vis);
        }
    }
    vis.clear();
    return flag;
}

std::vector<int> GraphAlgorithms::euler_trail() const
{
    if (_e.size() == 0)
    {
        // 若m=0返回任意一个合法顶点组成的序列.图有 n 个顶点,编号为 1~n.图可能不连通,允许重边和自环.
        return std::vector<int>(1, 1);
    }
    std::vector<int> deg(_g.size(), 0);
    std::vector<int> odd_vertex;
    for (const int v : _v)
    {
        deg[v] = _g[v].size();
        if ((deg[v] & 1) == 1)
        {
            odd_vertex.push_back(v);
        }
    }
    if (odd_vertex.size() != 0 && odd_vertex.size() != 2)
    {
        // 若不存在欧拉迹,C++ 返回空向量
        return std::vector<int>();
    }
    std::vector<int> cur(_g.size(), 0);
    std::list<int> ans_trail_list;
    std::multiset<std::pair<int, int>> edge_vis;
    if (odd_vertex.size() == 2)
        ans_trail_list = _eular_trail(odd_vertex.front(), edge_vis, cur);
    else
    {
        int v = 0;
        for (int u : _v)
            if (deg[u])
            {
                v = u;
                break;
            }
        ans_trail_list = _eular_trail(v, edge_vis, cur);
    }
    for (const int u : _v)
    {
        while (cur[u] < _g[u].size() &&
               edge_vis.count({u, _g[u][cur[u]]}) + edge_vis.count({_g[u][cur[u]], u}) >=
                   edge_multiplicity(u, _g[u][cur[u]]))
            cur[u]++;
        if (cur[u] < _g[u].size())
        {
            // 图不连通
            // 若不存在欧拉迹,C++ 返回空向量
            return std::vector<int>();
        }
    }
    return std::vector<int>(ans_trail_list.begin(), ans_trail_list.end());
}

bool GraphAlgorithms::_is_bipartite(int u, std::vector<vis_type> &vis) const
{
    bool flag = true;
    for (const int v : _g[u])
    {
        if (vis[v] == UNVIS)
        {
            vis[v] = vis[u] == SAME ? DIF : SAME;
            flag = flag && _is_bipartite(v, vis);
        }
        else if (vis[v] == vis[u])
        {
            return false;
        }
    }
    return flag;
}

//------------------------------------------------------------------------------
// Code block: Main Function
//------------------------------------------------------------------------------
/*
    单模块自测入口：仅当定义了 OFFLINE_TEST 时参与编译，UNIFIED_TEST 时不参与编译。
    @returns 进程退出码
*/
#ifdef OFFLINE_TEST
using namespace std;
int main()
{
}
#endif
