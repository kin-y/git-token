begin_unit
begin_comment
comment|/*  * "git mv" builtin command  *  * Copyright (C) 2006 Johannes Schindelin  */
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
file|"dir.h"
end_include
begin_include
include|#
directive|include
file|"cache-tree.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|builtin_mv_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|builtin_mv_usage
index|[]
init|=
block|{
literal|"git mv [options]<source>...<destination>"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|copy_pathspec
specifier|static
specifier|const
name|char
modifier|*
modifier|*
name|copy_pathspec
parameter_list|(
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|,
name|int
name|count
parameter_list|,
name|int
name|base_name
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|result
init|=
name|xmalloc
argument_list|(
operator|(
name|count
operator|+
literal|1
operator|)
operator|*
sizeof|sizeof
argument_list|(
specifier|const
name|char
operator|*
argument_list|)
argument_list|)
decl_stmt|;
name|memcpy
argument_list|(
name|result
argument_list|,
name|pathspec
argument_list|,
name|count
operator|*
sizeof|sizeof
argument_list|(
specifier|const
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|result
index|[
name|count
index|]
operator|=
name|NULL
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|count
condition|;
name|i
operator|++
control|)
block|{
name|int
name|length
init|=
name|strlen
argument_list|(
name|result
index|[
name|i
index|]
argument_list|)
decl_stmt|;
name|int
name|to_copy
init|=
name|length
decl_stmt|;
while|while
condition|(
name|to_copy
operator|>
literal|0
operator|&&
name|is_dir_sep
argument_list|(
name|result
index|[
name|i
index|]
index|[
name|to_copy
operator|-
literal|1
index|]
argument_list|)
condition|)
name|to_copy
operator|--
expr_stmt|;
if|if
condition|(
name|to_copy
operator|!=
name|length
operator|||
name|base_name
condition|)
block|{
name|char
modifier|*
name|it
init|=
name|xmemdupz
argument_list|(
name|result
index|[
name|i
index|]
argument_list|,
name|to_copy
argument_list|)
decl_stmt|;
name|result
index|[
name|i
index|]
operator|=
name|base_name
condition|?
name|strdup
argument_list|(
name|basename
argument_list|(
name|it
argument_list|)
argument_list|)
else|:
name|it
expr_stmt|;
block|}
block|}
return|return
name|get_pathspec
argument_list|(
name|prefix
argument_list|,
name|result
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|add_slash
specifier|static
specifier|const
name|char
modifier|*
name|add_slash
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|path
argument_list|)
decl_stmt|;
if|if
condition|(
name|path
index|[
name|len
operator|-
literal|1
index|]
operator|!=
literal|'/'
condition|)
block|{
name|char
modifier|*
name|with_slash
init|=
name|xmalloc
argument_list|(
name|len
operator|+
literal|2
argument_list|)
decl_stmt|;
name|memcpy
argument_list|(
name|with_slash
argument_list|,
name|path
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|with_slash
index|[
name|len
operator|++
index|]
operator|=
literal|'/'
expr_stmt|;
name|with_slash
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|with_slash
return|;
block|}
return|return
name|path
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|lock_file
specifier|static
name|struct
name|lock_file
name|lock_file
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_mv
name|int
name|cmd_mv
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
name|i
decl_stmt|,
name|newfd
decl_stmt|;
name|int
name|verbose
init|=
literal|0
decl_stmt|,
name|show_only
init|=
literal|0
decl_stmt|,
name|force
init|=
literal|0
decl_stmt|,
name|ignore_errors
init|=
literal|0
decl_stmt|;
name|struct
name|option
name|builtin_mv_options
index|[]
init|=
block|{
name|OPT__VERBOSE
argument_list|(
operator|&
name|verbose
argument_list|,
literal|"be verbose"
argument_list|)
block|,
name|OPT__DRY_RUN
argument_list|(
operator|&
name|show_only
argument_list|,
literal|"dry run"
argument_list|)
block|,
name|OPT__FORCE
argument_list|(
operator|&
name|force
argument_list|,
literal|"force move/rename even if target exists"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'k'
argument_list|,
name|NULL
argument_list|,
operator|&
name|ignore_errors
argument_list|,
literal|"skip move/rename errors"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|source
decl_stmt|,
modifier|*
modifier|*
name|destination
decl_stmt|,
modifier|*
modifier|*
name|dest_path
decl_stmt|;
enum|enum
name|update_mode
block|{
name|BOTH
init|=
literal|0
block|,
name|WORKING_DIRECTORY
block|,
name|INDEX
block|}
modifier|*
name|modes
enum|;
name|struct
name|stat
name|st
decl_stmt|;
name|struct
name|string_list
name|src_for_dst
init|=
name|STRING_LIST_INIT_NODUP
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
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
name|builtin_mv_options
argument_list|,
name|builtin_mv_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|--
name|argc
operator|<
literal|1
condition|)
name|usage_with_options
argument_list|(
name|builtin_mv_usage
argument_list|,
name|builtin_mv_options
argument_list|)
expr_stmt|;
name|newfd
operator|=
name|hold_locked_index
argument_list|(
operator|&
name|lock_file
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_cache
argument_list|()
operator|<
literal|0
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"index file corrupt"
argument_list|)
argument_list|)
expr_stmt|;
name|source
operator|=
name|copy_pathspec
argument_list|(
name|prefix
argument_list|,
name|argv
argument_list|,
name|argc
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|modes
operator|=
name|xcalloc
argument_list|(
name|argc
argument_list|,
sizeof|sizeof
argument_list|(
expr|enum
name|update_mode
argument_list|)
argument_list|)
expr_stmt|;
name|dest_path
operator|=
name|copy_pathspec
argument_list|(
name|prefix
argument_list|,
name|argv
operator|+
name|argc
argument_list|,
literal|1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|dest_path
index|[
literal|0
index|]
index|[
literal|0
index|]
operator|==
literal|'\0'
condition|)
comment|/* special case: "." was normalized to "" */
name|destination
operator|=
name|copy_pathspec
argument_list|(
name|dest_path
index|[
literal|0
index|]
argument_list|,
name|argv
argument_list|,
name|argc
argument_list|,
literal|1
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|lstat
argument_list|(
name|dest_path
index|[
literal|0
index|]
argument_list|,
operator|&
name|st
argument_list|)
operator|&&
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
name|dest_path
index|[
literal|0
index|]
operator|=
name|add_slash
argument_list|(
name|dest_path
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|destination
operator|=
name|copy_pathspec
argument_list|(
name|dest_path
index|[
literal|0
index|]
argument_list|,
name|argv
argument_list|,
name|argc
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|argc
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"destination '%s' is not a directory"
argument_list|,
name|dest_path
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|destination
operator|=
name|dest_path
expr_stmt|;
block|}
comment|/* Checking */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|src
init|=
name|source
index|[
name|i
index|]
decl_stmt|,
modifier|*
name|dst
init|=
name|destination
index|[
name|i
index|]
decl_stmt|;
name|int
name|length
decl_stmt|,
name|src_is_dir
decl_stmt|;
specifier|const
name|char
modifier|*
name|bad
init|=
name|NULL
decl_stmt|;
if|if
condition|(
name|show_only
condition|)
name|printf
argument_list|(
name|_
argument_list|(
literal|"Checking rename of '%s' to '%s'\n"
argument_list|)
argument_list|,
name|src
argument_list|,
name|dst
argument_list|)
expr_stmt|;
name|length
operator|=
name|strlen
argument_list|(
name|src
argument_list|)
expr_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|src
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
name|bad
operator|=
name|_
argument_list|(
literal|"bad source"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|src
argument_list|,
name|dst
argument_list|,
name|length
argument_list|)
operator|&&
operator|(
name|dst
index|[
name|length
index|]
operator|==
literal|0
operator|||
name|dst
index|[
name|length
index|]
operator|==
literal|'/'
operator|)
condition|)
block|{
name|bad
operator|=
name|_
argument_list|(
literal|"can not move directory into itself"
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|(
name|src_is_dir
operator|=
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
operator|)
operator|&&
name|lstat
argument_list|(
name|dst
argument_list|,
operator|&
name|st
argument_list|)
operator|==
literal|0
condition|)
name|bad
operator|=
name|_
argument_list|(
literal|"cannot move directory over file"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|src_is_dir
condition|)
block|{
specifier|const
name|char
modifier|*
name|src_w_slash
init|=
name|add_slash
argument_list|(
name|src
argument_list|)
decl_stmt|;
name|int
name|len_w_slash
init|=
name|length
operator|+
literal|1
decl_stmt|;
name|int
name|first
decl_stmt|,
name|last
decl_stmt|;
name|modes
index|[
name|i
index|]
operator|=
name|WORKING_DIRECTORY
expr_stmt|;
name|first
operator|=
name|cache_name_pos
argument_list|(
name|src_w_slash
argument_list|,
name|len_w_slash
argument_list|)
expr_stmt|;
if|if
condition|(
name|first
operator|>=
literal|0
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Huh? %.*s is in index?"
argument_list|)
argument_list|,
name|len_w_slash
argument_list|,
name|src_w_slash
argument_list|)
expr_stmt|;
name|first
operator|=
operator|-
literal|1
operator|-
name|first
expr_stmt|;
for|for
control|(
name|last
operator|=
name|first
init|;
name|last
operator|<
name|active_nr
condition|;
name|last
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|path
init|=
name|active_cache
index|[
name|last
index|]
operator|->
name|name
decl_stmt|;
if|if
condition|(
name|strncmp
argument_list|(
name|path
argument_list|,
name|src_w_slash
argument_list|,
name|len_w_slash
argument_list|)
condition|)
break|break;
block|}
name|free
argument_list|(
operator|(
name|char
operator|*
operator|)
name|src_w_slash
argument_list|)
expr_stmt|;
if|if
condition|(
name|last
operator|-
name|first
operator|<
literal|1
condition|)
name|bad
operator|=
name|_
argument_list|(
literal|"source directory is empty"
argument_list|)
expr_stmt|;
else|else
block|{
name|int
name|j
decl_stmt|,
name|dst_len
decl_stmt|;
if|if
condition|(
name|last
operator|-
name|first
operator|>
literal|0
condition|)
block|{
name|source
operator|=
name|xrealloc
argument_list|(
name|source
argument_list|,
operator|(
name|argc
operator|+
name|last
operator|-
name|first
operator|)
operator|*
sizeof|sizeof
argument_list|(
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|destination
operator|=
name|xrealloc
argument_list|(
name|destination
argument_list|,
operator|(
name|argc
operator|+
name|last
operator|-
name|first
operator|)
operator|*
sizeof|sizeof
argument_list|(
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|modes
operator|=
name|xrealloc
argument_list|(
name|modes
argument_list|,
operator|(
name|argc
operator|+
name|last
operator|-
name|first
operator|)
operator|*
sizeof|sizeof
argument_list|(
expr|enum
name|update_mode
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|dst
operator|=
name|add_slash
argument_list|(
name|dst
argument_list|)
expr_stmt|;
name|dst_len
operator|=
name|strlen
argument_list|(
name|dst
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
name|last
operator|-
name|first
condition|;
name|j
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|path
init|=
name|active_cache
index|[
name|first
operator|+
name|j
index|]
operator|->
name|name
decl_stmt|;
name|source
index|[
name|argc
operator|+
name|j
index|]
operator|=
name|path
expr_stmt|;
name|destination
index|[
name|argc
operator|+
name|j
index|]
operator|=
name|prefix_path
argument_list|(
name|dst
argument_list|,
name|dst_len
argument_list|,
name|path
operator|+
name|length
operator|+
literal|1
argument_list|)
expr_stmt|;
name|modes
index|[
name|argc
operator|+
name|j
index|]
operator|=
name|INDEX
expr_stmt|;
block|}
name|argc
operator|+=
name|last
operator|-
name|first
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|cache_name_pos
argument_list|(
name|src
argument_list|,
name|length
argument_list|)
operator|<
literal|0
condition|)
name|bad
operator|=
name|_
argument_list|(
literal|"not under version control"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|lstat
argument_list|(
name|dst
argument_list|,
operator|&
name|st
argument_list|)
operator|==
literal|0
condition|)
block|{
name|bad
operator|=
name|_
argument_list|(
literal|"destination exists"
argument_list|)
expr_stmt|;
if|if
condition|(
name|force
condition|)
block|{
comment|/* 				 * only files can overwrite each other: 				 * check both source and destination 				 */
if|if
condition|(
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
operator|||
name|S_ISLNK
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
name|warning
argument_list|(
name|_
argument_list|(
literal|"%s; will overwrite!"
argument_list|)
argument_list|,
name|bad
argument_list|)
expr_stmt|;
name|bad
operator|=
name|NULL
expr_stmt|;
block|}
else|else
name|bad
operator|=
name|_
argument_list|(
literal|"Cannot overwrite"
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|string_list_has_string
argument_list|(
operator|&
name|src_for_dst
argument_list|,
name|dst
argument_list|)
condition|)
name|bad
operator|=
name|_
argument_list|(
literal|"multiple sources for the same target"
argument_list|)
expr_stmt|;
else|else
name|string_list_insert
argument_list|(
operator|&
name|src_for_dst
argument_list|,
name|dst
argument_list|)
expr_stmt|;
if|if
condition|(
name|bad
condition|)
block|{
if|if
condition|(
name|ignore_errors
condition|)
block|{
if|if
condition|(
operator|--
name|argc
operator|>
literal|0
condition|)
block|{
name|memmove
argument_list|(
name|source
operator|+
name|i
argument_list|,
name|source
operator|+
name|i
operator|+
literal|1
argument_list|,
operator|(
name|argc
operator|-
name|i
operator|)
operator|*
sizeof|sizeof
argument_list|(
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|memmove
argument_list|(
name|destination
operator|+
name|i
argument_list|,
name|destination
operator|+
name|i
operator|+
literal|1
argument_list|,
operator|(
name|argc
operator|-
name|i
operator|)
operator|*
sizeof|sizeof
argument_list|(
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|i
operator|--
expr_stmt|;
block|}
block|}
else|else
name|die
argument_list|(
name|_
argument_list|(
literal|"%s, source=%s, destination=%s"
argument_list|)
argument_list|,
name|bad
argument_list|,
name|src
argument_list|,
name|dst
argument_list|)
expr_stmt|;
block|}
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|src
init|=
name|source
index|[
name|i
index|]
decl_stmt|,
modifier|*
name|dst
init|=
name|destination
index|[
name|i
index|]
decl_stmt|;
name|enum
name|update_mode
name|mode
init|=
name|modes
index|[
name|i
index|]
decl_stmt|;
name|int
name|pos
decl_stmt|;
if|if
condition|(
name|show_only
operator|||
name|verbose
condition|)
name|printf
argument_list|(
name|_
argument_list|(
literal|"Renaming %s to %s\n"
argument_list|)
argument_list|,
name|src
argument_list|,
name|dst
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|show_only
operator|&&
name|mode
operator|!=
name|INDEX
operator|&&
name|rename
argument_list|(
name|src
argument_list|,
name|dst
argument_list|)
operator|<
literal|0
operator|&&
operator|!
name|ignore_errors
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"renaming '%s' failed"
argument_list|)
argument_list|,
name|src
argument_list|)
expr_stmt|;
if|if
condition|(
name|mode
operator|==
name|WORKING_DIRECTORY
condition|)
continue|continue;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|src
argument_list|,
name|strlen
argument_list|(
name|src
argument_list|)
argument_list|)
expr_stmt|;
name|assert
argument_list|(
name|pos
operator|>=
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|show_only
condition|)
name|rename_cache_entry_at
argument_list|(
name|pos
argument_list|,
name|dst
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|active_cache_changed
condition|)
block|{
if|if
condition|(
name|write_cache
argument_list|(
name|newfd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
operator|||
name|commit_locked_index
argument_list|(
operator|&
name|lock_file
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Unable to write new index file"
argument_list|)
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
