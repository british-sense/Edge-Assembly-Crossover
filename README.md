# 枝組み立て交叉(Edge Assembly Crossover : EAX)

巡回セールスマン問題(Traveling Salesman Problem : TSP)を解く遺伝的アルゴリズムを用いた交叉手法です.

論文は下記から手にはいります.

[巡回セールスマン問題に対する交叉 : 枝組み立て交叉の提案と評価](https://jsai.ixsq.nii.ac.jp/ej/?action=pages_view_main&active_action=repository_view_main_item_detail&item_id=4807&item_no=1&page_id=13&block_id=23)


  
各パラメータは
  
>TRIAL : 試行回数  
>GENERATION : 世代数  
>POPULATION : 母集団数
>CHILDREN : 子個体数  
>OPT_DIST : 最適解の距離
>DEPTH : 探索するときの深さ

を表しています.

TSPの問題タイプはEUC_2Dのみに対応しています.

実行は第1引数にファイル名を入れてもらえれば大丈夫です.
ex.)./a.out rat575.tsp
