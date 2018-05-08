#include <cstdio>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>
#include <climits>
#include <chrono>

using namespace std;

#define TRIAL 30
#define GENERATION 500
#define POPULATION 300
#define CHILDREN 200

static vector<list<pair<int, int> > > NNlist;     // pair<cost, id>
static vector<vector<int> > cost;

class Next{
	
	public:
		int left;
		int right;

	public:
        Next& operator=(const Next& node){
            left = node.left;
            right = node.right;

            return *this;
        }
		bool operator==(const Next& node){
			return ((right == node.right && left == node.left) || (right == node.left && left == node.right));
		}
		bool operator!=(const Next& node){
            return (!((right == node.right && left == node.left) || (right == node.left && left == node.right)));
        }
};

class Point{
	
	public:
		int id;
		double x;
		double y;
		Next node;

	public:
		Point& operator=(const Point& pt){
            id = pt.id;
            x = pt.x;
            y = pt.y;
            node = pt.node;

            return *this;
        }

        bool operator==(const Point& pt){
            return (id == pt.id && node == pt.node);
        }

        bool operator!=(const Point& pt){
            return (node != pt.node);
        }

        bool operator<(const Point& pt){
            return id < pt.id;
        }

        bool operator>(const Point& pt){
            return id > pt.id;
        }
};

class TSP{

    public:
        int size;
        vector<Point> tour;

    public:
        TSP& operator=(const TSP& cycle){
            size = cycle.size;
            tour = cycle.tour;

            return *this;
        }

        bool operator==(const TSP& cycle){
			return equal(tour.begin(), tour.end(), cycle.tour.begin());
        }

		bool operator!=(const TSP& cycle){
			return !equal(tour.begin(), tour.end(), cycle.tour.begin());
        }

    public:
        // データ読み込み
        void input(char filename[]){
            int i;
            string str;

            tour.resize(size);

            ifstream file(filename);

            if(file.fail()){
                cerr << "File do not exist." << endl;
                exit(-1);
            }

            // 始め6行を捨てる
            for(i = 0; i < 6; i++){
                getline(file, str);
            }

            //ファイル読み込み
            for(i = 0; i < size; i++){
                getline(file, str);
                sscanf(str.data(), "%d %lf %lf", &tour[i].id,  &tour[i].x, &tour[i].y);
            }
        }

        // 2点間の距離
        int distance(Point &p1, Point &p2){
            double x = p1.x - p2.x;
            double y = p1.y - p2.y;
            return (int)(sqrt((x * x) + (y * y)) + 0.5);
        }

        // 近傍リストとエッジコストの計算
        void relation(){
            
            int i, j;
            int id;
            int dist;

            for(i = 1; i <= size; i++){
                for(j = i + 1; j <= size; j++){

                    dist = distance(tour[i - 1], tour[j - 1]);

                    id = j;
                    NNlist[i].push_back(make_pair(dist, id));
                    NNlist[j].push_back(make_pair(dist, i));

                    cost[i][j] = dist;
                    cost[j][i] = dist;
                }
                NNlist[i].sort();
            }
        }

        // 2-opt
        void t_opt(){
            int i, j, k;
            int depth, improve = 1;
            int p1, p2, p3, p4;
            int t3, t4;
            int tmp_distance, max_distance;
            list<pair<int, int> >::iterator list_itr;
        
            while(improve){
                improve = 0;
                for(i = 0; i < size; i++){
            
                    max_distance = 0;
            
                    p1 = i; p2 = (i + 1) % size;
            
                    for(list_itr = NNlist[tour[p1].id].begin(), depth = 0; depth < 20; list_itr++, depth++){
            
                        // t3を探す
                        for(j = 0; j < size; j++){
                            if(tour[j].id == list_itr->second){
                                t3 = j; t4 = (j + 1) % size;
                                break;
                            }
                        }

                        tmp_distance = cost[tour[p1].id][tour[p2].id] + cost[tour[t3].id][tour[t4].id] - cost[tour[p1].id][tour[t3].id] - cost[tour[p2].id][tour[t4].id];
                        if(max_distance < tmp_distance){
                            max_distance = tmp_distance;
                            p3 = t3;    p4 = t4;
                        }
                    }
            
                    // 改善があったなら逆位
                    if(max_distance > 0){
                        if(p2 < p3){
                            for(k = 0; k < (p3 - p2 + 1) / 2; k++){
                                swap(tour[p2 + k], tour[p3 - k]);
                            }   
                        }else{
                            for(k = 0; k < (size - (p2 - p3) + 1) / 2; k++){
                                swap(tour[(p2 + k) % size], tour[(p3 - k + size) % size]);
                            }
                        }
                        improve = 1;
                    }
                }
            }
        }

        // 全てidが同じ値かを調べる
        int fill(){
            for(int i = 0; i < size; i++){
                if(tour[i] != tour[(i + 1) % size]) return 0;
            }
            return 1;
        }
};

class Parent{

    public:
        TSP cycle;
        int fitness;

    public:
		Parent& operator=(const Parent& p){
			cycle = p.cycle;
			fitness = p.fitness;
		}

        bool operator==(const Parent& p){
			return cycle == p.cycle;
        }

		bool operator!=(const Parent& p){
			return cycle != p.cycle;
        }

        bool operator<(const Parent& p){
			return fitness < p.fitness;
		}

		bool operator>(const Parent& p){
			return fitness > p.fitness;
		}

    public:
        // 初期化
        void Init(const TSP &data){

			int i;

            cycle = data;
			random_shuffle(cycle.tour.begin(), cycle.tour.end());
			cycle.t_opt();
			for(i = 0; i < cycle.size; i++){
				cycle.tour[i].node.left = cycle.tour[(i + cycle.size - 1) % cycle.size].id;
				cycle.tour[i].node.right = cycle.tour[(i + 1) % cycle.size].id;
			}
			sort(cycle.tour.begin(), cycle.tour.end());
			Evaluate();
		}

        // 評価値(一巡する経路長)
        int Evaluate(){
			fitness = 0;
			for(int i = 0; i < cycle.size; i++){
				fitness += cost[cycle.tour[i].id][cycle.tour[i].node.left];
				fitness += cost[cycle.tour[i].id][cycle.tour[i].node.right];
			}
			fitness /= 2;
		}

        // 島に分ける
        void Island(vector<TSP> &Eset){

            int i, j, k, num;
            int prev, current, next;
            vector<TSP> Rset;

            if(Eset.size() == 0) return;

            Rset = Eset;

            random_shuffle(Rset.begin(), Rset.end());
            num = (rand() % Rset.size()) + 1;
            Rset.resize(num);


            for(i = 0; i < num; i++){

                // ABcycleの親Aのエッジ処理
                for(j = 0; j < Rset[i].size; j += 2){
                    prev = Rset[i].tour[(j - 1 + Rset[i].size) % Rset[i].size].id;
                    current = Rset[i].tour[j].id;
                    next = Rset[i].tour[j + 1].id;

                    if(cycle.tour[current - 1].node.left == next){
                        cycle.tour[current - 1].node.left = prev;
                    }else{
                        cycle.tour[current - 1].node.right = prev;
                    }
                }

                // ABcycleの親Bのエッジ処理
                for(j = 1; j < Rset[i].size; j += 2){
                    prev = Rset[i].tour[j - 1].id;
                    current = Rset[i].tour[j].id;
                    next = Rset[i].tour[(j + 1) % Rset[i].size].id;

                    if(cycle.tour[current - 1].node.left == prev){
                        cycle.tour[current - 1].node.left = next;
                    }else{
                        cycle.tour[current - 1].node.right = next;
                    }
                }
            }
        }

        // エッジとエッジの評価をする
        void edge_distance(int &t1, int &t2, int &t3, int &t4, int &p1, int &p2, int &p3, int &p4, int &min_distance){

            int e1, e2, e3, e4;

            e1 = cost[t1][t2], e2 = cost[t3][t4];
            e3 = cost[t1][t3], e4 = cost[t2][t4];

            if(min_distance > (- e1 - e2 + e3 + e4)){
                p1 = t1;
                p2 = t2;
                p3 = t3;
                p4 = t4;
                min_distance = (- e1 - e2 + e3 + e4);
            }

            e3 = cost[t1][t4];
            e4 = cost[t2][t3];

            if(min_distance > (- e1 - e2 + e3 + e4)){
                p1 = t1;
                p2 = t2;
                p3 = t4;
                p4 = t3;
                min_distance = (- e1 - e2 + e3 + e4);
            }
        }

        // 島の接続
        void Conect(){
            
            int i;
            int sum, size, num, target;
            int prev, current;
            vector<int> numbering(cycle.size, 0);     //島に番号をつける
            vector<pair<int, pair<int, int> > > mapping;    //島のsizeとnumberとその島に含まれるノード1つを登録

            //島のナンバリングと大きさ, ノードなどの登録
            sum = 0, size = 0, num = 1;
            prev = 0, current = 1, target = current;
            
            while(true){
                if(numbering[current - 1] == 0){
                    numbering[current - 1] = num;
                    size++;
                }else{
                    mapping.push_back(make_pair(size, make_pair(num, target)));
                    num++;
                    sum += size;
                    if(sum == cycle.size) break;
                    size = 0;
                    prev = 0;
                    for(i = 0; i < cycle.size; i++){
                        if(numbering[i] == 0){
                            current = i + 1;
                            target = current;
                            break;
                        }
                    }
                }

                if(prev != cycle.tour[current - 1].node.left){
                    prev = current;
                    current = cycle.tour[current - 1].node.left;
                }else{
                    prev = current;			
                    current = cycle.tour[current - 1].node.right;			
                }
            }

            int t1, t2, t3, t4;
            int p1, p2, p3, p4;
            int min_distance, depth = 20;
            list<pair<int, int> >::iterator list_itr;

            // 最小の島のエッジと最も評価が良くなるエッジを探す
            while(mapping.size() != 1){

                sort(mapping.begin(), mapping.end());

                min_distance = INT_MAX;
                prev = 0;
                current = mapping[0].second.second;

                for(i = 0; i < mapping[0].first; i++){
                    for(list_itr = NNlist[current].begin(), depth = 0; depth < 20; list_itr++, depth++){

                        // 近傍のノードが自身の島だった場合
                        if(numbering[current - 1] == numbering[list_itr->second - 1]) continue;

                        // min->left, near->left
                        t1 = current, t2 = cycle.tour[t1 - 1].node.left;
	    		    	t3 = list_itr->second, t4 = cycle.tour[t3 - 1].node.left;
                        edge_distance(t1, t2, t3, t4, p1, p2, p3, p4, min_distance);

                        // min->left, near->right
                        t1 = current, t2 = cycle.tour[t1 - 1].node.left;
	    		    	t3 = list_itr->second, t4 = cycle.tour[t3 - 1].node.right;
                        edge_distance(t1, t2, t3, t4, p1, p2, p3, p4, min_distance);
                        
                        // min->right, near->left
                        t1 = current, t2 = cycle.tour[t1 - 1].node.right;
	    		    	t3 = list_itr->second, t4 = cycle.tour[t3 - 1].node.left;
                        edge_distance(t1, t2, t3, t4, p1, p2, p3, p4, min_distance);

                        // min->right, near->right
                        t1 = current, t2 = cycle.tour[t1 - 1].node.right;
	    		    	t3 = list_itr->second, t4 = cycle.tour[t3 - 1].node.right;
                        edge_distance(t1, t2, t3, t4, p1, p2, p3, p4, min_distance);
                    }
                    if(prev != cycle.tour[current - 1].node.left){
                        prev = current;
                        current = cycle.tour[current - 1].node.left;
                    }else{
                        prev = current;			
                        current = cycle.tour[current - 1].node.right;			
                    }
                }

                // 繋ぎ変え
                if(cycle.tour[p1 - 1].node.left == p2){
                    cycle.tour[p1 - 1].node.left = p3;
                }else{
                    cycle.tour[p1 - 1].node.right = p3;
                }

                if(cycle.tour[p2 - 1].node.left == p1){
                    cycle.tour[p2 - 1].node.left = p4;
                }else{
                    cycle.tour[p2 - 1].node.right = p4;
                }

                if(cycle.tour[p3 - 1].node.left == p4){
                    cycle.tour[p3 - 1].node.left = p1;
                }else{
                    cycle.tour[p3 - 1].node.right = p1;
                }

                if(cycle.tour[p4 - 1].node.left == p3){
                    cycle.tour[p4 - 1].node.left = p2;
                }else{
                    cycle.tour[p4 - 1].node.right = p2;
                }

                // 島の大きさを接続先の島に増やして, 先頭の島(最小の島)を消す
                for(i = 1; i < mapping.size(); i++){
                    if(numbering[p3 - 1] == mapping[i].second.first){
                        mapping[i].first += mapping[0].first;
                    }
                }

                int island_num = numbering[p1 - 1];
                for(i = 0; i < cycle.size; i++){
                    if(numbering[i] == island_num){
                        numbering[i] = numbering[p3 - 1];
                    }
                }
                mapping.erase(mapping.begin());	
            }
        }

        // 親の更新
        void Update(const vector<Parent> &children){
            int i, min, child_distance = INT_MAX;
            for(i = 0; i < CHILDREN; i++){
                if(child_distance > children[i].fitness){
                    child_distance = children[i].fitness;
                    min = i;
                }
            }

            if(fitness > child_distance){
                fitness = children[min].fitness;
                cycle = children[min].cycle;
            }
        }
};

// ABcycleが出来ているか判定
void judge_ABcycle(TSP &trace, vector<TSP> &Eset, int &group){

    int i, end = trace.size - 1;
    TSP ABcycle;

    // ABcycleが作られている場合それを取り出す
    for(i = 0; i < end; i++){
        if(trace.tour[i].id == trace.tour[end].id && (end - i) % 2 == 0){

            ABcycle.tour.insert(ABcycle.tour.begin(), trace.tour.begin() + i + 1, trace.tour.end());

            // ABcycleが線になるもの以外を登録
            if((end - i) != 2){
                // 先頭が親Aになるように回す
                if(group == 0){
                    ABcycle.tour.push_back(ABcycle.tour[0]);
                    ABcycle.tour.erase(ABcycle.tour.begin());
                }
                ABcycle.size = end - i;
                Eset.push_back(ABcycle);
            }

            trace.tour.erase(trace.tour.begin() + i + 1, trace.tour.end());
            trace.size = i + 1;
            break;
        }
    }

}

// ABcycleで繋ぐエッジを探す
void select_edge(TSP &trace, TSP &stock, int &current, int &next){
   
    int side = rand() % 2;

    // currentノードに繋がるnextノードをstockAから探す
    if(side == 0){
        if(stock.tour[current - 1].node.left != 0){
            next = stock.tour[current - 1].node.left;
        }else{
            next = stock.tour[current - 1].node.right;                        
        }
    }else{
        if(stock.tour[current - 1].node.right != 0){
            next = stock.tour[current - 1].node.right;
        }else{
            next = stock.tour[current - 1].node.left;                        
        }
    }

    // ノードを追加する
    trace.tour.push_back(stock.tour[next - 1]);

    // 見つけたnodeの値を0にして消去
    if(stock.tour[current - 1].node.left == next){
        stock.tour[current - 1].node.left = 0;
    }else{
        stock.tour[current - 1].node.right = 0;
    }
    if(stock.tour[next - 1].node.left == current){
        stock.tour[next - 1].node.left = 0;
    }else{
        stock.tour[next - 1].node.right = 0;                    
    }

    // 隣接するノードが両方なくなっていればそこのidを0にする
    if(stock.tour[current - 1].node.left == stock.tour[current - 1].node.right) stock.tour[current - 1].id = 0;
    if(stock.tour[next - 1].node.left == stock.tour[next - 1].node.right) stock.tour[next - 1].id = 0;
}

// ABcycleの生成
void ABcycle(Parent &parentA, Parent &parentB, vector<TSP> &Eset){

    TSP stockA, stockB, trace;
    int group, side, current, next;   // prev, current, nextにはidを登録

    Eset.clear();

    if(parentA == parentB) return;

    stockA = parentA.cycle;
    stockB = parentB.cycle;

    while(!stockA.fill()){

        // 初期ノードの選択
        do{
            current = (rand() % stockA.size) + 1;
        }while(stockA.tour[current - 1].id == 0);

        trace.tour.clear();
        trace.size = 0;
        trace.tour.push_back(stockA.tour[current - 1]);
        trace.size++;
        group = 0;

        do{

            // 親Aのエッジを取り出す
            if(group == 0){
                select_edge(trace, stockA, current, next);
                group = 1;
            }else{
                select_edge(trace, stockB, current, next);
                group = 0;
            }

            trace.size++;

            // ABcycleが作られているかの判定
            judge_ABcycle(trace, Eset, group);

            current = next;
        }while(trace.size != 1);
    }
}

// 交叉(EdgeAssemblyCrossover)
void EAX(Parent &depos, Parent &parentA, Parent &parentB){

    int i;

    vector<TSP> Eset;
    vector<Parent> children(CHILDREN, parentA);

    ABcycle(parentA, parentB, Eset);
    
    for(i = 0; i < CHILDREN; i++){

        children[i].Island(Eset);

        children[i].Conect();

        children[i].Evaluate();
    } 
    depos.Update(children);
}

// 母集団全体の調査
void Analyze(const vector<Parent> &parent, int &min, double &ave){

	int i, j;
    int prev, current;

	min = INT_MAX;
	ave = 0;
	for(i = 0; i < POPULATION; i++){
		ave += parent[i].fitness;
		if(min > parent[i].fitness) min = parent[i].fitness;
        if(min == 6773){
            FILE *fp;
            fp = fopen("rat575opttour.txt", "w");
            current = 1;
            for(j = 0; j < parent[i].cycle.size; j++){
                fprintf(fp, "%d\n", parent[i].cycle.tour[current - 1].id);
                if(prev != parent[i].cycle.tour[current - 1].node.left){
                    prev = current;
                    current = parent[i].cycle.tour[current - 1].node.left;
                }else{
                    prev = current;
                    current = parent[i].cycle.tour[current - 1].node.right;
                }

            }
            fclose(fp);
        }
	}
	ave /= POPULATION;
}

// エッジの差を数える
int Ediff(vector<vector<int> > &edge_diff, Parent &parentA, Parent &parentB){
    
    int i, j, count = 0;

    edge_diff.clear();
    edge_diff.resize(parentA.cycle.size + 1);
    for(i = 0; i < parentA.cycle.size + 1; i++){
        edge_diff[i].resize(parentA.cycle.size + 1);
    }

    for(i = 0; i < parentA.cycle.size; i++){
        if(edge_diff[parentA.cycle.tour[i].id][parentA.cycle.tour[i].node.left] == 0){
            edge_diff[parentA.cycle.tour[i].id][parentA.cycle.tour[i].node.left] = 1;
            edge_diff[parentA.cycle.tour[i].node.left][parentA.cycle.tour[i].id] = 1;
            count++;
        }
        if(edge_diff[parentA.cycle.tour[i].id][parentA.cycle.tour[i].node.right] == 0){
            edge_diff[parentA.cycle.tour[i].id][parentA.cycle.tour[i].node.right] = 1;
            edge_diff[parentA.cycle.tour[i].node.right][parentA.cycle.tour[i].id] = 1;
            count++;
        }
        if(edge_diff[parentB.cycle.tour[i].id][parentB.cycle.tour[i].node.left] == 0){
            edge_diff[parentB.cycle.tour[i].id][parentB.cycle.tour[i].node.left] = 1;
            edge_diff[parentB.cycle.tour[i].node.left][parentB.cycle.tour[i].id] = 1;
            count++;
        }
        if(edge_diff[parentB.cycle.tour[i].id][parentB.cycle.tour[i].node.right] == 0){
            edge_diff[parentB.cycle.tour[i].id][parentB.cycle.tour[i].node.right] = 1;
            edge_diff[parentB.cycle.tour[i].node.right][parentB.cycle.tour[i].id] = 1;
            count++;
        }
    }
    return count - parentA.cycle.size;  
}

// エッジ差が最小(0を除く)になる個体を返す
Parent min_Ediff(Parent &parentA, vector<Parent> &parent, vector<vector<int> > &edge_diff, vector<int> &diff){

    int i, j, count, min = INT_MAX, index = 0;
    for(i = 0; i < POPULATION; i++){
        count = 0;
        edge_diff.clear();
        edge_diff.resize(parentA.cycle.size + 1);
        for(j = 0; j < parentA.cycle.size + 1; j++){
            edge_diff[j].resize(parentA.cycle.size + 1);
        }

        for(j = 0; j < parentA.cycle.size; j++){
            if(edge_diff[parentA.cycle.tour[j].id][parentA.cycle.tour[j].node.left] == 0){
                edge_diff[parentA.cycle.tour[j].id][parentA.cycle.tour[j].node.left] = 1;
                edge_diff[parentA.cycle.tour[j].node.left][parentA.cycle.tour[j].id] = 1;
                count++;
            }
            if(edge_diff[parentA.cycle.tour[j].id][parentA.cycle.tour[j].node.right] == 0){
                edge_diff[parentA.cycle.tour[j].id][parentA.cycle.tour[j].node.right] = 1;
                edge_diff[parentA.cycle.tour[j].node.right][parentA.cycle.tour[j].id] = 1;
                count++;
            }
            if(edge_diff[parent[i].cycle.tour[j].id][parent[i].cycle.tour[j].node.left] == 0){
                edge_diff[parent[i].cycle.tour[j].id][parent[i].cycle.tour[j].node.left] = 1;
                edge_diff[parent[i].cycle.tour[j].node.left][parent[i].cycle.tour[j].id] = 1;
                count++;
            }
            if(edge_diff[parent[i].cycle.tour[j].id][parent[i].cycle.tour[j].node.right] == 0){
                edge_diff[parent[i].cycle.tour[j].id][parent[i].cycle.tour[j].node.right] = 1;
                edge_diff[parent[i].cycle.tour[j].node.right][parent[i].cycle.tour[j].id] = 1;
                count++;
            }
        }
        if(min > count && count != 0){
            max = count;
            index = i;
        }
    }

    diff.push_back(count - parentA.cycle.size);
    return parent[index];
}

// 平均, 分散の計算
void calc(vector<int> &diff, double &eave, double &evar){

    int i;
    eave = 0, evar = 0;
    
    for(i = 0; i < diff.size(); i++){
        eave += diff[i];
    }
    eave /= diff.size();

    for(i = 0; i < diff.size(); i++){
        evar += ((double)diff[i] - eave) * ((double)diff[i] - eave);
    }
    evar /= diff.size();
}

int main(int argc, char* argv[]){

    srand(0);

    if(argc != 2){
		cout << "input filename!" << endl;
		exit(-1);
	}

    char filename[256];
	strcpy(filename, argv[1]);
	
	char type[256];
	int size;
	sscanf(argv[1], "%[^0123456789]%d.tsp", type, &size);

	FILE *fp;
	char str[256];
	sprintf(str, "./%s%dresult/fast.txt", type, size);
	fp = fopen(str, "w");

	cout << "dataset    : " << type << size << endl;
	cout << "trial      : " << TRIAL << endl;
	cout << "generation : " << GENERATION << endl;
	cout << "population : " << POPULATION << endl;
	cout << "children   : " << CHILDREN << endl;

	fprintf(fp, "dataset       : %s%d\n", type, size);
	fprintf(fp, "trial         : %d\n", TRIAL);
	fprintf(fp, "generation    : %d\n", GENERATION);
	fprintf(fp, "population    : %d\n", POPULATION);
	fprintf(fp, "children      : %d\n", CHILDREN);

    TSP data;
    data.size = size;
    data.input(filename);

    // 近傍リストとエッジ距離を求める
    NNlist.resize(size + 1);
    cost.resize(size + 1, vector<int>(size + 1));
    data.relation();

    // エッジの差を測る
    vector<vector<int> > edge_diff(size + 1, vector<int>(size + 1));
    vector<int> diff;
    double eave, evar;

    int i, j, k;
    int min;
    double ave;
	double gene_ave = 0;
	
	chrono::system_clock::time_point  start, end;
	start = chrono::system_clock::now();

    for(i = 0; i < TRIAL; i++){

        cout << "trial..." << i + 1 << endl;
        cout << "Gene" << '\t' << "min" << '\t' << "ave" << '\t' << "Eave" << '\t' << "Evar" << endl;
		fprintf(fp, "trial...%d\n", i + 1);
		fprintf(fp, "Gene\tmin\tave\tEave\tEvar\n");

        // 母集団生成
        vector<Parent> parent(POPULATION);
        vector<Parent> depos(POPULATION);
		for(j = 0; j < POPULATION; j++){
			parent[j].Init(data);
		}

        for(j = 0; j < GENERATION; j++){

            depos = parent;
            diff.clear();

            for(k = 0; k < POPULATION; k++){

                Parent parentA, parentB;

                parentA = parent[k];
                parentB = max_Ediff(parentA, parent, edge_diff, diff);

                // diff.push_back(Ediff(edge_diff, parentA, parentB));

                // 交叉
                EAX(depos[k], parentA, parentB);
            }

            parent = depos;

            Analyze(parent, min, ave);
            calc(diff, eave, evar);

			cout << j + 1 << '\t' << min << '\t' << ave << '\t' << eave << '\t' << evar << endl;	
			fprintf(fp, "%d\t%d\t%lf\t%lf\t%lf\n", j + 1, min, ave, eave, evar);
			
			if(min == 6773){
				gene_ave += (j + 1);
				break;
			}

            random_shuffle(parent.begin(), parent.end());
        }
    }
	end = chrono::system_clock::now();  // 計測終了時間
	double elapsed = chrono::duration_cast<chrono::seconds>(end - start).count();
	cout << elapsed << endl;

	fprintf(fp, "time : %lf\n", elapsed);
	fprintf(fp, "convergence generation : %lf\n", gene_ave / TRIAL);

	fclose(fp);

    return 0;
}