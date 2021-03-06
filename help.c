begin_unit
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
file|"levenshtein.h"
end_include
begin_include
include|#
directive|include
file|"help.h"
end_include
begin_include
include|#
directive|include
file|"common-cmds.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"column.h"
end_include
begin_include
include|#
directive|include
file|"version.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_function
DECL|function|add_cmdname
name|void
name|add_cmdname
parameter_list|(
name|struct
name|cmdnames
modifier|*
name|cmds
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|struct
name|cmdname
modifier|*
name|ent
decl_stmt|;
name|FLEX_ALLOC_MEM
argument_list|(
name|ent
argument_list|,
name|name
argument_list|,
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|ent
operator|->
name|len
operator|=
name|len
expr_stmt|;
name|ALLOC_GROW
argument_list|(
name|cmds
operator|->
name|names
argument_list|,
name|cmds
operator|->
name|cnt
operator|+
literal|1
argument_list|,
name|cmds
operator|->
name|alloc
argument_list|)
expr_stmt|;
name|cmds
operator|->
name|names
index|[
name|cmds
operator|->
name|cnt
operator|++
index|]
operator|=
name|ent
expr_stmt|;
block|}
end_function
begin_function
DECL|function|clean_cmdnames
specifier|static
name|void
name|clean_cmdnames
parameter_list|(
name|struct
name|cmdnames
modifier|*
name|cmds
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|cmds
operator|->
name|cnt
condition|;
operator|++
name|i
control|)
name|free
argument_list|(
name|cmds
operator|->
name|names
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|cmds
operator|->
name|names
argument_list|)
expr_stmt|;
name|cmds
operator|->
name|cnt
operator|=
literal|0
expr_stmt|;
name|cmds
operator|->
name|alloc
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmdname_compare
specifier|static
name|int
name|cmdname_compare
parameter_list|(
specifier|const
name|void
modifier|*
name|a_
parameter_list|,
specifier|const
name|void
modifier|*
name|b_
parameter_list|)
block|{
name|struct
name|cmdname
modifier|*
name|a
init|=
operator|*
operator|(
expr|struct
name|cmdname
operator|*
operator|*
operator|)
name|a_
decl_stmt|;
name|struct
name|cmdname
modifier|*
name|b
init|=
operator|*
operator|(
expr|struct
name|cmdname
operator|*
operator|*
operator|)
name|b_
decl_stmt|;
return|return
name|strcmp
argument_list|(
name|a
operator|->
name|name
argument_list|,
name|b
operator|->
name|name
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|uniq
specifier|static
name|void
name|uniq
parameter_list|(
name|struct
name|cmdnames
modifier|*
name|cmds
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|j
decl_stmt|;
if|if
condition|(
operator|!
name|cmds
operator|->
name|cnt
condition|)
return|return;
for|for
control|(
name|i
operator|=
name|j
operator|=
literal|1
init|;
name|i
operator|<
name|cmds
operator|->
name|cnt
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|cmds
operator|->
name|names
index|[
name|i
index|]
operator|->
name|name
argument_list|,
name|cmds
operator|->
name|names
index|[
name|j
operator|-
literal|1
index|]
operator|->
name|name
argument_list|)
condition|)
name|free
argument_list|(
name|cmds
operator|->
name|names
index|[
name|i
index|]
argument_list|)
expr_stmt|;
else|else
name|cmds
operator|->
name|names
index|[
name|j
operator|++
index|]
operator|=
name|cmds
operator|->
name|names
index|[
name|i
index|]
expr_stmt|;
block|}
name|cmds
operator|->
name|cnt
operator|=
name|j
expr_stmt|;
block|}
end_function
begin_function
DECL|function|exclude_cmds
name|void
name|exclude_cmds
parameter_list|(
name|struct
name|cmdnames
modifier|*
name|cmds
parameter_list|,
name|struct
name|cmdnames
modifier|*
name|excludes
parameter_list|)
block|{
name|int
name|ci
decl_stmt|,
name|cj
decl_stmt|,
name|ei
decl_stmt|;
name|int
name|cmp
decl_stmt|;
name|ci
operator|=
name|cj
operator|=
name|ei
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|ci
operator|<
name|cmds
operator|->
name|cnt
operator|&&
name|ei
operator|<
name|excludes
operator|->
name|cnt
condition|)
block|{
name|cmp
operator|=
name|strcmp
argument_list|(
name|cmds
operator|->
name|names
index|[
name|ci
index|]
operator|->
name|name
argument_list|,
name|excludes
operator|->
name|names
index|[
name|ei
index|]
operator|->
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmp
operator|<
literal|0
condition|)
name|cmds
operator|->
name|names
index|[
name|cj
operator|++
index|]
operator|=
name|cmds
operator|->
name|names
index|[
name|ci
operator|++
index|]
expr_stmt|;
elseif|else
if|if
condition|(
name|cmp
operator|==
literal|0
condition|)
block|{
name|ei
operator|++
expr_stmt|;
name|free
argument_list|(
name|cmds
operator|->
name|names
index|[
name|ci
operator|++
index|]
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|cmp
operator|>
literal|0
condition|)
name|ei
operator|++
expr_stmt|;
block|}
while|while
condition|(
name|ci
operator|<
name|cmds
operator|->
name|cnt
condition|)
name|cmds
operator|->
name|names
index|[
name|cj
operator|++
index|]
operator|=
name|cmds
operator|->
name|names
index|[
name|ci
operator|++
index|]
expr_stmt|;
name|cmds
operator|->
name|cnt
operator|=
name|cj
expr_stmt|;
block|}
end_function
begin_function
DECL|function|pretty_print_cmdnames
specifier|static
name|void
name|pretty_print_cmdnames
parameter_list|(
name|struct
name|cmdnames
modifier|*
name|cmds
parameter_list|,
name|unsigned
name|int
name|colopts
parameter_list|)
block|{
name|struct
name|string_list
name|list
init|=
name|STRING_LIST_INIT_NODUP
decl_stmt|;
name|struct
name|column_options
name|copts
decl_stmt|;
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|cmds
operator|->
name|cnt
condition|;
name|i
operator|++
control|)
name|string_list_append
argument_list|(
operator|&
name|list
argument_list|,
name|cmds
operator|->
name|names
index|[
name|i
index|]
operator|->
name|name
argument_list|)
expr_stmt|;
comment|/* 	 * always enable column display, we only consult column.* 	 * about layout strategy and stuff 	 */
name|colopts
operator|=
operator|(
name|colopts
operator|&
operator|~
name|COL_ENABLE_MASK
operator|)
operator||
name|COL_ENABLED
expr_stmt|;
name|memset
argument_list|(
operator|&
name|copts
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|copts
argument_list|)
argument_list|)
expr_stmt|;
name|copts
operator|.
name|indent
operator|=
literal|"  "
expr_stmt|;
name|copts
operator|.
name|padding
operator|=
literal|2
expr_stmt|;
name|print_columns
argument_list|(
operator|&
name|list
argument_list|,
name|colopts
argument_list|,
operator|&
name|copts
argument_list|)
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|list
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|is_executable
specifier|static
name|int
name|is_executable
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
name|stat
argument_list|(
name|name
argument_list|,
operator|&
name|st
argument_list|)
operator|||
comment|/* stat, not lstat */
operator|!
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
return|return
literal|0
return|;
if|#
directive|if
name|defined
argument_list|(
name|GIT_WINDOWS_NATIVE
argument_list|)
block|{
comment|/* cannot trust the executable bit, peek into the file instead */
name|char
name|buf
index|[
literal|3
index|]
init|=
block|{
literal|0
block|}
decl_stmt|;
name|int
name|n
decl_stmt|;
name|int
name|fd
init|=
name|open
argument_list|(
name|name
argument_list|,
name|O_RDONLY
argument_list|)
decl_stmt|;
name|st
operator|.
name|st_mode
operator|&=
operator|~
name|S_IXUSR
expr_stmt|;
if|if
condition|(
name|fd
operator|>=
literal|0
condition|)
block|{
name|n
operator|=
name|read
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
literal|2
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
operator|==
literal|2
condition|)
comment|/* DOS executables start with "MZ" */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"#!"
argument_list|)
operator|||
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"MZ"
argument_list|)
condition|)
name|st
operator|.
name|st_mode
operator||=
name|S_IXUSR
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
block|}
endif|#
directive|endif
return|return
name|st
operator|.
name|st_mode
operator|&
name|S_IXUSR
return|;
block|}
end_function
begin_function
DECL|function|list_commands_in_dir
specifier|static
name|void
name|list_commands_in_dir
parameter_list|(
name|struct
name|cmdnames
modifier|*
name|cmds
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|DIR
modifier|*
name|dir
init|=
name|opendir
argument_list|(
name|path
argument_list|)
decl_stmt|;
name|struct
name|dirent
modifier|*
name|de
decl_stmt|;
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|len
decl_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
return|return;
if|if
condition|(
operator|!
name|prefix
condition|)
name|prefix
operator|=
literal|"git-"
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|buf
argument_list|,
literal|"%s/"
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|len
operator|=
name|buf
operator|.
name|len
expr_stmt|;
while|while
condition|(
operator|(
name|de
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
specifier|const
name|char
modifier|*
name|ent
decl_stmt|;
name|size_t
name|entlen
decl_stmt|;
if|if
condition|(
operator|!
name|skip_prefix
argument_list|(
name|de
operator|->
name|d_name
argument_list|,
name|prefix
argument_list|,
operator|&
name|ent
argument_list|)
condition|)
continue|continue;
name|strbuf_setlen
argument_list|(
operator|&
name|buf
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|buf
argument_list|,
name|de
operator|->
name|d_name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|is_executable
argument_list|(
name|buf
operator|.
name|buf
argument_list|)
condition|)
continue|continue;
name|entlen
operator|=
name|strlen
argument_list|(
name|ent
argument_list|)
expr_stmt|;
name|strip_suffix
argument_list|(
name|ent
argument_list|,
literal|".exe"
argument_list|,
operator|&
name|entlen
argument_list|)
expr_stmt|;
name|add_cmdname
argument_list|(
name|cmds
argument_list|,
name|ent
argument_list|,
name|entlen
argument_list|)
expr_stmt|;
block|}
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|load_command_list
name|void
name|load_command_list
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|struct
name|cmdnames
modifier|*
name|main_cmds
parameter_list|,
name|struct
name|cmdnames
modifier|*
name|other_cmds
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|env_path
init|=
name|getenv
argument_list|(
literal|"PATH"
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|exec_path
init|=
name|git_exec_path
argument_list|()
decl_stmt|;
if|if
condition|(
name|exec_path
condition|)
block|{
name|list_commands_in_dir
argument_list|(
name|main_cmds
argument_list|,
name|exec_path
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
name|qsort
argument_list|(
name|main_cmds
operator|->
name|names
argument_list|,
name|main_cmds
operator|->
name|cnt
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|main_cmds
operator|->
name|names
argument_list|)
argument_list|,
name|cmdname_compare
argument_list|)
expr_stmt|;
name|uniq
argument_list|(
name|main_cmds
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|env_path
condition|)
block|{
name|char
modifier|*
name|paths
decl_stmt|,
modifier|*
name|path
decl_stmt|,
modifier|*
name|colon
decl_stmt|;
name|path
operator|=
name|paths
operator|=
name|xstrdup
argument_list|(
name|env_path
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
if|if
condition|(
operator|(
name|colon
operator|=
name|strchr
argument_list|(
name|path
argument_list|,
name|PATH_SEP
argument_list|)
operator|)
condition|)
operator|*
name|colon
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|exec_path
operator|||
name|strcmp
argument_list|(
name|path
argument_list|,
name|exec_path
argument_list|)
condition|)
name|list_commands_in_dir
argument_list|(
name|other_cmds
argument_list|,
name|path
argument_list|,
name|prefix
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|colon
condition|)
break|break;
name|path
operator|=
name|colon
operator|+
literal|1
expr_stmt|;
block|}
name|free
argument_list|(
name|paths
argument_list|)
expr_stmt|;
name|qsort
argument_list|(
name|other_cmds
operator|->
name|names
argument_list|,
name|other_cmds
operator|->
name|cnt
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|other_cmds
operator|->
name|names
argument_list|)
argument_list|,
name|cmdname_compare
argument_list|)
expr_stmt|;
name|uniq
argument_list|(
name|other_cmds
argument_list|)
expr_stmt|;
block|}
name|exclude_cmds
argument_list|(
name|other_cmds
argument_list|,
name|main_cmds
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|list_commands
name|void
name|list_commands
parameter_list|(
name|unsigned
name|int
name|colopts
parameter_list|,
name|struct
name|cmdnames
modifier|*
name|main_cmds
parameter_list|,
name|struct
name|cmdnames
modifier|*
name|other_cmds
parameter_list|)
block|{
if|if
condition|(
name|main_cmds
operator|->
name|cnt
condition|)
block|{
specifier|const
name|char
modifier|*
name|exec_path
init|=
name|git_exec_path
argument_list|()
decl_stmt|;
name|printf_ln
argument_list|(
name|_
argument_list|(
literal|"available git commands in '%s'"
argument_list|)
argument_list|,
name|exec_path
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
name|pretty_print_cmdnames
argument_list|(
name|main_cmds
argument_list|,
name|colopts
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|other_cmds
operator|->
name|cnt
condition|)
block|{
name|printf_ln
argument_list|(
name|_
argument_list|(
literal|"git commands available from elsewhere on your $PATH"
argument_list|)
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
name|pretty_print_cmdnames
argument_list|(
name|other_cmds
argument_list|,
name|colopts
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|cmd_group_cmp
specifier|static
name|int
name|cmd_group_cmp
parameter_list|(
specifier|const
name|void
modifier|*
name|elem1
parameter_list|,
specifier|const
name|void
modifier|*
name|elem2
parameter_list|)
block|{
specifier|const
name|struct
name|cmdname_help
modifier|*
name|e1
init|=
name|elem1
decl_stmt|;
specifier|const
name|struct
name|cmdname_help
modifier|*
name|e2
init|=
name|elem2
decl_stmt|;
if|if
condition|(
name|e1
operator|->
name|group
operator|<
name|e2
operator|->
name|group
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|e1
operator|->
name|group
operator|>
name|e2
operator|->
name|group
condition|)
return|return
literal|1
return|;
return|return
name|strcmp
argument_list|(
name|e1
operator|->
name|name
argument_list|,
name|e2
operator|->
name|name
argument_list|)
return|;
block|}
end_function
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
name|int
name|current_grp
init|=
operator|-
literal|1
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
name|qsort
argument_list|(
name|common_cmds
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|common_cmds
argument_list|)
argument_list|,
sizeof|sizeof
argument_list|(
name|common_cmds
index|[
literal|0
index|]
argument_list|)
argument_list|,
name|cmd_group_cmp
argument_list|)
expr_stmt|;
name|puts
argument_list|(
name|_
argument_list|(
literal|"These are common Git commands used in various situations:"
argument_list|)
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
if|if
condition|(
name|common_cmds
index|[
name|i
index|]
operator|.
name|group
operator|!=
name|current_grp
condition|)
block|{
name|printf
argument_list|(
literal|"\n%s\n"
argument_list|,
name|_
argument_list|(
name|common_cmd_groups
index|[
name|common_cmds
index|[
name|i
index|]
operator|.
name|group
index|]
argument_list|)
argument_list|)
expr_stmt|;
name|current_grp
operator|=
name|common_cmds
index|[
name|i
index|]
operator|.
name|group
expr_stmt|;
block|}
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
name|_
argument_list|(
name|common_cmds
index|[
name|i
index|]
operator|.
name|help
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|is_in_cmdlist
name|int
name|is_in_cmdlist
parameter_list|(
name|struct
name|cmdnames
modifier|*
name|c
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|c
operator|->
name|cnt
condition|;
name|i
operator|++
control|)
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|s
argument_list|,
name|c
operator|->
name|names
index|[
name|i
index|]
operator|->
name|name
argument_list|)
condition|)
return|return
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|autocorrect
specifier|static
name|int
name|autocorrect
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|aliases
specifier|static
name|struct
name|cmdnames
name|aliases
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|git_unknown_cmd_config
specifier|static
name|int
name|git_unknown_cmd_config
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
specifier|const
name|char
modifier|*
name|p
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"help.autocorrect"
argument_list|)
condition|)
name|autocorrect
operator|=
name|git_config_int
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
comment|/* Also use aliases for command lookup */
if|if
condition|(
name|skip_prefix
argument_list|(
name|var
argument_list|,
literal|"alias."
argument_list|,
operator|&
name|p
argument_list|)
condition|)
name|add_cmdname
argument_list|(
operator|&
name|aliases
argument_list|,
name|p
argument_list|,
name|strlen
argument_list|(
name|p
argument_list|)
argument_list|)
expr_stmt|;
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
begin_function
DECL|function|levenshtein_compare
specifier|static
name|int
name|levenshtein_compare
parameter_list|(
specifier|const
name|void
modifier|*
name|p1
parameter_list|,
specifier|const
name|void
modifier|*
name|p2
parameter_list|)
block|{
specifier|const
name|struct
name|cmdname
modifier|*
specifier|const
modifier|*
name|c1
init|=
name|p1
decl_stmt|,
modifier|*
decl_stmt|const
modifier|*
name|c2
init|=
name|p2
decl_stmt|;
specifier|const
name|char
modifier|*
name|s1
init|=
operator|(
operator|*
name|c1
operator|)
operator|->
name|name
decl_stmt|,
modifier|*
name|s2
init|=
operator|(
operator|*
name|c2
operator|)
operator|->
name|name
decl_stmt|;
name|int
name|l1
init|=
operator|(
operator|*
name|c1
operator|)
operator|->
name|len
decl_stmt|;
name|int
name|l2
init|=
operator|(
operator|*
name|c2
operator|)
operator|->
name|len
decl_stmt|;
return|return
name|l1
operator|!=
name|l2
condition|?
name|l1
operator|-
name|l2
else|:
name|strcmp
argument_list|(
name|s1
argument_list|,
name|s2
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|add_cmd_list
specifier|static
name|void
name|add_cmd_list
parameter_list|(
name|struct
name|cmdnames
modifier|*
name|cmds
parameter_list|,
name|struct
name|cmdnames
modifier|*
name|old
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|ALLOC_GROW
argument_list|(
name|cmds
operator|->
name|names
argument_list|,
name|cmds
operator|->
name|cnt
operator|+
name|old
operator|->
name|cnt
argument_list|,
name|cmds
operator|->
name|alloc
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
name|old
operator|->
name|cnt
condition|;
name|i
operator|++
control|)
name|cmds
operator|->
name|names
index|[
name|cmds
operator|->
name|cnt
operator|++
index|]
operator|=
name|old
operator|->
name|names
index|[
name|i
index|]
expr_stmt|;
name|free
argument_list|(
name|old
operator|->
name|names
argument_list|)
expr_stmt|;
name|old
operator|->
name|cnt
operator|=
literal|0
expr_stmt|;
name|old
operator|->
name|names
operator|=
name|NULL
expr_stmt|;
block|}
end_function
begin_comment
comment|/* An empirically derived magic number */
end_comment
begin_define
DECL|macro|SIMILARITY_FLOOR
define|#
directive|define
name|SIMILARITY_FLOOR
value|7
end_define
begin_define
DECL|macro|SIMILAR_ENOUGH
define|#
directive|define
name|SIMILAR_ENOUGH
parameter_list|(
name|x
parameter_list|)
value|((x)< SIMILARITY_FLOOR)
end_define
begin_decl_stmt
DECL|variable|bad_interpreter_advice
specifier|static
specifier|const
name|char
name|bad_interpreter_advice
index|[]
init|=
name|N_
argument_list|(
literal|"'%s' appears to be a git command, but we were not\n"
literal|"able to execute it. Maybe git-%s is broken?"
argument_list|)
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|help_unknown_cmd
specifier|const
name|char
modifier|*
name|help_unknown_cmd
parameter_list|(
specifier|const
name|char
modifier|*
name|cmd
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|n
decl_stmt|,
name|best_similarity
init|=
literal|0
decl_stmt|;
name|struct
name|cmdnames
name|main_cmds
decl_stmt|,
name|other_cmds
decl_stmt|;
name|memset
argument_list|(
operator|&
name|main_cmds
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|main_cmds
argument_list|)
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|other_cmds
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|other_cmds
argument_list|)
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|aliases
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|aliases
argument_list|)
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|git_unknown_cmd_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
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
name|add_cmd_list
argument_list|(
operator|&
name|main_cmds
argument_list|,
operator|&
name|aliases
argument_list|)
expr_stmt|;
name|add_cmd_list
argument_list|(
operator|&
name|main_cmds
argument_list|,
operator|&
name|other_cmds
argument_list|)
expr_stmt|;
name|qsort
argument_list|(
name|main_cmds
operator|.
name|names
argument_list|,
name|main_cmds
operator|.
name|cnt
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|main_cmds
operator|.
name|names
argument_list|)
argument_list|,
name|cmdname_compare
argument_list|)
expr_stmt|;
name|uniq
argument_list|(
operator|&
name|main_cmds
argument_list|)
expr_stmt|;
comment|/* This abuses cmdname->len for levenshtein distance */
for|for
control|(
name|i
operator|=
literal|0
operator|,
name|n
operator|=
literal|0
init|;
name|i
operator|<
name|main_cmds
operator|.
name|cnt
condition|;
name|i
operator|++
control|)
block|{
name|int
name|cmp
init|=
literal|0
decl_stmt|;
comment|/* avoid compiler stupidity */
specifier|const
name|char
modifier|*
name|candidate
init|=
name|main_cmds
operator|.
name|names
index|[
name|i
index|]
operator|->
name|name
decl_stmt|;
comment|/* 		 * An exact match means we have the command, but 		 * for some reason exec'ing it gave us ENOENT; probably 		 * it's a bad interpreter in the #! line. 		 */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|candidate
argument_list|,
name|cmd
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
name|bad_interpreter_advice
argument_list|)
argument_list|,
name|cmd
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
comment|/* Does the candidate appear in common_cmds list? */
while|while
condition|(
name|n
operator|<
name|ARRAY_SIZE
argument_list|(
name|common_cmds
argument_list|)
operator|&&
operator|(
name|cmp
operator|=
name|strcmp
argument_list|(
name|common_cmds
index|[
name|n
index|]
operator|.
name|name
argument_list|,
name|candidate
argument_list|)
operator|)
operator|<
literal|0
condition|)
name|n
operator|++
expr_stmt|;
if|if
condition|(
operator|(
name|n
operator|<
name|ARRAY_SIZE
argument_list|(
name|common_cmds
argument_list|)
operator|)
operator|&&
operator|!
name|cmp
condition|)
block|{
comment|/* Yes, this is one of the common commands */
name|n
operator|++
expr_stmt|;
comment|/* use the entry from common_cmds[] */
if|if
condition|(
name|starts_with
argument_list|(
name|candidate
argument_list|,
name|cmd
argument_list|)
condition|)
block|{
comment|/* Give prefix match a very good score */
name|main_cmds
operator|.
name|names
index|[
name|i
index|]
operator|->
name|len
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
block|}
name|main_cmds
operator|.
name|names
index|[
name|i
index|]
operator|->
name|len
operator|=
name|levenshtein
argument_list|(
name|cmd
argument_list|,
name|candidate
argument_list|,
literal|0
argument_list|,
literal|2
argument_list|,
literal|1
argument_list|,
literal|3
argument_list|)
operator|+
literal|1
expr_stmt|;
block|}
name|qsort
argument_list|(
name|main_cmds
operator|.
name|names
argument_list|,
name|main_cmds
operator|.
name|cnt
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|main_cmds
operator|.
name|names
argument_list|)
argument_list|,
name|levenshtein_compare
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|main_cmds
operator|.
name|cnt
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Uh oh. Your system reports no Git commands at all."
argument_list|)
argument_list|)
expr_stmt|;
comment|/* skip and count prefix matches */
for|for
control|(
name|n
operator|=
literal|0
init|;
name|n
operator|<
name|main_cmds
operator|.
name|cnt
operator|&&
operator|!
name|main_cmds
operator|.
name|names
index|[
name|n
index|]
operator|->
name|len
condition|;
name|n
operator|++
control|)
empty_stmt|;
comment|/* still counting */
if|if
condition|(
name|main_cmds
operator|.
name|cnt
operator|<=
name|n
condition|)
block|{
comment|/* prefix matches with everything? that is too ambiguous */
name|best_similarity
operator|=
name|SIMILARITY_FLOOR
operator|+
literal|1
expr_stmt|;
block|}
else|else
block|{
comment|/* count all the most similar ones */
for|for
control|(
name|best_similarity
operator|=
name|main_cmds
operator|.
name|names
index|[
name|n
operator|++
index|]
operator|->
name|len
init|;
operator|(
name|n
operator|<
name|main_cmds
operator|.
name|cnt
operator|&&
name|best_similarity
operator|==
name|main_cmds
operator|.
name|names
index|[
name|n
index|]
operator|->
name|len
operator|)
condition|;
name|n
operator|++
control|)
empty_stmt|;
comment|/* still counting */
block|}
if|if
condition|(
name|autocorrect
operator|&&
name|n
operator|==
literal|1
operator|&&
name|SIMILAR_ENOUGH
argument_list|(
name|best_similarity
argument_list|)
condition|)
block|{
specifier|const
name|char
modifier|*
name|assumed
init|=
name|main_cmds
operator|.
name|names
index|[
literal|0
index|]
operator|->
name|name
decl_stmt|;
name|main_cmds
operator|.
name|names
index|[
literal|0
index|]
operator|=
name|NULL
expr_stmt|;
name|clean_cmdnames
argument_list|(
operator|&
name|main_cmds
argument_list|)
expr_stmt|;
name|fprintf_ln
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"WARNING: You called a Git command named '%s', "
literal|"which does not exist.\n"
literal|"Continuing under the assumption that you meant '%s'"
argument_list|)
argument_list|,
name|cmd
argument_list|,
name|assumed
argument_list|)
expr_stmt|;
if|if
condition|(
name|autocorrect
operator|>
literal|0
condition|)
block|{
name|fprintf_ln
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"in %0.1f seconds automatically..."
argument_list|)
argument_list|,
operator|(
name|float
operator|)
name|autocorrect
operator|/
literal|10.0
argument_list|)
expr_stmt|;
name|sleep_millisec
argument_list|(
name|autocorrect
operator|*
literal|100
argument_list|)
expr_stmt|;
block|}
return|return
name|assumed
return|;
block|}
name|fprintf_ln
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"git: '%s' is not a git command. See 'git --help'."
argument_list|)
argument_list|,
name|cmd
argument_list|)
expr_stmt|;
if|if
condition|(
name|SIMILAR_ENOUGH
argument_list|(
name|best_similarity
argument_list|)
condition|)
block|{
name|fprintf_ln
argument_list|(
name|stderr
argument_list|,
name|Q_
argument_list|(
literal|"\nDid you mean this?"
argument_list|,
literal|"\nDid you mean one of these?"
argument_list|,
name|n
argument_list|)
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
name|n
condition|;
name|i
operator|++
control|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"\t%s\n"
argument_list|,
name|main_cmds
operator|.
name|names
index|[
name|i
index|]
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_version
name|int
name|cmd_version
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
comment|/* 	 * The format of this string should be kept stable for compatibility 	 * with external projects that rely on the output of "git version". 	 */
name|printf
argument_list|(
literal|"git version %s\n"
argument_list|,
name|git_version_string
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_struct
DECL|struct|similar_ref_cb
struct|struct
name|similar_ref_cb
block|{
DECL|member|base_ref
specifier|const
name|char
modifier|*
name|base_ref
decl_stmt|;
DECL|member|similar_refs
name|struct
name|string_list
modifier|*
name|similar_refs
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|append_similar_ref
specifier|static
name|int
name|append_similar_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|struct
name|object_id
modifier|*
name|oid
parameter_list|,
name|int
name|flags
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|struct
name|similar_ref_cb
modifier|*
name|cb
init|=
operator|(
expr|struct
name|similar_ref_cb
operator|*
operator|)
operator|(
name|cb_data
operator|)
decl_stmt|;
name|char
modifier|*
name|branch
init|=
name|strrchr
argument_list|(
name|refname
argument_list|,
literal|'/'
argument_list|)
operator|+
literal|1
decl_stmt|;
specifier|const
name|char
modifier|*
name|remote
decl_stmt|;
comment|/* A remote branch of the same name is deemed similar */
if|if
condition|(
name|skip_prefix
argument_list|(
name|refname
argument_list|,
literal|"refs/remotes/"
argument_list|,
operator|&
name|remote
argument_list|)
operator|&&
operator|!
name|strcmp
argument_list|(
name|branch
argument_list|,
name|cb
operator|->
name|base_ref
argument_list|)
condition|)
name|string_list_append
argument_list|(
name|cb
operator|->
name|similar_refs
argument_list|,
name|remote
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|guess_refs
specifier|static
name|struct
name|string_list
name|guess_refs
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|)
block|{
name|struct
name|similar_ref_cb
name|ref_cb
decl_stmt|;
name|struct
name|string_list
name|similar_refs
init|=
name|STRING_LIST_INIT_NODUP
decl_stmt|;
name|ref_cb
operator|.
name|base_ref
operator|=
name|ref
expr_stmt|;
name|ref_cb
operator|.
name|similar_refs
operator|=
operator|&
name|similar_refs
expr_stmt|;
name|for_each_ref
argument_list|(
name|append_similar_ref
argument_list|,
operator|&
name|ref_cb
argument_list|)
expr_stmt|;
return|return
name|similar_refs
return|;
block|}
end_function
begin_function
DECL|function|help_unknown_ref
name|void
name|help_unknown_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
specifier|const
name|char
modifier|*
name|cmd
parameter_list|,
specifier|const
name|char
modifier|*
name|error
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|string_list
name|suggested_refs
init|=
name|guess_refs
argument_list|(
name|ref
argument_list|)
decl_stmt|;
name|fprintf_ln
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"%s: %s - %s"
argument_list|)
argument_list|,
name|cmd
argument_list|,
name|ref
argument_list|,
name|error
argument_list|)
expr_stmt|;
if|if
condition|(
name|suggested_refs
operator|.
name|nr
operator|>
literal|0
condition|)
block|{
name|fprintf_ln
argument_list|(
name|stderr
argument_list|,
name|Q_
argument_list|(
literal|"\nDid you mean this?"
argument_list|,
literal|"\nDid you mean one of these?"
argument_list|,
name|suggested_refs
operator|.
name|nr
argument_list|)
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
name|suggested_refs
operator|.
name|nr
condition|;
name|i
operator|++
control|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"\t%s\n"
argument_list|,
name|suggested_refs
operator|.
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
block|}
name|string_list_clear
argument_list|(
operator|&
name|suggested_refs
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
