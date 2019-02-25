#include "Manager.hpp"

Manager::Manager(bool prefix, bool modules)
: prefix(prefix)
, modules(modules) {}

bool Manager::Get(std::string& rec, bool blocking){
    if (RecBuf.size()) {
        rec = RecBuf[0];
        RecBuf.erase(RecBuf.begin());

        if (rec.substr(0, 3) == "0rf") {
            Reget();
            return Get(rec, blocking);
        }
        return true;
    } else {
        if (blocking) {
            std::string buf;
            getline(std::cin, buf);
            RecBuf.push_back(buf);
            return Get(rec, blocking);
        } else {
            char buf[512];
            int offset = 0;
            int ret = 1;

            fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) | O_NONBLOCK);
            do {
                ret = read(0, buf + offset, 1);
                ++offset;
            } while (ret == 1 && offset < 512 && buf[offset-1]);
            fcntl(0, F_SETFL, fcntl(0, F_GETFL, 0) ^ O_NONBLOCK);

            if (offset - 1 != 0) {
                std::string out(buf, offset);
                Helpers::Split(RecBuf, out, "\n");
                return Get(rec, blocking);
            }

            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return false;
            } else {
                return false;
            }
        }
    }
}

int Manager::GetPerms(const std::string& mask) {
    std::cout << "ga" << mask << std::endl;
    std::string buff;
    while (true) {
        getline(std::cin, buff);
        if (buff.substr(0, 3) == "0ga") {
            return stoi(buff.substr(3, buff.length() - 3));
        } else {
            RecBuf.push_back(buff);
        }
    }
}

void Manager::GetKey(const std::string& service, std::string& ans) {
    std::cout << "gk" << service << std::endl;
    std::string buff;
    while (true) {
        getline(std::cin, buff);
        if (buff.substr(0, 3) == "0gk") {
            ans = buff.substr(3, buff.length() - 3);
            return;
        } else {
            RecBuf.push_back(buff);
        }
    }
}

void Manager::IrcSend(const std::string& str) {
    std::cout << "is" << str << std::endl;
}

void Manager::Off() {
    std::cout << "it" << std::endl;
}

void Manager::MimicIrcSend(std::string& str) {
    std::cout << "im" << str << std::endl;
}

void Manager::Reget() {
    bool gprefix = true;
    bool gmodules = true;
    std::string buff;
    if (prefix) {
        gprefix = false;
        std::cout << "gp" << std::endl;
    }
    if (modules) {
        gmodules = false;
        std::cout << "gm" << std::endl;
    }
    while (true) {
        getline(std::cin, buff);
        std::string beg = buff.substr(0, 3);
        std::string end = buff.substr(3, buff.length() - 3);
        if (!gprefix && beg == "0gp") {
            Data.Prefix = end;
            gprefix = true;
        } else if (!gmodules && beg == "0gm") {
            Helpers::Split(Data.Modules, end, " ");
            gmodules = true;
        } else {
            RecBuf.push_back(buff);
        }
        if (gprefix && gmodules) {
            break;
        }
    }
}
