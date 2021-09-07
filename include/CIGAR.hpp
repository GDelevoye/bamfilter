//
// Created by guillaume on 04/09/2021.
//

#ifndef BAMFILTER_CIGAR_HPP
#define BAMFILTER_CIGAR_HPP

#include <vector>
#include <map>
#include <set>

namespace CIGAR {
    std::string get_field(std::string &inputstring, int field);

    unsigned int listdigits_to_number(std::vector<int> listdigits);

    void parse_CIGAR(std::string &CIGARstring,std::map<std::string,long> &outputmap);

    float homemade_identity(unsigned int reflen,
                            unsigned int seqlen,
                            unsigned int number_matches);

    void get_fields(std::string &inputstring, std::set<int> fields, std::map<int,std::string> &output);

    template <typename T>
    T compute_reflen(   T matchs,
                        T substitutions,
                        T deletions){
        T reflen = deletions + matchs + substitutions;
        return reflen;
    }

//    M+I+S+=+X = seqlen
    // https://en.wikipedia.org/wiki/Sequence_alignment#Representations
    template <typename T>
    T compute_seqlen(   T M,
                        T I,
                        T S,
                        T Eq,
                        T X){
        T seqlen = M+I+S+Eq+X;
        return seqlen;
    }
}

#endif //BAMFILTER_CIGAR_HPP
