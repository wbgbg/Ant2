#ifndef __ROUTE_H__
#define __ROUTE_H__
#include <vector>
#include <set>
#include <iostream>
#include <map>
#define START_PHENO 30
#define MUTLI_PHENO 100
#define ALPHA 1.7
#define BETA 5.0
#define Q_PHENO 18000.0
#define Q_DIST 100.0
#define PHENO_DECREASE 0.8
#define REDUCE_PHENO 0.9
#define ANT_NUM 1
#define minPheno 50
#define maxPheno 500
using namespace std;
class DirectedEdge {
public:
    DirectedEdge() {
        this->exist = false;
    }
    DirectedEdge(int destNode, int edgeNum, int edgeCost, double pheno);
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
    NewDirectedEdge(int destNode,int edgeNum, int edgeCost, double pheno, vector<DirectedEdge*> &passedEdges, set<int> &passedNodes);
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
        _visited[sourcePosition] = true;
    }
    ~Ant() {}//析构函数
    void travel();
    void update();
    void downdate();
    static void releasePheno();//信息素挥发
    static int countEdges();
    //static void printPheno();
    static vector<vector<DirectedEdge> > originMap;//图的临接表
    static vector<vector<NewDirectedEdge> > newMap;
    static set<int> demandSet;
    static vector<NewDirectedEdge*> currentAnswer;//指向每个边的指针数组
    static int currentCost;//当前最优路径
    static int destPosition;
    static int nodes;
    static int calc;
    static int distance[601][601];
    bool arrived;
    set<int> _tabuLists;
private:
    int _position;
    vector<bool> _visited;
    vector<NewDirectedEdge*> _visitedEdge;
    NewDirectedEdge *selectEdge();
    int _cost;
    int _num;
};
void search_route(char *graph[5000], int edge_num, char *condition);

#endif