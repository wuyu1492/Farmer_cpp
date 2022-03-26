//
//  farmer.cpp
//  
//
//  Created by 吴宇 on 3/23/22.
//

#include "farmer.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;



int main(){
    ifstream in_file("input.txt");
    string line;
    int K, M, N;
    if(in_file.is_open()){
        getline(in_file, line);
        istringstream iss(line);
        iss >> K >> M >> N;
        Farm farm_h(K, M, N);
        cout << "K = " << K << " M = " << M << " N = " << N << endl;
        for (int i=0; i<K; i++){
            int p, t;
            getline(in_file, line);
            istringstream iss(line);
            iss >> p >> t;
            cout << line<< endl;
            farm_h.grass_vec[i].init(p, t);
        }
        
        for(int i = 0; i<M; i++){
            int n;
            getline(in_file, line);
            istringstream iss(line);
            iss >> n;
            cout << line << endl;
            farm_h.nhoj[i] = n;
        }
        
        int ans = farm_h.get_best_tasti();
        cout << "Answer is " << ans << endl;
    }
    else{
        cout << "file not opened!!!"<< endl;
        return 0;
    }
    in_file.close();
    
    
}
