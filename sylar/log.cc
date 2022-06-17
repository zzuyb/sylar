#include<log.h>
#include<iostream>
#include<vector>
namespace sylar{
Logger::Logger(const std::string& name = "root"){

}

void Logger::log(LogLevel::Level level, const LogEvent::ptr& event){
    if(level>=m_level){
        for(auto& i : m_appenders){
            i->log(level,event);
        }
    }
}
void Logger::debug(LogEvent::ptr event){
    log(LogLevel::DEBUG, event);
}
void Logger::info(LogEvent::ptr event){
    log(LogLevel::INFO, event);
}
void Logger::warn(LogEvent::ptr event){
    log(LogLevel::WARN, event);
}
void Logger::error(LogEvent::ptr event){
    log(LogLevel::ERROR, event);
}
void Logger::fatal(LogEvent::ptr event){
    log(LogLevel::FATAL, event);
}
void Logger::addAppender(LogAppender::ptr appender){
    m_appenders.push_back(appender);
}
void Logger::delAppender(LogAppender::ptr appender){
    for(auto it = m_appenders.begin(); it != m_appenders.end(); ++it){
        if(*it == appender){
            m_appenders.erase(it);
            break;
        }
    }
}

FileLogAppender::FileLogAppender(const std::string& filename){

}
void FileLogAppender::log(LogLevel::Level level, const LogEvent::ptr& event){
    if(level>=m_level){
        m_fileofs<<m_formatter->format(event);
    }
}

bool FileLogAppender::reopen(){
    if(m_fileofs){
        m_fileofs.close();
    }
    m_fileofs.open(m_filename);
    return !!m_fileofs;//!!表示强制转换为bool 非0为true，0为false
}
void StdoutLogAppender::log(LogLevel::Level level, const LogEvent::ptr& event){
    if(level >= m_level){
        std::cout << m_formatter->format(event) << std::endl;
    }
}
LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern){}

std::string LogFormatter::format(const LogEvent::ptr& ev){
    std::stringstream ss;
    for(auto& i : m_items){
        i->format(ss,ev);
    }
    return ss.str();
}
//%xxx %xxx{xxx} %%
void LogFormatter::init(){
    //str,format,type
    std::vector<std::tuple<std::string,std::string,int>> vec;
    std::string str;
    for(size_t i=0;i<m_pattern.size();++i){
        if(m_pattern[i] == '%'){
            if(i+1 < m_pattern.size() && m_pattern[i+1] == '%'){
                str.push_back(m_pattern[i]);
                str.push_back(m_pattern[i+1]);
                i++;
                vec.push_back(std::make_tuple(str,str,0));
                str.clear();
            }else{
                str.push_back(m_pattern[i]);
                if(i+1 < m_pattern.size() && m_pattern[i+1] == '{'){
                    i++;
                    while(i < m_pattern.size() && m_pattern[i] != '}'){
                        str.push_back(m_pattern[i]);
                        i++;
                    }
                    if(i < m_pattern.size()){
                        str.push_back(m_pattern[i]);
                        vec.push_back(std::make_tuple(str,str,0));
                        str.clear();
                    }
                }else{
                    vec.push_back(std::make_tuple(str,str,0));
                    str.clear();
                }
            }
        }else{
            str.push_back(m_pattern[i]);
        }
    }
}
}