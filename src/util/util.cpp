//------------------------------------------------------------------
// util.cpp
//
// Author:           JuanJakobo
// Date:             14.06.2020
//
//-------------------------------------------------------------------
#include "util.h"
#include "inkview.h"
#include "eventHandler.h"

#include <string>
#include <math.h>

#include <signal.h>

pid_t child_pid = -1;

using std::string;

size_t Util::writeCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

size_t Util::writeData(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written = iv_fwrite(ptr, size, nmemb, stream);
    return written;
}

//https://github.com/pmartin/pocketbook-demo/blob/master/devutils/wifi.cpp
bool Util::connectToNetwork()
{
    iv_netinfo *netinfo = NetInfo();
    if (netinfo->connected)
        return true;

    const char *network_name = nullptr;
    int result = NetConnect2(network_name, 1);
    if (result != 0)
    {
        return false;
    }

    netinfo = NetInfo();
    if (netinfo->connected)
        return true;

    return false;
}

int Util::progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
    if (dltotal <= 0.0)
        return 0;

    int percentage = round(dlnow / dltotal * 100);
    if (percentage % 10 == 0)
        UpdateProgressbar("Ausgabe wird heruntergeladen.", percentage);

    return 0;
}

string Util::readStringSetting(const std::string &name)
{
    iconfigedit *temp = nullptr;
    iconfig *SettingsConfig = OpenConfig(SETTINGS_PATH.c_str(), temp);
    std::string value = ReadString(SettingsConfig, name.c_str(), "");
    CloseConfigNoSave(SettingsConfig);

    return value;
}

void Util::writeStringSetting(const std::string &name, const std::string &value)
{
    iconfigedit *temp = nullptr;
    iconfig *SettingsConfig = OpenConfig(SETTINGS_PATH.c_str(), temp);
    WriteString(SettingsConfig, name.c_str(), value.c_str());
    CloseConfig(SettingsConfig);
}

void kill_child(int sig)
{
    //SIGKILL
    kill(child_pid, SIGTERM);
}

void Util::updatePBLibrary(int seconds)
{
    UpdateProgressbar("Aktualisieren der Pocketbook Bibliothek.", 99);
    //https://stackoverflow.com/questions/6501522/how-to-kill-a-child-process-by-the-parent-process
    signal(SIGALRM, (void (*)(int))kill_child);
    child_pid = fork();
    if (child_pid > 0)
    {
        //parent
        alarm(seconds);
        wait(NULL);
    }
    else if (child_pid == 0)
    {
        //child
        string cmd = "/ebrmain/bin/scanner.app";
        execlp(cmd.c_str(), cmd.c_str(), (char *)NULL);
        exit(1);
    }
}
