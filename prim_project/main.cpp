#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>
#include <limits>
#include <queue>
#include <cmath>
#include <utility>

using namespace ::std;

//each node and its info

//there were global variables
vector<int> cost;

struct comparator {
    bool operator()(const pair<int, int> x, const pair<int, int> y) {
        return x.second > y.second;
    }

};

template<class Container>

void split1(const string &str, Container &cont) {
    istringstream iss(str);
    copy(istream_iterator<string>(iss),
         istream_iterator<string>(),
         back_inserter(cont));
}

class MyGraph {

public:

    int N = 0;
    int M = 0;
    const int _MAX = numeric_limits<int>::max();

    vector<vector<int>> adjList;
    vector<vector<int>> query;

    vector<int> cost;
    vector<int> height;
    vector<int> parent;
    vector<vector<int>> mst_parent;
    vector<vector<int>> mst_height;
    vector<vector<int>> mstList;
    vector<bool> isFixed;
    vector<int> depth;


public:

    void initGraph(string _path) {
        ios_base::sync_with_stdio(false);
        ifstream infile(_path);

        //read row and column quantity

        string line = "";
        getline(infile, line);
        vector<string> input;
        split1(line, input);

        M = stoi(input[0]);
        N = stoi(input[1]);

        adjList = vector<vector<int>>((M * N) + 1);
        height = vector<int>((M * N) + 1);
        cost = vector<int>((M * N) + 1);
        parent = vector<int>((M * N) + 1);
        isFixed = vector<bool>((M * N) + 1);
        depth = vector<int>((M * N) + 1);
        mstList = vector<vector<int>>(M * N + 1);//mstList array for each node
        mst_height = vector<vector<int>>(M * N + 1);
        mst_parent = vector<vector<int>>(M * N + 1);


        for (int i = 1; i <= M * N; i++) {
            isFixed[i] = false;
            cost[i] = _MAX;
            parent[i] = -1;
            depth[i] = 0;

        }

        vector<string> words;
        //initing height
        for (int i = 1; i <= M; i++) {
            words.clear();
            getline(infile, line);
            split1(line, words);

            for (int j = 1; j <= N; j++) {
                height[(i - 1) * N + j] = stoi(words[j - 1]);
            }
        }
        input.clear();

        //initing query
        getline(infile, line);
        split1(line, input);
        int queryNum = stoi(input[0]);
        query = vector<vector<int>>(queryNum);

        for (int i = 0; i < queryNum; i++) {
            input.clear();
            getline(infile, line);
            split1(line, input);
            query[i].push_back(stoi(input[0]));
            query[i].push_back(stoi(input[1]));
            query[i].push_back(stoi(input[2]));
            query[i].push_back(stoi(input[3]));
        }
        //initializing graph
        for (int i = 1; i <= M; i++) {
            for (int j = 1; j <= N; j++) {
                int myIndex = N * (i - 1) + j;

                if (j + 1 <= N) {
                    adjList[myIndex].push_back(myIndex + 1);
                    adjList[myIndex + 1].push_back(myIndex);
                }
                if (i + 1 <= M) {
                    adjList[myIndex].push_back((myIndex + N));
                    adjList[N * i + j].push_back(myIndex);
                }
            }
        }
        prim();
        //doing precomputing stuff
        createMSTList();

        dfs(1);

        infile.close();
    }

    void prim() {
        priority_queue<pair<int, int>, vector<pair<int, int>>, comparator> pq;
        cost[1] = 0;
        depth[1] = 0;
        pair<int, int> p(1, cost[1]);
        pq.push(p);

        while (!pq.empty()) {
            pair<int, int> v = pq.top();
            pq.pop();
            if (!isFixed[v.first]) {
                isFixed[v.first] = true;

                for (int i = 0; i < adjList[v.first].size(); i++) {
                    int w = adjList[v.first][i];
                    if (!isFixed[w]) {
                        int weight = abs(height[v.first] - height[w]);

                        if (weight < cost[w]) {
                            cost[w] = weight;
                            parent[w] = v.first;
                            depth[w] = depth[v.first] + 1;
                            pair<int, int> wPair(w, cost[w]);
                            pq.push(wPair);


                        }
                    }
                }
            }
        }
    }

    void dfs(int v) {
        int d = depth[v] + 1;
        if (!mstList[v].empty()) {
            for (int w : mstList[v]) {
                mst_parent[w].push_back(v);
                mst_height[w].push_back(cost[w]);

                int i = 0;
                int k = d;
                while (k > 1) {
                    k /= 2;

                    int par = mst_parent[w][i];
                    mst_parent[w].push_back(mst_parent[par][i]);
                    mst_height[w].push_back(max(mst_height[w][i], mst_height[par][i]));

                    i++;
                }
                dfs(w);
            }
        }
        else {

        }
    }

    void createMSTList() {
        for (int i = 2; i <= M * N; i++) {
            if (parent[i] != -1)
                mstList[parent[i]].push_back(i);
        }
    }

    int get_LCA(int sx, int sy, int tx, int ty) {
        int s = (sx - 1) * N + sy;
        int t = (tx - 1) * N + ty;
        int lowD, lowIndex, highD, highIndex;
        int level = (int) log2(M * N);

        if (depth[s] < depth[t]) {
            //lowD = depth[t];
            lowIndex = t;
            highD = depth[s];
            highIndex = s;
        } else {
            //lowD = depth[s];
            lowIndex = s;
            highD = depth[t];
            highIndex = t;
        }
        int diffDepth = depth[lowIndex] - depth[highIndex];
        int maximum = 0;
        int tempmaximum = 0;
        while (diffDepth > 0) {
            int x = diffDepth;
            int count = 0;
            while (x > 0) {
                x /= 2;
                count++;
            }
            diffDepth -= (int) pow(2, count - 1);
            maximum = max(maximum, mst_height[lowIndex][count - 1]);

            if (count >= 0)
                lowIndex = mst_parent[lowIndex][count - 1];
        }
        if (lowIndex == highIndex) {
            return maximum;
        }
        while (lowIndex != highIndex) {
            int count = 0;
            while (highD >= pow(2, count) && mst_parent[lowIndex][count] != mst_parent[highIndex][count]) {
                count++;
            }
            if(count!=0)
                count--;
            highD -= pow(2, count);

            tempmaximum = max(mst_height[highIndex][count], mst_height[lowIndex][count]);
            maximum = max(maximum, tempmaximum);

            lowIndex = mst_parent[lowIndex][count];
            highIndex = mst_parent[highIndex][count];
        }

        return max(maximum, tempmaximum);
    }

    MyGraph(string _path) {
        initGraph(_path);
    }

};

int main(int argc, char *argv[]) {


    if (argc != 3) {
        cout << "no input" << endl;
        return 1;
    }
    ofstream output;

    string path = argv[1];

    MyGraph g(path);


    ofstream myFile(argv[2]);
    for (int x = 0; x < g.query.size(); x++) {
        myFile << g.get_LCA(g.query[x][0], g.query[x][1], g.query[x][2], g.query[x][3]) << endl;
    }
    /*int x = 0;
    int i1 = 0;
    int i2 = 0;
    ifstream in1("output.txt");
    ifstream in2(".txt");
    for(int i = 1; i < g.query.size(); i++) {
        in1 >> i1;
        in2 >> i2;
        if(i1!=i2) {
            x++;
        }
    }*/
    myFile.close();
    return 0;
}