/*
默认类中的变量都以下划线开头
未完成: 拷贝构造函数（暂时不需要）,不考虑自环;
测试相关:测试代码（main函数）在单模块测试时保留,UNFIED_TEST时不参与编译.
*/
#include <cassert>
#include <ctime>
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
using namespace std;

typedef int size_type;

/*
假设顶点编号v \in [1,N)
*/
#define MAX_VERTEX_ID ((int)(1e5)+3)

#ifndef UNIFIED_TEST
#define OFFLINE_TEST
#endif

/*
默认edge_induced_subgraph / vertex_induced_subgraph 不需要保留原始边的编号
*/
class Graph
{
    /*顶点集*/
    std::set<int> _v;
    /*边集(默认无向边用一对反平行有向边存储)*/
    std::multimap<std::pair<int, int>, int> _e;
    /*邻接表*/
    std::vector<std::vector<int>> _g;


    //DFS及其拓展tarjan算法(find_cut_vertex,find_cut_edge)所用时间戳dfs_time,(访问vis[](使用_dfn代替));
    int _dfs_time;
    //时间戳标记_dfn[]
    vector<int> _dfn;
    //当前节点以及后代节点的反向边能连到的最浅的节点的时间戳
    vector<int> _low;
    //割点集合_cut_vertex
    vector<int> _cut_vertex;

    #define IS_IN_RANGE(x) ((0 < (x) && (x) < _v.size()))

    //_find_cut_vertex(u/*当前节点*/,f/*dfs树上的父节点，对于根节点f设为0*/)为了代码易于编写，暂时使用递归来写，不用栈模拟
    void _find_cut_vertex(int u,int f)
    {
        _low[u]=_dfn[u] = ++ _dfs_time;
        bool is_cut_vertex=false;
        int child_cnt=0;
        for(int v : _g[u])
        {
            if(v==f)    //不回头访问父节点
                continue;
            if(!_dfn[v])    //树边
            {
                _find_cut_vertex(v,u);
                child_cnt++;
                if(f != 0 && _low[v] >= _dfn[u] /*小心啊啊啊*/)
                    is_cut_vertex=true;               
                _low[u] = min(_low[u],_low[v]);
            }else{  //后向边
                _low[u] = min(_low[u],_dfn[v]);
            }
        }
        if(f == 0 && child_cnt > 1) //是根节点且有多个子节点
            is_cut_vertex=true;
        if(is_cut_vertex)
            _cut_vertex.push_back(u);
    }
public:
    // Graph()：构造一张空图。
    Graph();
    // 释放空间
    ~Graph();

    bool is_vertex_in_v(int x) const { return _v.find(x) != _v.end(); }
    // add_vertex(x)：尝试加入编号为 x 的顶点。若顶点已经存在，则图不发生变化。返回调用后的顶点数。
    int add_vertex(int x);
    // add_edge(u, v)：当 u、v 均存在且不是自环时加入一条无向边，否则忽略此次调用。
    // 每条成功加入的边按成功加入顺序获得从 1 开始的编号。
    void add_edge(int u, int v);

    // vertex_count()：返回当前顶点数。
    int vertex_count() const { return _v.size(); };
    // edge_count()：返回当前边数；重边需要分别计数。
    int edge_count() const { return _e.size() / 2; };
    // degree(x)：返回顶点 x 的度。调用时保证 x 存在。
    int degree(int x) const
    {
        assert(IS_IN_RANGE(x));
        return (int)_g[x].size();
    };
    // edge_multiplicity(u, v)：返回端点为 u、v 的边数。无向边的端点顺序不影响结果。
    int edge_multiplicity(int u, int v) const
    {
        assert(IS_IN_RANGE(u));
        assert(IS_IN_RANGE(v));
        return (int)_e.count({u, v});
    }

    // vertex_induced_subgraph(vertices)：返回由给定点集导出的一个新 Graph 对象。
    Graph vertex_induced_subgraph(const std::vector<int> &vertices) const;
    // edge_induced_subgraph(edge_ids)：返回由给定边集导出的一个新 Graph 对象。
    Graph edge_induced_subgraph(const std::vector<int> &edge_ids) const;

    // print():打印图的顶点集，边集，邻接表（Debug）
    void print() const
    {
        cerr<<"Graph:"<<endl;
        cerr << "vertex set:" << endl;
        for (int v : _v)
            cerr << v << " ";
        cerr << endl;

        cerr << "edge set" << endl;
        for (auto e : _e)
            cerr << '(' << e.first.first << ',' << e.first.second << ')' << " id:" << e.second << endl;
        cerr << endl;

        cerr << "adjacency list" << endl;
        for (int u : _v)
        {
            cerr << "vertex:" << u << endl;
            for (int v : _g[u])
                cerr << v << " ";
            cerr << endl;
        }
        cerr<<endl;
    }
    vector<int> find_cut_vertex()
    {
        _dfs_time = 0;
        _cut_vertex.clear();
        _dfn.resize(_g.size());
        _low.resize(_g.size());
        fill(_dfn.begin(),_dfn.end(),0);
        for(int v : _v)
            if(!_dfn[v])
                _find_cut_vertex(v,0);
        return _cut_vertex;
    }
};

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

            if(v > u/*去掉反平行边*/)
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
            if (!ans._v.count(v) || v > u/*去掉反平行边*/)
                continue;
 
            ans.add_edge(u, v);
        }
    }
    return ans;
}

int Graph::add_vertex(int x)
{
    assert(0<(x) && (x) <= MAX_VERTEX_ID);
    if (!_v.count(x))
    {
        _v.insert(x);
        if (x >= (int)_g.size()/*小心*/)
        {
            _g.resize(x + 1, std::vector<int>(0));
        }
    }
    return /*小心*/ (int)_v.size();
}
void Graph::add_edge(int u, int v)
{
    assert(u != v);
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

#ifdef OFFLINE_TEST
int main()
{
    int n=100;
    Graph g;
    for(int i=1;i<=n;++i)
        g.add_vertex(i);
    
    //for(int i=1;i<=n;++i)
    //    for(int j=i-1;j>=1;--j)
    //        g.add_edge(i,j);
    
    printf("use time:%ldms",clock());
    return 0;
}
#endif
