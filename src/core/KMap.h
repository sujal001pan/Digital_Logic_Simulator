#pragma once

#include <vector>
#include <string>
#include "dls/Types.h"

namespace dls {

/**
 * @brief Generates Gray-code ordered Karnaugh Maps for 2, 3 and 4 variables.
 */
class KMapGenerator {
private:
    int m_numVars = 0;
    int m_rowCount = 0;
    int m_colCount = 0;

    std::vector<std::vector<KMapCell>> m_grid;

    const std::vector<int> GRAY1 = {0,1};
    const std::vector<int> GRAY2 = {0,1,3,2};

public:
    KMapGenerator() = default;

    /**
     * @brief Generates a K-map from a truth table.
     * @param table Truth table generated from ExpressionTree.
     * @param numVars Number of variables (2–4).
     */
    void generate(const std::vector<TruthRow>& table, int numVars){

        m_numVars = numVars;
        m_grid.clear();

        switch(numVars){

        case 2:
            m_rowCount = 2;
            m_colCount = 2;
            break;

        case 3:
            m_rowCount = 2;
            m_colCount = 4;
            break;

        case 4:
            m_rowCount = 4;
            m_colCount = 4;
            break;

        default:
            return;
        }

        m_grid.assign(m_rowCount,
                      std::vector<KMapCell>(m_colCount));

        const auto& rowGray =
            (m_rowCount==4)?GRAY2:GRAY1;

        const auto& colGray =
            (m_colCount==4)?GRAY2:GRAY1;

        for(int r=0;r<m_rowCount;r++){

            for(int c=0;c<m_colCount;c++){

                int rowBits=rowGray[r];
                int colBits=colGray[c];

                int minterm=0;

                if(numVars==2){

                    minterm=(rowBits<<1)|colBits;
                }
                else if(numVars==3){

                    minterm=(rowBits<<2)|colBits;
                }
                else{

                    minterm=(rowBits<<2)|colBits;
                }

                bool value=false;

                if(minterm<(int)table.size())
                    value=table[minterm].output;

                m_grid[r][c]={
                    r,
                    c,
                    minterm,
                    value
                };
            }
        }
    }

    const std::vector<std::vector<KMapCell>>& getGrid() const{
        return m_grid;
    }

    int getRowCount() const{
        return m_rowCount;
    }

    int getColCount() const{
        return m_colCount;
    }

    // Row labels for GUI rendering.
    std::vector<std::string> getRowLabels() const{

        if(m_numVars==2)
            return {"0","1"};

        if(m_numVars==3)
            return {"0","1"};

        if(m_numVars==4)
            return {"00","01","11","10"};

        return {};
    }

    // Column labels for GUI rendering.
    std::vector<std::string> getColLabels() const{

        if(m_numVars==2)
            return {"0","1"};

        if(m_numVars==3)
            return {"00","01","11","10"};

        if(m_numVars==4)
            return {"00","01","11","10"};

        return {};
    }
};

} // namespace dls