#pragma once
#include "windows.h"
#include <string>

class CProfile
{
public:
    CProfile(void) ;
    virtual ~CProfile(void);
    void SetFile( std::string name ) {
        m_filename = name;
    }
    void SetSection( std::string section ) {
        m_section = section;
    }
    int WriteString( std::string key, std::string value ) {
        return WritePrivateProfileStringA( m_section.c_str(), key.c_str(), value.c_str(), (m_path + m_filename).c_str() );
    }
    int WriteInteger( std::string key, int value ) {
        char buff[1024];
        sprintf( buff, "%d", value );
        return WriteString( key, buff );
    }
    int ReadString( std::string key, std::string& value ) {
        char buff[1024];
        int ret =  GetPrivateProfileStringA( m_section.c_str(), key.c_str(), "", buff, 1024, (m_path + m_filename).c_str() );
        if ( ret > 0 ) value = buff;
        return ret;
    }
    bool IsInteger( std::string key ) {
        ReadInteger( key );
        return m_errcode == 0;
    }
    int ReadInteger( std::string key ) {
        std::string s;
        if ( ReadString( key, s ) > 0 ) {
            int ret;
            if ( sscanf( s.c_str(), "%d", &ret ) > 0 ) {
                m_errcode = 0;
                return ret;
            }
            m_errcode = 1;
        }
        m_errcode = 2;
        return 0;
    }
private:
    int m_errcode;
    std::string m_path;
    std::string m_filename;
    std::string m_section;
};

