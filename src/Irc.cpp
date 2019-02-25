#include "Irc.hpp"

Irc::Irc(Log* logger, Pref* preferences)
: RWait(true)
, Logger(logger)
, Preferences(preferences) {}

bool Irc::Connect() {
    int status;
    struct addrinfo hints;
    struct addrinfo *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string port(std::to_string(Preferences->Config.serverPort));
    status = getaddrinfo(Preferences->Config.serverHost.c_str(), port.c_str(), &hints, &res);
    if (status != 0) {
        Logger->Write(Log::Level::Error, "getaddrinfo error ", gai_strerror(status));
        freeaddrinfo(res);
        return false;
    }

    Socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (Socket == -1) {
        Logger->Write(Log::Level::Error, "Failed to create socket: ", strerror(errno));
        freeaddrinfo(res);
        return false;
    }

    if (connect(Socket, res->ai_addr, res->ai_addrlen) == -1) {
        Logger->Write(Log::Level::Error, "Failed to connect to socket: ", strerror(errno));
        freeaddrinfo(res);
        return false;
    }
    freeaddrinfo(res);

    IsUp = true;
    Logger->Write("Connected to ", Preferences->Config.serverHost, " on port ", Preferences->Config.serverPort);
    return true;
}

void Irc::Send(std::string toWrite) {
    std::stringstream ss(toWrite);
    std::string to;

    while (std::getline(ss,to,'\n')) {
        to += "\r\n";
        if (send(Socket, to.c_str(), to.length(), 0) == -1) {
            IsUp = false;
            Logger->Write(Log::Level::Error, "Failed to send message: ", to.substr(0, to.length() - 2));
        } else {
            Logger->Write("Sent: ", to.substr(0, to.length() - 2));
        }
    }
}

void Irc::Register() {
    if (Preferences->Config.pass.length() > 0) {
        Send("PASS " + Preferences->Config.pass);
    }
    Send("NICK " + Preferences->Config.nickname);
    Send("USER " + Preferences->Config.username + " 8 localhost :" + Preferences->Config.realname);
}

bool Irc::Receive(std::string& rec, std::vector<std::string>& vec) {
    if (ReceiveBuffer.size() > 0) {
        std::string raw = ReceiveBuffer[0];
        ReceiveBuffer.erase(ReceiveBuffer.begin());
        Logger->Write("Received: ", raw);
        rec = raw;
        Helpers::Split(vec, raw, " ");
        if (vec.size() < 6) {
            vec.resize(6);
        }

        if (vec[0] == "PING") {
            Send("PONG " + vec[1]);
        }

        if (vec[1] == "001") {
            for (auto const& x : Preferences->Config.startupCommands) {
                Send(x);
            }
            for (auto const& x : Preferences->Assets.channels) {
                Send("JOIN " + x.first + " " + x.second);
            }
        }

        rec = raw;
        return true;
    } else {
        int rc;
        if (RWait) {
            rc = recv(Socket, &Buffer, 4096, 0);
        } else {
            rc = recv(Socket, &Buffer, 4096, MSG_DONTWAIT);
        }

        if (rc == 0) {
            IsUp = false;
            return false;
        } else if (rc == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return false;
            } else {
                Logger->Write(Log::Level::Error, "Socket Error - See Log");
                IsUp = false;
                return false;
            }
        } else {
            std::string raw(Buffer, rc);
            Helpers::Split(ReceiveBuffer, raw, "\r\n");
            return Receive(rec, vec);
        }
    }
}

void Irc::Terminate() {
    Send("QUIT :Bye!");
    close(Socket);
    IsUp = false;
}
