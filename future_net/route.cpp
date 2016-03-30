#include "route.h"
#include "lib/lib_record.h"
#include "lib/util.cpp"
#include <algorithm>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <iterator>
#include <set>
#include <deque>
#include <string>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <map>
#include <complex>
#include <cmath>
#include <unistd.h>
using namespace std;
//你要完成的功能总入口

vector<vector<DirectedEdge> > Ant::originMap(601);
vector<vector<NewDirectedEdge> > Ant::newMap(601);
int Ant::destPosition;
set<int> Ant::demandSet;
int Ant::calc;
vector<NewDirectedEdge *> Ant::currentAnswer;
int Ant::currentCost;
DirectedEdge::DirectedEdge(int destNode, int edgeNum, int edgeCost) {
    this->destNode = destNode;
    this->edgeNum = edgeNum;
    this->edgeCost = edgeCost;
    this->pheno = START_PHENO;
    this->exist = true;
}
int tooAlpha=0,tooBeta=0;

NewDirectedEdge* Ant::selectEdge() {
    double sum=0.0;
    vector<pair<NewDirectedEdge *, double> > partial;
    partial.clear();
    for (int edge=0;edge<newMap[_position].size();edge++) {
        NewDirectedEdge* nextEdge = &newMap[_position][edge];
        if (_tabuLists.size()!=(demandSet.size()-1) && nextEdge->destNode==destPosition) {
            continue;
        }
        if (_tabuLists.find(nextEdge->destNode)==_tabuLists.end()) {
            bool flag=true;
            for (auto nodeI : nextEdge->passedNodes) {
                if (_visited[nodeI]) {
                    flag=false;
                    break;
                }
            }
            if (flag) {
                if (nextEdge->edgeCost==0) {
                    cout << "cost 0";
                    break;
                }
                double answer = pow(nextEdge->pheno,ALPHA)*pow(Q_DIST/nextEdge->edgeCost,BETA);
                partial.push_back(pair<NewDirectedEdge *, double>(nextEdge, answer));
                sum+=answer;
            }
        }
    }
    double answer = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/sum));
    for (int edge=0;edge<partial.size();edge++) {
        NewDirectedEdge *nextEdge =partial[edge].first;
        double percentage = partial[edge].second;
        if (percentage > answer) {
            return nextEdge;
        } else {
            answer -= percentage;
        }//按照各个边的概率选择下一条边
    }
    return nullptr;
}

NewDirectedEdge::NewDirectedEdge(int destNode, int edgeNum, int edgeCost, vector<DirectedEdge*> &passedEdges, set<int> &passedNodes):DirectedEdge(destNode, edgeNum, edgeCost) {
    this->passedEdges = passedEdges;
    this->passedNodes = passedNodes;
}

int maxlen=0;//当前含V'节点最多的路径中的V'节点数

void Ant::travel() {
    if (_position == destPosition) {
        arrived=true;
        if (currentCost>_cost) {
            //for (auto routine : _visitedEdge) {
            //    cout <<  " towards " << routine->destNode <<"edge"<<routine->edgeNum << endl;
            //    cout << routine->destNode;
            //}
            cout << "cost:" << _cost << endl;
            //getchar();
            currentCost = _cost;
            currentAnswer = _visitedEdge;
        }//如果当前cost小于最好的currentCost
        return;
    } else {
        NewDirectedEdge *selectedEdge=selectEdge();
        if (selectedEdge!= nullptr) {
            _position = selectedEdge->destNode;
            for (auto points : selectedEdge->passedNodes) {
                _visited[points]=true;
            }
            _visitedEdge.push_back(selectedEdge);
            _cost+=selectedEdge->edgeCost;
            _tabuLists.insert(selectedEdge->destNode);
            travel();
        }
    }
}

void Ant::update() {
    for (NewDirectedEdge* passEdge : _visitedEdge) {
        passEdge->pheno+=Q_PHENO/_cost;
        //cout << Q_PHENO/_cost << endl;
    }
}//迭代较优的路径更新信息素
void Ant::downdate() {
    for (NewDirectedEdge* passEdge: _visitedEdge) {
        passEdge->pheno*= REDUCE_PHENO;
    }
}
void Ant::releasePheno(){
    for (int i=0;i<newMap.size();i++) {
        for (NewDirectedEdge &edge : newMap[i]) {
            edge.pheno *= PHENO_DECREASE;
        }
    }
}//信息素挥发

struct TimeOut : public exception
{
    const char * what () const throw ()
    {
        return "timeout";
    }
};

void Ant::printPheno(){
    set<int> xSet = {1,9,14,75,156,111,171,106,29,25,54,50,64,142,159,81,170,84,107,141};
    double sum=0;
    int num=0;
    for (int i=0;i<newMap.size();i++) {
        for (auto edge : newMap[i]) {
            sum += edge.pheno;
            num++;
            for (auto edgeI : edge.passedEdges) {
                if (xSet.find(edgeI->destNode) != xSet.end()) {
                    cout << "pheno :" << edge.pheno << endl;
                    break;
                }
            }
        }
    }
    cout << "avarage pheno " << sum/num << endl;
}

void timeOver(int x) {
    signal(SIGPROF, timeOver);
    if (Ant::currentAnswer.size()!=0) {
        for (auto &answerIter : Ant::currentAnswer) {
            reverse(answerIter->passedEdges.begin(),answerIter->passedEdges.end());
            for (auto answer2Iter : answerIter->passedEdges) {
                record_result(answer2Iter->edgeNum);
            }
        }
    }
    throw TimeOut();//抛出异常
    //timeCount++;
    //cout << timeCount << "s has passed " << maxSize << endl;
}

void spfa(const int &sourceNode, vector<vector<DirectedEdge> > &adjMap) {
    const int &INF=0x7FFFFFFF,&NODE=adjMap.size();//用邻接表的大小传递顶点个数，减少参数传递
    vector<int> destDist;
    destDist.assign(NODE,INF);//初始化距离为无穷大
    deque<int> que(1,sourceNode);//处理队列
    vector<int> precNode(600,-1);
    vector<DirectedEdge*> precEdge(600,0);
    vector<bool> flag(NODE,0);//标志数组，判断是否在队列中
    destDist[sourceNode]=0;//出发点到自身路径长度为0
    flag[sourceNode]=1;//入队
    while(!que.empty())
    {
        const int now=que.front();//当前处理的点，由于后面被删除，不可定义成常量引用
        que.pop_front();
        flag[now]=0;//将该点拿出队列
        for(int i=0; i!=adjMap[now].size(); ++i)//遍历所有与当前点有路径的点
        {
            const int &next=adjMap[now][i].destNode;//目标点，不妨定义成常量引用，稍稍快些
            if(destDist[now]<INF&&//若距离已知（否则下面右式计算结果必爆int），且
               //注：与运算先判断左式是否成立，若不成立则右式不会被判断
               destDist[next]>destDist[now]+adjMap[now][i].edgeCost)//优于当前值
            {
                destDist[next]=destDist[now]+adjMap[now][i].edgeCost;//更新
                precNode[next]=now;
                precEdge[next]=&adjMap[now][i];
                if(!flag[next])//若未在处理队列中
                {
                    if(que.empty()||//空队列，或（或运算实现原理类似与运算）
                       destDist[next]<destDist[que.front()])//优先级高于队首（SLF）
                        que.push_front(next);//放在队首
                    else que.push_back(next);//否则放在队尾
                    flag[next]=1;//入队
                }
            }
        }
    }
    for (auto nodeI : Ant::demandSet) {
        if (nodeI==sourceNode) {
            continue;
        }
        int nodeJ=nodeI;
        vector<DirectedEdge*> passedEdges;
        set<int> passedNodes;
        passedEdges.clear();
        passedNodes.clear();
        int cost=0;
        while (precNode[nodeJ]!=-1) {
            passedNodes.insert(nodeJ);
            passedEdges.push_back(precEdge[nodeJ]);
            cost+=precEdge[nodeJ]->edgeCost;
            nodeJ=precNode[nodeJ];
        }
        if (cost>0) {
            NewDirectedEdge newEdge(nodeI,0,cost,passedEdges,passedNodes);
            Ant::newMap[sourceNode].push_back(newEdge);
        }
    }
}

void search_route(char *topo[5000], int edge_num, char *demand)
{
    srand(time(0));//用当前时间值种随机数种子
    signal(SIGPROF, timeOver);//SIGALRM触发timeover函数
    struct itimerval tick;
    tick.it_value.tv_sec=9;
    tick.it_value.tv_usec=0;//it_value指定初始定时时间
    tick.it_interval.tv_sec=0;
    tick.it_interval.tv_usec=10000;//it_interval指定定时间隔
    int res=setitimer(ITIMER_PROF, &tick, NULL);//抛出SIGALRM信号
    if (res!=0) {
        cout << "set timer fail errno:" << errno << endl;
    }
    
    cout << "search_route begin" << endl;
    vector<int> demandVec;
    strToVector(demand, ",|", &demandVec);//字符串转换成必经节点数组demandVec
    const int sourceNode = demandVec[0];
    const int destNode = demandVec[1];
    for (int i=1;i<demandVec.size();i++) {
        Ant::demandSet.insert(demandVec[i]);
    }
    
    for (int nodeI=0; nodeI<edge_num; nodeI++) {
        vector<int> topoVec;
        strToVector(topo[nodeI], ",", &topoVec);
        DirectedEdge edgeNow(topoVec[2], topoVec[0], topoVec[3]);
        Ant::originMap[topoVec[1]].push_back(edgeNow);
    }//字符串转换成临接表originMap

    for (auto nodeI : Ant::demandSet) {
        spfa(nodeI,Ant::originMap);
    }
    spfa(sourceNode,Ant::originMap);

    Ant::currentCost=99999;
    Ant::currentAnswer.clear();
    Ant::destPosition=destNode;
    vector<Ant> antTeam;
    try {
        int round=0;
        while (true) {
            round++;
            Ant::calc=0;
            for (int i = 0; i < 1; i++) {
                antTeam.push_back(Ant(sourceNode, i));
                //cout << "send a new ant" << endl;
                antTeam[i].travel();
                if (antTeam[i].arrived) {
                    Ant::calc++;
                }
            }//所有蚂蚁走一圈
            //Ant::releasePheno();//信息素挥发
            for (int i = 0; i < 1; i++) {
                if(antTeam[i].arrived) {
                    antTeam[i].update();
                } else {
                    antTeam[i].downdate();
                }
            }//到达最终节点的蚂蚁判断是否更新信息素
            //cout << Ant::calc << " ants arrived" << endl;
            //Ant::printPheno();//打印测试节点的信息素
            //sleep(1);
            antTeam.clear();
        }
    } 
    catch (TimeOut &err) {
        cout << "over" << endl;
    }
    catch(exception& e)
    {
        cout << "over" << endl;
    }
}
