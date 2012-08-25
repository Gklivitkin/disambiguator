/** @file */

#ifndef PATENT_DISAMBIGMAIN_H
#define PATENT_DISAMBIGMAIN_H

namespace EngineConfiguration {
    bool config_engine( const char * filename, std::ostream & os );
}

namespace BlockingConfiguration {
    class cBlockingDetail {
    public:
        StringManipulator * m_psm;
        std::string m_columnname;
        unsigned int m_dataindex;
        int m_begin;
        unsigned int m_nchar;
        bool m_isforward;
        cBlockingDetail()  {
            m_psm  = new StringNoSpaceTruncate ;
        }
        cBlockingDetail ( const cBlockingDetail & rhs ) {
            m_psm = rhs.m_psm->clone();
            m_columnname = rhs.m_columnname;
            m_dataindex = rhs.m_dataindex;
            m_begin = rhs.m_begin;
            m_nchar = rhs.m_nchar;
            m_isforward = rhs.m_isforward;
        }
        ~cBlockingDetail() { delete m_psm; }
    };
    int config_blocking ( const char * filename, const string & module_id);
}

int Full_Disambiguation( const char * EngineConfigFile, const char * BlockingConfigFile );




#endif /* PATENT_DISAMBIGMAIN_H */
