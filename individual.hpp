#ifndef individual_hpp
#define individual_hpp

#include <climits>
#include <algorithm>

#include "Tour.hpp"

class Individual{

public:
    Tour cycle;
    int fitness;

public:
    Individual& operator=(const Individual& indiv){
        cycle = indiv.cycle;
        fitness = indiv.fitness;
        return *this;
    }

    bool operator==(const Individual& indiv)const{
        return cycle == indiv.cycle;
    }

    bool operator!=(const Individual& indiv)const{
        return cycle != indiv.cycle;
    }

    bool operator<(const Individual& indiv)const{
        return fitness < indiv.fitness;
    }

    bool operator>(const Individual& indiv)const{
        return fitness > indiv.fitness;
    }

public:
    void evaluate(const std::vector<std::vector<int> >& cost){
        fitness = 0;
        for(int i = 0; i < cycle.size; i++){
            fitness += cost[cycle.tour[i].id][cycle.tour[i].left];
            fitness += cost[cycle.tour[i].id][cycle.tour[i].right];
        }
        fitness /= 2;
    }

    void init(const Tour& data, std::vector<std::vector<int> >& cost, std::vector<std::list<std::pair<int, int> > >& NNlist){

        cycle = data;
        std::shuffle(cycle.tour.begin(), cycle.tour.end(), mt);
        cycle.t_opt(cost, NNlist);
        for(int i = 0; i < cycle.size; i++){
            cycle.tour[i].left = cycle.tour[(i + cycle.size - 1) % cycle.size].id;
            cycle.tour[i].right = cycle.tour[(i + 1) % cycle.size].id;
        }
        std::sort(cycle.tour.begin(), cycle.tour.end());
        evaluate(cost);
    }

    void divide_tour(std::vector<Tour> Eset){

        if(Eset.size() == 0) return;

        std::uniform_int_distribution<> rand(1, Eset.size());
        std::shuffle(Eset.begin(), Eset.end(), mt);
        int num = rand(mt);
        Eset.resize(num);

        for(int i = 0; i < num; i++){

            int prev, current, next;

            // ABcycleの親Aのエッジ処理
            for(int j = 0; j < Eset[i].size; j += 2){
                prev = Eset[i].tour[(j - 1 + Eset[i].size) % Eset[i].size].id;
                current = Eset[i].tour[j].id;
                next = Eset[i].tour[j + 1].id;

                if(cycle.tour[current - 1].left == next){
                    cycle.tour[current - 1].left = prev;
                }else{
                    cycle.tour[current - 1].right = prev;
                }
            }

            // ABcycleの親Bのエッジ処理
            for(int j = 1; j < Eset[i].size; j += 2){
                prev = Eset[i].tour[j - 1].id;
                current = Eset[i].tour[j].id;
                next = Eset[i].tour[(j + 1) % Eset[i].size].id;

                if(cycle.tour[current - 1].left == prev){
                    cycle.tour[current - 1].left = next;
                }else{
                    cycle.tour[current - 1].right = next;
                }
            }
        }
    }

    // エッジとエッジを切り替えるかの評価をする
    void switch_edge_evaluate(int t1, int t2, int t3, int t4, int& p1, int& p2, int& p3, int& p4, int& min_distance, const std::vector<std::vector<int> >& cost){

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

    void conect_subtour(const std::vector<std::vector<int> >& cost, const std::vector<std::list<std::pair<int, int> > >& NNlist){
            
        std::vector<int> numbering(cycle.size, 0);     // 島に番号をつける
        std::vector<std::pair<int, std::pair<int, int> > > mapping;    // 島のsizeとnumberとその島に含まれるノード1つを登録

        //島のナンバリングと大きさ, ノードなどの登録
        int sum = 0, size = 0, num = 1;
        int prev = 0, current = 1, target = current;
        
        while(true){
            if(numbering[current - 1] == 0){
                numbering[current - 1] = num;
                size++;
            }else{
                mapping.push_back(std::make_pair(size, std::make_pair(num, target)));
                num++;
                sum += size;
                if(sum == cycle.size) break;
                size = 0;
                prev = 0;
                for(int i = 0; i < cycle.size; i++){
                    if(numbering[i] == 0){
                        current = i + 1;
                        target = current;
                        break;
                    }
                }
            }

            if(prev != cycle.tour[current - 1].left){
                prev = current;
                current = cycle.tour[current - 1].left;
            }else{
                prev = current;			
                current = cycle.tour[current - 1].right;			
            }
        }

        // 最小の島のエッジと最も評価が良くなるエッジを探す
        while(mapping.size() != 1){

            int t1, t2, t3, t4;
            int p1, p2, p3, p4;
            int min_distance;

            std::sort(mapping.begin(), mapping.end());

            min_distance = INT_MAX;
            prev = 0;
            current = mapping[0].second.second;
            
            for(int i = 0; i < mapping[0].first; i++){
                int depth = 0;
                auto list_itr = NNlist[current].begin();
                while(depth < DEPTH){

                    // 近傍のノードが自身の島だった場合
                    if(numbering[current - 1] == numbering[list_itr->second - 1]){
                        depth++;
                        list_itr++;
                        continue;
                    }

                    // min->left, near->left
                    t1 = current, t2 = cycle.tour[t1 - 1].left;
                    t3 = list_itr->second, t4 = cycle.tour[t3 - 1].left;
                    switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost);

                    // min->left, near->right
                    t1 = current, t2 = cycle.tour[t1 - 1].left;
                    t3 = list_itr->second, t4 = cycle.tour[t3 - 1].right;
                    switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost);
                    
                    // min->right, near->left
                    t1 = current, t2 = cycle.tour[t1 - 1].right;
                    t3 = list_itr->second, t4 = cycle.tour[t3 - 1].left;
                    switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost);

                    // min->right, near->right
                    t1 = current, t2 = cycle.tour[t1 - 1].right;
                    t3 = list_itr->second, t4 = cycle.tour[t3 - 1].right;
                    switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost);

                    depth++;
                    list_itr++;
                }

                if(prev != cycle.tour[current - 1].left){
                    prev = current;
                    current = cycle.tour[current - 1].left;
                }else{
                    prev = current;			
                    current = cycle.tour[current - 1].right;			
                }
            }

            // 繋ぎ変え
            if(cycle.tour[p1 - 1].left == p2){
                cycle.tour[p1 - 1].left = p3;
            }else{
                cycle.tour[p1 - 1].right = p3;
            }

            if(cycle.tour[p2 - 1].left == p1){
                cycle.tour[p2 - 1].left = p4;
            }else{
                cycle.tour[p2 - 1].right = p4;
            }

            if(cycle.tour[p3 - 1].left == p4){
                cycle.tour[p3 - 1].left = p1;
            }else{
                cycle.tour[p3 - 1].right = p1;
            }

            if(cycle.tour[p4 - 1].left == p3){
                cycle.tour[p4 - 1].left = p2;
            }else{
                cycle.tour[p4 - 1].right = p2;
            }

            // 島の大きさを接続先の島に増やして, 先頭の島(最小の島)を消す
            for(int i = 1; i < mapping.size(); i++){
                if(numbering[p3 - 1] == mapping[i].second.first){
                    mapping[i].first += mapping[0].first;
                }
            }

            int island_num = numbering[p1 - 1];
            for(int i = 0; i < cycle.size; i++){
                if(numbering[i] == island_num){
                    numbering[i] = numbering[p3 - 1];
                }
            }
            mapping.erase(mapping.begin());	
        }
    }

    void output(){
        for(int i = 0; i < cycle.size; i++) std::cout << cycle.tour[i].left << " ";
        std::cout << std::endl;
        for(int i = 0; i < cycle.size; i++) std::cout << cycle.tour[i].id << " ";
        std::cout << std::endl;
        for(int i = 0; i < cycle.size; i++) std::cout << cycle.tour[i].right << " ";
        std::cout << std::endl;
    }
};

#endif