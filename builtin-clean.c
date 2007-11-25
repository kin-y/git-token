begin_unit
begin_comment
comment|/*  * "git clean" builtin command  *  * Copyright (C) 2007 Shawn Bohrer  *  * Based on git-clean.sh by Pavel Roskin  */
end_comment
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|force
specifier|static
name|int
name|force
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_comment
DECL|variable|force
comment|/* unset */
end_comment
begin_decl_stmt
DECL|variable|builtin_clean_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|builtin_clean_usage
index|[]
init|=
block|{
literal|"git-clean [-d] [-f] [-n] [-q] [-x | -X] [--]<paths>..."
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|git_clean_config
specifier|static
name|int
name|git_clean_config
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
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"clean.requireforce"
argument_list|)
condition|)
name|force
operator|=
operator|!
name|git_config_bool
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
name|git_default_config
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|cmd_clean
name|int
name|cmd_clean
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
name|j
decl_stmt|;
name|int
name|show_only
init|=
literal|0
decl_stmt|,
name|remove_directories
init|=
literal|0
decl_stmt|,
name|quiet
init|=
literal|0
decl_stmt|,
name|ignored
init|=
literal|0
decl_stmt|;
name|int
name|ignored_only
init|=
literal|0
decl_stmt|,
name|baselen
init|=
literal|0
decl_stmt|,
name|config_set
init|=
literal|0
decl_stmt|;
name|struct
name|strbuf
name|directory
decl_stmt|;
name|struct
name|dir_struct
name|dir
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
decl_stmt|,
modifier|*
name|base
decl_stmt|;
specifier|static
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT__QUIET
argument_list|(
operator|&
name|quiet
argument_list|)
block|,
name|OPT__DRY_RUN
argument_list|(
operator|&
name|show_only
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'f'
argument_list|,
name|NULL
argument_list|,
operator|&
name|force
argument_list|,
literal|"force"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'d'
argument_list|,
name|NULL
argument_list|,
operator|&
name|remove_directories
argument_list|,
literal|"remove whole directories"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'x'
argument_list|,
name|NULL
argument_list|,
operator|&
name|ignored
argument_list|,
literal|"remove ignored files, too"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'X'
argument_list|,
name|NULL
argument_list|,
operator|&
name|ignored_only
argument_list|,
literal|"remove only ignored files"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|git_config
argument_list|(
name|git_clean_config
argument_list|)
expr_stmt|;
if|if
condition|(
name|force
operator|<
literal|0
condition|)
name|force
operator|=
literal|0
expr_stmt|;
else|else
name|config_set
operator|=
literal|1
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|options
argument_list|,
name|builtin_clean_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|dir
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|dir
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|ignored_only
condition|)
name|dir
operator|.
name|show_ignored
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|ignored
operator|&&
name|ignored_only
condition|)
name|die
argument_list|(
literal|"-x and -X cannot be used together"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|show_only
operator|&&
operator|!
name|force
condition|)
name|die
argument_list|(
literal|"clean.requireForce%s set and -n or -f not given; "
literal|"refusing to clean"
argument_list|,
name|config_set
condition|?
literal|""
else|:
literal|" not"
argument_list|)
expr_stmt|;
name|dir
operator|.
name|show_other_directories
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|ignored
condition|)
name|setup_standard_excludes
argument_list|(
operator|&
name|dir
argument_list|)
expr_stmt|;
name|pathspec
operator|=
name|get_pathspec
argument_list|(
name|prefix
argument_list|,
name|argv
argument_list|)
expr_stmt|;
name|read_cache
argument_list|()
expr_stmt|;
comment|/* 	 * Calculate common prefix for the pathspec, and 	 * use that to optimize the directory walk 	 */
name|baselen
operator|=
name|common_prefix
argument_list|(
name|pathspec
argument_list|)
expr_stmt|;
name|path
operator|=
literal|"."
expr_stmt|;
name|base
operator|=
literal|""
expr_stmt|;
if|if
condition|(
name|baselen
condition|)
name|path
operator|=
name|base
operator|=
name|xmemdupz
argument_list|(
operator|*
name|pathspec
argument_list|,
name|baselen
argument_list|)
expr_stmt|;
name|read_directory
argument_list|(
operator|&
name|dir
argument_list|,
name|path
argument_list|,
name|base
argument_list|,
name|baselen
argument_list|,
name|pathspec
argument_list|)
expr_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|directory
argument_list|,
literal|0
argument_list|)
expr_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<
name|dir
operator|.
name|nr
condition|;
operator|++
name|j
control|)
block|{
name|struct
name|dir_entry
modifier|*
name|ent
init|=
name|dir
operator|.
name|entries
index|[
name|j
index|]
decl_stmt|;
name|int
name|len
decl_stmt|,
name|pos
decl_stmt|,
name|specs
decl_stmt|;
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|char
modifier|*
name|seen
decl_stmt|;
comment|/* 		 * Remove the '/' at the end that directory 		 * walking adds for directory entries. 		 */
name|len
operator|=
name|ent
operator|->
name|len
expr_stmt|;
if|if
condition|(
name|len
operator|&&
name|ent
operator|->
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
name|len
operator|--
expr_stmt|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|ent
operator|->
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|pos
condition|)
continue|continue;
comment|/* exact match */
name|pos
operator|=
operator|-
name|pos
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|pos
operator|<
name|active_nr
condition|)
block|{
name|ce
operator|=
name|active_cache
index|[
name|pos
index|]
expr_stmt|;
if|if
condition|(
name|ce_namelen
argument_list|(
name|ce
argument_list|)
operator|==
name|len
operator|&&
operator|!
name|memcmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|ent
operator|->
name|name
argument_list|,
name|len
argument_list|)
condition|)
continue|continue;
comment|/* Yup, this one exists unmerged */
block|}
if|if
condition|(
operator|!
name|lstat
argument_list|(
name|ent
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
operator|&&
operator|(
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
operator|)
condition|)
block|{
name|int
name|matched_path
init|=
literal|0
decl_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|directory
argument_list|,
name|ent
operator|->
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|pathspec
condition|)
block|{
for|for
control|(
name|specs
operator|=
literal|0
init|;
name|pathspec
index|[
name|specs
index|]
condition|;
operator|++
name|specs
control|)
comment|/* nothing */
empty_stmt|;
name|seen
operator|=
name|xcalloc
argument_list|(
name|specs
argument_list|,
literal|1
argument_list|)
expr_stmt|;
comment|/* Check if directory was explictly passed as 				 * pathspec.  If so we want to remove it */
if|if
condition|(
name|match_pathspec
argument_list|(
name|pathspec
argument_list|,
name|ent
operator|->
name|name
argument_list|,
name|ent
operator|->
name|len
argument_list|,
name|baselen
argument_list|,
name|seen
argument_list|)
condition|)
name|matched_path
operator|=
literal|1
expr_stmt|;
name|free
argument_list|(
name|seen
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|show_only
operator|&&
operator|(
name|remove_directories
operator|||
name|matched_path
operator|)
condition|)
block|{
name|printf
argument_list|(
literal|"Would remove %s\n"
argument_list|,
name|directory
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|quiet
operator|&&
operator|(
name|remove_directories
operator|||
name|matched_path
operator|)
condition|)
block|{
name|remove_dir_recursively
argument_list|(
operator|&
name|directory
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|remove_directories
operator|||
name|matched_path
condition|)
block|{
name|printf
argument_list|(
literal|"Removing %s\n"
argument_list|,
name|directory
operator|.
name|buf
argument_list|)
expr_stmt|;
name|remove_dir_recursively
argument_list|(
operator|&
name|directory
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|show_only
condition|)
block|{
name|printf
argument_list|(
literal|"Would not remove %s\n"
argument_list|,
name|directory
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|printf
argument_list|(
literal|"Not removing %s\n"
argument_list|,
name|directory
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
name|strbuf_reset
argument_list|(
operator|&
name|directory
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|show_only
condition|)
block|{
name|printf
argument_list|(
literal|"Would remove %s\n"
argument_list|,
name|ent
operator|->
name|name
argument_list|)
expr_stmt|;
continue|continue;
block|}
elseif|else
if|if
condition|(
operator|!
name|quiet
condition|)
block|{
name|printf
argument_list|(
literal|"Removing %s\n"
argument_list|,
name|ent
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
name|unlink
argument_list|(
name|ent
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
block|}
name|strbuf_release
argument_list|(
operator|&
name|directory
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
