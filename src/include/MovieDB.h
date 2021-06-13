#pragma once

#include <cstring>
#include <fstream>

class MovieDB {
    class Movie {
        public:
            short year;
            unsigned int id;
            float rating;
            char title[64];
    } _movie;

    template <class T>
    class Index {
        public:
            T key;
            int address;
    };

    Index<short> _s_inx;
    Index<float> _f_inx;
    Index<char[64]> _c_inx;

    std::fstream _dat;
    std::fstream _ind;

    std::string _masterFile = "./data/data.dat";
    std::string _titleFile = "./data/title.ind";
    std::string _yearFile = "./data/year.ind";
    std::string _ratingFile = "./data/rating.ind";
    std::string _tmpFile = "./data/tmp.ind";

public:
    MovieDB();
    ~MovieDB();

    void add();
    void list(int order);
    void del(int id);
    void reset();

private:
    int exists(int id);
    template <class T, typename F>
    void sortIndexFile(const char fileName[], Index<T> inx, F compare);
};
