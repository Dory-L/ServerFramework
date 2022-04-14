#pragma once
#include <memory>
#include <string>
#include <sstream> //序列化
#include <boost/lexical_cast.hpp> //内纯转化
#include "log.h"
#include <yaml-cpp/yaml.h>

namespace dory
{
class ConfigVarbase {
public:
    typedef std::shared_ptr<ConfigVarbase> ptr;
    ConfigVarbase(const std::string& name, const std::string& description = "")
        :m_name(name)
        ,m_description(description) {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }
    virtual ~ConfigVarbase() {}
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }

    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;
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

//FromStr operator()(const std::string&)
//ToStr std::string operator()(const T&)
template<class T, class FromStr = LexicalCast<std::string, T>
                ,class ToStr = LexicalCast<T, std::string> >
class ConfigVar : public ConfigVarbase {
public:
    typedef std::shared_ptr<ConfigVar> ptr;

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
            DORY_LOG_ERROR(DORY_LOG_ROOT()) << "ConfigVar::toString exception"
                << e.what() << " convert: string to" << typeid(m_val).name();
        }
        return false;
    }
    const T getValue() const { return m_val; }
    void setValue(const T& v) { m_val = v; }
private:
    T m_val;
};

class Config
{
public:
    typedef std::map<std::string, ConfigVarbase::ptr> ConfigVarMap;//v放基类，可以动态绑定

    template<class T>

    //创建和查找
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
            const T& default_value, const std::string& description = "") {
        auto tmp = Lookup<T>(name);
        if (tmp) {
            DORY_LOG_INFO(DORY_LOG_ROOT()) << "Lookup name=" << name << " exists";
            return tmp;
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