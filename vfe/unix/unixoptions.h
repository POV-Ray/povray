//******************************************************************************
///
/// @file vfe/unix/unixoptions.h
///
/// Processing system for options in povray.conf, command line and environment
/// variables.
///
/// @author Christoph Hormann <chris_hormann@gmx.de>
/// @author based on unix.cpp Elements by Nicolas Calimet
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

#ifndef POVRAY_VFE_UNIX_UNIXOPTIONS_H
#define POVRAY_VFE_UNIX_UNIXOPTIONS_H

#include "vfe.h"

#include <list>
#include <iostream>
#include <string>
#include <boost/algorithm/string.hpp>

using boost::to_lower_copy;

/*
 * [NC]
 * Default values for the location of the povray library and configuration.
 * These constants don't have to be in config.h .
 */
#ifndef POVLIBDIR
# define POVLIBDIR  "/usr/local/share/" PACKAGE "-" VERSION_BASE
#endif

#ifndef POVCONFDIR
# define POVCONFDIR  "/usr/local/etc/" PACKAGE "/" VERSION_BASE
#endif

#ifndef POVCONFDIR_BACKWARD
# define POVCONFDIR_BACKWARD  "/usr/local/etc"
#endif


namespace vfePlatform
{
    /**
        @brief Processing system for options in povray.conf, command line and environment variables.

        @author Christoph Hormann <chris_hormann@gmx.de>

        @date June 2006

        This class wraps the processing of platform specific options set via

            - povray.conf files in the form:

                [section]
                name=value

            - command line options (which have to be removed before the options vector
                is passed to the core code for further processing)

            - environment variables

        It provides access to the settings made with those options to other parts of
        the code and allows subsystems like the display classes to register their own
        options.

        Registering custom options:

            even options not registered are read from povray.conf when present and
            their values are made available.  To register an options the public method
            Register() is provided.  It takes a vector of Option_Info structs containing
            the option information.  See disp_text.cpp for an (empty) example.

        The option values are currently stored as strings and the system does not
        distinguish between different types.

        Furthermore it processes the IO-restrictions settings and manages the povray.ini
        locations.

        Inner workings:

            The options are stored in the list m_user_options.

            The constructor sets up the locations of the configuration files
            (previously unix_create_globals()), adds the standard options to the list
            and processes the povray.conf files.  The io-restrictions settings from
            those files are read with the old proven system and stored in their own
            list (m_permitted_paths).  At the end these are transferred into the
            corresponding vfeSession fields.  All unknown options in the povray.conf
            files are read into a temporary list (m_custom_conf_options).

            The actual options processing is done in ProcessOptions(). There all
            options from the povray.conf files are transferred into the list.  Then
            the values of all options which can be set via command line and environment
            variables are determined.  Environment variables override povray.conf
            settings and command line options override environment variables.  The read
            command line options are removed from the argument list like previously in
            the xwindow code.

        An instance of this class is part of vfeUnixSession and can be accesses through
        vfeUnixSession::GetUnixOptions().

    */
    class UnixOptionsProcessor
    {
      public:
        /*
         * [NC] structures to handle configuration-related (povray.conf) code.
         */
        enum FileIO {IO_UNSET, IO_NONE, IO_READONLY, IO_RESTRICTED, IO_UNKNOWN};
        enum ShellOut {SHL_UNSET, SHL_ALLOWED, SHL_FORBIDDEN, SHL_UNKNOWN};

        /// permission path for IO restrictions settings
        struct UnixPath
        {
            std::string str;
            bool descend, writable;

            UnixPath(const std::string &s, bool desc = false, bool wrt = false) : str(s), descend(desc), writable(wrt) { }
        };

        /**
             Option of a povray.conf file of the form
             [Section]
             Name=Value
        */
        struct Conf_Option
        {
            std::string Section;
            std::string Name;
            std::string Value;

            Conf_Option(const std::string &Sect, const std::string &Nm, const std::string &Val = "") : Section(Sect), Name(Nm), Value(Val) { }
        };

        /**
             A platform specific configuration option
             with configuration file settings,
             Command line option (optional) and
             Environment variable (optional)

             This stucture is used by the Display classes to
             provide their own options and by the options
             processor to store the option values
        */
        struct Option_Info
        {
            std::string Section;
            std::string Name;
            std::string Value;
            std::string CmdOption;
            std::string EnvVariable;
            std::string Comment;
            bool has_param;

            Option_Info(const std::string &Sect ,
                        const std::string &Nm,
                        const std::string &Val = "",
                        bool par = false,
                        const std::string &Cmd = "",
                        const std::string &Env = "",
                        const std::string &Comm = "")
                : Section(Sect), Name(Nm), Value(Val), has_param(par), CmdOption(Cmd), EnvVariable(Env), Comment(Comm) { }

            /// only checks identity of the option, not of the selected value.
            bool operator==(Option_Info const& other) const
            {
                return to_lower_copy(Section) == to_lower_copy(other.Section) && to_lower_copy(Name) == to_lower_copy(other.Name);
            }

            /// compares sections (for sorting)
            // [JG] was reversed in previous versions, must return true when (*this) < other
            // order is per Section then per Name
            bool operator<(Option_Info const& other) const
            {
                if (to_lower_copy(other.Section) < to_lower_copy(Section))
                    return false;
                if (to_lower_copy(other.Section) == to_lower_copy(Section))
                    if (to_lower_copy(other.Name) < to_lower_copy(Name))
                        return false;
                return true;
            }
        };

        UnixOptionsProcessor(vfeSession *session);
        virtual ~UnixOptionsProcessor() {} ;

        /**
             called by the Display classes to register their custom options

             @param options Vector of Option_Info structs containing the options
        */
        void Register(const Option_Info options[]);

        /**
             Converts a file path to standard form replacing
             relative notations.
        */
        std::string CanonicalizePath(const std::string &path);

        /**
             Finds out the default location for temporary files.
             Set by an option, alternatively '/tmp/'.

             @returns temporary path including a trailing slash
        */
        std::string GetTemporaryPath(void);

        /**
             Finds the value of a certain option from
             either configuration file, command line
             or environment variable.

             @param option The option to query

             The value is returned in option.Value.
        */
        void QueryOption(Option_Info &option);

        /**
             Finds the value of a certain option via
             section and option name and returns it as
             a string.
        */
        std::string QueryOptionString(const std::string &section, const std::string &name);

        /**
             Finds the value of a certain option via
             section and option name and returns it as
             an int.  If the options value is not convertible
             to int dflt is returned instead.
        */
        int QueryOptionInt(const std::string &section, const std::string &name, const int dflt = 0);

        /**
             Finds the value of a certain option via
             section and option name and returns it as
             a float  If the options value is not convertible
             to float dflt is returned instead.
        */
        float QueryOptionFloat(const std::string &section, const std::string &name, const float dflt = 0.0);

        /**
             Check if a certain option has been set
             this is the case if a parameter free option is present of
             if the parameter is 'on', 'yes', 'true' or '1'.

             @returns true if set, false otherwise
        */
        bool isOptionSet(const Option_Info &option);
        bool isOptionSet(const std::string &section, const std::string &name);

        /**
             Adds the custom povray.conf options with their values
             to the main options list and reads the environment
             variables and command line options where advised.

             The argument list is replaced with a version
             with the custom options removed.
        */
        void ProcessOptions(int *argc, char **argv[]);

        /**
             Search for povray.ini at standard locations
             and add it to opts.
        */
        void Process_povray_ini(vfeRenderOptions &opts);

        /**
             Print all currrently registered command line
             options in an option summary.
        */
        void PrintOptions(void);

        /**
             Determines if IO restrictions are enabled at compile
             time and via povray.conf settings.

             @param write if false and permissions are 'free read' returns false.
        */
        bool isIORestrictionsEnabled(bool write);

        /**
             Prints an approriate error message if IO restrictions
             prevent access to a file.

             @param fnm File not permitted to access
             @param write If write acccess was requested
             @param is_user_setting if denial was due to user setting
        */
        void IORestrictionsError(const std::string &fnm, bool write, bool is_user_setting);

        bool ShelloutPermitted(const std::string& command, const std::string& parameters) const { return m_shellout == SHL_ALLOWED; }

     protected:
        /// list of standard options
        static const Option_Info Standard_Options[];

        std::string unix_getcwd(void);
        std::string basename(const std::string &path);
        std::string dirname(const std::string &path);
        std::string unix_readlink(const std::string &path);
        std::string pre_process_conf_line(const std::string &input);
        void add_permitted_path(std::list<UnixPath> &paths, const std::string &input, const std::string &conf_name, unsigned long line_number);
        void parse_conf_file(std::istream &Stream, const std::string &conf_name, bool user_mode);
        void process_povray_conf(void);
        void remove_arg(int *argc, char *argv[], int index);
        bool file_exist(const std::string &name);

        vfeSession *m_Session;
        std::string  m_home;
        std::string  m_user_dir;
        std::string  m_sysconf;                // system conf filename
        std::string  m_userconf;               // user conf filename
        std::string  m_conf;                   // selected conf file
        std::string  m_sysini, m_sysini_old;   // system ini filename
        std::string  m_userini, m_userini_old; // user ini filename
        FileIO  m_file_io;
        ShellOut m_shellout;
        std::list<UnixPath> m_permitted_paths;
        std::list<Conf_Option> m_custom_conf_options;
        std::list<Option_Info> m_user_options;
    };
}
// end of namespace vfePlatform

#endif // POVRAY_VFE_UNIX_UNIXOPTIONS_H
