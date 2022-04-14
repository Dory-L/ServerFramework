#include "../dory/config.h"
#include "../dory/log.h"
#include <yaml-cpp/yaml.h>

dory::ConfigVar<int>::ptr g_int_value_config = 
    dory::Config::Lookup("system.port", (int)8080, "system port");

dory::ConfigVar<float>::ptr g_float_value_config = 
    dory::Config::Lookup("system.value", (float)10.2f, "system value");

dory::ConfigVar<std::vector<int>>::ptr g_int_vec_value_config = 
    dory::Config::Lookup("system.int_vec", std::vector<int> {1,2}, "system int vec");

//测试yaml
void print_yaml(const YAML::Node& node, int level) {
    if (node.IsScalar()) {
        DORY_LOG_INFO(DORY_LOG_ROOT()) << std::string(level * 4, ' ') << node.Scalar() << " - " << node.Type() << " - " << level; //NodeType: Undefined, Null, Scalar, Sequence, Map
    } else if (node.IsNull()) {
        DORY_LOG_INFO(DORY_LOG_ROOT()) << std::string(level * 4, ' ') << "NULL - " << node.Type() << " - " << level;
    } else if (node.IsMap()) {
        for (auto it = node.begin();
                it != node.end(); it++) {
            DORY_LOG_INFO(DORY_LOG_ROOT()) << std::string(level * 4, ' ') <<  it->first << " - " << it->second.Type() << " - " << level;
            print_yaml(it->second, level + 1);  
        }
    } else if (node.IsSequence()) {
        for (size_t i = 0; i < node.size(); i++) {
            DORY_LOG_INFO(DORY_LOG_ROOT()) << std::string(level * 4, ' ') << i << " - " << node[i].Type() << " - " << level;
            print_yaml(node[i], level + 1);
        }
    }
}

void test_yaml() {
    YAML::Node root = YAML::LoadFile("/home/lly/projectfile/dory/workspace/bin/conf/log.yml");
    print_yaml(root, 0);

    //DORY_LOG_INFO(DORY_LOG_ROOT()) << root;
}

//测试Config
void test_config() {
    DORY_LOG_INFO(DORY_LOG_ROOT()) << "before: " << g_int_value_config->getValue();
    DORY_LOG_INFO(DORY_LOG_ROOT()) << "before: " << g_float_value_config->toString();

    auto v = g_int_vec_value_config->getValue();
    for (auto& i : v) {
        DORY_LOG_INFO(DORY_LOG_ROOT()) << "before int_vec" << i;
    }

    YAML::Node root = YAML::LoadFile("/home/lly/projectfile/dory/workspace/bin/conf/log.yml");
    dory::Config::LoadFromYaml(root);

    DORY_LOG_INFO(DORY_LOG_ROOT()) << "after: " << g_int_value_config->getValue();
    DORY_LOG_INFO(DORY_LOG_ROOT()) << "after: " << g_float_value_config->toString();

    v = g_int_vec_value_config->getValue();
    for (auto& i : v) {
        DORY_LOG_INFO(DORY_LOG_ROOT()) << "after int_vec" << i;
    }
}

int main(int argc, char** argv) {
    
    test_config();
    //test_yaml();
    return 0;
}