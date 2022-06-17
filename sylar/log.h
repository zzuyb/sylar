#ifndef __SYLAR_LOG_R__
#define __SYLAR_LOG_R__
#include<string>
#include<stdint.h>
#include<memory>
#include<list>
#include<sstream>
#include<fstream>
namespace sylar{
//日志时间类型
class LogEvent{
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent();

        const char* getFile() const {return m_file;}
        int32_t getLine() const {return m_line;}
        uint32_t getElapse() const {return m_elapse;}
        uint32_t getThreadId() const {return m_threadId;}
        uint32_t getFiberId() const {return m_fiberId;}
        const std::string& getContent () const {return m_content;}
    private:
        const char* m_file=nullptr;//文件名
        int32_t m_line;//行号
        uint32_t m_elapse;//耗时
        uint32_t m_threadId;//线程id
        uint32_t m_fiberId;//协程id
        uint64_t m_time;//时间戳
        std::string m_content;//日志内容
};
//日志级别
class LogLevel{
    public:
        enum Level{
            UNKNOW=0, 
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5,
        };

        static const char* ToString(Level level);
};
//日志格式器
class LogFormatter{
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string& pattern);
        
        std::string format(LogLevel::Level level, const LogEvent::ptr& ev);
    public:
        class FormatItem{
            public:
                typedef std::shared_ptr<FormatItem> ptr;
                virtual ~FormatItem(){}
                virtual void format(std::ostream os,LogLevel::Level level, const LogEvent::ptr& ev) = 0; 
        };
        void init();  
    private:
        bool m_error=false;
        std::string m_pattern;
        std::list<FormatItem::ptr> m_items;
};

//日志输出地
class LogAppender{
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender(){}
        virtual void log(LogLevel::Level level,const LogEvent::ptr& ev) = 0;
        void setFormattor(LogFormatter::ptr formatter){m_formatter = formatter;}
        LogFormatter::ptr getFormatter(){return m_formatter;}
    protected:
        LogLevel::Level m_level;
        LogFormatter::ptr m_formatter;
};

//日志器
class Logger{
    public:
        typedef std::shared_ptr<Logger> ptr;
        Logger(const std::string& name = "root");
        void log(LogLevel::Level level, const LogEvent::ptr& event);

        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);
        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);

        LogLevel::Level getLevel() const { return m_level; }
        void setLevel(LogLevel::Level level) { m_level = level; }

    private:
        std::string m_name;//日志器名称
        LogLevel::Level m_level;//日志级别
        std::list <LogAppender::ptr> m_appenders;//日志输出地
};

class StdoutLogAppender : public LogAppender{
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(LogLevel::Level level, const LogEvent::ptr& event) override;
};
class FileLogAppender : public LogAppender{
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string& filename);
        void log(LogLevel::Level level, const LogEvent::ptr& event) override;
        bool reopen();
    private:
        std::string m_filename;
        std::ofstream m_fileofs;
};
}
#endif