//
//  farmer.hpp
//  
//
//  Created by 吴宇 on 3/23/22.
//

#ifndef farmer_hpp
#define farmer_hpp

#include <iostream>
#include <cmath>
#include <queue>
#include <vector>
using namespace std;

class Grass{
public:
    int pos;
    int tasti;
    bool is_claimed;
    int max_dist; // maximum distance required for John to take
    
    void init(int p, int t) {
        pos = p;
        tasti = t;
        is_claimed = false;
        max_dist = -1;
    }
    int get_tasti(){ return (is_claimed? 0 : tasti);}
};

struct seg_s{
    int left;
    int right;
    int tasti; // sum of tasti
    vector<Grass*> g_vec;
    
    int get_tasti(){ // sum unclaimed tastiness
        int t = 0;
        for(int i=0; i<g_vec.size(); i++){
            t = t + g_vec[i]->get_tasti();
        }
        tasti = t;
        return t;
    }
    
    bool operator < (const seg_s &seg_h) const {
        return (tasti < seg_h.tasti);
    }
};


class Farm {
public:
    int K;
    int M;
    int N;
    
    Grass** grass_vec;
    int* nhoj; // nhoj's cow
    queue<seg_s> seg_q;
    vector<seg_s> seg_vec;
    
    Farm(int, int, int);
    ~Farm();

    //void calc_john_tast();
    void rm_occu_patch(); // find patch already taken by Nhoj
    int binary_search_nhoj(int, int, int);
    // get the max distance required for John to take a patch
    void get_max_dist();
    // patch to segment
    void gen_segments();
    // sort segment by tasti, and select best
    int get_max_tasti();
    
    int get_best_tasti();
};

int Farm::get_best_tasti(){
    rm_occu_patch();
    get_max_dist();
    gen_segments();
    return get_max_tasti();
}

Farm::Farm(int k, int m, int n){
    K = k;
    M = m;
    N = n;
    grass_vec = new Grass*[K]; // patch location
    for(int i=0; i<K; i++){
        grass_vec[i] = new Grass;
    }
    nhoj = new int[M];
}

Farm::~Farm(){
    for(int i=0; i<K; i++){
        delete grass_vec[i];
    }
    delete[] grass_vec;
    delete[] nhoj;
}

void Farm::rm_occu_patch(){
    // if occupied, set tastiness to 0
    // assume sorted ascending
    int i = 0;
    int j = 0;
    for (; i<M;){
        if (nhoj[i] == grass_vec[j]->pos){
            grass_vec[j]->tasti = 0;
            grass_vec[j]->is_claimed = true;
            i++;
            j++;
        }
        else if(nhoj[i] < grass_vec[j]->pos){ // Nhoj on the left
            i++;
        }
        else{
            // nhoj[i]>p[j] // Nhoj on the right
            j++;
        }
        if(j>=K){
            return;
        }
    }
    cout << "Finished remove occupied patch, display grass pos" << endl;
    for(int i=0; i<K; i++){
        cout << "i = " << i << ", pos = " << grass_vec[i]->pos;
        cout << ", tasti = " << grass_vec[i]->tasti << endl;
    }
}

int Farm::binary_search_nhoj(int pos, int low, int high){
    // find the closest Nhoj's cow to patch at pos
    // using binary search
    if(nhoj[low]==pos){
        return low;
    }
    if(nhoj[high]==pos){
        return high;
    }
    int mid = (low + high)/2;
    if ((mid==low) || (mid==high)){
        int low_dist = abs(pos - nhoj[low]);
        int high_dist = abs(nhoj[high] - pos);
        if (low_dist<high_dist)
            return low;
        else
            return high;
    }
    else{
        if (nhoj[mid] < pos)
            return binary_search_nhoj(pos, mid, high);
        else // Nhoj[mid] >= pos
            return binary_search_nhoj(pos, low, mid);
    }
}

void Farm::get_max_dist(){
    // dist(John, patch) must be smaller than dist(Nhoj, patch)
    // assume sorted ascending
    // for each patch, find the closest Nhoj's cow
    cout << "Calc max dist for John to take a patch" << endl;
    for (int i = 0; i<K; i++){
        if (grass_vec[i]->is_claimed)
            continue;
        int pos = grass_vec[i]->pos;
        int j = binary_search_nhoj(pos, 0, M-1);
        grass_vec[i]->max_dist = abs(nhoj[j]-pos);
        cout << "patch[" << i << "].max_dist = ";
        cout << grass_vec[i]->max_dist << endl;
    }
}

void Farm::gen_segments(){
    // for each patch, generate a segment
    // iteration 1
    cout << "1st Iteration start, display segments:" << endl;
    for(int i = 0; i < K; i++){
        if(!grass_vec[i]->is_claimed){
            seg_s seg;
            int max_dist = grass_vec[i]->max_dist;
            int pos = grass_vec[i]->pos;
            seg.left = pos - max_dist;
            if (seg.left < 0)
                seg.left = 0;
            seg.right = pos + max_dist;
            seg.tasti = grass_vec[i]->tasti;
            seg.g_vec.push_back(grass_vec[i]);
            seg_q.push(seg);
            cout << "[" << seg.left << ", " << seg.right << "] ";
            cout << "tasti = " << seg.tasti << endl;
        }
    }
    cout << "1st iteration finished... 2nd iteration starts" << endl;
    // iteration 2 : resolve overlapping cases
    // foreach segment, check left with previous seg
    // until reach the 1st seg, or the seg not overlapping
    // for each overlapping, update tasti
    while (!seg_q.empty()){
        seg_s seg1 = seg_q.front();
        seg_q.pop();
        if(seg_vec.size()==0){
            seg_vec.push_back(seg1);
        }
        else{
            vector<seg_s>::iterator it = seg_vec.end();
            it--;
            if(it->right <= seg1.left){
                // no overlapping
                seg_vec.push_back(seg1);
            }
            else if(it->left < seg1.left){
                if(it->right < seg1.right){
                    // partial overlapping
                    // break previous segment into 2 parts
                    seg_s seg_overlap;
                    seg_overlap.left = seg1.left;
                    seg_overlap.right = it->right;
                    seg_overlap.g_vec = it->g_vec;
                    seg_overlap.g_vec.insert(seg_overlap.g_vec.end(), seg1.g_vec.begin(), seg1.g_vec.end());
                    it->right = seg1.left;
                    // append new seg
                    seg_s seg_new;
                    seg_new.left = seg_overlap.right;
                    seg_new.right = seg1.right;
                    seg_new.g_vec = seg1.g_vec;
                    seg_vec.push_back(seg_overlap);
                    seg_vec.push_back(seg_new);
                }
                else{ // it->right == seg1.right
                    // break previous seg into 2 parts, no new seg
                    seg_s seg_overlap;
                    seg_overlap.left = seg1.left;
                    seg_overlap.right = it->right;
                    //seg_overlap.tasti = it->tasti + seg1.tasti;
                    seg_overlap.g_vec = it->g_vec;
                    seg_overlap.g_vec.insert(seg_overlap.g_vec.end(), seg1.g_vec.begin(), seg1.g_vec.end());
                    it->right = seg1.left;
                    seg_vec.push_back(seg_overlap);
                }
            }
            else if (it->left == seg1.left){
                // previous seg overlapping
                //it->tasti = it->tasti + seg1.tasti;
                it->g_vec.insert(it->g_vec.end(), seg1.g_vec.begin(), seg1.g_vec.end());
                seg_s seg_new;
                seg_new.left = it->right;
                seg_new.right = seg1.right;
                seg_new.g_vec = seg1.g_vec;
                seg_vec.push_back(seg_new);
            }
            else {
                // it->left > seg1.left
                // for each previous segment, update tasti
                seg_s seg_new;
                seg_new.left = it->right;
                seg_new.right = seg1.right;
                //seg_new.tasti = seg1.tasti;
                seg_new.g_vec = seg1.g_vec;
                while(it!= seg_vec.begin()){
                    it--;
                    if(it->right > seg1.left){
                        it->g_vec.insert(it->g_vec.end(), seg1.g_vec.begin(), seg1.g_vec.end());
                    }
                    else{
                        break;
                    }
                }
                seg_vec.push_back(seg_new);
            }
        }
    }
    cout << "2nd iteration finished, display seg:" <<endl;
    for (int i = 0; i< seg_vec.size(); i++){
        cout << "[" << seg_vec[i].left << ", ";
        cout << seg_vec[i].right << "], tasti = ";
        cout << seg_vec[i].get_tasti() << endl;
    }
}

int Farm::get_max_tasti(){
    // sort seg_t by its total_tastiness
    sort(seg_vec.begin(), seg_vec.end());
    int total_tasti = 0;
    if (N < seg_vec.size()){
        vector<seg_s>::iterator it = seg_vec.end();
        it--;
        for(int i=0; i<N; i++){
            total_tasti = total_tasti + it->get_tasti(); // it->tasti is updated
            for(int j = 0; j < it->g_vec.size(); j++){
                it->g_vec[j]->is_claimed = true;
            }
            cout << "pos = [" << it->left << ", ";
            cout << it->right << "], tasti = " << it->tasti << endl;
            seg_vec.pop_back();
            vector<seg_s>::iterator it_2 = seg_vec.end();
            for(int j=0; j < seg_vec.size(); j++)
                seg_vec[j].get_tasti();
            sort(seg_vec.begin(), seg_vec.end());
            it = seg_vec.end();
            it--;
        }
    }
    else{ // N >= seg number
        // sum up all the tasti
        vector<seg_s>::iterator it = seg_vec.end();
        while(it!=seg_vec.begin()){
            it--;
            total_tasti = total_tasti + it->get_tasti();
            for(int j = 0; j < it->g_vec.size(); j++){
                it->g_vec[j]->is_claimed = true;
            }
        }
    }
    return total_tasti;
}

#endif /* farmer_hpp */
