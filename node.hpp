#ifndef node_hpp
#define node_hpp

class Node{
	
public:
    int id;
    double x;
    double y;
    int left;
    int right;

public:
    Node& operator=(const Node& n){
        id = n.id;
        x = n.x;
        y = n.y;
        left = n.left;
        right = n.right;
        return *this;
    }

    bool operator==(const Node& n)const{
        return ((id == n.id && left == n.left && right == n.right) || (id == n.id && left == n.right && right == n.left));
    }

    bool operator!=(const Node& n)const{
        return !((id == n.id && left == n.left && right == n.right) || (id == n.id && left == n.right && right == n.left));
    }

    bool operator<(const Node& n)const{
        return id < n.id;
    }

    bool operator>(const Node& n)const{
        return id > n.id;
    }
};

#endif