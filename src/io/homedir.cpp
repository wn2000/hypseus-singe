/*
 *  homedir.cpp
 */

#include "config.h"

#include "homedir.h"
#include "mpo_fileio.h"
#include "conout.h"

#include <filesystem>
namespace fs = std::filesystem;

homedir g_homedir; // our main homedir class (statically allocated to minimize
                   // risk of memory leak)

// no arguments because this is statically allocated
homedir::homedir()
{
    m_appdir = "."; // our current directory must be our app directory, so a '.'
                    // here is sufficient
    m_homedir = "."; // using curdir is a sensible default for the constructor
}

string homedir::get_homedir() const { return m_homedir; }

void homedir::set_homedir(const string &s)
{
    m_homedir = s;

    // create writable directories if they don't exist
    fs::path homepath = m_homedir;
    fs::create_directories(homepath);
    fs::create_directories(homepath / "ram");
    fs::create_directories(homepath / "roms");
    fs::create_directories(homepath / "logs");
    fs::create_directories(homepath / "fonts");
    fs::create_directories(homepath / "bezels");
    fs::create_directories(homepath / "screenshots");
}

string homedir::get_romfile(const string &s) const
{
    return find_file((fs::path("roms") / s).string(), true);
}

string homedir::get_ramfile(const string &s) const
{
    return find_file((fs::path("ram") / s).string(), false);
}

string homedir::get_framefile(const string &s) const
{
    // Framefiles may be passed as a fully-qualified path.  If so, see if it
    // exists first before trying the directories.
    if (mpo_file_exists(s.c_str())) {
        return s;
    }

    return find_file((fs::path("framefile") / s).string(), true);
}

string homedir::find_file(const string& fileName, bool bFallback) const
{
    // try homedir first
    string result = (fs::path(m_homedir) / fileName).string();

    // if file does not exist in home directory and we are allowed to fallback
    // to app dir
    if (!mpo_file_exists(result.c_str()) && bFallback) {
        result = (fs::path(m_appdir) / fileName).string();
    }
    // else file either exists or we cannot fall back

    return result;
}
