#ifndef tour_hpp
#define tour_hpp

#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <string>

#include "parameter.hpp"
#include "node.hpp"

class Tour{

public:
    int size;
    std::vector<Node> tour;

public:
    Tour& operator=(const Tour& t){
        size = t.size;
        tour = t.tour;
        return *this;
    }

    bool operator==(const Tour& t)const{
        return equal(tour.begin(), tour.end(), t.tour.begin());
    }

    bool operator!=(const Tour& t)const{
        return !equal(tour.begin(), tour.end(), t.tour.begin());
    }

public:
    void input_file(const std::string& filename){

        std::ifstream inputfile(filename);

        if(inputfile.fail()){
            std::cerr << "inputfile can not open." << std::endl;
        }

        std::string str;
        
        std::getline(inputfile, str);
        NAME = str;

        std::getline(inputfile, str);
        COMMENT = str;

        std::getline(inputfile, str);
        TYPE = str;

        std::getline(inputfile, str);
        DIMENSION = str;
        sscanf(DIMENSION.data(), "DIMENSION : %d", &size);

        std::getline(inputfile, str);
        EDGE_WEIGHT_TYPE = str;

        std::getline(inputfile, str);

        tour.resize(size);

        for(int i = 0; i < size; i++){
            std::getline(inputfile, str);
            sscanf(str.data(), "%d %lf %lf", &tour[i].id,  &tour[i].x, &tour[i].y);
        }
        inputfile.close();
    }

    int euc_2d(const Node& n1, const Node& n2){
        double x = n1.x - n2.x;
        double y = n1.y - n2.y;
        return (int)(std::sqrt((x * x) + (y * y)) + 0.5);
    }

    void calc_citycost_nnlist(std::vector<std::vector<int> >& cost, std::vector<std::list<std::pair<int, int> > >& NNlist){
        
        int id, dist;

        for(int i = 1; i <= size; i++){
            for(int j = i + 1; j <= size; j++){

                dist = euc_2d(tour[i - 1], tour[j - 1]);

                id = j;
                NNlist[i].push_back(std::make_pair(dist, id));
                NNlist[j].push_back(std::make_pair(dist, i));

                cost[i][j] = dist;
                cost[j][i] = dist;
            }
            NNlist[i].sort();
        }
    }

    void t_opt(std::vector<std::vector<int> >& cost, std::vector<std::list<std::pair<int, int> > >& NNlist){

        bool improve = true;    
        while(improve){

            improve = false;

            int p1, p2, p3, p4;
            int t3, t4;
            int tmp_distance, max_distance;
            for(int i = 0; i < size; i++){
        
                max_distance = 0;
        
                p1 = i; p2 = (i + 1) % size;
        
                int depth = 0;
                auto list_itr = NNlist[tour[p1].id].begin();
                while(depth < DEPTH){
        
                    for(int j = 0; j < size; j++){
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

                    depth++;
                    list_itr++;
                }
        
                // 改善があったなら逆位
                if(max_distance > 0){
                    if(p2 < p3){
                        for(int k = 0; k < (p3 - p2 + 1) / 2; k++){
                            std::swap(tour[p2 + k], tour[p3 - k]);
                        }   
                    }else{
                        for(int k = 0; k < (size - (p2 - p3) + 1) / 2; k++){
                            std::swap(tour[(p2 + k) % size], tour[(p3 - k + size) % size]);
                        }
                    }
                    improve = true;
                }
            }
        }
    }

    // 全てidが同じ値かを調べる
    bool fill_id(){
        for(int i = 0; i < size; i++){
            if(tour[i] != tour[(i + 1) % size]) return false;
        }
        return true;
    }
};

#endif