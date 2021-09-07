//
// Created by guillaume on 04/09/2021.
//

#ifndef BAMFILTER_FILTERING_HPP
#define BAMFILTER_FILTERING_HPP

#include "CIGAR.hpp"

namespace filtering {
        bool isokay_filtering(float min_identity,std::string &line);

}

#endif //BAMFILTER_FILTERING_HPP
