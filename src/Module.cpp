#include "Module.hpp"

Module::Module()
: pid(0) {
}

bool Module::Alive() {
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);
    if (result == 0) {
        return true;
    } else {
        return false;
    }
}

bool Module::Start(const std::string& file) {
    int inpipefd[2];
    int outpipefd[2];

    pipe2(inpipefd, O_NONBLOCK);
    pipe(outpipefd);

    pid = fork();

    if (pid == 0) {
        dup2(outpipefd[0], STDIN_FILENO);
        dup2(inpipefd[1], STDOUT_FILENO);
        dup2(inpipefd[1], STDERR_FILENO);

        prctl(PR_SET_PDEATHSIG, SIGTERM);

        execl(file.c_str(), "mod", NULL);
        exit(143);
    } else if (pid < 0) {
        return false;
    } else {
        close(outpipefd[0]);
        close(inpipefd[1]);

        writefd = outpipefd[1];
        readfd = inpipefd[0];

        name = file;
        return Alive();
    }
}

void Module::Send(const std::string& str) {
    std::string snd = str + "\n";
    write(writefd, snd.c_str(), snd.size());
}

void Module::Refresh() {
    Send("0rf");
}

int Module::Get(std::string& rec) {
    char readbuf[256];
    int offset = 0;
    int ret = 1;
    do {
        ret = read(readfd, readbuf + offset, 1);
        ++offset;
    } while (ret == 1 && offset < 255 && readbuf[offset-1] != '\n');

    if (offset-1 != 0) {
        rec = std::string(readbuf, offset);
        return 1;
    }

    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return 0;
    } else {
        return -1;
    }
}

void Module::Terminate() {
    int status;
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);
}
