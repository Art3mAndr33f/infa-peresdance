#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <map>
#include <set>

#include <boost/property_tree/ptree.hpp>
using boost::property_tree::ptree;
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>

#include "account.h"
#include "password_item.h"

using namespace std;
/*
 * Storage: (хранит все пароли, которые соотв. данному юзеру)
 *      1. Инициализация: проверка существования файлов (их создание)                       [done]
 *      2. Проверка наличия данного аккаунта по логину в authorisation.xml (ExistAccount)   [done]
 *      3. Проверка данного мастер-пароля в authorisation.xml
 *      4. Запись Account в authorisation.xml (проверка уникальности в PassMan)             [done]
 *      5. Парсинг всех паролей соотв. логину -- вызывается в PassMan сразу после того,     [done]
 *              если пользователь успешно авторизовался
 *      6. Поиск паролей по переданному Email (без парсинга)                                [done]
 *      7. Поиск паролей по переданному AppName (без парсинга)                              [done]
 *      8. Добавление нового пароля (принимает PasswordItem)                                [done]
 *          8.0 Проверяет поступивший PasswordItem на уникальность
 *          8.1 Добавляет PasswordItem в контейнер
 *          8.2 Добавляет PasswordItem в файл data.xml
 *
 *      - шифрование            [to do]
 *      - замена зарезервированных XML символов     [done]
 * */

enum class AUTHORISE_RES {
    SUCCESS,
    INCORRECT,      // неверный пароль
    NOACC           // не зарегистрован
};

enum class FIND_RES {
    SUCCESS,
    NOTFOUND,
    ERROR
};

class Storage {
public:
    Storage();
    void Init();
    void ParseAllDataByLogin(const string& login);
    bool ExistDataFile();
    bool ExistAuthoFile();
    AUTHORISE_RES ConfirmAuthorisation(Account& account);
    bool RegisterAccount(Account& account);
    bool ExistAccount(const string& user_name);
    bool ExistPasswordItem(PasswordItem& pass_item);
    bool AddPasswordItem(PasswordItem& pass_item);
    static string PreprocessingToXML(string s);
    string PreprocessingFromXML(string s);
    boost::property_tree::ptree ChildByPasswordItem(PasswordItem& pass_item);
    pair<FIND_RES, vector<PasswordItem>> SelectAllByEmail(const string& email); // вызываются из PassMan для соотв. юзера
    pair<FIND_RES, vector<PasswordItem>> SelectAllByAppName(const string& app);
private:
    const string default_data_path_ = "data.xml";
    const string default_autho_path_ = "authorisation.xml";
    const string default_xml_data_ = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                        "<password_items></password_items>";
    const string default_autho_data_ = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                                       "<accounts></accounts>";
    string active_login;
    vector<PasswordItem> all_passwords_; // all password items by active_login user
};

string Storage::PreprocessingToXML(string s) {
    string res = "";
    for (size_t i = 0; i < s.length(); i++) {
        if (s[i] == '<') res += "&lt;";
        else if (s[i] == '>') res += "&gt;";
        else if (s[i] == '&') res += "&amp;";
        else if (s[i] == '\"') res += "&quot;";
        else if (s[i] == '\'') res += "&apos;";
        else res += s[i];
    }
    return res;
}

string Storage::PreprocessingFromXML(string s) {
    string res = "";
    for (size_t i = 0; i < s.length(); i++){
        if (s[i] == '&'){
            if (s[i+1] == 'l') {
                res += '<';
                i += 3;
            } else if (s[i+1] == 'g') {
                res += '>';
                i += 3;
            } else if (s[i+1] == 'q') {
                res += '\"';
                i += 5;
            } else if (s[i+1] == 'a' && s[i+2] == 'm') {
                res += '&';
                i += 4;
            } else if (s[i+1] == 'a' && s[i+2] == 'p') {
                res += '\'';
                i += 5;
            }
        } else res += s[i];
    }
    return res;
}

void Storage::Init() {
    if (!ExistDataFile()) {
        ofstream data(default_data_path_);
        if (data.is_open()) {}
        data << default_xml_data_ << endl;
    }
    if (!ExistAuthoFile()) {
        ofstream author(default_autho_path_);
        if (author.is_open()) {
            author << default_autho_data_ << endl;
        }
    }
}

pair<FIND_RES, vector<PasswordItem>> Storage::SelectAllByAppName(const string& app) {
    vector<PasswordItem> res_v = {};
    for(const auto& item : all_passwords_) {
        if(item.GetAppName() == app) {
            res_v.push_back(item);
        }
    }
    pair<FIND_RES, vector<PasswordItem>> result;
    if(res_v.empty()) {
        result.first = FIND_RES::NOTFOUND;
    } else {
        result.first = FIND_RES::SUCCESS;
    }
    result.second = res_v;
    return result;
}

pair<FIND_RES, vector<PasswordItem>> Storage::SelectAllByEmail(const string& email) {
    vector<PasswordItem> res_v = {};
    for(const auto& item : all_passwords_) {
        if(item.GetEmail() == email) {
            res_v.push_back(item);
        }
    }
    pair<FIND_RES, vector<PasswordItem>> result;
    if(res_v.empty()) {
        result.first = FIND_RES::NOTFOUND;
    } else {
        result.first = FIND_RES::SUCCESS;
    }
    result.second = res_v;
    return result;
}


void Storage::ParseAllDataByLogin(const string& login) {
    ptree pt;
    read_xml(default_data_path_, pt);
    BOOST_FOREACH(ptree::value_type& password_item, pt.get_child("password_items")) {
        string tmp_login = PreprocessingFromXML(password_item.second.get_child("login").get_value("default"));
        if(tmp_login == login) {
            string user_name = PreprocessingFromXML(password_item.second.get_child("user_name").get_value("default"));
            string password = PreprocessingFromXML(password_item.second.get_child("password").get_value("default"));
            string email = PreprocessingFromXML(password_item.second.get_child("email").get_value("default"));
            string url = PreprocessingFromXML(password_item.second.get_child("url").get_value("default"));
            string app_name = PreprocessingFromXML(password_item.second.get_child("app_name").get_value("default"));
            PasswordItem item(password, email, user_name, url, app_name);
            all_passwords_.push_back(item);
        }
    }
    active_login = login;
}


Storage::Storage() = default;

bool Storage::ExistDataFile() {
    struct stat buffer{};
    return stat(default_data_path_.c_str(), &buffer) != -1;
}

bool Storage::ExistAuthoFile() {
    struct stat buffer{};
    return stat(default_autho_path_.c_str(), &buffer) != -1;
}

AUTHORISE_RES Storage::ConfirmAuthorisation(Account &account) {
    // парсим xml с аакаунтами
    ptree pt;
    bool exist = false;
    read_xml(default_autho_path_, pt);
    BOOST_FOREACH(ptree::value_type & acc, pt.get_child("accounts")) {
        string tmp_user_name = PreprocessingFromXML(acc.second.get_child("user_name").get_value("default"));
        string tmp_master_pass = PreprocessingFromXML(acc.second.get_child("master_password").get_value("default"));
        if(account.GetUserName() == tmp_user_name) {
            exist = true;
            if(account.GetMasterPass() == tmp_master_pass) {
                return AUTHORISE_RES::SUCCESS;
            }
        }
    }
    return exist ? AUTHORISE_RES::INCORRECT : AUTHORISE_RES::NOACC;
}


bool Storage::ExistAccount(const string& user_name) {
    ptree pt;
    read_xml(default_autho_path_, pt);
    BOOST_FOREACH(ptree::value_type & acc, pt.get_child("accounts")) {
        string tmp_user_name = PreprocessingFromXML(acc.second.get_child("user_name").get_value("default"));
        if(user_name == tmp_user_name) {
            return true;
        }
    }
    return false;
}

bool Storage::RegisterAccount(Account& acc) {
    if(ExistAccount(acc.GetUserName())) {
        return false;
    }
    ptree pt;
    read_xml(default_autho_path_, pt);

    ptree child;
    child.add("user_name", PreprocessingToXML(acc.GetUserName()));
    child.add("master_password", PreprocessingToXML(acc.GetMasterPass()));
    pt.add_child("accounts.account", child);

    write_xml(default_autho_path_, pt);
    return true;
}

bool Storage::ExistPasswordItem(PasswordItem& pass_item) {
    return find(all_passwords_.begin(), all_passwords_.end(), pass_item) != all_passwords_.end();
}

boost::property_tree::ptree Storage::ChildByPasswordItem(PasswordItem& pass_item) {
    ptree child;
    child.add("login", PreprocessingToXML(active_login));
    child.add("password", PreprocessingToXML(pass_item.GetPassword()));
    child.add("email", PreprocessingToXML(pass_item.GetEmail()));
    child.add("user_name", PreprocessingToXML(pass_item.GetEmail()));
    child.add("url", PreprocessingToXML(pass_item.GetUrl()));
    child.add("app_name", PreprocessingToXML(pass_item.GetAppName()));

    return child;
}

bool Storage::AddPasswordItem(PasswordItem& pass_item) {
    if(ExistPasswordItem(pass_item)) {
        return false;
    }
    ptree pt;
    read_xml(default_data_path_, pt);

    ptree child = ChildByPasswordItem(pass_item);
    pt.add_child("password_items.password_item", child);
    write_xml(default_data_path_, pt);
    all_passwords_.push_back(pass_item);
    return true;
}
