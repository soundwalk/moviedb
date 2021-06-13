#include <iostream>
#include <cstring>
#include <limits>
#include <filesystem>

#include "Message.h"
#include "MovieDB.h"

MovieDB::MovieDB() {
    std::filesystem::create_directory("data");
    _dat.open(_masterFile, std::ios::in);
    if (!_dat) {
        _dat.open(_masterFile, std::ios::out);
        _dat.close();
        _dat.clear();
        _ind.open(_titleFile, std::ios::out);
        _ind.close();
        _ind.clear();
        _ind.open(_yearFile, std::ios::out);
        _ind.close();
        _ind.clear();
        _ind.open(_ratingFile, std::ios::out);
        _ind.close();
        _ind.clear();
    } else {
        _dat.close();
        _dat.clear();
    }
}

MovieDB::~MovieDB() {

}

template <class T, typename F>
void MovieDB::sortIndexFile(const char fileName[], Index<T> index, F compare) {
    Index<T> inx;
    Index<T> tmp;
    int found = 0;
    int counter = 0;
    
    _ind.open(fileName, std::ios::in | std::ios::out | std::ios::binary);
    while(true) {
        _ind.seekg(counter*sizeof(Index<T>));
        counter++;
        _ind.read((char *)&inx, sizeof(Index<T>));
        if (_ind.eof())
            break;
        
        if (compare(inx, index) && !found) {
            found = 1;
            _ind.seekp((int)_ind.tellg() - sizeof(Index<T>));
            _ind.write ((char*)&index, sizeof(Index<T>));
            memcpy(&tmp, &inx, sizeof(Index<T>));
        } else
            if (found) {
                _ind.seekp((int)_ind.tellg() - sizeof(Index<T>));
                _ind.write ((char *)&tmp, sizeof(Index<T>));
                memcpy(&tmp, &inx, sizeof(Index<T>));
            }
    }
    _ind.close();
    _ind.clear();
}

void MovieDB::add() {
    _dat.open(_masterFile, std::ios::in | std::ios::out | std::ios::ate | std::ios::binary);
    if (_dat.tellg()) {
        _dat.seekp(sizeof(Movie), std::ios::end);
        _dat.read((char*)&_movie, sizeof(Movie));
        _movie.id++;
    } else
        _movie.id = 1;
    _dat.close();
    _dat.clear();

    bool fail = false;

    std::cout << "   Title: ";
    std::cin.clear();
    std::cin.ignore();
    std::cin.getline(_movie.title, sizeof(_movie.title));

    if (!strlen(_movie.title))
        strcpy(_movie.title, "Unknown");

    do {
        std::cout << "   Year: ";
        std::cin.clear();
        std::cin >> _movie.year;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "   " << Message::WRONG_INPUT << std::endl;
            fail = true;
        } else
            if (_movie.year < 0) {
                std::cout << "   " << Message::OUT_OF_RANGE << std::endl;
                fail = true;
            } else
                fail = false;
    } while (fail);

    do {
        std::cout << "   Rating: ";
        std::cin.clear();
        std::cin >> _movie.rating;

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "   " << Message::WRONG_INPUT << std::endl;
            fail = true;
        } else
            if (_movie.rating < 0) {
                std::cout << "   " << Message::OUT_OF_RANGE << std::endl;
                fail = true;
            } else
                fail = false;
    } while (fail);
    
    int address;

    _dat.open(_masterFile, std::ios::in | std::ios::out | std::ios::ate | std::ios::binary);
    address = _dat.tellp();
    _dat.write((char*)&_movie, sizeof(Movie));
    _dat.close();
    _dat.clear();

    _s_inx.key = _movie.year;
    _s_inx.address = address;
    _ind.open(_yearFile, std::ios::app | std::ios::binary);
    _ind.write((char*)&_s_inx, sizeof(Index<short>));
    _ind.close();
    _ind.clear();

    sortIndexFile(_yearFile.c_str(), _s_inx, [](const Index<short>& i1, const Index<short>& i2) -> bool {
        return i1.key < i2.key ? true : false;
    });

    _f_inx.key = _movie.rating;
    _f_inx.address = address;
    _ind.open(_ratingFile, std::ios::app | std::ios::binary);
    _ind.write((char*)&_f_inx, sizeof(Index<float>));
    _ind.close();
    _ind.clear();

    sortIndexFile(_ratingFile.c_str(), _f_inx, [](const Index<float>& i1, const Index<float>& i2) -> bool {
        return i1.key < i2.key ? true : false;
    });
    
    strcpy(_c_inx.key, _movie.title);
    _c_inx.address = address;
    _ind.open(_titleFile, std::ios::app | std::ios::binary);
    _ind.write((char*)&_c_inx, sizeof(Index<char[64]>));
    _ind.close();
    _ind.clear();

    sortIndexFile(_titleFile.c_str(), _c_inx, [](const Index<char[64]>& i1, const Index<char[64]>& i2) -> bool {
        return strcmp(i1.key, i2.key) > 0 ? true : false;
    });

}

int MovieDB::exists(int id) {
    int address = 0;
    _dat.open(_masterFile, std::ios::in | std::ios::binary);
    while (true) {
        _dat.read((char*)&_movie, sizeof(Movie));
        if (_dat.eof())
            break;
        if (_movie.id == id) {
            address = (int)_dat.tellg() - sizeof(Movie);
            break;
        }
    }
    _dat.close();
    _dat.clear();
    return address;
}

void MovieDB::del(int id) {
    if (int address = exists(id)) {
        _dat.open(_masterFile, std::ios::in | std::ios::binary);
        _ind.open(_tmpFile, std::ios::out | std::ios::binary);
        while (true) {
            _dat.read((char*)&_movie, sizeof(Movie));
            if (_dat.eof())
                break;
            if (_movie.id != id)
                _ind.write((char*)&_movie, sizeof(Movie));
        }
        _dat.close();
        _dat.clear();
        _ind.close();
        _ind.clear();
        
        std::filesystem::remove(_masterFile);
        std::filesystem::remove(_titleFile);
        std::filesystem::remove(_yearFile);
        std::filesystem::remove(_ratingFile);

        std::filesystem::rename(_tmpFile, _masterFile);

        std::fstream ind1, ind2, ind3;
        _dat.open(_masterFile, std::ios::in | std::ios::binary);
        while (true) {
            _dat.read((char*)&_movie, sizeof(Movie));
            if (_dat.eof())
                break;

            strcpy(_c_inx.key, _movie.title);
            _s_inx.key = _movie.year;
            _f_inx.key = _movie.rating;

            _c_inx.address = (int)_dat.tellg() - sizeof(Movie);
            _s_inx.address = (int)_dat.tellg() - sizeof(Movie);
            _f_inx.address = (int)_dat.tellg() - sizeof(Movie);

            ind1.open(_titleFile, std::ios::app | std::ios::binary);
            ind2.open(_yearFile, std::ios::app | std::ios::binary);
            ind3.open(_ratingFile, std::ios::app | std::ios::binary);

            ind1.write((char*)&_c_inx, sizeof(Index<char[64]>));
            ind2.write((char*)&_s_inx, sizeof(Index<short>));
            ind3.write((char*)&_f_inx, sizeof(Index<float>));

            ind1.close();
            ind1.clear();
            ind2.close();
            ind2.clear();
            ind3.close();
            ind3.clear();

            sortIndexFile(_titleFile.c_str(), _c_inx, [](const Index<char[64]>& i1, const Index<char[64]>& i2) -> bool {
                return strcmp(i1.key, i2.key) > 0 ? true : false;
            });

            sortIndexFile(_yearFile.c_str(), _s_inx, [](const Index<short>& i1, const Index<short>& i2) -> bool {
                return i1.key < i2.key ? true : false;
            });

            sortIndexFile(_ratingFile.c_str(), _f_inx, [](const Index<float>& i1, const Index<float>& i2) -> bool {
                return i1.key < i2.key ? true : false;
            });
        }

        _dat.close();
        _dat.clear();

        std::cout << std::endl
            << "Movie with ID " << id << " successfully deleted!" << std::endl;
    } else
        std::cout << std::endl
            << "Movie with ID " << id << " not found." << std::endl;
}

void MovieDB::list(int order) {
    const char separator = ' ';
    const short unsigned idWidth = 5;
    const short unsigned titleWidth = 64;
    const short unsigned yearWidth = 7;
    const short unsigned ratingWidth = 7;

    std::cout << std::left << std::setw(idWidth) << std::setfill(separator) << "ID";
    std::cout << std::left << std::setw(titleWidth) << std::setfill(separator) << "Title";
    std::cout << std::left << std::setw(yearWidth) << std::setfill(separator) << "Year";
    std::cout << std::left << std::setw(ratingWidth) << std::setfill(separator) << "Rating";
    std::cout << std::endl;

    switch (order) {
        case 1:
            _dat.open(_masterFile, std::ios::in | std::ios::binary);
            while (true) {
                _dat.read((char*)&_movie, sizeof(Movie));
                if (_dat.eof())
                    break;
                std::cout << std::left << std::setw(idWidth) << std::setfill(separator) << _movie.id
                    << std::left << std::setw(titleWidth) << std::setfill(separator) << _movie.title
                    << std::left << std::setw(yearWidth) << std::setfill(separator) << _movie.year
                    << std::left << std::setw(ratingWidth) << std::setfill(separator) << _movie.rating
                    << std::endl;
            }
            _dat.close();
            _dat.clear();
            break;

        case 2:
            _dat.open(_masterFile, std::ios::in | std::ios::binary);
            _ind.open(_titleFile, std::ios::in | std::ios::binary);
            while (true) {
                _ind.read((char*)&_c_inx, sizeof(Index<char[64]>));
                if (_ind.eof())
                    break;
                _dat.seekg(_c_inx.address);
                _dat.read((char*)&_movie, sizeof(Movie));
                std::cout << std::left << std::setw(idWidth) << std::setfill(separator) << _movie.id
                    << std::left << std::setw(titleWidth) << std::setfill(separator) << _movie.title
                    << std::left << std::setw(yearWidth) << std::setfill(separator) << _movie.year
                    << std::left << std::setw(ratingWidth) << std::setfill(separator) << _movie.rating
                    << std::endl;
            }
            _ind.close();
            _ind.clear();
            _dat.close();
            _dat.clear();
            break;

        case 3:
            _dat.open(_masterFile, std::ios::in | std::ios::binary);
            _ind.open(_yearFile, std::ios::in | std::ios::binary);
            while (true) {
                _ind.read((char*)&_s_inx, sizeof(Index<short>));
                if (_ind.eof())
                    break;
                _dat.seekg(_s_inx.address);
                _dat.read((char*)&_movie, sizeof(Movie));
                std::cout << std::left << std::setw(idWidth) << std::setfill(separator) << _movie.id
                    << std::left << std::setw(titleWidth) << std::setfill(separator) << _movie.title
                    << std::left << std::setw(yearWidth) << std::setfill(separator) << _movie.year
                    << std::left << std::setw(ratingWidth) << std::setfill(separator) << _movie.rating
                    << std::endl;
            }
            _ind.close();
            _ind.clear();
            _dat.close();
            _dat.clear();
            break;

        case 4:
            _dat.open(_masterFile, std::ios::in | std::ios::binary);
            _ind.open(_ratingFile, std::ios::in | std::ios::binary);
            while (true) {
                _ind.read((char*)&_f_inx, sizeof(Index<float>));
                if (_ind.eof())
                    break;
                _dat.seekg(_f_inx.address);
                _dat.read((char*)&_movie, sizeof(Movie));
                std::cout << std::left << std::setw(idWidth) << std::setfill(separator) << _movie.id
                    << std::left << std::setw(titleWidth) << std::setfill(separator) << _movie.title
                    << std::left << std::setw(yearWidth) << std::setfill(separator) << _movie.year
                    << std::left << std::setw(ratingWidth) << std::setfill(separator) << _movie.rating
                    << std::endl;
            }
            _ind.close();
            _ind.clear();
            _dat.close();
            _dat.clear();
            break;

        default:
            break;
    }
}

void MovieDB::reset() {
    _dat.open(_masterFile, std::ios::out);
    _dat.close();
    _dat.clear();
    _ind.open(_titleFile, std::ios::out);
    _ind.close();
    _ind.clear();
    _ind.open(_yearFile, std::ios::out);
    _ind.close();
    _ind.clear();
    _ind.open(_ratingFile, std::ios::out);
    _ind.close();
    _ind.clear();
}