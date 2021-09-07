//
// Created by guillaume on 04/09/2021.
//
#include <vector>
#include <set>
#include <string>
#include <map>
#include <cmath>
#include <algorithm>

#include <boost/format.hpp>
#include <stdexcept>

/*
 * CIGAR parsing tools
 */
namespace CIGAR {

    /*
     * Computes my homemade identity score
     * I think here, we suppose that both the reference and the query are expected to be 100% right if the alignment
     * is well done. Meaning, we want the fraction of nucleotides that are matched among all considered nucleotides
     * --> 2*numbermatches / (seqlen + reflen)
     * That differs significantly from the BLAST definition
     * But the BLAST definition, for my use case, would be stupid
     * https://medium.com/computational-biology/how-to-interpret-blast-results-ee304216fd5
     */
    float homemade_identity(unsigned int reflen,
                            unsigned int seqlen,
                            unsigned int number_matches){
        float answer = 2*number_matches / (seqlen+reflen);
        return answer;

    }

    /*
     * Returns the nth field in a tab separated line
    */
    std::string get_field(std::string &inputstring, int field) {

        std::string fieldstring;
        int field_number = 0;
        int i = 0;

        while (field_number != field) {
            if (inputstring[i] == '\t') {
                field_number += 1;
            }
            i += 1;
        }

        //... start reading
        while (i < inputstring.length() and inputstring[i] != '\t') {
            fieldstring.push_back(inputstring[i]);
            i += 1;
        }

        return fieldstring;
    }

    /*
     * Same function as above, but with as many fields as wanted
     */
    void get_fields(std::string &inputstring, std::set<int> fields, std::map<int,std::string> &output) {

        std::string fieldstring;
        int field_number = 0;
        int i = 0;
        int size_asked = fields.size();

        while(output.size() != size_asked){

            while ( find(fields.begin(),fields.end(),field_number) != fields.end()){ // While field_number can be found in the fields
                if (inputstring[i] == '\t') {
                    field_number += 1;
                }
                i += 1;
            }

            //... start reading
            while (i < inputstring.length() and inputstring[i] != '\t') {
                fieldstring.push_back(inputstring[i]);
                i += 1;
            }
            output[field_number] = fieldstring;
            field_number += 1;

            fieldstring.clear();
            i++;

        }

    }

    /*
     * From [1,4,5,6] (for instance), returns 1456
     */
    long listdigits_to_number(std::vector<int> listdigits){
        long final_digit = 0;
        long append;
        int lsize = listdigits.size();

        for (int i = 0; i < lsize; i++){
            append = std::pow(10,(lsize-i-1)) * listdigits[i];
            final_digit += append;
        }

        return final_digit;
    }

    /*
     * Modifies its second argument to make a std::map<string,long> correponding to the parsed CIGAR
     */
    void parse_CIGAR(std::string &CIGARstring,std::map<std::string,long> &outputmap)
    {

//        std::map<std::string,long> outputmap;
        outputmap["*"] = 0;
        outputmap["I"] = 0;
        outputmap["M"] = 0;
        outputmap["N"] = 0;
        outputmap["P"] = 0;
        outputmap["="] = 0;
        outputmap["D"] = 0;
        outputmap["X"] = 0;
        outputmap["H"] = 0;
        outputmap["S"] = 0;

        char tmp_char;
        std::vector<int> tmplist_digits;

        for(int i = 0; i < CIGARstring.length();i++){
            tmp_char = CIGARstring[i];

            if(isdigit(tmp_char)){
                int this_integer = (int) tmp_char - 48; // ASCII
                tmplist_digits.push_back(this_integer);
            }
            else{
                if (tmp_char == '*'){
                    outputmap["*"] = 1;
                    return;
                }

                long nb_nt = listdigits_to_number(tmplist_digits);
                tmplist_digits.clear();

                auto align_type = std::string();
                align_type.push_back(tmp_char);

                outputmap[align_type] = outputmap[align_type]+nb_nt;
            }
        }

    }



}