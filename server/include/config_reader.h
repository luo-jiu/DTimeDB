//
// Created by illumwang on 24-3-2.
//

#ifndef DTIMEDB_CONFIG_READER_H
#define DTIMEDB_CONFIG_READER_H
#include <string>
#include <vector>
#include <map>
typedef std::map<std::string, std::map<std::string, std::string>*> STR_MAP;
typedef STR_MAP::iterator STR_MAP_ITER;

class config_reader {
public:
    ~config_reader();
    std::string GetString(const std::string& section, const std::string& key, const std::string& default_value = "");
    std::vector<std::string> GetStringList(const std::string& section, const std::string& key);
    unsigned GetNumber(const std::string& section, const std::string& key, unsigned default_value = 0);
    bool GetBool(const std::string& section, const std::string& key, bool default_value = false);
    float GetFloat(const std::string& section, const std::string& key, const float& default_value);

    static bool setPath(const std::string& path);
    static config_reader *ins();
private:
    config_reader() { }

    bool isSection(std::string line, std::string& section);
    unsigned parseNumber(const std::string& s);
    std::string trimLeft(const std::string& s);
    std::string trimRight(const std::string& s);
    std::string trim(const std::string& s);
    bool Load(const std::string& path);

    static config_reader* config;

    STR_MAP _map;
};
#endif //DTIMEDB_CONFIG_READER_H
