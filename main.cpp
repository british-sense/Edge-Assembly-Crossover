#include "eax.hpp"

// 母集団の調査
void analyze(const std::vector<Individual>& population, double &min, double &max, double &ave){

	min = INT_MAX;
    max = 0;
	ave = 0;
	for(int i = 0; i < POPULATION; i++){
		if(min > population[i].fitness) min = population[i].fitness;
		if(max < population[i].fitness) max = population[i].fitness;
		ave += population[i].fitness;
	}
	ave /= POPULATION;
}


int main(int argc, char* argv[]){

    std::string filename(argv[1]);
    filename = "./TSPLIB/" + filename;

    // データの読み込み
    Tour data;
    data.input_file(filename);

    // 近傍リストの作成・コスト表の作成
    std::vector<std::vector<int> > cost(data.size + 1, std::vector<int>(data.size + 1, 0));
    std::vector<std::list<std::pair<int, int> > > NNlist(data.size + 1);
    data.calc_citycost_nnlist(cost, NNlist);

    std::cout << "Instance : " << argv[1] << std::endl;
    std::cout << "opt distance : " << OPT_DIST << std::endl;
    std::cout << "trial : " << TRIAL << std::endl;
    std::cout << "generation : " << GENERATION << std::endl;
    std::cout << "population : " << POPULATION << std::endl;
    std::cout << "children : " << CHILDREN << std::endl;

    std::string outfilename = "./result/" + std::string(argv[1]) + "_" + std::to_string(POPULATION) + "_" + std::to_string(CHILDREN) + ".txt";
    std::ofstream outputfile(outfilename);
    outputfile << "Instance : " << argv[1] << std::endl;
    outputfile << "opt distance : " << OPT_DIST << std::endl;
    outputfile << "trial : " << TRIAL << std::endl;
    outputfile << "generation : " << GENERATION << std::endl;
    outputfile << "population : " << POPULATION << std::endl;
    outputfile << "children : " << CHILDREN << std::endl;

    double opt = 0, opt_ave = 0, conv_gene = 0;

    for(int i = 0; i < TRIAL; i++){

        std::cout << "trial..." << i + 1 << std::endl;
        outputfile << "trial..." << i + 1 << std::endl;

        // 母集団の生成
        std::vector<Individual> population(POPULATION);
        std::vector<Individual> next_population(POPULATION);

        // 母集団の初期化
        for(int j = 0; j < POPULATION; j++){
            population[j].init(data, cost, NNlist);
        }

        double min, max, ave;
        std::cout << "gene" << '\t' << "min" << '\t' << "max" << '\t' << "ave" << std::endl;
        outputfile << "gene" << '\t' << "min" << '\t' << "max" << '\t' << "ave" << std::endl;

        for(int j = 0; j < GENERATION; j++){

            for(int k = 0; k < POPULATION; k++){

                // 親の選択
                Individual parentA = population[k];
                Individual parentB = population[(k + 1) % POPULATION];

                // 交叉
                next_population[k] = EAX(parentA, parentB, cost, NNlist);
            }

            population = next_population;

            analyze(population, min, max, ave);

            std::cout << j + 1 << '\t' << min << '\t' << max << '\t' << ave << std::endl;
            outputfile << j + 1 << '\t' << min << '\t' << max << '\t' << ave << std::endl;

            if(min == max){
                if(min == OPT_DIST) opt++;
                opt_ave += min;
                conv_gene += j + 1;
                break;
            }

            std::shuffle(population.begin(), population.end(), mt);
        }
    }

    std::cout << "opt count : " << opt << std::endl;
    std::cout << "opt average : " << opt_ave / TRIAL << std::endl;
    std::cout << "convergence generation : " << conv_gene / TRIAL << std::endl;
    
    outputfile << "opt count : " << opt << std::endl;
    outputfile << "opt average : " << opt_ave / TRIAL << std::endl;
    outputfile << "convergence generation : " << conv_gene / TRIAL << std::endl;

    outputfile.close();

    return 0;
}