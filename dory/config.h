#pragma once
#include <memory>
#include <string>
#include <sstream> //序列化
#include <boost/lexical_cast.hpp> //内纯转化
#include "log.h"
#include <yaml-cpp/yaml.h>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace dory
{
class ConfigVarbase {
public:
    typedef std::shared_ptr<ConfigVarbase> ptr;
    ConfigVarbase(const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description) {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);//字母都转为小写
    }
    virtual ~ConfigVarbase() {}
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;
    virtual std::string getTypeName() const = 0;
private:
    std::string m_name;
    std::string m_description;
};

//F from_type, T to_type
template<class F, class T>
class LexicalCast {
public:
    T operator() (const F& v) {
        return boost::lexical_cast<T>(v);
    }
};

//以下为偏特化类型
template<class T>
class LexicalCast<std::string, std::vector<T> > {
public:
    std::vector<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::vector<T> vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator() (const std::vector<T>& v) {
        YAML::Node node;
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::list<T> > {
public:
    std::list<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::list<T> lst;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            lst.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return lst;
    }
};

template<class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator() (const std::list<T>& v) {
        YAML::Node node;
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::set<T> > {
public:
    std::set<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::set<T> st;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            st.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return st;
    }
};

template<class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator() (const std::set<T>& v) {
        YAML::Node node;
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::unordered_set<T> > {
public:
    std::unordered_set<T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_set<T> st;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            st.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return st;
    }
};

template<class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator() (const std::unordered_set<T>& v) {
        YAML::Node node;
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::map<std::string, T> > {
public:
    std::map<std::string, T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::map<std::string, T> mp;
        std::stringstream ss;
        for (auto it = node.begin();
                it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            mp.insert(std::make_pair(it->first.Scalar(),  LexicalCast<std::string, T>()(ss.str())));
        }
        return mp;
    }
};

template<class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator() (const std::map<std::string, T>& v) {
        YAML::Node node;
        for (auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::unordered_map<std::string, T> > {
public:
    std::unordered_map<std::string, T> operator() (const std::string& v) {
        YAML::Node node = YAML::Load(v);
        typename std::unordered_map<std::string, T> mp;
        std::stringstream ss;
        for (auto it = node.begin();
                it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            mp.insert(std::make_pair(it->first.Scalar(),  LexicalCast<std::string, T>()(ss.str())));
        }
        return mp;
    }
};

template<class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator() (const std::unordered_map<std::string, T>& v) {
        YAML::Node node;
        for (auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

//FromStr operator()(const std::string&)
//ToStr std::string operator()(const T&)
template<class T, class FromStr = LexicalCast<std::string, T>
                ,class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarbase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;
    typedef std::function<void (const T& old_value, const T& new_value)> on_change_cb;

    ConfigVar(const std::string& name
            ,const T& default_value
            ,const std::string description = "")
        : ConfigVarbase(name, description)
        ,m_val(default_value) {
    }

    std::string toString() override {
        try {
            //return boost::lexical_cast<std::string>(m_val);//lexical_cast字面值转换，支持以文本形式表示的任意类型之间的公共转换
            return ToStr()(m_val);
        } catch (std::exception& e) {                      //lexical_cast无法执行转换操作时会抛出异常bad_lexical_cast，为保证健壮性需要使用try/catch来保护代码
            DORY_LOG_ERROR(DORY_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << " convert: " << typeid(m_val).name() << " to string";
        }
        return "";
    }

    bool fromString(const std::string& val) override {
        try {
            //m_val = boost::lexical_cast<T>(val);
            setValue(FromStr()(val));
        } catch (std::exception& e) {
            DORY_LOG_ERROR(DORY_LOG_ROOT()) << "ConfigVar::toString exception "
                << e.what() << " convert: string to " << typeid(m_val).name() << " - " << val;
        }
        return false;
    }
    const T getValue() const { return m_val; }

    void setValue(const T& v) { 
        if (v == m_val) { //新值旧值相同直接返回
            return;
        }
        for (auto& i : m_cbs) { //调用回调函数
            (i.second)(m_val, v);
        }
        m_val = v;
    }
    std::string getTypeName() const override { return typeid(T).name(); }

    //监听
    void addListener(uint64_t key, on_change_cb cb) {
        m_cbs[key] = cb;
    }

    //删除
    void delListener(uint64_t key) {
        m_cbs.erase(key);
    }

    //获取
    on_change_cb getListener(uint64_t key) {
        auto it = m_cbs.find(key);
        return it == m_cbs.end() ? nullptr : it->second;
    }

    void clearListener() {
        m_cbs.clear();
    }
private:
    T m_val;

    //变更回调函数组，function没有比较函数，因此用map存储，方便删除
    //uint64_t key, 要求唯一，一般可以用hash
    std::map<uint64_t, on_change_cb> m_cbs;
};

class Config
{
public:
    typedef std::map<std::string, ConfigVarbase::ptr> ConfigVarMap;//v放基类，可以动态绑定

    //创建和查找
    //关键字name，默认值default_value，描述信息description
    //如果没有查找到就创建并返回该ConfigVar，如果查找到了直接返回结果
    template<class T> 
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
            const T& default_value, const std::string& description = "") {
        auto it = s_datas.find(name);
        if (it != s_datas.end()) { //key存在
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T> >(it->second);//转换失败返回空指针
            if (tmp) {  //key存在且类型相同
                DORY_LOG_INFO(DORY_LOG_ROOT()) << "Lookup name=" << name << " exists";
                return tmp;
            } else {    //key存在且类型不同，报错
                DORY_LOG_ERROR(DORY_LOG_ROOT()) << "Lookup name=" << name << " exists but type not " 
                        << typeid(T).name() << " real_type=" << it->second->getTypeName()
                        << " " << it->second->toString();
                return nullptr;
            }
        }

        if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._0123456789")
                != std::string::npos) {
            DORY_LOG_ERROR(DORY_LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, default_value, description));
        s_datas[name] = v;
        return v;
    }

    //根据name查找
    //return:成功-ConfigVar指针 失败-nullptr
    template<class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        auto it = s_datas.find(name);
        if (it == s_datas.end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T> > (it->second);
    }

    static void LoadFromYaml(const YAML::Node& root);
    static ConfigVarbase::ptr LookupBase(const std::string& name);
private:
    static ConfigVarMap s_datas;

};


} // namespace dory
