#pragma once

#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>

#include "dls/Types.h"

namespace dls {

class QuineMcCluskeySimplifier {

private:
    int m_numVars;
    std::vector<std::string> m_varNames;

    std::string toBinary(int value) const {

        std::string bits;

        for(int i=m_numVars-1;i>=0;i--)
            bits += ((value>>i)&1)?'1':'0';

        return bits;
    }

    bool canCombine(const std::string& a,
                    const std::string& b,
                    int& diff) const {

        diff=-1;
        int count=0;

        for(size_t i=0;i<a.size();i++){

            if(a[i]!=b[i]){

                count++;
                diff=(int)i;
            }
        }

        return count==1;
    }

    std::string termToExpression(const std::string& term) const{

        std::string out;

        for(size_t i=0;i<term.size();i++){

            if(term[i]=='1')
                out += m_varNames[i];

            else if(term[i]=='0')
                out += m_varNames[i] + "'";

            if(i+1<term.size() &&
               (term[i]=='0'||term[i]=='1'))
                out += ".";
        }

        while(!out.empty() && out.back()=='.')
            out.pop_back();

        return out.empty() ? "1" : out;
    }

public:

    QuineMcCluskeySimplifier(int vars,
                             std::vector<std::string> names)
        : m_numVars(vars),
          m_varNames(std::move(names)){}

    std::pair<std::string,std::vector<Grouping>>
    simplify(const std::vector<int>& minterms){

        if(minterms.empty())
            return {"0",{}};

        std::map<std::string,std::vector<int>> current;

        for(int m:minterms)
            current[toBinary(m)]={m};

        std::map<std::string,std::vector<int>> primeTerms;

        while(true){

            bool changed=false;

            std::map<std::string,std::vector<int>> next;
            std::set<std::string> used;

            for(auto it1=current.begin();
                it1!=current.end();++it1){

                for(auto it2=std::next(it1);
                    it2!=current.end();++it2){

                    int diff;

                    if(canCombine(it1->first,
                                  it2->first,
                                  diff)){

                        changed=true;

                        used.insert(it1->first);
                        used.insert(it2->first);

                        std::string combined=it1->first;
                        combined[diff]='-';

                        auto merged=it1->second;
                        merged.insert(merged.end(),
                                      it2->second.begin(),
                                      it2->second.end());

                        std::sort(merged.begin(),merged.end());
                        merged.erase(
                            std::unique(merged.begin(),merged.end()),
                            merged.end());

                        next[combined]=merged;
                    }
                }
            }

            for(auto& t:current){

                if(!used.count(t.first))
                    primeTerms[t.first]=t.second;
            }

            if(!changed)
                break;

            current=next;
        }

        std::vector<Grouping> groups;
        std::set<int> covered;
        std::string expression;

        for(auto& pi:primeTerms){

            bool useful=false;

            for(int m:pi.second){

                if(!covered.count(m)){
                    useful=true;
                    covered.insert(m);
                }
            }

            if(useful){

                Grouping g;
                g.mintermsCovered=pi.second;
                g.termExpression=termToExpression(pi.first);

                groups.push_back(g);

                if(!expression.empty())
                    expression+=" + ";

                expression+=g.termExpression;
            }
        }

        return {expression,groups};
    }
};

} // namespace dls