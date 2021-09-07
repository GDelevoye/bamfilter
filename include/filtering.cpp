//
// Created by guillaume on 04/09/2021.
//

#include <string>
#include <map>
#include "CIGAR.hpp"

#include "spdlog/spdlog.h"


namespace filtering {

    bool isokay_filtering(float min_identity,std::string &line){

        auto cigar = CIGAR::get_field(line, 5);

        std::map<std::string,long> parsedCIGAR;
        CIGAR::parse_CIGAR(cigar,parsedCIGAR);  // parsedCIGAR has been modified inplace

        if (parsedCIGAR.at("*") != 0){
            return false;
        }

        // https://stackoverflow.com/questions/1474894/why-isnt-the-operator-const-for-stl-maps
        auto matches = parsedCIGAR.at("M");
        auto eq = parsedCIGAR.at("=");
        auto insertions = parsedCIGAR.at("I");
        auto deletions = parsedCIGAR.at("D");
        auto substitutions = parsedCIGAR.at("X");
//        auto hardclipped = parsedCIGAR.at("H");
        auto softclipped = parsedCIGAR.at("S");

        auto nbmatches = matches + eq;
        auto reflen = CIGAR::compute_reflen(matches+eq,substitutions,deletions);
        auto seqlen = CIGAR::compute_seqlen(matches,insertions,softclipped,eq,substitutions);

        float identity = CIGAR::homemade_identity(reflen,seqlen,nbmatches);


        if (identity >= min_identity){
            return true;
        }
        else{
            return false;
        }

    }

}