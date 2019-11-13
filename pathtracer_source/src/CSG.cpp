//
// Created by serizba on 30/12/18.
//

#include <list>
#include "CSG.h"

using interval = tuple<double, double, int, TMatrix, TMatrix>;

list<interval> intersection(list<interval> l1, list<interval> l2) {
    list<interval> res = {};
    for (auto i1 : l1) {
        for (auto i2 : l2) {
            if (get<0>(i2) <= get<1>(i1) && get<0>(i1) <= get<1>(i2)) {
                TMatrix ts, te;
                double ds, de;
                if (get<0>(i1) < get<0>(i2)) {
                    ds = get<0>(i2);
                    ts = get<3>(i2);
                } else {
                    ds =get<0>(i1);
                    ts = get<3>(i1);
                }
                if (get<1>(i1) < get<1>(i2)) {
                    de = get<1>(i1);
                    te = get<4>(i1);
                } else {
                    de = get<1>(i2);
                    te = get<4>(i2);
                }

                res.emplace_back(make_tuple(ds,de,1,ts,te));
            }
        }
    }
    return res;
}

list<interval> myunion(list<interval> l1, list<interval> l2) {
    list<interval> res = {};
    bool start1 = true;
    bool start2 = true;
    double start = 0;
    TMatrix ts;
    int count = 0;

    while(!l1.empty() && !l2.empty()){
        if (start1 && start2){
            if (get<0>(l1.front())< get<0>(l2.front())){
                count++;
                if (count==1 && start==0){
                    start = get<0>(l1.front());
                    ts = get<3>(l1.front());
                }
                start1 = false;

            } else {
                count++;
                if (count==1 && start==0){
                    start = get<0>(l2.front());
                    ts = get<3>(l2.front());
                }
                start2 = false;

            }
        } else if (start1 && !start2){
            if (get<0>(l1.front())< get<1>(l2.front())){
                count++;
                if (count==1 && start==0){
                    start = get<0>(l1.front());
                    ts = get<3>(l1.front());
                }
                start1 = false;
            } else {
                count--;
                if (count==0){
                    res.emplace_back(make_tuple(start,get<1>(l2.front()),0,ts,get<4>(l2.front())));
                    start=0;
                }
                start2 = true;
                l2.pop_front();
            }
        } else if (!start1 && start2){
            if (get<0>(l2.front())< get<1>(l1.front())){
                count++;
                if (count==1 && start==0){
                    start = get<0>(l2.front());
                    ts = get<3>(l2.front());
                }
                start2 = false;
            } else {
                count--;
                if (count==0){
                    res.emplace_back(make_tuple(start,get<1>(l1.front()),0,ts,get<4>(l1.front())));
                    start=0;
                }
                start1 = true;
                l1.pop_front();
            }
        } else {
            if (get<1>(l1.front())< get<1>(l2.front())){
                count--;
                if (count==0){
                    res.emplace_back(make_tuple(start,get<1>(l1.front()),0,ts,get<4>(l1.front())));
                    start=0;
                }
                start1 = true;
                l1.pop_front();
            } else {
                count--;
                if (count==0){
                    res.emplace_back(make_tuple(start,get<1>(l2.front()),0,ts,get<4>(l2.front())));
                    start=0;
                }
                start2 = true;
                l2.pop_front();
            }
        }
    }

    while(!l1.empty()){
        if(!start1){
            res.emplace_back(make_tuple(start,get<1>(l1.front()),0,ts,get<4>(l1.front())));
            start1=true;
            l1.pop_front();
        } else {
            res.emplace_back(l1.front());
            l1.pop_front();
        }
    }
    while(!l2.empty()){
        if(!start2){
            res.emplace_back(make_tuple(start,get<1>(l2.front()),0,ts,get<4>(l2.front())));
            start2=true;
            l2.pop_front();
        } else {
            res.emplace_back(l2.front());
            l2.pop_front();
        }
    }

    return res;
}

list<interval> difference(list<interval> l1, list<interval> l2) {
    list<interval> res = {};
    if (l1.empty() || l2.empty()) { return l1;}
    double sp1 = get<0>(l1.front());
    double sp2 = get<0>(l2.front());
    TMatrix t1 = get<3>(l1.front());
    TMatrix t2 = get<3>(l2.front());

    while(!l1.empty() && !l2.empty()) {
        if (sp1 == sp2) {
            if (get<1>(l1.front()) < get<1>(l2.front())) {
                sp2 = get<1>(l1.front());
                t2 = get<4>(l1.front());
                l1.pop_front();
                if (!l1.empty()) {
                    sp1 = get<0>(l1.front());
                    t1 = get<3>(l1.front());
                }
            } else {
                if (get<1>(l1.front()) == get<1>(l2.front())) {
                    l1.pop_front();
                }
                sp1 = get<1>(l2.front());
                t1 = get<4>(l2.front());
                l2.pop_front();
                if (!l2.empty()) {
                    sp2 = get<0>(l2.front());
                    t2 = get<3>(l2.front());
                }
            }
        } else if (sp1 < sp2) {
            if (get<1>(l1.front()) < get<0>(l2.front())) {
                res.emplace_back(make_tuple(sp1, get<1>(l1.front()), 0, t1, get<4>(l1.front())));
                l1.pop_front();
                if (!l1.empty()) {
                    sp1 = get<0>(l1.front());
                    t1 = get<3>(l1.front());
                }
            } else {
                res.emplace_back(make_tuple(sp1, sp2, 0, t1, t2));
                sp1 = sp2;
                t1 = t2;
            }
        } else {
            if (get<1>(l2.front())<sp1) {
                l2.pop_front();
                if (!l2.empty()) {
                    sp2 = get<0>(l2.front());
                    t2 = get<3>(l2.front());
                }
            } else {
                sp2 = sp1;
                t2 = t1;
            }
        }
    }

    if (!l1.empty()) {
        res.emplace_back(sp1, get<1>(l1.front()), 0, t1, get<4>(l1.front()));
        l1.pop_front();
        while (!l1.empty()) {
            res.emplace_back(l1.front());
            l1.pop_front();
        }
    }
    return res;
}

list<interval> recorrer(CSGTree* t, vector<interval> ray) {
    if (t->right == nullptr && t->left == nullptr) {
        // is a leaf
        if (get<2>(ray[t->op]) == -1) {
            return {};
        }
        return {ray[t->op]};
    } else {
        auto l = recorrer(t->left, ray);
        auto r = recorrer(t->right, ray);
        switch (t->op) {
            case 1:
                // intersection
                return intersection(r,l);
            case 2:
                // minus
                return difference(l,r);
            case 3:
                //union
                return myunion(l,r);

            default:
                return {};
        }
    }
}

tuple<double, TMatrix> CSG::intersect(DirPoint o, DirPoint p) {
    // There is intersection with the tree
    vector<interval> ray(this->figures.size(), {0., 0., -1, TMatrix(), TMatrix()});
    int num = 0;
    for (int i=0; i<this->figures.size(); i++) {
        double d = numeric_limits<double>::max();
        TMatrix t1;
        tie(d,t1) = figures[i]->intersect(o, p);
        if (d < numeric_limits<double >::max() && d > epsilon) {
            double d2;
            TMatrix t2;
            tie(d2,t2) = figures[i]->intersect(t1[3], t1[3]+DirPoint::normalize(p-o));
            if (d2 < numeric_limits<double>::max()) {
                ray[i] = {d, d + d2, i, t1, t2};
                num++;
            } else {
                ray[i] = {0, d, i, {}, t1};
            }
        }
    }

    if (num==0) {return {numeric_limits<double>::max(), TMatrix()};}

    // Sort the list and return first interval
    auto l = recorrer(this->t, ray);
    //sort(l.begin(), l.end(), [] (interval i1, interval i2) { return get<0>(i1) < get<0>(i2);});
    if (l.empty()) {
        return {numeric_limits<double>::max(), TMatrix()};
    }
    l.sort([] (interval i1, interval i2) { return get<0>(i1) < get<0>(i2);});
    return {get<0>(l.front()), get<3>(l.front())};

}


void obtainFigures(CSGTree* t, vector<Figure*>& f) {
    if (t->right == nullptr && t->left == nullptr) {
        t->op = f.size();
        f.push_back(t->f);
    } else {
        obtainFigures(t->right, f);
        obtainFigures(t->left, f);
    }
}

CSG::CSG(CSGTree *t, BRDF brdf) {
    this->t = t;
    this->brdf = brdf;
    obtainFigures(t, this->figures);
}

BRDF CSG::getBRDF(DirPoint p) const {
    if (this->brdf.isTextured()){
        DirPoint center = p + ((this->p1 - p) - ((this->p1 - p) * this->v) * this->v);

        // get u,v (theta and height) in range [0,1]
        double v = ((this->p1 - center).modulus())/ ((this->p1-this->p2).modulus());
        if (v > 1) v=1.0; // numeric aproximations

        double cosTh = DirPoint::normalize(p-center)*DirPoint::normalize(this->ref);
        double sinTh = DirPoint::normalize(p-center).cross(DirPoint::normalize(this->ref)).modulus();
        double u;
        if (cosTh >= 0 && sinTh >= 0){
            u = sinTh / 4;
        } else if(cosTh <= 0 && sinTh >=0){
            u = 0.25 - cosTh/4;
        } else if(cosTh <=0 && sinTh <=0){
            u = 0.5 - sinTh/4;
        } else {
            u = 0.75 + cosTh/4;
        }

        //cout << u << " " << v << endl;
        return this->brdf.getBRDF(static_cast<float>(u), static_cast<float>(v));
    } else {
        return this->brdf;
    }
}

CSG::CSG(CSGTree *t, BRDF brdf, DirPoint p1, DirPoint p2, double r, DirPoint ref) {
    this->t = t;
    this->brdf = brdf;
    obtainFigures(t, this->figures);
    this->r = r;
    this->p1 = p1;
    this->p2 = p2;
    this->v = DirPoint::normalize(p2-p1);
    this->ref = DirPoint::normalize(this->v.cross(ref));
}
