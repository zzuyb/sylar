#include<log.h>
#include<iostream>
#include<vector>
namespace sylar{

const char*LogLevel::ToString(LogLevel::Level level) {
    switch(level){
#define XX(name) \
    case LogLevel::name:\
        return #name; \
        break;
    
    XX(DEBUG);
    XX(INFO);
    XX(WARN);
    XX(ERROR);
    XX(FATAL);

#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

Logger::Logger(const std::string& name)
    :m_name(name){}


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
        m_fileofs<<m_formatter->format(level,event);
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
        std::cout << m_formatter->format(level,event) << std::endl;
    }
}
LogFormatter::LogFormatter(const std::string& pattern)
    :m_pattern(pattern){}

std::string LogFormatter::format(LogLevel::Level level,const LogEvent::ptr& ev){
    std::stringstream ss;
    for(auto& i : m_items){
        i->format(ss,level,ev);
    }
    return ss.str();
}
//%xxx %xxx{xxx} %%
void LogFormatter::init(){
    //str,format,type
    std::vector<std::tuple<std::string,std::string,int>> vec;
    std::string nstr;
    for(size_t i=0;i<m_pattern.size();++i){
            if(m_pattern[i]!='%'){
                nstr.append(1,m_pattern[i]);
                continue;
            }

            if((i+1)<m_pattern.size()){
                if(m_pattern[i+1]=='%'){
                    nstr.append(1,'%');
                    continue;
                }
            }
            size_t n=i+1;
            int fmt_status=0;
            size_t fmt_begin=0;

            std::string str;
            std::string fmt;

            while(n<m_pattern.size()){
                if((!fmt_status)&&isalpha(m_pattern[n])&&(m_pattern[n]!='{')&&(m_pattern[n]!='}')){
                    str=m_pattern.substr(i+1,n-i-1);
                    break;
                }

                if(fmt_status==0){
                        if(m_pattern[n]=='{'){
                        str=m_pattern.substr(i+1,n-i-1);
                        fmt_status=1;
                        fmt_begin=n;
                        ++n;
                        continue;
                    }
                }
                if(fmt_status==1){
                    if(m_pattern[n]=='}'){
                        fmt=m_pattern.substr(fmt_begin+1,n-fmt_begin-1);
                        fmt_status=0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if(n==m_pattern.size()){
                    if(str.empty()){
                        str=m_pattern.substr(i+1);
                    }
                }
            }
            if(fmt_status==0){
                if(!nstr.empty()){
                    vec.push_back(std::make_tuple(nstr,std::string(),0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str,fmt,1));
                i=n-1;
            }else if(fmt_status==1){
                std::cout<<"pattern parse error: "<<m_pattern<<" - "<<m_pattern.substr(i)<<std::endl;
                m_error=true;
                vec.push_back(std::make_tuple("<<pattern_error>>",fmt,0));
            }
        }

        if(!nstr.empty()){
            vec.push_back(std::make_tuple(nstr,"",0));
        } 
        //%m -- 消息体
        //%p -- level
        //%r -- 启动后的时间
        //%c -- 日志名称
        //%t -- 线程id
        //%n -- 回车换行
        //%d -- 时间
        //%f -- 文件名
        //%l -- 行号
    }

class MessageFormatItem : public LogFormatter::FormatItem{
    public:
        void format(std::ostream& os, const LogEvent::ptr& ev) {
            os<<ev->getContent();
        }
};

class LevelFormatItem : public LogFormatter::FormatItem{
    public:
        void format(std::ostream& os, LogLevel::Level level,const LogEvent::ptr& ev) {
            os<<LogLevel::ToString(level);
        }
};




}