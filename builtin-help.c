begin_unit
begin_comment
comment|/*  * builtin-help.c  *  * Builtin help command  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_include
include|#
directive|include
file|"common-cmds.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"help.h"
end_include
begin_struct
DECL|struct|man_viewer_list
specifier|static
struct|struct
name|man_viewer_list
block|{
DECL|member|next
name|struct
name|man_viewer_list
modifier|*
name|next
decl_stmt|;
DECL|member|name
name|char
name|name
index|[
name|FLEX_ARRAY
index|]
decl_stmt|;
block|}
DECL|variable|man_viewer_list
modifier|*
name|man_viewer_list
struct|;
end_struct
begin_struct
DECL|struct|man_viewer_info_list
specifier|static
struct|struct
name|man_viewer_info_list
block|{
DECL|member|next
name|struct
name|man_viewer_info_list
modifier|*
name|next
decl_stmt|;
DECL|member|info
specifier|const
name|char
modifier|*
name|info
decl_stmt|;
DECL|member|name
name|char
name|name
index|[
name|FLEX_ARRAY
index|]
decl_stmt|;
block|}
DECL|variable|man_viewer_info_list
modifier|*
name|man_viewer_info_list
struct|;
end_struct
begin_enum
DECL|enum|help_format
enum|enum
name|help_format
block|{
DECL|enumerator|HELP_FORMAT_MAN
name|HELP_FORMAT_MAN
block|,
DECL|enumerator|HELP_FORMAT_INFO
name|HELP_FORMAT_INFO
block|,
DECL|enumerator|HELP_FORMAT_WEB
name|HELP_FORMAT_WEB
block|, }
enum|;
end_enum
begin_decl_stmt
DECL|variable|show_all
specifier|static
name|int
name|show_all
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|help_format
specifier|static
name|enum
name|help_format
name|help_format
init|=
name|HELP_FORMAT_MAN
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|builtin_help_options
specifier|static
name|struct
name|option
name|builtin_help_options
index|[]
init|=
block|{
name|OPT_BOOLEAN
argument_list|(
literal|'a'
argument_list|,
literal|"all"
argument_list|,
operator|&
name|show_all
argument_list|,
literal|"print all available commands"
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|'m'
argument_list|,
literal|"man"
argument_list|,
operator|&
name|help_format
argument_list|,
literal|"show man page"
argument_list|,
name|HELP_FORMAT_MAN
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|'w'
argument_list|,
literal|"web"
argument_list|,
operator|&
name|help_format
argument_list|,
literal|"show manual in web browser"
argument_list|,
name|HELP_FORMAT_WEB
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|'i'
argument_list|,
literal|"info"
argument_list|,
operator|&
name|help_format
argument_list|,
literal|"show info page"
argument_list|,
name|HELP_FORMAT_INFO
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, }
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|builtin_help_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|builtin_help_usage
index|[]
init|=
block|{
literal|"git help [--all] [--man|--web|--info] [command]"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|parse_help_format
specifier|static
name|enum
name|help_format
name|parse_help_format
parameter_list|(
specifier|const
name|char
modifier|*
name|format
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|format
argument_list|,
literal|"man"
argument_list|)
condition|)
return|return
name|HELP_FORMAT_MAN
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|format
argument_list|,
literal|"info"
argument_list|)
condition|)
return|return
name|HELP_FORMAT_INFO
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|format
argument_list|,
literal|"web"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|format
argument_list|,
literal|"html"
argument_list|)
condition|)
return|return
name|HELP_FORMAT_WEB
return|;
name|die
argument_list|(
literal|"unrecognized help format '%s'"
argument_list|,
name|format
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_man_viewer_info
specifier|static
specifier|const
name|char
modifier|*
name|get_man_viewer_info
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|struct
name|man_viewer_info_list
modifier|*
name|viewer
decl_stmt|;
for|for
control|(
name|viewer
operator|=
name|man_viewer_info_list
init|;
name|viewer
condition|;
name|viewer
operator|=
name|viewer
operator|->
name|next
control|)
block|{
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|name
argument_list|,
name|viewer
operator|->
name|name
argument_list|)
condition|)
return|return
name|viewer
operator|->
name|info
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|check_emacsclient_version
specifier|static
name|int
name|check_emacsclient_version
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|strbuf
name|buffer
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|child_process
name|ec_process
decl_stmt|;
specifier|const
name|char
modifier|*
name|argv_ec
index|[]
init|=
block|{
literal|"emacsclient"
block|,
literal|"--version"
block|,
name|NULL
block|}
decl_stmt|;
name|int
name|version
decl_stmt|;
comment|/* emacsclient prints its version number on stderr */
name|memset
argument_list|(
operator|&
name|ec_process
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|ec_process
argument_list|)
argument_list|)
expr_stmt|;
name|ec_process
operator|.
name|argv
operator|=
name|argv_ec
expr_stmt|;
name|ec_process
operator|.
name|err
operator|=
operator|-
literal|1
expr_stmt|;
name|ec_process
operator|.
name|stdout_to_stderr
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|ec_process
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Failed to start emacsclient."
argument_list|)
return|;
name|strbuf_read
argument_list|(
operator|&
name|buffer
argument_list|,
name|ec_process
operator|.
name|err
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|ec_process
operator|.
name|err
argument_list|)
expr_stmt|;
comment|/* 	 * Don't bother checking return value, because "emacsclient --version" 	 * seems to always exits with code 1. 	 */
name|finish_command
argument_list|(
operator|&
name|ec_process
argument_list|)
expr_stmt|;
if|if
condition|(
name|prefixcmp
argument_list|(
name|buffer
operator|.
name|buf
argument_list|,
literal|"emacsclient"
argument_list|)
condition|)
block|{
name|strbuf_release
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"Failed to parse emacsclient version."
argument_list|)
return|;
block|}
name|strbuf_remove
argument_list|(
operator|&
name|buffer
argument_list|,
literal|0
argument_list|,
name|strlen
argument_list|(
literal|"emacsclient"
argument_list|)
argument_list|)
expr_stmt|;
name|version
operator|=
name|atoi
argument_list|(
name|buffer
operator|.
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|version
operator|<
literal|22
condition|)
block|{
name|strbuf_release
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"emacsclient version '%d' too old (< 22)."
argument_list|,
name|version
argument_list|)
return|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|exec_woman_emacs
specifier|static
name|void
name|exec_woman_emacs
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|page
parameter_list|)
block|{
if|if
condition|(
operator|!
name|check_emacsclient_version
argument_list|()
condition|)
block|{
comment|/* This works only with emacsclient version>= 22. */
name|struct
name|strbuf
name|man_page
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
operator|!
name|path
condition|)
name|path
operator|=
literal|"emacsclient"
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|man_page
argument_list|,
literal|"(woman \"%s\")"
argument_list|,
name|page
argument_list|)
expr_stmt|;
name|execlp
argument_list|(
name|path
argument_list|,
literal|"emacsclient"
argument_list|,
literal|"-e"
argument_list|,
name|man_page
operator|.
name|buf
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|warning
argument_list|(
literal|"failed to exec '%s': %s"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|exec_man_konqueror
specifier|static
name|void
name|exec_man_konqueror
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|page
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|display
init|=
name|getenv
argument_list|(
literal|"DISPLAY"
argument_list|)
decl_stmt|;
if|if
condition|(
name|display
operator|&&
operator|*
name|display
condition|)
block|{
name|struct
name|strbuf
name|man_page
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|const
name|char
modifier|*
name|filename
init|=
literal|"kfmclient"
decl_stmt|;
comment|/* It's simpler to launch konqueror using kfmclient. */
if|if
condition|(
name|path
condition|)
block|{
specifier|const
name|char
modifier|*
name|file
init|=
name|strrchr
argument_list|(
name|path
argument_list|,
literal|'/'
argument_list|)
decl_stmt|;
if|if
condition|(
name|file
operator|&&
operator|!
name|strcmp
argument_list|(
name|file
operator|+
literal|1
argument_list|,
literal|"konqueror"
argument_list|)
condition|)
block|{
name|char
modifier|*
name|new
init|=
name|xstrdup
argument_list|(
name|path
argument_list|)
decl_stmt|;
name|char
modifier|*
name|dest
init|=
name|strrchr
argument_list|(
name|new
argument_list|,
literal|'/'
argument_list|)
decl_stmt|;
comment|/* strlen("konqueror") == strlen("kfmclient") */
name|strcpy
argument_list|(
name|dest
operator|+
literal|1
argument_list|,
literal|"kfmclient"
argument_list|)
expr_stmt|;
name|path
operator|=
name|new
expr_stmt|;
block|}
if|if
condition|(
name|file
condition|)
name|filename
operator|=
name|file
expr_stmt|;
block|}
else|else
name|path
operator|=
literal|"kfmclient"
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|man_page
argument_list|,
literal|"man:%s(1)"
argument_list|,
name|page
argument_list|)
expr_stmt|;
name|execlp
argument_list|(
name|path
argument_list|,
name|filename
argument_list|,
literal|"newTab"
argument_list|,
name|man_page
operator|.
name|buf
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|warning
argument_list|(
literal|"failed to exec '%s': %s"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|exec_man_man
specifier|static
name|void
name|exec_man_man
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|page
parameter_list|)
block|{
if|if
condition|(
operator|!
name|path
condition|)
name|path
operator|=
literal|"man"
expr_stmt|;
name|execlp
argument_list|(
name|path
argument_list|,
literal|"man"
argument_list|,
name|page
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|warning
argument_list|(
literal|"failed to exec '%s': %s"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|exec_man_cmd
specifier|static
name|void
name|exec_man_cmd
parameter_list|(
specifier|const
name|char
modifier|*
name|cmd
parameter_list|,
specifier|const
name|char
modifier|*
name|page
parameter_list|)
block|{
name|struct
name|strbuf
name|shell_cmd
init|=
name|STRBUF_INIT
decl_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|shell_cmd
argument_list|,
literal|"%s %s"
argument_list|,
name|cmd
argument_list|,
name|page
argument_list|)
expr_stmt|;
name|execl
argument_list|(
literal|"/bin/sh"
argument_list|,
literal|"sh"
argument_list|,
literal|"-c"
argument_list|,
name|shell_cmd
operator|.
name|buf
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|warning
argument_list|(
literal|"failed to exec '%s': %s"
argument_list|,
name|cmd
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_man_viewer
specifier|static
name|void
name|add_man_viewer
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|struct
name|man_viewer_list
modifier|*
modifier|*
name|p
init|=
operator|&
name|man_viewer_list
decl_stmt|;
name|size_t
name|len
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
while|while
condition|(
operator|*
name|p
condition|)
name|p
operator|=
operator|&
operator|(
operator|(
operator|*
name|p
operator|)
operator|->
name|next
operator|)
expr_stmt|;
operator|*
name|p
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
operator|(
sizeof|sizeof
argument_list|(
operator|*
operator|*
name|p
argument_list|)
operator|+
name|len
operator|+
literal|1
operator|)
argument_list|)
expr_stmt|;
name|strncpy
argument_list|(
operator|(
operator|*
name|p
operator|)
operator|->
name|name
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|supported_man_viewer
specifier|static
name|int
name|supported_man_viewer
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
return|return
operator|(
operator|!
name|strncasecmp
argument_list|(
literal|"man"
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
operator|||
operator|!
name|strncasecmp
argument_list|(
literal|"woman"
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
operator|||
operator|!
name|strncasecmp
argument_list|(
literal|"konqueror"
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
operator|)
return|;
block|}
end_function
begin_function
DECL|function|do_add_man_viewer_info
specifier|static
name|void
name|do_add_man_viewer_info
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|size_t
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
name|struct
name|man_viewer_info_list
modifier|*
name|new
init|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|new
argument_list|)
operator|+
name|len
operator|+
literal|1
argument_list|)
decl_stmt|;
name|strncpy
argument_list|(
name|new
operator|->
name|name
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|new
operator|->
name|info
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
name|new
operator|->
name|next
operator|=
name|man_viewer_info_list
expr_stmt|;
name|man_viewer_info_list
operator|=
name|new
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_man_viewer_path
specifier|static
name|int
name|add_man_viewer_path
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|size_t
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
if|if
condition|(
name|supported_man_viewer
argument_list|(
name|name
argument_list|,
name|len
argument_list|)
condition|)
name|do_add_man_viewer_info
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|value
argument_list|)
expr_stmt|;
else|else
name|warning
argument_list|(
literal|"'%s': path for unsupported man viewer.\n"
literal|"Please consider using 'man.<tool>.cmd' instead."
argument_list|,
name|name
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|add_man_viewer_cmd
specifier|static
name|int
name|add_man_viewer_cmd
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|size_t
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
if|if
condition|(
name|supported_man_viewer
argument_list|(
name|name
argument_list|,
name|len
argument_list|)
condition|)
name|warning
argument_list|(
literal|"'%s': cmd for supported man viewer.\n"
literal|"Please consider using 'man.<tool>.path' instead."
argument_list|,
name|name
argument_list|)
expr_stmt|;
else|else
name|do_add_man_viewer_info
argument_list|(
name|name
argument_list|,
name|len
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|add_man_viewer_info
specifier|static
name|int
name|add_man_viewer_info
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|name
init|=
name|var
operator|+
literal|4
decl_stmt|;
specifier|const
name|char
modifier|*
name|subkey
init|=
name|strrchr
argument_list|(
name|name
argument_list|,
literal|'.'
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|subkey
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|subkey
argument_list|,
literal|".path"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
return|;
return|return
name|add_man_viewer_path
argument_list|(
name|name
argument_list|,
name|subkey
operator|-
name|name
argument_list|,
name|value
argument_list|)
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|subkey
argument_list|,
literal|".cmd"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
return|;
return|return
name|add_man_viewer_cmd
argument_list|(
name|name
argument_list|,
name|subkey
operator|-
name|name
argument_list|,
name|value
argument_list|)
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|git_help_config
specifier|static
name|int
name|git_help_config
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
name|void
modifier|*
name|cb
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"help.format"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
return|;
name|help_format
operator|=
name|parse_help_format
argument_list|(
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"man.viewer"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
return|;
name|add_man_viewer
argument_list|(
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|var
argument_list|,
literal|"man."
argument_list|)
condition|)
return|return
name|add_man_viewer_info
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
return|;
return|return
name|git_default_config
argument_list|(
name|var
argument_list|,
name|value
argument_list|,
name|cb
argument_list|)
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|main_cmds
DECL|variable|other_cmds
specifier|static
name|struct
name|cmdnames
name|main_cmds
decl_stmt|,
name|other_cmds
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|list_common_cmds_help
name|void
name|list_common_cmds_help
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|longest
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ARRAY_SIZE
argument_list|(
name|common_cmds
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|longest
operator|<
name|strlen
argument_list|(
name|common_cmds
index|[
name|i
index|]
operator|.
name|name
argument_list|)
condition|)
name|longest
operator|=
name|strlen
argument_list|(
name|common_cmds
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
block|}
name|puts
argument_list|(
literal|"The most commonly used git commands are:"
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ARRAY_SIZE
argument_list|(
name|common_cmds
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
name|printf
argument_list|(
literal|"   %s   "
argument_list|,
name|common_cmds
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
name|mput_char
argument_list|(
literal|' '
argument_list|,
name|longest
operator|-
name|strlen
argument_list|(
name|common_cmds
index|[
name|i
index|]
operator|.
name|name
argument_list|)
argument_list|)
expr_stmt|;
name|puts
argument_list|(
name|common_cmds
index|[
name|i
index|]
operator|.
name|help
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|is_git_command
specifier|static
name|int
name|is_git_command
parameter_list|(
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
return|return
name|is_in_cmdlist
argument_list|(
operator|&
name|main_cmds
argument_list|,
name|s
argument_list|)
operator|||
name|is_in_cmdlist
argument_list|(
operator|&
name|other_cmds
argument_list|,
name|s
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|prepend
specifier|static
specifier|const
name|char
modifier|*
name|prepend
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|cmd
parameter_list|)
block|{
name|size_t
name|pre_len
init|=
name|strlen
argument_list|(
name|prefix
argument_list|)
decl_stmt|;
name|size_t
name|cmd_len
init|=
name|strlen
argument_list|(
name|cmd
argument_list|)
decl_stmt|;
name|char
modifier|*
name|p
init|=
name|xmalloc
argument_list|(
name|pre_len
operator|+
name|cmd_len
operator|+
literal|1
argument_list|)
decl_stmt|;
name|memcpy
argument_list|(
name|p
argument_list|,
name|prefix
argument_list|,
name|pre_len
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|p
operator|+
name|pre_len
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
return|return
name|p
return|;
block|}
end_function
begin_function
DECL|function|cmd_to_page
specifier|static
specifier|const
name|char
modifier|*
name|cmd_to_page
parameter_list|(
specifier|const
name|char
modifier|*
name|git_cmd
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_cmd
condition|)
return|return
literal|"git"
return|;
elseif|else
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|git_cmd
argument_list|,
literal|"git"
argument_list|)
condition|)
return|return
name|git_cmd
return|;
elseif|else
if|if
condition|(
name|is_git_command
argument_list|(
name|git_cmd
argument_list|)
condition|)
return|return
name|prepend
argument_list|(
literal|"git-"
argument_list|,
name|git_cmd
argument_list|)
return|;
else|else
return|return
name|prepend
argument_list|(
literal|"git"
argument_list|,
name|git_cmd
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|setup_man_path
specifier|static
name|void
name|setup_man_path
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|strbuf
name|new_path
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|const
name|char
modifier|*
name|old_path
init|=
name|getenv
argument_list|(
literal|"MANPATH"
argument_list|)
decl_stmt|;
comment|/* We should always put ':' after our path. If there is no 	 * old_path, the ':' at the end will let 'man' to try 	 * system-wide paths after ours to find the manual page. If 	 * there is old_path, we need ':' as delimiter. */
name|strbuf_addstr
argument_list|(
operator|&
name|new_path
argument_list|,
name|system_path
argument_list|(
name|GIT_MAN_PATH
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
operator|&
name|new_path
argument_list|,
literal|':'
argument_list|)
expr_stmt|;
if|if
condition|(
name|old_path
condition|)
name|strbuf_addstr
argument_list|(
operator|&
name|new_path
argument_list|,
name|old_path
argument_list|)
expr_stmt|;
name|setenv
argument_list|(
literal|"MANPATH"
argument_list|,
name|new_path
operator|.
name|buf
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|new_path
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|exec_viewer
specifier|static
name|void
name|exec_viewer
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|page
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|info
init|=
name|get_man_viewer_info
argument_list|(
name|name
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|name
argument_list|,
literal|"man"
argument_list|)
condition|)
name|exec_man_man
argument_list|(
name|info
argument_list|,
name|page
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|name
argument_list|,
literal|"woman"
argument_list|)
condition|)
name|exec_woman_emacs
argument_list|(
name|info
argument_list|,
name|page
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|name
argument_list|,
literal|"konqueror"
argument_list|)
condition|)
name|exec_man_konqueror
argument_list|(
name|info
argument_list|,
name|page
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|info
condition|)
name|exec_man_cmd
argument_list|(
name|info
argument_list|,
name|page
argument_list|)
expr_stmt|;
else|else
name|warning
argument_list|(
literal|"'%s': unknown man viewer."
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_man_page
specifier|static
name|void
name|show_man_page
parameter_list|(
specifier|const
name|char
modifier|*
name|git_cmd
parameter_list|)
block|{
name|struct
name|man_viewer_list
modifier|*
name|viewer
decl_stmt|;
specifier|const
name|char
modifier|*
name|page
init|=
name|cmd_to_page
argument_list|(
name|git_cmd
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|fallback
init|=
name|getenv
argument_list|(
literal|"GIT_MAN_VIEWER"
argument_list|)
decl_stmt|;
name|setup_man_path
argument_list|()
expr_stmt|;
for|for
control|(
name|viewer
operator|=
name|man_viewer_list
init|;
name|viewer
condition|;
name|viewer
operator|=
name|viewer
operator|->
name|next
control|)
block|{
name|exec_viewer
argument_list|(
name|viewer
operator|->
name|name
argument_list|,
name|page
argument_list|)
expr_stmt|;
comment|/* will return when unable */
block|}
if|if
condition|(
name|fallback
condition|)
name|exec_viewer
argument_list|(
name|fallback
argument_list|,
name|page
argument_list|)
expr_stmt|;
name|exec_viewer
argument_list|(
literal|"man"
argument_list|,
name|page
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"no man viewer handled the request"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_info_page
specifier|static
name|void
name|show_info_page
parameter_list|(
specifier|const
name|char
modifier|*
name|git_cmd
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|page
init|=
name|cmd_to_page
argument_list|(
name|git_cmd
argument_list|)
decl_stmt|;
name|setenv
argument_list|(
literal|"INFOPATH"
argument_list|,
name|system_path
argument_list|(
name|GIT_INFO_PATH
argument_list|)
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|execlp
argument_list|(
literal|"info"
argument_list|,
literal|"info"
argument_list|,
literal|"gitman"
argument_list|,
name|page
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"no info viewer handled the request"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_html_page_path
specifier|static
name|void
name|get_html_page_path
parameter_list|(
name|struct
name|strbuf
modifier|*
name|page_path
parameter_list|,
specifier|const
name|char
modifier|*
name|page
parameter_list|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
specifier|const
name|char
modifier|*
name|html_path
init|=
name|system_path
argument_list|(
name|GIT_HTML_PATH
argument_list|)
decl_stmt|;
comment|/* Check that we have a git documentation directory. */
if|if
condition|(
name|stat
argument_list|(
name|mkpath
argument_list|(
literal|"%s/git.html"
argument_list|,
name|html_path
argument_list|)
argument_list|,
operator|&
name|st
argument_list|)
operator|||
operator|!
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
name|die
argument_list|(
literal|"'%s': not a documentation directory."
argument_list|,
name|html_path
argument_list|)
expr_stmt|;
name|strbuf_init
argument_list|(
name|page_path
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
name|page_path
argument_list|,
literal|"%s/%s.html"
argument_list|,
name|html_path
argument_list|,
name|page
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * If open_html is not defined in a platform-specific way (see for  * example compat/mingw.h), we use the script web--browse to display  * HTML.  */
end_comment
begin_ifndef
ifndef|#
directive|ifndef
name|open_html
end_ifndef
begin_function
DECL|function|open_html
specifier|static
name|void
name|open_html
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|execl_git_cmd
argument_list|(
literal|"web--browse"
argument_list|,
literal|"-c"
argument_list|,
literal|"help.browser"
argument_list|,
name|path
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|show_html_page
specifier|static
name|void
name|show_html_page
parameter_list|(
specifier|const
name|char
modifier|*
name|git_cmd
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|page
init|=
name|cmd_to_page
argument_list|(
name|git_cmd
argument_list|)
decl_stmt|;
name|struct
name|strbuf
name|page_path
decl_stmt|;
comment|/* it leaks but we exec bellow */
name|get_html_page_path
argument_list|(
operator|&
name|page_path
argument_list|,
name|page
argument_list|)
expr_stmt|;
name|open_html
argument_list|(
name|page_path
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_help
name|int
name|cmd_help
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|int
name|nongit
decl_stmt|;
specifier|const
name|char
modifier|*
name|alias
decl_stmt|;
name|load_command_list
argument_list|(
literal|"git-"
argument_list|,
operator|&
name|main_cmds
argument_list|,
operator|&
name|other_cmds
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|builtin_help_options
argument_list|,
name|builtin_help_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|show_all
condition|)
block|{
name|printf
argument_list|(
literal|"usage: %s\n\n"
argument_list|,
name|git_usage_string
argument_list|)
expr_stmt|;
name|list_commands
argument_list|(
literal|"git commands"
argument_list|,
operator|&
name|main_cmds
argument_list|,
operator|&
name|other_cmds
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|git_more_info_string
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|setup_git_directory_gently
argument_list|(
operator|&
name|nongit
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|git_help_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|argv
index|[
literal|0
index|]
condition|)
block|{
name|printf
argument_list|(
literal|"usage: %s\n\n"
argument_list|,
name|git_usage_string
argument_list|)
expr_stmt|;
name|list_common_cmds_help
argument_list|()
expr_stmt|;
name|printf
argument_list|(
literal|"\n%s\n"
argument_list|,
name|git_more_info_string
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|alias
operator|=
name|alias_lookup
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|alias
operator|&&
operator|!
name|is_git_command
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
condition|)
block|{
name|printf
argument_list|(
literal|"`git %s' is aliased to `%s'\n"
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|,
name|alias
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
switch|switch
condition|(
name|help_format
condition|)
block|{
case|case
name|HELP_FORMAT_MAN
case|:
name|show_man_page
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
break|break;
case|case
name|HELP_FORMAT_INFO
case|:
name|show_info_page
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
break|break;
case|case
name|HELP_FORMAT_WEB
case|:
name|show_html_page
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
break|break;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
