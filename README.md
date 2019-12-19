# Edge Assembly Crossover : EAX

EAX is a crossover of the traveling salesman problem (TSP) used by the genetic algorithm (GA) developed by Nagata.

The following paper will be implemented.

[巡回セールスマン問題に対する交叉 : 枝組み立て交叉の提案と評価](https://jsai.ixsq.nii.ac.jp/ej/?action=pages_view_main&active_action=repository_view_main_item_detail&item_id=4807&item_no=1&page_id=13&block_id=23)

[A Powerful Genetic Algorithm Using Edge Assembly Crossover for the Traveling Salesman Problem](https://pubsonline.informs.org/doi/abs/10.1287/ijoc.1120.0506?journalCode=ijoc)


  
This code has the following parameters:
  
> TRIAL: trial count.  
> GENERATION: number of generation.   
> POPULATION: number of population.     
> CHILDREN: number of offspring.    
> OPT_DIST: the distance of optimal solution.  
> DEPTH: depth of searching.   

This code can only be used for TSP instance of EUC_2D type.

Enter the file name as the second argument and execute it.
ex.) ./a.out rat575.tsp
