#ifndef __ROUTE_H__
#define __ROUTE_H__
#include <vector>
#include <set>
#include <iostream>
#define START_PHENO 100
#define ALPHA 0.3
#define BETA 2.5
#define Q_PHENO 10000.0
#define Q_DIST 100.0
#define PHENO_DECREASE 0.95
#define REDUCE_PHENO 0.999
using namespace std;
class DirectedEdge {
public:
    DirectedEdge() {
        this->exist = false;
    }
    DirectedEdge(int destNode, int edgeNum, int edgeCost);
    void printEdge(int sourceNode) {
        cout << sourceNode << "," << this->destNode << "," << this->edgeNum << "," << this->edgeCost << endl;
    }
    virtual ~DirectedEdge(){}
    int destNode;
    int edgeNum;
    int edgeCost;
    double pheno;
    bool exist;
};

class NewDirectedEdge : public DirectedEdge{
public:
    NewDirectedEdge(int destNode,int edgeNum, int edgeCost, vector<DirectedEdge*> &passedEdges, set<int> &passedNodes);
    set<int> passedNodes;
    vector<DirectedEdge*> passedEdges;
};

class Ant {
public:
    Ant(const int sourcePosition,const int num ):_visited(601,false) {
        _position=sourcePosition;
        _visitedEdge.clear();
        _cost=0;
        _num = num;
        arrived=false;
        _tabuLists.clear();
    }
    ~Ant() {}//析构函数
    void travel();
    void update();
    void downdate();
    static void releasePheno();//信息素挥发
    static void printPheno();
    static vector<vector<DirectedEdge> > originMap;//图的临接表
    static vector<vector<NewDirectedEdge> > newMap;
    static set<int> demandSet;
    static vector<NewDirectedEdge*> currentAnswer;//指向每个边的指针数组
    static int currentCost;//当前最优路径
    static int destPosition;
    static int calc;
    bool arrived;
private:
    int _position;
    vector<bool> _visited;
    set<int> _tabuLists;
    vector<NewDirectedEdge*> _visitedEdge;
    NewDirectedEdge *selectEdge();
    int _cost;
    int _num;
};
void search_route(char *graph[5000], int edge_num, char *condition);

#endif