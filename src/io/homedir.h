/*
 *  homedir.h
 */

#ifndef HOMEDIR_H
#define HOMEDIR_H

#include <string>

class homedir
{
  public:
    // Constructor/destructors
    homedir();

    // Properties
    std::string get_homedir() const;
    void set_homedir(const std::string &s);
    std::string get_romfile(const std::string &s) const;
    std::string get_ramfile(const std::string &s) const;
    std::string get_framefile(const std::string &s) const;

    // Searches homedir for a filename indicated by 'fileName' and if it doesn't
    // find it,
    //  searches the application directory for the same filename (if 'bFallback'
    //  is true).
    // Returns the found path,
    //  or an empty string if neither location yielded the file.
    std::string find_file(const std::string& fileName, bool bFallback = true) const;

  private:
    std::string m_appdir; // Directory the app was launched from
    std::string m_homedir; //"Home" directory to search first (defaults to appdir)
};

extern homedir g_homedir; // our global game class.  Instead of having every
                          // .cpp file define this, we put it here.

#endif
