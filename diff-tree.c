begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_decl_stmt
DECL|variable|show_root_diff
specifier|static
name|int
name|show_root_diff
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|no_commit_id
specifier|static
name|int
name|no_commit_id
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|verbose_header
specifier|static
name|int
name|verbose_header
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|ignore_merges
specifier|static
name|int
name|ignore_merges
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|read_stdin
specifier|static
name|int
name|read_stdin
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|header
specifier|static
specifier|const
name|char
modifier|*
name|header
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|header_prefix
specifier|static
specifier|const
name|char
modifier|*
name|header_prefix
init|=
literal|""
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|commit_format
specifier|static
name|enum
name|cmit_fmt
name|commit_format
init|=
name|CMIT_FMT_RAW
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|diff_options
specifier|static
name|struct
name|diff_options
name|diff_options
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|call_diff_setup_done
specifier|static
name|void
name|call_diff_setup_done
parameter_list|(
name|void
parameter_list|)
block|{
name|diff_setup_done
argument_list|(
operator|&
name|diff_options
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|call_diff_flush
specifier|static
name|int
name|call_diff_flush
parameter_list|(
name|void
parameter_list|)
block|{
name|diffcore_std
argument_list|(
operator|&
name|diff_options
argument_list|)
expr_stmt|;
if|if
condition|(
name|diff_queue_is_empty
argument_list|()
condition|)
block|{
name|int
name|saved_fmt
init|=
name|diff_options
operator|.
name|output_format
decl_stmt|;
name|diff_options
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_NO_OUTPUT
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|diff_options
argument_list|)
expr_stmt|;
name|diff_options
operator|.
name|output_format
operator|=
name|saved_fmt
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|header
condition|)
block|{
if|if
condition|(
operator|!
name|no_commit_id
condition|)
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|header
argument_list|,
name|diff_options
operator|.
name|line_termination
argument_list|)
expr_stmt|;
name|header
operator|=
name|NULL
expr_stmt|;
block|}
name|diff_flush
argument_list|(
operator|&
name|diff_options
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|diff_tree_sha1_top
specifier|static
name|int
name|diff_tree_sha1_top
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|old
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|new
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
name|int
name|ret
decl_stmt|;
name|call_diff_setup_done
argument_list|()
expr_stmt|;
name|ret
operator|=
name|diff_tree_sha1
argument_list|(
name|old
argument_list|,
name|new
argument_list|,
name|base
argument_list|,
operator|&
name|diff_options
argument_list|)
expr_stmt|;
name|call_diff_flush
argument_list|()
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|diff_root_tree
specifier|static
name|int
name|diff_root_tree
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|new
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
name|int
name|retval
decl_stmt|;
name|void
modifier|*
name|tree
decl_stmt|;
name|struct
name|tree_desc
name|empty
decl_stmt|,
name|real
decl_stmt|;
name|call_diff_setup_done
argument_list|()
expr_stmt|;
name|tree
operator|=
name|read_object_with_reference
argument_list|(
name|new
argument_list|,
literal|"tree"
argument_list|,
operator|&
name|real
operator|.
name|size
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
name|die
argument_list|(
literal|"unable to read root tree (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|new
argument_list|)
argument_list|)
expr_stmt|;
name|real
operator|.
name|buf
operator|=
name|tree
expr_stmt|;
name|empty
operator|.
name|buf
operator|=
literal|""
expr_stmt|;
name|empty
operator|.
name|size
operator|=
literal|0
expr_stmt|;
name|retval
operator|=
name|diff_tree
argument_list|(
operator|&
name|empty
argument_list|,
operator|&
name|real
argument_list|,
name|base
argument_list|,
operator|&
name|diff_options
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree
argument_list|)
expr_stmt|;
name|call_diff_flush
argument_list|()
expr_stmt|;
return|return
name|retval
return|;
block|}
end_function
begin_function
DECL|function|generate_header
specifier|static
specifier|const
name|char
modifier|*
name|generate_header
parameter_list|(
specifier|const
name|char
modifier|*
name|commit
parameter_list|,
specifier|const
name|char
modifier|*
name|parent
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|,
name|unsigned
name|long
name|len
parameter_list|)
block|{
specifier|static
name|char
name|this_header
index|[
literal|16384
index|]
decl_stmt|;
name|int
name|offset
decl_stmt|;
if|if
condition|(
operator|!
name|verbose_header
condition|)
return|return
name|commit
return|;
name|offset
operator|=
name|sprintf
argument_list|(
name|this_header
argument_list|,
literal|"%s%s (from %s)\n"
argument_list|,
name|header_prefix
argument_list|,
name|commit
argument_list|,
name|parent
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|pretty_print_commit
argument_list|(
name|commit_format
argument_list|,
name|msg
argument_list|,
name|len
argument_list|,
name|this_header
operator|+
name|offset
argument_list|,
sizeof|sizeof
argument_list|(
name|this_header
argument_list|)
operator|-
name|offset
argument_list|)
expr_stmt|;
return|return
name|this_header
return|;
block|}
end_function
begin_function
DECL|function|diff_tree_commit
specifier|static
name|int
name|diff_tree_commit
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|commit
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|unsigned
name|long
name|size
decl_stmt|,
name|offset
decl_stmt|;
name|char
modifier|*
name|buf
init|=
name|read_object_with_reference
argument_list|(
name|commit
argument_list|,
literal|"commit"
argument_list|,
operator|&
name|size
argument_list|,
name|NULL
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|buf
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|name
condition|)
block|{
specifier|static
name|char
name|commit_name
index|[
literal|60
index|]
decl_stmt|;
name|strcpy
argument_list|(
name|commit_name
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
argument_list|)
argument_list|)
expr_stmt|;
name|name
operator|=
name|commit_name
expr_stmt|;
block|}
comment|/* Root commit? */
if|if
condition|(
name|show_root_diff
operator|&&
name|memcmp
argument_list|(
name|buf
operator|+
literal|46
argument_list|,
literal|"parent "
argument_list|,
literal|7
argument_list|)
condition|)
block|{
name|header
operator|=
name|generate_header
argument_list|(
name|name
argument_list|,
literal|"root"
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|diff_root_tree
argument_list|(
name|commit
argument_list|,
literal|""
argument_list|)
expr_stmt|;
block|}
comment|/* More than one parent? */
if|if
condition|(
name|ignore_merges
condition|)
block|{
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|buf
operator|+
literal|46
operator|+
literal|48
argument_list|,
literal|"parent "
argument_list|,
literal|7
argument_list|)
condition|)
return|return
literal|0
return|;
block|}
name|offset
operator|=
literal|46
expr_stmt|;
while|while
condition|(
name|offset
operator|+
literal|48
operator|<
name|size
operator|&&
operator|!
name|memcmp
argument_list|(
name|buf
operator|+
name|offset
argument_list|,
literal|"parent "
argument_list|,
literal|7
argument_list|)
condition|)
block|{
name|unsigned
name|char
name|parent
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|buf
operator|+
name|offset
operator|+
literal|7
argument_list|,
name|parent
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|header
operator|=
name|generate_header
argument_list|(
name|name
argument_list|,
name|sha1_to_hex
argument_list|(
name|parent
argument_list|)
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|diff_tree_sha1_top
argument_list|(
name|parent
argument_list|,
name|commit
argument_list|,
literal|""
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|header
operator|&&
name|verbose_header
condition|)
block|{
name|header_prefix
operator|=
literal|"\ndiff-tree "
expr_stmt|;
comment|/* 			 * Don't print multiple merge entries if we 			 * don't print the diffs. 			 */
block|}
name|offset
operator|+=
literal|48
expr_stmt|;
block|}
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|diff_tree_stdin
specifier|static
name|int
name|diff_tree_stdin
parameter_list|(
name|char
modifier|*
name|line
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|line
argument_list|)
decl_stmt|;
name|unsigned
name|char
name|commit
index|[
literal|20
index|]
decl_stmt|,
name|parent
index|[
literal|20
index|]
decl_stmt|;
specifier|static
name|char
name|this_header
index|[
literal|1000
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|len
operator|||
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
return|return
operator|-
literal|1
return|;
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|line
argument_list|,
name|commit
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|isspace
argument_list|(
name|line
index|[
literal|40
index|]
argument_list|)
operator|&&
operator|!
name|get_sha1_hex
argument_list|(
name|line
operator|+
literal|41
argument_list|,
name|parent
argument_list|)
condition|)
block|{
name|line
index|[
literal|40
index|]
operator|=
literal|0
expr_stmt|;
name|line
index|[
literal|81
index|]
operator|=
literal|0
expr_stmt|;
name|sprintf
argument_list|(
name|this_header
argument_list|,
literal|"%s (from %s)\n"
argument_list|,
name|line
argument_list|,
name|line
operator|+
literal|41
argument_list|)
expr_stmt|;
name|header
operator|=
name|this_header
expr_stmt|;
return|return
name|diff_tree_sha1_top
argument_list|(
name|parent
argument_list|,
name|commit
argument_list|,
literal|""
argument_list|)
return|;
block|}
name|line
index|[
literal|40
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|diff_tree_commit
argument_list|(
name|commit
argument_list|,
name|line
argument_list|)
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|diff_tree_usage
specifier|static
specifier|const
name|char
name|diff_tree_usage
index|[]
init|=
literal|"git-diff-tree [--stdin] [-m] [-s] [-v] [--pretty] [-t] [-r] [--root] "
literal|"[<common diff options>]<tree-ish> [<tree-ish>] [<path>...]\n"
literal|"  -r            diff recursively\n"
literal|"  --root        include the initial commit as diff against /dev/null\n"
name|COMMON_DIFF_OPTIONS_HELP
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|nr_sha1
decl_stmt|;
name|char
name|line
index|[
literal|1000
index|]
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|2
index|]
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|prefix
init|=
name|setup_git_directory
argument_list|()
decl_stmt|;
name|git_config
argument_list|(
name|git_diff_config
argument_list|)
expr_stmt|;
name|nr_sha1
operator|=
literal|0
expr_stmt|;
name|diff_setup
argument_list|(
operator|&
name|diff_options
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|diff_opt_cnt
decl_stmt|;
specifier|const
name|char
modifier|*
name|arg
decl_stmt|;
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
name|arg
operator|=
operator|*
name|argv
expr_stmt|;
if|if
condition|(
operator|!
name|arg
condition|)
break|break;
if|if
condition|(
operator|*
name|arg
operator|!=
literal|'-'
condition|)
block|{
if|if
condition|(
name|nr_sha1
operator|<
literal|2
operator|&&
operator|!
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
index|[
name|nr_sha1
index|]
argument_list|)
condition|)
block|{
name|nr_sha1
operator|++
expr_stmt|;
continue|continue;
block|}
break|break;
block|}
name|diff_opt_cnt
operator|=
name|diff_opt_parse
argument_list|(
operator|&
name|diff_options
argument_list|,
name|argv
argument_list|,
name|argc
argument_list|)
expr_stmt|;
if|if
condition|(
name|diff_opt_cnt
operator|<
literal|0
condition|)
name|usage
argument_list|(
name|diff_tree_usage
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|diff_opt_cnt
condition|)
block|{
name|argv
operator|+=
name|diff_opt_cnt
operator|-
literal|1
expr_stmt|;
name|argc
operator|-=
name|diff_opt_cnt
operator|-
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|argv
operator|++
expr_stmt|;
name|argc
operator|--
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-r"
argument_list|)
condition|)
block|{
name|diff_options
operator|.
name|recursive
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-t"
argument_list|)
condition|)
block|{
name|diff_options
operator|.
name|recursive
operator|=
literal|1
expr_stmt|;
name|diff_options
operator|.
name|tree_in_recursive
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-m"
argument_list|)
condition|)
block|{
name|ignore_merges
operator|=
literal|0
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-v"
argument_list|)
condition|)
block|{
name|verbose_header
operator|=
literal|1
expr_stmt|;
name|header_prefix
operator|=
literal|"diff-tree "
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--pretty"
argument_list|,
literal|8
argument_list|)
condition|)
block|{
name|verbose_header
operator|=
literal|1
expr_stmt|;
name|header_prefix
operator|=
literal|"diff-tree "
expr_stmt|;
name|commit_format
operator|=
name|get_commit_format
argument_list|(
name|arg
operator|+
literal|8
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--stdin"
argument_list|)
condition|)
block|{
name|read_stdin
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--root"
argument_list|)
condition|)
block|{
name|show_root_diff
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--no-commit-id"
argument_list|)
condition|)
block|{
name|no_commit_id
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|diff_tree_usage
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|diff_options
operator|.
name|output_format
operator|==
name|DIFF_FORMAT_PATCH
condition|)
name|diff_options
operator|.
name|recursive
operator|=
literal|1
expr_stmt|;
name|diff_tree_setup_paths
argument_list|(
name|get_pathspec
argument_list|(
name|prefix
argument_list|,
name|argv
argument_list|)
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|nr_sha1
condition|)
block|{
case|case
literal|0
case|:
if|if
condition|(
operator|!
name|read_stdin
condition|)
name|usage
argument_list|(
name|diff_tree_usage
argument_list|)
expr_stmt|;
break|break;
case|case
literal|1
case|:
name|diff_tree_commit
argument_list|(
name|sha1
index|[
literal|0
index|]
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
break|break;
case|case
literal|2
case|:
name|diff_tree_sha1_top
argument_list|(
name|sha1
index|[
literal|0
index|]
argument_list|,
name|sha1
index|[
literal|1
index|]
argument_list|,
literal|""
argument_list|)
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|read_stdin
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|diff_options
operator|.
name|detect_rename
condition|)
name|diff_options
operator|.
name|setup
operator||=
operator|(
name|DIFF_SETUP_USE_SIZE_CACHE
operator||
name|DIFF_SETUP_USE_CACHE
operator|)
expr_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|line
argument_list|,
sizeof|sizeof
argument_list|(
name|line
argument_list|)
argument_list|,
name|stdin
argument_list|)
condition|)
name|diff_tree_stdin
argument_list|(
name|line
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
