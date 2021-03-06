#include "route.h"
#include "lib/lib_record.h"
#include "lib/util.cpp"
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <iterator>
#include <deque>
#include <string>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <complex>
#include <cmath>
#include <unistd.h>
#include <fstream>
#include <string.h>
using namespace std;
//你要完成的功能总入口

vector<vector<DirectedEdge> > Ant::originMap(601);
vector<vector<NewDirectedEdge> > Ant::newMap(601);
int Ant::destPosition;
set<int> Ant::demandSet;
int Ant::calc;
int Ant::nodes=0;
vector<NewDirectedEdge *> Ant::currentAnswer;
int Ant::currentCost;
bool stopFlag = false;
vector<int> beenTimes(601);
int Ant::distance[601][601]={};
map<string, double> constArg;
//vector<int> bestAnswer = {254,236,265,227,240}

DirectedEdge::DirectedEdge(int destNode, int edgeNum, int edgeCost, double pheno) {
    this->destNode = destNode;
    this->edgeNum = edgeNum;
    this->edgeCost = edgeCost;
    this->pheno = pheno;
    this->exist = true;
}
int tooAlpha=0,tooBeta=0;

NewDirectedEdge* Ant::selectEdge() {
    double sum1=0.0,sum2=0.0;
    //int mutli;
    int max = 0;
    double answer1;
    vector<pair<NewDirectedEdge *, double> > partial1,partial2;
    partial1.clear();
    partial2.clear();
    for (int edge=0;edge<newMap[_position].size();edge++) {
        NewDirectedEdge* nextEdge = &newMap[_position][edge];
        if((nextEdge->edgeNum==-1)&&(nextEdge->edgeCost>max)){
            max = nextEdge->edgeCost;
        }
    }
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
                if(nextEdge->edgeNum <= -1){
                    answer1 = pow(nextEdge->pheno,constArg.at("ALPHA"))*pow(constArg.at("Q_DIST")/nextEdge->edgeCost,constArg.at("BETA"));
                }
                else{
                    if(max!=0){
                        answer1 = pow(nextEdge->pheno,constArg.at("ALPHA"))*pow(constArg.at("Q_DIST")/max,constArg.at("BETA"));
                    }
                    else{
                        answer1 = pow(nextEdge->pheno,constArg.at("ALPHA"))*pow(constArg.at("Q_DIST")/nextEdge->edgeCost,constArg.at("BETA"));
                    }
                }
                //if(nextEdge->edgeNum == -1){
                //double answer1 = pow(nextEdge->pheno,constArg.at("ALPHA"))*pow(constArg.at("Q_DIST")/(nextEdge->edgeCost*mutli),constArg.at("BETA"));
                partial1.push_back(pair<NewDirectedEdge *, double>(nextEdge, answer1));
                    //cout<<nextEdge->edgeNum<<" "<<answer<<endl;
                sum1+=answer1;
                //}
                //else{
                //    double answer2 = pow(nextEdge->pheno,constArg.at("ALPHA"))*pow(constArg.at("Q_DIST")/nextEdge->edgeCost,constArg.at("BETA"));
                //    partial2.push_back(pair<NewDirectedEdge *, double>(nextEdge, answer2));
                    //cout<<nextEdge->edgeNum<<" "<<answer<<endl;
                //    sum2+=answer2;
                //}
            }
        }
    }
    //cout << "---" << endl;
    //getchar();
    double answer = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX/sum1));
    for (int edge=0;edge<partial1.size();edge++) {
        NewDirectedEdge *nextEdge =partial1[edge].first;
        double percentage = partial1[edge].second;
        if (percentage > answer) {
            return nextEdge;
        } else {
            answer -= percentage;
        }//按照各个边的概率选择下一条边
    }
    return nullptr;
}

NewDirectedEdge::NewDirectedEdge(int destNode, int edgeNum, int edgeCost, double pheno, vector<DirectedEdge*> &passedEdges, set<int> &passedNodes):DirectedEdge(destNode, edgeNum, edgeCost, pheno) {
    this->passedEdges = passedEdges;
    this->passedNodes = passedNodes;
}

int maxlen=0;//当前含V'节点最多的路径中的V'节点数

int Ant::countEdges() {
    int ans=0;
    for (int i=0; i<nodes;i++) {
        for (int j=0; j<newMap[i].size();j++) {
            if (newMap[i][j].pheno>0 && newMap[i][j].edgeNum<0) {
                ans++;
                //cout << i << "->" << newMap[i][j].destNode << ":" << newMap[i][j].edgeCost << endl;
            }
        }
    }
    return ans;
}

void Ant::travel() {
    //cout<<_position<<endl;
    if ((_position == destPosition)) {
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
            if(demandSet.find(_position)!=demandSet.end()){
                _tabuLists.insert(selectedEdge->destNode);
            }
            
            travel();
        }
    }
}

void Ant::update() {
    for (NewDirectedEdge* passEdge : _visitedEdge) {
        passEdge->pheno+=constArg.at("Q_PHENO")/_cost;
        //cout << passEdge->pheno<< endl;
        if(passEdge->pheno > maxPheno){
            passEdge->pheno = maxPheno;
        }
    }
}//迭代较优的路径更新信息素
void Ant::downdate() {
    for (NewDirectedEdge* passEdge: _visitedEdge) {
        passEdge->pheno *= constArg.at("REDUCE_PHENO");
        if(passEdge->edgeNum==-1){
            if(passEdge->pheno < minPheno){
                passEdge->pheno = minPheno;
            }
        }
        else{
            if(passEdge->pheno < 5){
                passEdge->pheno = 5;
            }
        }
    }
}
void Ant::releasePheno(){
    for (int i=0;i<newMap.size();i++) {
        for (NewDirectedEdge &edge : newMap[i]) {
            edge.pheno *= constArg.at("PHENO_DECREASE");
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
/*
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
*/
void timeOver(int x) {
    stopFlag = true;
    //throw TimeOut();//抛出异常
    //timeCount++;
    //cout << timeCount << "s has passed " << maxSize << endl;
}

void spfa(const int &sourceNode, vector<vector<DirectedEdge> > &adjMap, set<int> forbidLists) {
    //cout << "sourceNode:" << sourceNode << endl;
    if (forbidLists.find(sourceNode)!=forbidLists.end()) {
        return;
    }
    const int &INF=0x7FFFFFFF,&NODE=adjMap.size();//用邻接表的大小传递顶点个数，减少参数传递
    vector<int> destDist;
    destDist.assign(NODE,INF);//初始化距离为无穷大
    deque<int> que(1,sourceNode);//处理队列
    vector<int> precNode(601,-1);
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
            if (forbidLists.find(next)!=forbidLists.end()) {
                continue;
            }
            if(destDist[now]<INF&&//若距离已知（否则下面右式计算结果必爆int），且
               //注：与运算先判断左式是否成立，若不成立则右式不会被判断
               destDist[next]>destDist[now]+adjMap[now][i].edgeCost)//优于当前值
            {
                destDist[next]=destDist[now]+adjMap[now][i].edgeCost;//更新
                precNode[next]=now;
                precEdge[next]=&adjMap[now][i];
                //cout << adjMap[now][i].edgeNum << "-" << &adjMap[now][i] << endl;
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
            //cout << nodeJ << endl;
            beenTimes[nodeJ]++;
            passedNodes.insert(nodeJ);
            passedEdges.push_back(precEdge[nodeJ]);
            //cout << precEdge[nodeJ]->edgeNum << "-" << precEdge[nodeJ] << "-" << precEdge[nodeJ]->destNode << endl;
            cost+=precEdge[nodeJ]->edgeCost;
            nodeJ=precNode[nodeJ];
            if (nodeJ!=sourceNode && Ant::demandSet.find(nodeJ) != Ant::demandSet.end()) {
                cost=0;
                break;
            }
        }
        if (cost>0) {
            beenTimes[sourceNode]++;
            if (Ant::distance[sourceNode][nodeI]>0 && Ant::distance[sourceNode][nodeI]==cost) {
                continue;
            }
            Ant::distance[sourceNode][nodeI]=cost;
            if (forbidLists.size()==0) {
                NewDirectedEdge newEdge(nodeI, -1, cost, MUTLI_PHENO, passedEdges, passedNodes);
                Ant::newMap[sourceNode].push_back(newEdge);
            } else {
                NewDirectedEdge newEdge(nodeI, -2, cost, MUTLI_PHENO, passedEdges, passedNodes);
                Ant::newMap[sourceNode].push_back(newEdge);
                Ant::distance[sourceNode][nodeI]=-2;
            }
        }
    }
}

void getArgs(ifstream &fin,map<string, double> &seg) {
    string argName;
    while (fin >> argName) {
        double argValue;
        fin >> argValue;
        seg[argName] = argValue;
    }
}

void search_route(char *topo[5000], int edge_num, char *demand)
{
    constArg.clear();
    constArg["ALPHA"]=ALPHA;
    constArg["BETA"]=BETA;
    constArg["Q_PHENO"]=Q_PHENO;
    constArg["Q_DIST"]=Q_DIST;
    constArg["REDUCE_PHENO"]=REDUCE_PHENO;
    constArg["PHENO_DECREASE"]=PHENO_DECREASE;
    constArg["START_PHENO"]=START_PHENO;
    srand(time(0));//用当前时间值种随机数种子
    signal(SIGPROF, timeOver);//SIGALRM触发timeover函数
    struct itimerval tick;
    tick.it_value.tv_sec=9.7;
    tick.it_value.tv_usec=0;//it_value指定初始定时时间
    tick.it_interval.tv_sec=0;
    tick.it_interval.tv_usec=10000;//it_interval指定定时间
    cout << "search_route begin" << endl;
    vector<int> demandVec;
    strToVector(demand, ",|", &demandVec);//字符串转换成必经节点数组demandVec
    const int sourceNode = demandVec[0];
    const int destNode = demandVec[1];
    int count = 0;
    for (int i=1;i<demandVec.size();i++) {
        Ant::demandSet.insert(demandVec[i]);
    }
    for (int i=0;i<601;i++) {
        Ant::originMap[i].reserve(601);
    }
    for (int nodeI=0; nodeI<edge_num; nodeI++) {
        vector<int> topoVec;   
        strToVector(topo[nodeI], ",", &topoVec);
        set<int> passedNodes={topoVec[2]};
        DirectedEdge edgeNow(topoVec[2], topoVec[0], topoVec[3],0);
        Ant::originMap[topoVec[1]].push_back(edgeNow);
        DirectedEdge* place=&(*(--Ant::originMap[topoVec[1]].end()));
        vector<DirectedEdge*> passedEdges={place};
        NewDirectedEdge edgeNow2(topoVec[2],topoVec[0],topoVec[3],START_PHENO,passedEdges,passedNodes);
        Ant::newMap[topoVec[1]].push_back(edgeNow2);
        Ant::nodes=max(Ant::nodes,max(topoVec[1]+1,topoVec[2]+1));
        //cout << edgeNow2.passedEdges[0]->edgeNum << ":" << edgeNow2.passedEdges[0] << ":" << edgeNow2.passedEdges[0]->destNode << endl;
    }//字符串转换成临接表originMap
    //getchar();

    memset(Ant::distance,-1,sizeof(Ant::distance));
    beenTimes.assign(Ant::nodes,0);
    set<int> forbidLists = {};
    forbidLists.clear();
    for (auto nodeI : Ant::demandSet) {
        spfa(nodeI,Ant::originMap,forbidLists);
    }
    spfa(sourceNode,Ant::originMap,forbidLists);
    cout << "edges:" << Ant::countEdges() << endl;
    vector<pair<int,int> > beenSorted;
    beenSorted.clear();

    for (int i=0;i<Ant::nodes;i++) {
        if (beenTimes[i]>0) {
            beenSorted.push_back(pair<int,int>(i,beenTimes[i]));
            //cout << i << ":" << beenTimes[i] << endl;
        }
    }

    sort(beenSorted.begin(),beenSorted.end(),[](const pair<int,int> &x,const pair<int,int> &y) -> bool { return x.second > y.second; });
    for (int i=0; i<=min(5, static_cast<int>(beenSorted.size()*0.02)); i++) {
        forbidLists.insert(beenSorted[i].first);
    }
/*
    for (auto x : forbidLists) {
        cout << "forbid:" << x << endl;
    }
*/
    for (auto nodeI : Ant::demandSet) {
        spfa(nodeI,Ant::originMap,forbidLists);
    }
    spfa(sourceNode,Ant::originMap,forbidLists);
    cout << "edges:" << Ant::countEdges() << endl;

    for (int i=0;i<Ant::nodes;i++) {
        for (int j=0;j<Ant::newMap[i].size();j++) {
            auto edge=Ant::newMap[i][j];
            if (Ant::distance[i][edge.destNode]==-2 && edge.edgeNum!=-2) {
                Ant::newMap[i][j].pheno=0;
            }
        }
    }

    cout << "edges:" << Ant::countEdges() << endl;

    Ant::currentCost=99999;
    Ant::currentAnswer.clear();
    Ant::destPosition=destNode;
    vector<Ant> antTeam;
    try {
        int res=setitimer(ITIMER_PROF, &tick, NULL);//抛出SIGALRM信号
        if (res!=0) {
            cout << "set timer fail errno:" << errno << endl;
        }
        int round=0;
        while (true) {
            round++;
            Ant::calc=0;
            if (stopFlag) {
                break;
            }
            for (int i = 0; i < ANT_NUM; i++) {
                antTeam.push_back(Ant(sourceNode, i));
                //cout << "send a new ant" << endl;
                antTeam[i].travel();
                if (antTeam[i].arrived) {
                    Ant::calc++;
                }
            }//所有蚂蚁走一圈
            //for (int i = 0; i < ANT_NUM; i++) {
            //    if(antTeam[i]._tabuLists.size() > maxlen) {
            //        maxlen = antTeam[i]._tabuLists.size();
            //    }
            //}
            for (int i = 0; i < ANT_NUM; i++) {
                if(antTeam[i].arrived) {
                    antTeam[i].update();
                    //Ant::releasePheno();
                    //cout << count <<endl;
                } 
                //else {
                //   antTeam[i].downdate();
                //}
            }//到达最终节点的蚂蚁判断是否更新信息素
            //Ant::releasePheno();//信息素挥发
            //cout << Ant::calc << " ants arrived" << endl;
            //Ant::printPheno();//打印测试节点的信息素
            //sleep(1);
            antTeam.clear();
            //maxlen = 0;
            //count++;
            
        }
    } 
    catch (TimeOut &err) {
        cout << "over" << endl;
    }
    catch(exception& e)
    {
        cout << "over" << endl;
    }
    ofstream fout("./results");
    fout << Ant::currentCost << endl;
    if (Ant::currentAnswer.size()!=0) {
        for (auto &answerIter : Ant::currentAnswer) {
            reverse(answerIter->passedEdges.begin(),answerIter->passedEdges.end());
            for (auto answer2Iter : answerIter->passedEdges) {
                record_result(answer2Iter->edgeNum);
            }
        }
    }
}
