//******************************************************************************
///
/// @file vfe/win/winoptions.cpp
///
/// Processing system for options in povray.conf, command line and environment
/// variables.
///
/// @author Trevor SANDY<trevor.sandy@gmial.com>
/// @author Based on unixoptions.cpp by Christoph Hormann <chris_hormann@gmx.de>
///
/// @copyright
/// @parblock
///
/// Persistence of Vision Ray Tracer ('POV-Ray') version 3.7.
/// Copyright 1991-2015 Persistence of Vision Raytracer Pty. Ltd.
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

#include "winoptions.h"
#include <fstream>
#include <sys/stat.h>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

namespace vfePlatform
{
    using std::cerr;
    using std::endl;

    bool gShelloutsPermittedWinCon = false;

    const WinConOptionsProcessor::Option_Info WinConOptionsProcessor::Standard_Options[] =
    {
        // section name, option name, default, has_param, command line parameter, environment variable name, help text
        WinConOptionsProcessor::Option_Info("general", "help", "off", false, "--help|-help|-h|-?", "", "display usage information"),
        WinConOptionsProcessor::Option_Info("general", "temppath", "", true, "", "POV_TEMP_DIR", "directory for temporary files"),
        WinConOptionsProcessor::Option_Info("general", "version", "off", false, "--version|-version|--V", "", "display program version"),
        WinConOptionsProcessor::Option_Info("general", "benchmark", "off", false, "--benchmark|-benchmark", "", "run the standard POV-Ray benchmark"),
        WinConOptionsProcessor::Option_Info("", "", "", false, "", "", "") // has to be last
    };

    WinConOptionsProcessor::WinConOptionsProcessor(vfeSession *session) :
        m_Session(session)
    {
		// User's home directory
		m_home = "";
		char* homeBuffer = new char[MAX_PATH];
		if ((homeBuffer = getenv("USERPROFILE")) > 0)
			m_home = homeBuffer;
		else
			fprintf(stderr, "%s: Could not get the user's home directory.\n", PACKAGE);

        // Default values for I/O restrictions: everything is allowed.
        // Any restrictions must come from system or user configuration.
        m_file_io  = IO_UNSET;
        m_shellout = SHL_UNSET;

        // system configuration file
        m_conf    = "";
        m_sysconf = POVCONFDIR "\\povray.conf";

        // user configuration file
        if (m_home.length() > 0)
        {
            m_user_dir = m_home + "\\" PACKAGE "\\" VERSION_BASE;
            m_userconf = m_home + "\\" PACKAGE "\\" VERSION_BASE "\\povray.conf";
        }
        else
        {
            m_user_dir = "";
            m_userconf = "";
        }

        // system ini file
        m_sysini     = POVCONFDIR "\\ini\\povray.ini";
        m_sysini_old = POVCONFDIR_BACKWARD "\\povray.ini";

        // user ini file
        if (m_home.length() > 0)
            m_userini = m_home + "\\" PACKAGE "\\" VERSION_BASE "\\povray.ini";
        else
            m_userini = "";

        if (m_home.length() > 0)
            m_userini_old = m_home + "\\povrayrc";
        else
            m_userini_old = "";

#ifdef WIN_DEBUG
        cerr << "PATHS" << endl;
        cerr << "  HOME        = " << m_home << endl;
        cerr << "  SYSCONF     = " << m_sysconf << endl;
        cerr << "  USERCONF    = " << m_userconf << endl;
        cerr << "  SYSINI      = " << m_sysini << endl;
        cerr << "  SYSINI_OLD  = " << m_sysini_old << endl;
        cerr << "  USERINI     = " << m_userini << endl;
        cerr << "  USERINI_OLD = " << m_userini_old << endl;
//#endif
//#if 0
        cerr << "--- tests ---" << endl;
        cerr << "## win_getcwd: " << win_getcwd() << endl;
        cerr << "## basename(SYSCONF): " << basename(m_sysconf) << endl;
        cerr << "## dirname(SYSCONF): " << dirname(m_sysconf) << endl;
        cerr << "## basename(\\test\\path\\): " << basename("\\test\\path\\") << endl;
        cerr << "## dirname(\\test\\path\\): " << dirname("\\test\\path\\") << endl;
        cerr << "## CanonicalizePath(..\\test\\to\\file\\): " << CanonicalizePath("..\\test\\to\\file\\") << endl;
        cerr << "## CanonicalizePath(\\another\\dir\\..\\file): " << CanonicalizePath("\\another\\dir\\../file") << endl;
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

    string WinConOptionsProcessor::GetTemporaryPath(void)
    {
		string path = QueryOptionString("general", "temppath");
		if (path.length() == 0)
		{
			char str[MAX_PATH];
			if (GetTempPath(sizeof(str) - 7, str) == 0)
				throw vfeException("Could not get temp dir from Windows API");
			strcat(str, "povwin\\");

			// if we fail to create our temp dir, just use the default one
			if (CreateDirectory(str, NULL) == 0 && GetLastError() != ERROR_ALREADY_EXISTS)
				if (GetTempPath(sizeof(str), str))
				{
					path = str;
					return path;
				}
			path = win_getcwd();
		}
		if (path[path.length() - 1] != '\\')
			path = path + "\\";
		return path;
    }

    void WinConOptionsProcessor::PrintOptions(void)
    {
        cerr << endl;
        cerr << "Platform specific command line options:" << endl;

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
                    cerr << endl;
                    cerr << "  '" << section << "' options:" << endl << endl;
                    section_new = false;
                }
                cerr << "    " << boost::format("%1% %|32t|%2%") % (*iter).CmdOption % (*iter).Comment << endl;
            }
        }
        cerr << endl;
    }

    void WinConOptionsProcessor::Register(const Option_Info options[])
    {
        for (int i = 0; options[i].Section != ""; i++)
        {
            list<Option_Info>::iterator iter = find(m_user_options.begin(), m_user_options.end(), options[i]);

            // add this option if not already there
            if (iter == m_user_options.end())
                m_user_options.push_back(options[i]);
        }
    }

    void WinConOptionsProcessor::QueryOption(Option_Info &option)
    {
        list<Option_Info>::iterator iter = find(m_user_options.begin(), m_user_options.end(), option);

        if (iter != m_user_options.end())
            option.Value = (*iter).Value;
        else
            option.Value = "";
    }

    string WinConOptionsProcessor::QueryOptionString(const string &section, const string &name)
    {
        Option_Info opt(section, name);
        QueryOption(opt);
        return opt.Value;
    }

    int WinConOptionsProcessor::QueryOptionInt(const string &section, const string &name, const int dflt)
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

    float WinConOptionsProcessor::QueryOptionFloat(const string &section, const string &name, const float dflt)
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

    bool WinConOptionsProcessor::isOptionSet(const Option_Info &option)
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

    bool WinConOptionsProcessor::isOptionSet(const string &section, const string &name)
    {
        for (list<Option_Info>::iterator iter = m_user_options.begin(); iter != m_user_options.end(); iter++)
        {
            if (((*iter).Section == section) && ((*iter).Name == name))
                return isOptionSet(*iter);
        }
        return false;
    }

    void WinConOptionsProcessor::ProcessOptions(int *argc, char **argv[])
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

#ifdef WIN_DEBUG
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
                char *tmp = getenv((*iter).EnvVariable.c_str());
                if (tmp) // variable defined?
                    (*iter).Value = tmp;
            }

            // command line options:
            // based on 3.6 XWIN_init_povray()
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

                nargv[nargc] = NULL;

                vector<string> CmdVariations;
                boost::split(CmdVariations, (*iter).CmdOption, boost::is_any_of("|"));

                for (vector<string>::iterator iter_c = CmdVariations.begin(); iter_c != CmdVariations.end(); iter_c++)
                {
                    for (int i = 1; i < nargc;)
                    {
                        if (string(nargv[i]) == (*iter_c))
                        {
                            if ((*iter).has_param)
                            {
                                int j = i + 1;
                                if (j < nargc && nargv[j] != NULL)
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
                        if (nargv[i] == NULL)
                            break;
                    }
                }

                *argv = nargv;
                *argc = nargc;
            }

#ifdef WIN_DEBUG
            cerr << "  " << (*iter).Name << " = " << (*iter).Value << "(" << (*iter).CmdOption << ", " << (*iter).EnvVariable << ")" << endl;
#endif
        }
    }

    // based on 3.x RemoveArgs() by Andreas Dilger
    void WinConOptionsProcessor::remove_arg(int *argc, char *argv[], int index)
    {
        if (index >= *argc || index == 0)
            return;

        if (argv[index] != NULL)
            free(argv[index]);

        for (; index < *argc; index++)
            argv[index] = argv[index + 1];

        (*argc)--;
    }

    // based on Windows _getcwd()
	string WinConOptionsProcessor::win_getcwd(void)
	{
		string m_cwd = "";
		char* cwdBuffer = new char[MAX_PATH]; // must not be NULL
		if ((cwdBuffer = _getcwd(NULL, 0)) == NULL)
		{
			fprintf(stderr, "%s: Could not get the user's home directory.\n", PACKAGE);
			return m_cwd;
		}
		m_cwd = cwdBuffer + string("\\");  // add final slash
		free(cwdBuffer);
		return m_cwd;
	}

    // based on unix_basename()
    string WinConOptionsProcessor::basename(const string &path)
    {
        if(path.length() < 2) // less than two characters
            return path;

        string s = path;
        if (s[s.length()-1] == '\\')
            s.erase(s.length()-1);
        string::size_type pos = s.rfind('\\');
        if (pos == 0)
            return s;
        s.erase(0, pos+1);
        return s;
    }

    // based on unix_dirname()
    string WinConOptionsProcessor::dirname(const string &path)
    {
        if(path.length() < 2)  // less than two characters
            return string("");

        string s = path;
        if (s[s.length()-1] == '\\')
            s.erase(s.length()-1);
        string::size_type pos = s.rfind('\\');
        if (pos == 0)
            return string("");
        s.erase(pos);
        return s;
    }

    // based on unix_readlink()
    string WinConOptionsProcessor::win_readlink(const string &path)
    {
        return string("");
    }

    // based on 3.6 UNIX_canonicalize_path()
    string WinConOptionsProcessor::CanonicalizePath(const string &path)
    {
        int   i;
        typedef struct { const char *match, *replace; } subst;
        const subst strings[] = {  // beware: order does matter
            { "%INSTALLDIR%", POVLIBDIR },
            { "%HOME%", m_home.c_str() },
            { "//", "/" },
            { "/./", "/" },
            { NULL, NULL }  // sentinel
        };

        // nothing to canonicalize; return an empty string
        if(path.length() == 0)
            return string("");

#ifdef WIN_DEBUG
        cerr << "  CANONICALIZE '" << path << "'" << endl;
#endif

        string s = path;

        // substitute all occurences of 'match' by 'replace'
        i = 0;
        while(strings[i].match)
        {
            boost::replace_all(s, strings[i].match, strings[i].replace);
            ++i;
#ifdef WIN_DEBUG
            cerr << "    su: " << s << endl;
#endif
        }

        // substitute the current working dir to the first ".\\"
        // or add the cwd to the first directory or "..\\"
        if (boost::starts_with(s, ".\\"))
        {
            s.erase(0, 2);
            s.insert(0, win_getcwd());
        }
        else if(s[0] != '\\' || boost::starts_with(s, "..\\"))
        {
            s.insert(0, win_getcwd());
        }

        // substitute all occurences of "dir\\.." by ""
        string tmp = s;
        string::size_type pos = s.find("\\..");
        while (pos != string::npos)
        {
            string::size_type pos2 = s.rfind('\\', pos-1);
            s.erase(pos2+1, pos-pos2+3);
#ifdef WIN_DEBUG
            cerr << "    su: " << s << endl;
#endif
            pos = s.find("\\..", pos+3);
        }

        // remove the last "\\." if any
        if (boost::ends_with(s, "\\."))
        {
            s.erase(s.length()-3);
#ifdef WIN_DEBUG
            cerr << "    su: " << s << endl;
#endif
        }

        return s;
    }

    // based on 3.6 pre_process_conf_line()
    string WinConOptionsProcessor::pre_process_conf_line(const string &input)
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

    // based on 3.6 add_permitted_path()
    void WinConOptionsProcessor::add_permitted_path(list<WinPath> &paths, const string &input, const string &conf_name, unsigned long line_number)
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
                    string directory = s.substr(begin, i-begin) + "\\";
                    s = CanonicalizePath(directory);

#ifdef WIN_DEBUG
                    cerr << "PERMITTED ADD '" << s << "'" << endl;
#endif
                    paths.push_back(WinPath(s, descend, writable));
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

    // based on 3.6 unix_parse_conf_file()
    void WinConOptionsProcessor::parse_conf_file(std::istream &Stream, const string &conf_name, bool user_mode)
    {
        list<WinPath> paths;
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
        typedef struct Section { const char *label; const SectionVal value; } Section;
        const Section sections[] =
        {
            { ""                   , NONE            },  // init
            { "[File I/O Security]", FILE_IO         },
            { "[Shellout Security]", SHELLOUT        },
            { "[Permitted Paths]"  , PERMITTED_PATHS },
            { NULL                 , UNKNOWN         }   // sentinel
        };

        typedef struct IOSettings { const char *label; const FileIO value; } IOSettings;
        const IOSettings io_settings[] =
        {
            { ""          , IO_UNSET      },
            { "none"      , IO_NONE       },
            { "read-only" , IO_READONLY   },
            { "restricted", IO_RESTRICTED },
            { NULL        , IO_UNKNOWN    }
        };

        typedef struct SHLSettings { const char *label; const ShellOut value; } SHLSettings;
        const SHLSettings shl_settings[] =
        {
            { ""         , SHL_UNSET     },
            { "allowed"  , SHL_ALLOWED   },
            { "forbidden", SHL_FORBIDDEN },
            { NULL       , SHL_UNKNOWN   }
        };

        // inits
        line_number = 0;
        user_file_io_rejected = false;
        file_io_is_set = shellout_is_set = false;
        section = NONE;
        file_io = IO_UNSET;
        shellout = SHL_UNSET;

#ifdef WIN_DEBUG
        cerr << PACKAGE << ": PARSE CONF FILE '" << conf_name << "'" << endl;
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
#ifdef WIN_DEBUG
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
				gShelloutsPermittedWinCon = shellout == SHL_ALLOWED;
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

#ifdef WIN_DEBUG
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

    // based on 3.6 unix_process_povray_conf()
    void WinConOptionsProcessor::process_povray_conf(void)
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
            m_permitted_paths.push_back(WinPath(string(POVLIBDIR "\\"), true, false));   // read*
            m_permitted_paths.push_back(WinPath(string(POVCONFDIR "\\"), false, false)); // read
        }

#ifdef WIN_DEBUG
        cerr << "PERMITTED PATHS" << endl;
#endif

        for(list<WinPath>::iterator iter = m_permitted_paths.begin(); iter != m_permitted_paths.end(); iter++)
        {
            m_Session->AddReadPath(iter->str, iter->descend);
#ifdef WIN_DEBUG
            fprintf(stderr,
                "  %s%s = \"%s\"\n",
                iter->writable ? "WRITE" : "READ", iter->descend ? "*" : "", iter->str.c_str()
            );
#endif
        }
    }

    bool WinConOptionsProcessor::file_exist(const string &name)
    {
        FILE *file = fopen(name.c_str(), "r");

        if(file != NULL)
            fclose(file);
        else
            return false;

        return true;
    }

    // based on 3.6 unix_process_povray_ini()
    void WinConOptionsProcessor::Process_povray_ini(vfeRenderOptions &opts)
    {
#ifdef WIN_DEBUG
		cerr << "READ POVINI" << endl;
#endif
		// try the file pointed to by POVINI
        string povini;
        char * povini_c = getenv("POVINI");
        if (povini_c)
        {
            povini = povini_c;
            if (file_exist(povini))
            {
#ifdef WIN_DEBUG
				cerr << "  Using INI Environment Varialbe: " << povini << endl;
#endif
                opts.AddINI(povini);

                return;
            }
            else
            {
                fprintf(stderr, "%s: INI environment: cannot open ", PACKAGE);
                perror(povini.c_str());
            }
        }

        // try any INI file in the current directory
        povini = ".\\povray.ini";
        if (file_exist(povini))
        {
#ifdef WIN_DEBUG
			cerr << "  Using current directory INI at: " << povini << endl;
#endif
            opts.AddINI(povini);
            return;
        }

        // try the user or system INI file
        if ((m_home.length() != 0) && file_exist(m_userini))
        {
#ifdef WIN_DEBUG
			cerr << "  Using user INI at: " << m_userini << endl;
#endif
            opts.AddINI(m_userini);
            return;
        }
        if (file_exist(m_sysini))
        {
#ifdef WIN_DEBUG
			cerr << "  Using system INI at: " << m_sysini << endl;
#endif
            opts.AddINI(m_sysini);
            return;
        }

        // try older user or system INI files
        if ((m_home.length() != 0) && file_exist(m_userini_old))
        {
#ifdef WIN_DEBUG
			cerr << "  Using legacy user INI at: " << m_userini << endl;
#endif
            opts.AddINI(m_userini_old);
            return;
        }
        if (file_exist(m_sysini_old))
        {
#ifdef WIN_DEBUG
			cerr << "  Using legacy system INI at: " << m_sysini << endl;
#endif
            opts.AddINI(m_sysini_old);
            return;
        }

		for (list<WinPath>::iterator iter = m_permitted_paths.begin(); iter != m_permitted_paths.end(); iter++)
		{
			string iniFile = iter->str + "povray.ini";
			if (file_exist(iniFile))
			{
#ifdef WIN_DEBUG
				cerr << "  Using INI at permitted path: " << iniFile << endl;
#endif
				opts.AddINI(iniFile);
				return;
			}
		}

        // warn that no INI file was found and add minimal library_path setting
        fprintf(stderr, "%s: cannot open an INI file, adding default library path\n", PACKAGE);
        opts.AddLibraryPath(string(POVLIBDIR "\\ini"));
    }

    bool WinConOptionsProcessor::isIORestrictionsEnabled(bool write)
    {
        if (IO_RESTRICTIONS_DISABLED)
            return false;
        if (!write &&  m_file_io < IO_RESTRICTED)
            return false;
        if(m_file_io < IO_READONLY)
            return false;
        return true;
    }

    void WinConOptionsProcessor::IORestrictionsError(const string &fnm, bool write, bool is_user_setting)
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
