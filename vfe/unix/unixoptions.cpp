//******************************************************************************
///
/// @file vfe/unix/unixoptions.cpp
///
/// Processing system for options in povray.conf, command line and environment
/// variables.
///
/// @author Christoph Hormann <chris_hormann@gmx.de>
/// @author Based on v3.6 elements by Nicolas Calimet
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.8.
/// Copyright 1991-2019 Persistence of Vision Raytracer Pty. Ltd.
///
/// POV-Ray is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as
/// published by the Free Software Foundation, either version 3 of the
/// License, or (at your option) any later version.
///
/// POV-Ray is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
///
/// ----------------------------------------------------------------------------
///
/// POV-Ray is based on the popular DKB raytracer version 2.12.
/// DKBTrace was originally written by David K. Buck.
/// DKBTrace Ver 2.0-2.12 were written by David K. Buck & Aaron A. Collins.
///
/// @endparblock
///
//*******************************************************************************

#include "unixoptions.h"

// C++ variants of C standard header files
#include <cstdlib>

// C++ standard header files
#include <fstream>
#include <vector>

// Boost header files
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

// Other library header files
#include <sys/stat.h>

namespace vfePlatform
{
    using std::cerr;
    using std::endl;
    using std::string;
    using std::list;

    extern bool gShelloutsPermittedFixThis;

    const UnixOptionsProcessor::Option_Info UnixOptionsProcessor::Standard_Options[] =
    {
        // section name, option name, default, has_param, command line parameter, environment variable name, help text
        UnixOptionsProcessor::Option_Info("general", "help", "off", false, "--help|-help|-h|-?", "", "display usage information"),
        UnixOptionsProcessor::Option_Info("general", "temppath", "", true, "", "POV_TEMP_DIR", "directory for temporary files"),
        UnixOptionsProcessor::Option_Info("general", "version", "off", false, "--version|-version|--V", "", "display program version"),
        UnixOptionsProcessor::Option_Info("general", "generation", "off", false, "--generation", "", "display program generation (short version number)"),
        UnixOptionsProcessor::Option_Info("general", "benchmark", "off", false, "--benchmark|-benchmark", "", "run the standard POV-Ray benchmark"),
        UnixOptionsProcessor::Option_Info("display", "window", "", true, "--preview|-y", "POV_PREVIEW", "choice of handler for preview (x11, sdl, text)"),
        UnixOptionsProcessor::Option_Info("", "", "", false, "", "", "") // has to be last
    };

    // based on v3.6 unix_create_globals()
    UnixOptionsProcessor::UnixOptionsProcessor(vfeSession *session) :
        m_Session(session)
    {
        char* value;
        value = std::getenv("HOME");
        m_home = value ? value:"";

        // Default values for I/O restrictions: everything is allowed.
        // Any restrictions must come from system or user configuration.
        m_file_io  = IO_UNSET;
        m_shellout = SHL_UNSET;

        // system configuration file
        m_conf    = "";
        m_sysconf = POVCONFDIR "/povray.conf";

        // user configuration file
        if (m_home.length() > 0)
        {
            m_user_dir = m_home + "/." PACKAGE "/" VERSION_BASE;
            m_userconf = m_home + "/." PACKAGE "/" VERSION_BASE "/povray.conf";
        }
        else
        {
            m_user_dir = "";
            m_userconf = "";
        }

        // system ini file
        m_sysini     = POVCONFDIR "/povray.ini";
        m_sysini_old = POVCONFDIR_BACKWARD "/povray.ini";

        // user ini file
        if (m_home.length() > 0)
            m_userini = m_home + "/." PACKAGE "/" VERSION_BASE "/povray.ini";
        else
            m_userini = "";

        if (m_home.length() > 0)
            m_userini_old = m_home + "/.povrayrc";
        else
            m_userini_old = "";

#ifdef UNIX_DEBUG
        cerr << "PATHS" << endl;
        cerr << "  HOME        = " << m_home << endl;
        cerr << "  SYSCONF     = " << m_sysconf << endl;
        cerr << "  USERCONF    = " << m_userconf << endl;
        cerr << "  SYSINI      = " << m_sysini << endl;
        cerr << "  SYSINI_OLD  = " << m_sysini_old << endl;
        cerr << "  USERINI     = " << m_userini << endl;
        cerr << "  USERINI_OLD = " << m_userini_old << endl;
#endif
#if 0
        cerr << "--- tests ---" << endl;
        cerr << "## unix_getcwd: " << unix_getcwd() << endl;
        cerr << "## basename(SYSCONF): " << basename(m_sysconf) << endl;
        cerr << "## dirname(SYSCONF): " << dirname(m_sysconf) << endl;
        cerr << "## basename(/test/path/): " << basename("/test/path/") << endl;
        cerr << "## dirname(/test/path/): " << dirname("/test/path/") << endl;
        cerr << "## CanonicalizePath(../test/to/file/): " << CanonicalizePath("../test/to/file/") << endl;
        cerr << "## CanonicalizePath(/another/dir/../file): " << CanonicalizePath("/another/dir/../file") << endl;
        cerr << "------------" << endl;
#endif

        // register all standard options
        for (int i = 0; Standard_Options[i].Section != ""; i++)
        {
            list<Option_Info>::iterator iter = find(m_user_options.begin(), m_user_options.end(), Standard_Options[i]);

            // add this option if not already there
            if (iter == m_user_options.end())
                m_user_options.push_back(Standard_Options[i]);
        }

        // process system and user povray.conf
        process_povray_conf();
    }

    string UnixOptionsProcessor::GetTemporaryPath(void)
    {
        string path = QueryOptionString("general", "temppath");
        if (path.length() == 0)
        {
            struct stat s;
            path = "/tmp/";
            if (stat (path.c_str(), &s) == 0  &&  S_ISDIR (s.st_mode)  &&  (s.st_mode & S_IRWXU) == S_IRWXU)
                return path;
            path = unix_getcwd();
        }
        if (path[path.length()-1] != '/')
            path = path + "/";
        return path;
    }

    void UnixOptionsProcessor::PrintOptions(void)
    {
        // TODO -- GNU/Linux customs would be to print to stdout (among other differences).

        std::cout << endl;
        std::cout << "Platform specific command line options:" << endl;

        string section("");
        bool section_new = false;

        for (list<Option_Info>::iterator iter = m_user_options.begin(); iter != m_user_options.end(); iter++)
        {
            if ((*iter).Section != section)
            {
                section = (*iter).Section;
                section_new = true;
            }
            if ((*iter).CmdOption != "")
            {
                if (section_new)
                {
                    std::cout << endl;
                    std::cout << "  '" << section << "' options:" << endl << endl;
                    section_new = false;
                }
                std::cout << "    " << boost::format("%1% %|32t|%2%") % (*iter).CmdOption % (*iter).Comment << endl;
                if (!((*iter).EnvVariable.empty()))
                {
                std::cout << "      also via $" << (*iter).EnvVariable << endl;
                }
                std::cout << "    " << boost::format(" %|32t|(%1%=%2%)") % (*iter).Name % iter->Value << endl;
            }
            else
            {
                if (section_new)
                {
                    std::cout << endl;
                    std::cout << "  '" << section << "' options:" << endl << endl;
                    section_new = false;
                }
                if (!((*iter).EnvVariable.empty()))
                {
                std::cout << "    $" << boost::format("%1% %|31t|%2%") % (*iter).EnvVariable % (*iter).Comment << endl;
                std::cout << "    " << boost::format(" %|32t|(%1%=%2%)") % (*iter).Name % iter->Value << endl;
                }
            }
        }
        std::cout << endl;
    }

    void UnixOptionsProcessor::Register(const Option_Info options[])
    {
        for (int i = 0; options[i].Section != ""; i++)
        {
            list<Option_Info>::iterator iter = find(m_user_options.begin(), m_user_options.end(), options[i]);

            // add this option if not already there
            if (iter == m_user_options.end())
                m_user_options.push_back(options[i]);
        }
    }

    void UnixOptionsProcessor::QueryOption(Option_Info &option)
    {
        list<Option_Info>::iterator iter = find(m_user_options.begin(), m_user_options.end(), option);

        if (iter != m_user_options.end())
            option.Value = (*iter).Value;
        else
            option.Value = "";
    }

    string UnixOptionsProcessor::QueryOptionString(const string &section, const string &name)
    {
        Option_Info opt(section, name);
        QueryOption(opt);
        return opt.Value;
    }

    int UnixOptionsProcessor::QueryOptionInt(const string &section, const string &name, const int dflt)
    {
        int res;
        try
        {
            res = boost::lexical_cast<int>(QueryOptionString(section, name));
        }
        catch(boost::bad_lexical_cast &)
        {
            res = dflt;
        }
        return res;
    }

    float UnixOptionsProcessor::QueryOptionFloat(const string &section, const string &name, const float dflt)
    {
        float res;
        try
        {
            res = boost::lexical_cast<float>(QueryOptionString(section, name));
        }
        catch(boost::bad_lexical_cast &)
        {
            res = dflt;
        }
        return res;
    }

    bool UnixOptionsProcessor::isOptionSet(const Option_Info &option)
    {
        list<Option_Info>::iterator iter = find(m_user_options.begin(), m_user_options.end(), option);

        if (iter != m_user_options.end())
        {
            if (!(*iter).has_param && ((*iter).Value == ""))
                return true;
            string val = (*iter).Value;
            boost::to_lower(val);
            if (val == "yes" || val == "on" || val == "true" || val == "1")
                return true;
        }

        return false;
    }

    bool UnixOptionsProcessor::isOptionSet(const string &section, const string &name)
    {
        for (list<Option_Info>::iterator iter = m_user_options.begin(); iter != m_user_options.end(); iter++)
        {
            if (((*iter).Section == section) && ((*iter).Name == name))
                return isOptionSet(*iter);
        }
        return false;
    }

    void UnixOptionsProcessor::ProcessOptions(int *argc, char **argv[])
    {
        // add custom configuration options found in povray.conf files
        for (list<Conf_Option>::iterator iter_c = m_custom_conf_options.begin(); iter_c != m_custom_conf_options.end(); iter_c++)
        {
            Option_Info new_opt((*iter_c).Section, (*iter_c).Name, (*iter_c).Value, ((*iter_c).Value != ""), "", "");

            list<Option_Info>::iterator iter = find(m_user_options.begin(), m_user_options.end(), new_opt);
            if (iter == m_user_options.end())
                m_user_options.push_back(new_opt);
            else
                (*iter).Value = (*iter_c).Value;
        }

        m_user_options.sort();

#ifdef UNIX_DEBUG
        cerr << "OPTIONS (" << m_user_options.size() << ")" << endl;
#endif

        // check command line options and environment variables of all registered options
        // this overrides povray.conf settings
        for (list<Option_Info>::iterator iter = m_user_options.begin(); iter != m_user_options.end(); iter++)
        {
            // in ascending order of priority:
            // environment variables:
            if ((*iter).EnvVariable != "")
            {
                char *tmp = std::getenv((*iter).EnvVariable.c_str());
                if (tmp) // variable defined?
                    (*iter).Value = tmp;
            }

            // command line options:
            // based on v3.6 XWIN_init_povray()
            if ((*iter).CmdOption != "")
            {
                int oargc = *argc;
                char **oargv = *argv;

                // TODO: free those when no more needed
                char **nargv = (char **)malloc((oargc + 1) * sizeof(char *));
                int nargc = oargc;
                for (int i = 0; i < nargc; i++)
                {
                    nargv[i] = (char *)malloc(strlen(oargv[i]) + 1);
                    strcpy(nargv[i], oargv[i]);
                }

                nargv[nargc] = nullptr;

                std::vector<string> CmdVariations;
                boost::split(CmdVariations, (*iter).CmdOption, boost::is_any_of("|"));

                for (std::vector<string>::iterator iter_c = CmdVariations.begin(); iter_c != CmdVariations.end(); iter_c++)
                {
                    for (int i = 1; i < nargc;)
                    {
                        if (string(nargv[i]) == (*iter_c))
                        {
                            if ((*iter).has_param)
                            {
                                int j = i + 1;
                                if (j < nargc && nargv[j] != nullptr)
                                {
                                    (*iter).Value = nargv[j];
                                    remove_arg(&nargc, nargv, j);
                                }
                            }
                            else
                                (*iter).Value = "on";  // FIXME [nc]
                            remove_arg(&nargc, nargv, i);
                        }
                        else
                            i++;
                        if (nargv[i] == nullptr)
                            break;
                    }
                }

                *argv = nargv;
                *argc = nargc;
            }

#ifdef UNIX_DEBUG
            cerr << "  " << (*iter).Name << " = " << (*iter).Value << "(" << (*iter).CmdOption << ", " << (*iter).EnvVariable << ")" << endl;
#endif
        }
    }

    // based on 3.x RemoveArgs() by Andreas Dilger
    void UnixOptionsProcessor::remove_arg(int *argc, char *argv[], int index)
    {
        if (index >= *argc || index == 0)
            return;

        if (argv[index] != nullptr)
            free(argv[index]);

        for (; index < *argc; index++)
            argv[index] = argv[index + 1];

        (*argc)--;
    }

    // based on v3.6 UNIX_getcwd()
    string UnixOptionsProcessor::unix_getcwd(void)
    {
#ifdef HAVE_GETCWD
        size_t len;

        len = 256;  // default buffer size
        char *tmp = new char[len];

        while (getcwd(tmp, len) == nullptr)  // buffer is too small
        {
            delete[] tmp;
            len *= 2;  // double buffer size and try again
            tmp = new char[len];
        }
#else
        string tmp = std::getenv("PWD");  // must not be `nullptr`; checked by configure
        if(tmp.length() == 0)        // run-time checks are safer anyway
        {
            // TODO: correct error handling
            char *errormsg =
                "Cannot determine the current working directory.\n"
                "Check that the PWD environment variable does exist and is valid.\n";
            if(no_error_call)
            {
                fprintf(stderr, "%s: %s\n", PACKAGE, errormsg);
                std::exit(EXIT_FAILURE);
            }
            else
                Error("%s", errormsg);
        }
#endif

        string s = tmp + string("/");  // add final slash

#ifdef HAVE_GETCWD
        delete[] tmp;
#endif

        return s;
    }

    // based on v3.6 unix_basename()
    string UnixOptionsProcessor::basename(const string &path)
    {
        if(path.length() < 2) // less than two characters
            return path;

        string s = path;
        if (s[s.length()-1] == '/')
            s.erase(s.length()-1);
        string::size_type pos = s.rfind('/');
        if (pos == 0)
            return s;
        s.erase(0, pos+1);
        return s;
    }

    // based on v3.6 unix_dirname()
    string UnixOptionsProcessor::dirname(const string &path)
    {
        if(path.length() < 2)  // less than two characters
            return string("");

        string s = path;
        if (s[s.length()-1] == '/')
            s.erase(s.length()-1);
        string::size_type pos = s.rfind('/');
        if (pos == 0)
            return string("");
        s.erase(pos);
        return s;
    }

    // based on v3.6 unix_readlink()
    string UnixOptionsProcessor::unix_readlink(const string &path)
    {
#ifdef HAVE_READLINK
        char   *tmp;
        size_t  len;
        int     status;

        len = 256;  // default buffer size
        tmp = new char[len];  // init with '\0'

        while(true)
        {
            status = readlink(path.c_str(), tmp, len-1);  // without terminating '\0'
            if(status < 0)  // an error occured, return empty string
            {
                delete[] tmp;
                return string("");
            }
            else if(status == len-1)  // the buffer is probably too small
            {
                delete[] tmp;
                len *= 2;  // double buffer size and try again
                tmp = new char[len];
            }
            else  // all right, let's go further
                break;
        }
        // add C string terminator
        tmp[status] = '\0';

        // do we have an absolute path ?
        if(tmp[0] != '/')  // no; concatenate symlink to the path dirname
        {
            string s = dirname(path) + "/" + tmp;
            delete[] tmp;
            return s;
        }
        else  // yes; just resize buffer
        {
            string s = tmp;
            delete[] tmp;
            return s;
        }
#else
        return string("");
#endif
    }

    // based on v3.6 UNIX_canonicalize_path()
    string UnixOptionsProcessor::CanonicalizePath(const string &path)
    {
        int   i;
        struct subst final { const char *match, *replace; };
        const subst strings[] = {  // beware: order does matter
            { "%INSTALLDIR%", POVLIBDIR },
            { "%HOME%", m_home.c_str() },
            { "//", "/" },
            { "/./", "/" },
            { nullptr, nullptr }  // sentinel
        };

        // nothing to canonicalize; return an empty string
        if(path.length() == 0)
            return string("");

#ifdef UNIX_DEBUG
        cerr << "  CANONICALIZE '" << path << "'" << endl;
#endif

        string s = path;

        // substitute all occurences of 'match' by 'replace'
        i = 0;
        while(strings[i].match)
        {
            boost::replace_all(s, strings[i].match, strings[i].replace);
            ++i;
#ifdef UNIX_DEBUG
            cerr << "    su: " << s << endl;
#endif
        }

        // substitute the current working dir to the first "./"
        // or add the cwd to the first directory or "../"
        if (boost::starts_with(s, "./"))
        {
            s.erase(0, 2);
            s.insert(0, unix_getcwd());
        }
        else if(s[0] != '/' || boost::starts_with(s, "../"))
        {
            s.insert(0, unix_getcwd());
        }

        // iteratively translate all symlinks in the path (dirname and basename)
#ifdef HAVE_READLINK
        i = 0;
        if(s[i] == '/')
            ++i;
        do
        {
            while(i < s.length() && s[i] != '/')
                ++i;
            string tmp1 = s.substr(0, i);
            string tmp2 = s.substr(i, s.length()-i);
            string tmp3 = unix_readlink(tmp1);
            if (tmp3.length() > 0)
            {
#ifdef UNIX_DEBUG
                cerr << "    ln: " << tmp1 << " -> " << tmp3 << endl;
                cerr << "  " << tmp3 << tmp2 << endl;
#endif
                s = tmp3 + tmp2;
                i = 0;  // start again from beginning
                if(s[i] == '/')
                    ++i;
            }
            else
            {
#ifdef UNIX_DEBUG
                cerr << "    ln: " << tmp1 << endl;
#endif
                ++i;
            }
        } while(i < s.length());
#endif  // HAVE_READLINK

        // substitute all occurences of "dir/.." by ""
        string tmp = s;
        string::size_type pos = s.find("/..");
        while (pos != string::npos)
        {
            string::size_type pos2 = s.rfind('/', pos-1);
            s.erase(pos2+1, pos-pos2+3);
#ifdef UNIX_DEBUG
            cerr << "    su: " << s << endl;
#endif
            pos = s.find("/..", pos+3);
        }

        // remove the last "/." if any
        if (boost::ends_with(s, "/."))
        {
            s.erase(s.length()-3);
#ifdef UNIX_DEBUG
            cerr << "    su: " << s << endl;
#endif
        }

        return s;
    }

    // based on v3.6 pre_process_conf_line()
    string UnixOptionsProcessor::pre_process_conf_line(const string &input)
    {
        string s = boost::trim_copy(input);

        // find comment mark
        string::size_type pos = s.find(";");
        if (pos != string::npos)
        {
            s.erase(pos);
            boost::trim(s);
        }

        return s;
    }

    // based on v3.6 add_permitted_path()
    void UnixOptionsProcessor::add_permitted_path(list<UnixPath> &paths, const string &input, const string &conf_name, unsigned long line_number)
    {
        char quote = 0;
        bool descend = false;
        bool writable = false;

        // read or read+write entry
        if (boost::starts_with(input, "read"))
        {
            int spos = 4;
            // we have a read+write path
            if (boost::starts_with(input, "read+write"))
            {
                writable = true;
                spos = 10;
            }
            // sub-directory search
            if (input[spos] == '*')
                descend = true;

            string::size_type pos = input.find('=');  // find equal sign
            if (pos != string::npos)  // equal sign found
            {
                int i = 0;
                string s = input.substr(pos+1, input.length()-pos-1);
                boost::trim(s);

                if(s[i] == '"' || s[i] == '\'')
                {
                    quote = s[i];  // store and skip quote
                    ++i;
                }
                int begin = i;
                while(i < s.length())  // find next space caracter or closing quote
                {
                    if(s[i] == quote  ||  (!quote  &&  isspace((int) s[i])))
                        break;
                    ++i;
                }
                if(quote  &&  s[i] != quote)  // no closing quote
                    fprintf(stderr,
                        "%s: %s: %lu: ignored entry: missing closing %c quote\n",
                        PACKAGE, conf_name.c_str(), line_number, quote
                    );
                else if(i-begin)  // store given directory
                {
                    string directory = s.substr(begin, i-begin) + "/";
                    s = CanonicalizePath(directory);

#ifdef UNIX_DEBUG
                    cerr << "PERMITTED ADD '" << s << "'" << endl;
#endif
                    paths.push_back(UnixPath(s, descend, writable));
                }
                else  // nothing found after the equal sign
                    fprintf(stderr,
                        "%s: %s: %lu: ignored entry: missing directory\n",
                        PACKAGE, conf_name.c_str(), line_number
                    );
            }
            else  // equal sign not found
                fprintf(stderr,
                    "%s: %s: %lu: ignored entry: missing equal sign\n",
                    PACKAGE, conf_name.c_str(), line_number
                );
        }
        // unknown entry
        else if(input.length() > 0)
            fprintf(stderr,
                "%s: %s: %lu: unknown '%s' setting\n",
                PACKAGE, conf_name.c_str(), line_number, input.c_str()
            );
    }

    // based on v3.6 unix_parse_conf_file()
    void UnixOptionsProcessor::parse_conf_file(std::istream &Stream, const string &conf_name, bool user_mode)
    {
        list<UnixPath> paths;
        string line;
        string Custom_Section;
        unsigned long   line_number;
        bool            user_file_io_rejected;
        FileIO          file_io;
        bool            file_io_is_set;
        ShellOut        shellout;
        bool            shellout_is_set;
        short           i;

        typedef enum { NONE, FILE_IO, SHELLOUT, PERMITTED_PATHS, UNKNOWN } SectionVal;
        SectionVal section;
        struct Section final { const char *label; const SectionVal value; };
        const Section sections[] =
        {
            { ""                   , NONE            },  // init
            { "[File I/O Security]", FILE_IO         },
            { "[Shellout Security]", SHELLOUT        },
            { "[Permitted Paths]"  , PERMITTED_PATHS },
            { nullptr              , UNKNOWN         }   // sentinel
        };

        struct IOSettings final { const char *label; const FileIO value; };
        const IOSettings io_settings[] =
        {
            { ""          , IO_UNSET      },
            { "none"      , IO_NONE       },
            { "read-only" , IO_READONLY   },
            { "restricted", IO_RESTRICTED },
            { nullptr     , IO_UNKNOWN    }
        };

        struct SHLSettings final { const char *label; const ShellOut value; };
        const SHLSettings shl_settings[] =
        {
            { ""         , SHL_UNSET     },
            { "allowed"  , SHL_ALLOWED   },
            { "forbidden", SHL_FORBIDDEN },
            { nullptr    , SHL_UNKNOWN   }
        };

        // inits
        line_number = 0;
        user_file_io_rejected = false;
        file_io_is_set = shellout_is_set = false;
        section = NONE;
        file_io = IO_UNSET;
        shellout = SHL_UNSET;

#ifdef UNIX_DEBUG
        cerr << "PARSE CONF '" << conf_name << "'" << endl;
#endif

        // Since the file format allows to read permitted paths before
        // setting [File I/O Security], the paths must be stored in a local
        // list which will be used only when the user setting for file I/O
        // is accepted.
        if(!user_mode)
            paths = m_permitted_paths;

        while(std::getline(Stream, line))
        {
            // check for failbit and badbit and eofbit
            if(!Stream.good())
            {
                // Only in case of the badbit we can assume that some lower
                // level system API function has set errno and perror() can be
                // used safely.
                if(Stream.bad())
                {
                    fprintf(stderr, "%s: error while reading/opening configuration file ", PACKAGE);
                    perror(conf_name.c_str());
                }
                break;
            }
            // preprocess line
            line = pre_process_conf_line(line);
            ++line_number;

            // skip empty line
            if(line.length() == 0)
                continue;

            // check section
            if(line[0] == '[')  // new section
            {
                Custom_Section = "";
                // search section
                for(i = 1; sections[i].label; ++i)
                    if(boost::starts_with(line, sections[i].label))
                        break;

                section = sections[i].value;

                // unknown section
                if(section == UNKNOWN)
                {
                    Custom_Section = boost::to_lower_copy(line);
                    Custom_Section.erase(0,1);
                    Custom_Section.erase(Custom_Section.find(']'));
                }
            }  // check section

            // process custom sections
            else if (Custom_Section.length() != 0)
            {
                // split at '='
                string::size_type pos = line.find('=');
                string option_name;
                string option_val;
                if (pos == string::npos)
                {
                    option_name = boost::to_lower_copy(line);
                    option_val = "";
                }
                else
                {
                    option_name = boost::to_lower_copy(line.substr(0, pos));
                    option_val = line.substr(pos+1, line.length()-pos-1);
                }
#ifdef UNIX_DEBUG
                cerr << "CUSTOM OPTION:" << endl;
                cerr << " Section: " << Custom_Section << endl;
                cerr << " Name: " << option_name << endl;
                cerr << " Value: " << option_val << endl;
#endif
                m_custom_conf_options.push_back(Conf_Option(Custom_Section, option_name, option_val));
            }

            // file I/O security
            else if(section == FILE_IO)
            {
                // search setting
                for(i = 0; io_settings[i].label; ++i)
                    if(line != string(io_settings[i].label))
                        break;
                file_io = io_settings[i].value;

                // multiple settings were found
                if(file_io_is_set)
                    fprintf(stderr,
                        "%s: %s: %lu: multiple settings for %s\n",
                        PACKAGE, conf_name.c_str(), line_number, sections[section].label
                    );
                if(file_io != IO_UNSET)
                    file_io_is_set = true;

                // unknown setting
                if(file_io == IO_UNKNOWN)
                {
                    fprintf(stderr,
                        "%s: %s: %lu: unknown '%s' setting for %s: ",
                        PACKAGE, conf_name.c_str(), line_number, line.c_str(), sections[section].label
                    );
                    if(user_mode)
                    {
                        fprintf(stderr,
                            "using system setting '%s'\n",
                            io_settings[m_file_io].label
                        );
                        file_io = m_file_io;
                        user_file_io_rejected = true;  // won't account for the user paths
                    }
                    else
                    {
                        fprintf(stderr, "I/O restrictions are disabled\n");
                        file_io = IO_NONE;
                    }
                }

                // user setting not allowed
                if(user_mode  &&  file_io < m_file_io)
                {
                    fprintf(stderr,
                        "%s: %s: %lu: "
                        "the user setting '%s' for %s is less restrictive than "
                        "the system setting '%s' in '%s': using system setting\n",
                        PACKAGE, conf_name.c_str(), line_number,
                        io_settings[  file_io].label, sections[section].label,
                        io_settings[m_file_io].label, m_conf.c_str()
                    );
                    file_io = m_file_io;
                    user_file_io_rejected = true;  // won't account for the user paths
                }

                m_file_io = file_io;
            }  // file I/O security

            // shellout security
            else if(section == SHELLOUT)
            {
                // search setting
                for(i = 0; shl_settings[i].label; ++i)
                    if(line == string(shl_settings[i].label))
                        break;
                shellout = shl_settings[i].value;

                // multiple settings were found
                if(shellout_is_set)
                    fprintf(stderr,
                        "%s: %s: %lu: multiple settings for %s\n",
                        PACKAGE, conf_name.c_str(), line_number, sections[section].label
                    );
                if(shellout != SHL_UNSET)
                    shellout_is_set = true;

                // unknown setting
                if(shellout == SHL_UNKNOWN)
                {
                    fprintf(stderr,
                        "%s: %s: %lu: unknown '%s' setting for %s: ",
                        PACKAGE, conf_name.c_str(), line_number, line.c_str(), sections[section].label
                    );
                    if(user_mode)
                    {
                        fprintf(stderr,
                            "using system setting '%s'\n",
                            shl_settings[m_shellout].label
                        );
                        shellout = m_shellout;
                    }
                    else
                    {
                        fprintf(stderr, "shellout security is disabled\n");
                        shellout = SHL_ALLOWED;
                    }
                }

                // user setting not allowed
                if(user_mode
                     && m_shellout == SHL_FORBIDDEN
                     && m_shellout != shellout)
                {
                    fprintf(stderr,
                        "%s: %s: %lu: "
                        "the user setting '%s' for %s is less restrictive than "
                        "the system '%s' setting in '%s': using system setting\n",
                        PACKAGE, conf_name.c_str(), line_number,
                        shl_settings[  shellout].label, sections[section].label,
                        shl_settings[m_shellout].label, m_conf.c_str()
                    );
                    shellout = m_shellout;
                }

                m_shellout = shellout;
                gShelloutsPermittedFixThis = shellout == SHL_ALLOWED;
            }  // shellout security

            // permitted paths
            else if(section == PERMITTED_PATHS)
                add_permitted_path(paths, line, conf_name, line_number);

        }  // Read file loop end

        // Only in case of the badbit we can assume that some lower
        // level system API function has set errno. Then, perror() can be
        // used safely.
        if(Stream.bad())
        {
            fprintf(stderr, "%s: error while reading/opening config file ", PACKAGE);
            perror(conf_name.c_str());
        }

#ifdef UNIX_DEBUG
        fprintf(stderr,
            "I/O RESTRICTIONS\n"
            "  file_io  = %d\tconfig->file_io  = %d\n"
            "  shellout = %d\tconfig->shellout = %d\n",
            file_io, m_file_io,
            shellout, m_shellout
        );
#endif

        // assign user settings and paths
        if(user_mode)
        {
            if(user_file_io_rejected)
            {
                fprintf(stderr,
                    "%s: %s: user permitted paths are ignored: using system paths\n",
                    PACKAGE, conf_name.c_str()
                );
            }
            else
            {
                m_permitted_paths = paths;  // assign new paths
            }
        }
    }

    // based on v3.6 unix_process_povray_conf()
    void UnixOptionsProcessor::process_povray_conf(void)
    {
        m_Session->ClearPaths();
        m_Session->AddExcludedPath(string(POVCONFDIR));
        if (m_user_dir.length() != 0)
            m_Session->AddExcludedPath(m_user_dir);

        // read system configuration file
        if(m_sysconf.length() != 0)
        {
            std::ifstream stream ( m_sysconf.c_str() );
            if ( stream.is_open() )
            {
                parse_conf_file(stream, m_sysconf, false);
                m_conf = m_sysconf;
            }
            else
            {
                fprintf(stderr, "%s: cannot open the system configuration file ", PACKAGE);
                perror(m_sysconf.c_str());
            }
        }

        // read user configuration file
        if(m_userconf.length() != 0)
        {
            std::ifstream stream ( m_userconf.c_str() );
            if ( stream.is_open() )
            {
                parse_conf_file(stream, m_userconf, true);
                m_conf = m_userconf;
            }
            else
            {
                fprintf(stderr, "%s: cannot open the user configuration file ", PACKAGE);
                perror(m_userconf.c_str());
            }
        }

        // no file was read, disable I/O restrictions
        if(m_conf.length() == 0)
            fprintf(stderr, "%s: I/O restrictions are disabled\n", PACKAGE);

        // if no paths specified, at least include POVLIBDIR and POVCONFDIR
        else if(m_permitted_paths.empty())
        {
            m_permitted_paths.push_back(UnixPath(string(POVLIBDIR "/"), true, false));   // read*
            m_permitted_paths.push_back(UnixPath(string(POVCONFDIR "/"), false, false)); // read
        }

#ifdef UNIX_DEBUG
        cerr << "PERMITTED PATHS" << endl;
#endif

        for(list<UnixPath>::iterator iter = m_permitted_paths.begin(); iter != m_permitted_paths.end(); iter++)
        {
            m_Session->AddReadPath(iter->str, iter->descend);
#ifdef UNIX_DEBUG
            fprintf(stderr,
                "  %s%s = \"%s\"\n",
                iter->writable ? "WRITE" : "READ", iter->descend ? "*" : "", iter->str.c_str()
            );
#endif
        }
    }

    bool UnixOptionsProcessor::file_exist(const string &name)
    {
        FILE *file = fopen(name.c_str(), "r");

        if (file != nullptr)
            fclose(file);
        else
            return false;

        return true;
    }

    // based on v3.6 unix_process_povray_ini()
    void UnixOptionsProcessor::Process_povray_ini(vfeRenderOptions &opts)
    {
        // try the file pointed to by POVINI
        string povini;
        char * povini_c = std::getenv("POVINI");
        if (povini_c)
        {
            povini = povini_c;
            if (file_exist(povini))
            {
                opts.AddINI(povini);
                return;
            }
            else
            {
                fprintf(stderr, "%s: POVINI environment: cannot open ", PACKAGE);
                perror(povini.c_str());
            }
        }

        // try any INI file in the current directory
        povini = "./povray.ini";
        if (file_exist(povini))
        {
            opts.AddINI(povini);
            return;
        }

        // try the user or system INI file
        if ((m_home.length() != 0) && file_exist(m_userini))
        {
            opts.AddINI(m_userini);
            return;
        }
        if (file_exist(m_sysini))
        {
            opts.AddINI(m_sysini);
            return;
        }

        // try older user or system INI files
        if ((m_home.length() != 0) && file_exist(m_userini_old))
        {
            opts.AddINI(m_userini_old);
            return;
        }
        if (file_exist(m_sysini_old))
        {
            opts.AddINI(m_sysini_old);
            return;
        }

        // warn that no INI file was found and add minimal library_path setting
        fprintf(stderr, "%s: cannot open an INI file, adding default library path\n", PACKAGE);
        opts.AddLibraryPath(string(POVLIBDIR "/include"));
    }

#if 0
    // based on v3.6 unix_subdir()
    static bool UnixOptionsProcessor::file_in_permitted_paths (const string &Filename, bool write)
    {
        // NOTE: Filename must be already canonicalized

        // no filename nor paths
        if(Filename.length() == 0  ||  m_permitted_paths.empty())
            return false;

        // scan the list of paths
        for (list<UnixPath>::iterator iter = m_permitted_paths.begin(); iter != m_permitted_paths.end(); iter++)
        {
            if ((*iter).descend)  // allows sub-directories
            {
                if (!write || (*iter).writable)
                {
#ifdef UNIX_DEBUG
                    fprintf(stderr,
                        "  FILE '%s' <-> %s* '%s'\n",
                        Filename.c_str(), path->writable ? "WRITE" : "READ", (*iter).str.c_str()
                    );
#endif
                    if (boost::starts_with(Filename, (*iter).str))  // match found
                    {
#ifdef UNIX_DEBUG
                        fprintf(stderr, "  OK\n");
#endif
                        return true;
                    }
                }
            }
            else  // check for exact match with path->str (without last slash)
            {
                string dirname = dirname(Filename);
                string pathname = (*iter).str;
                pathname.erase(pathname.length()-1);
                if (!write || (*iter).writable)
                {
#ifdef UNIX_DEBUG
                    fprintf(stderr,
                        "  DIRNAME '%s' <-> %s '%s'\n",
                        dirname.c_str(), path->writable ? "WRITE" : "READ", pathname.c_str()
                    );
#endif
                    if(dirname == pathname)
                    {
#ifdef UNIX_DEBUG
                        fprintf(stderr, "  OK\n");
#endif
                        return true;
                    }
                }
            }
        }

#ifdef UNIX_DEBUG
        fprintf(stderr, "  BAD\n");
#endif

        return false;
    }
#endif

    bool UnixOptionsProcessor::isIORestrictionsEnabled(bool write)
    {
        if (IO_RESTRICTIONS_DISABLED)
            return false;
        if (!write &&  m_file_io < IO_RESTRICTED)
            return false;
        if(m_file_io < IO_READONLY)
            return false;
        return true;
    }

    void UnixOptionsProcessor::IORestrictionsError(const string &fnm, bool write, bool is_user_setting)
    {
        if (is_user_setting)
        {
            if (write)
                fprintf(stderr, "%s: writing to '%s' is not permitted; check the configuration in '%s'\n", PACKAGE, fnm.c_str(), m_conf.c_str());
            else
                fprintf(stderr, "%s: reading from '%s' is not permitted; check the configuration in '%s'\n", PACKAGE, fnm.c_str(), m_conf.c_str());
        }
        else
        {
            fprintf(stderr, "%s: writing to '%s' is not permitted\n", PACKAGE, fnm.c_str());
        }

    }

}
// end of namespace vfePlatform
