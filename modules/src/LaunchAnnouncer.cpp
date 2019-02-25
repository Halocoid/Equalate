#include "Manager.hpp"
#include "Helpers.hpp"
#include "NielsLohmannjson.hpp"

using js = nlohmann::json;

#include <string>
#include <vector>
#include <curl/curl.h>
#include <stdexcept>
#include <chrono>
#include <iostream>
#include <iomanip>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

static int GetData(std::string& str, const std::string& link) {
    CURL* curl = curl_easy_init();
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    return res;
}

static void TitleLaunch(std::string& out, const js api) {
    out += "\002\003";
    switch (api["status"].get<int>()) {
        case 1: out += "09Go"; break;
        case 2: out += "04TBD"; break;
        case 3: out += "11Success"; break;
        case 4: default: out += "06Failure"; break;
    }
    out += "\003\002 - \002" + api["name"].get<std::string>() + "\002 on " + api["windowstart"].get<std::string>();
    if (api["vidURLs"].size()) {
        out += " | " + api["vidURLs"][0].get<std::string>();
    }
}

int main() {
    Manager man(true, false);
    std::vector<std::string> ex;
    std::string buf;
    while (true) {
        man.Get(buf, true);
        if (buf.substr(0, 3) == "0rc") {
            buf.erase(0, 3);
            Helpers::Split(ex, buf, " ");
            ex.resize(7);

            if (ex[3] == ":" + man.Data.Prefix + "launch") {
                if (!ex[4].empty() && std::all_of(ex[4].begin(), ex[4].end(), ::isdigit)) {
                    ex[5] = ex[4];
                    ex[4] = "next";
                }

                if (ex[4] == "next") {
                    unsigned next = 1;
                    std::string output;
                    do {
                        try {
                            next = std::stoul(ex[5]);
                            if (!(next > 0 && next < 100)) {
                                next = 1;
                            }
                        } catch (std::exception&) {
                            output = "The value of next must be an integer between 0 and 100 (exclusive)";
                            break;
                        }
                        std::string ret;
                        std::string req = "http://launchlibrary.net/1.4/launch?mode=verbose&next=";
                        req += std::to_string(next);

                        if (GetData(ret, req)) {
                            output = "Curl error";
                            break;
                        }

                        js api = js::parse(ret)["launches"][next - 1];
                        TitleLaunch(output, api);
                    } while (false);
                    man.IrcSend(Helpers::GetDest(ex) + output);
                } else if (ex[4] == "search") {
                    unsigned next;
                    char buf[11];
                    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    std::strftime(buf, 11, "%F", std::localtime(&now));

                    try {
                        next = std::stoul(ex[6]);
                    } catch (std::exception&) {
                        next = 1;
                    }
                    std::string output, ret;
                    std::string req = "https://launchlibrary.net/1.4/launch?mode=verbose&limit=1&name=";
                    req += ex[5] + "&startdate=" + buf += "&offset=" + std::to_string(next - 1);


                    if (GetData(ret, req)) {
                        output += "Curl error";
                    } else {
                        js api = js::parse(ret)["launches"];
                        if (api.size()) {
                            TitleLaunch(output, api[0]);
                        } else {
                            output += "Not found";
                        }
                    }
                    man.IrcSend(Helpers::GetDest(ex) + output);
                } else if (ex[4] == "searchpast") {
                    unsigned next;
                    char buf[11];
                    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    std::strftime(buf, 11, "%F", std::localtime(&now));

                    try {
                        next = std::stoul(ex[6]);
                    } catch (std::exception&) {
                        next = 1;
                    }
                    std::string output, ret;
                    std::string req = "https://launchlibrary.net/1.4/launch?mode=verbose&sort=desc&limit=1&name=";
                    req += ex[5] + "&enddate=" + buf + "&offset=" + std::to_string(next - 1);

                    if (GetData(ret, req)) {
                        output += "Curl error";
                    } else {
                        js api = js::parse(ret)["launches"];
                        if (api.size()) {
                            TitleLaunch(output, api[0]);
                        } else {
                            output += "Not found";
                        }
                    }
                    man.IrcSend(Helpers::GetDest(ex) + output);
                } else {
                    if (ex[4] != "help") {
                        man.IrcSend(Helpers::GetDest(ex) + "Unrecognised option " + ex[4]);
                    }
                    man.IrcSend(Helpers::GetDest(ex) + "Usage: {[next] <num>|{search[past]} <term> [num]}");
                }
            }

            ex.clear();
        }
    }
    return 0;
}
