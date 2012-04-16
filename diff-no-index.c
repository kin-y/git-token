begin_unit
begin_comment
comment|/*  * "diff --no-index" support  * Copyright (c) 2007 by Johannes Schindelin  * Copyright (c) 2008 by Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"color.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"diffcore.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"log-tree.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_function
DECL|function|read_directory
specifier|static
name|int
name|read_directory
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|string_list
modifier|*
name|list
parameter_list|)
block|{
name|DIR
modifier|*
name|dir
decl_stmt|;
name|struct
name|dirent
modifier|*
name|e
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|dir
operator|=
name|opendir
argument_list|(
name|path
argument_list|)
operator|)
condition|)
return|return
name|error
argument_list|(
literal|"Could not open directory %s"
argument_list|,
name|path
argument_list|)
return|;
while|while
condition|(
operator|(
name|e
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
condition|)
if|if
condition|(
name|strcmp
argument_list|(
literal|"."
argument_list|,
name|e
operator|->
name|d_name
argument_list|)
operator|&&
name|strcmp
argument_list|(
literal|".."
argument_list|,
name|e
operator|->
name|d_name
argument_list|)
condition|)
name|string_list_insert
argument_list|(
name|list
argument_list|,
name|e
operator|->
name|d_name
argument_list|)
expr_stmt|;
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_mode
specifier|static
name|int
name|get_mode
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
modifier|*
name|mode
parameter_list|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
operator|!
name|path
operator|||
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"/dev/null"
argument_list|)
condition|)
operator|*
name|mode
operator|=
literal|0
expr_stmt|;
ifdef|#
directive|ifdef
name|_WIN32
elseif|else
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|path
argument_list|,
literal|"nul"
argument_list|)
condition|)
operator|*
name|mode
operator|=
literal|0
expr_stmt|;
endif|#
directive|endif
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"-"
argument_list|)
condition|)
operator|*
name|mode
operator|=
name|create_ce_mode
argument_list|(
literal|0666
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|lstat
argument_list|(
name|path
argument_list|,
operator|&
name|st
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Could not access '%s'"
argument_list|,
name|path
argument_list|)
return|;
else|else
operator|*
name|mode
operator|=
name|st
operator|.
name|st_mode
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|queue_diff
specifier|static
name|int
name|queue_diff
parameter_list|(
name|struct
name|diff_options
modifier|*
name|o
parameter_list|,
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|)
block|{
name|int
name|mode1
init|=
literal|0
decl_stmt|,
name|mode2
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|get_mode
argument_list|(
name|name1
argument_list|,
operator|&
name|mode1
argument_list|)
operator|||
name|get_mode
argument_list|(
name|name2
argument_list|,
operator|&
name|mode2
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|mode1
operator|&&
name|mode2
operator|&&
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
operator|!=
name|S_ISDIR
argument_list|(
name|mode2
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"file/directory conflict: %s, %s"
argument_list|,
name|name1
argument_list|,
name|name2
argument_list|)
return|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
operator|||
name|S_ISDIR
argument_list|(
name|mode2
argument_list|)
condition|)
block|{
name|char
name|buffer1
index|[
name|PATH_MAX
index|]
decl_stmt|,
name|buffer2
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|struct
name|string_list
name|p1
init|=
name|STRING_LIST_INIT_DUP
decl_stmt|;
name|struct
name|string_list
name|p2
init|=
name|STRING_LIST_INIT_DUP
decl_stmt|;
name|int
name|len1
init|=
literal|0
decl_stmt|,
name|len2
init|=
literal|0
decl_stmt|,
name|i1
decl_stmt|,
name|i2
decl_stmt|,
name|ret
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|name1
operator|&&
name|read_directory
argument_list|(
name|name1
argument_list|,
operator|&
name|p1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|name2
operator|&&
name|read_directory
argument_list|(
name|name2
argument_list|,
operator|&
name|p2
argument_list|)
condition|)
block|{
name|string_list_clear
argument_list|(
operator|&
name|p1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|name1
condition|)
block|{
name|len1
operator|=
name|strlen
argument_list|(
name|name1
argument_list|)
expr_stmt|;
if|if
condition|(
name|len1
operator|>
literal|0
operator|&&
name|name1
index|[
name|len1
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
name|len1
operator|--
expr_stmt|;
name|memcpy
argument_list|(
name|buffer1
argument_list|,
name|name1
argument_list|,
name|len1
argument_list|)
expr_stmt|;
name|buffer1
index|[
name|len1
operator|++
index|]
operator|=
literal|'/'
expr_stmt|;
block|}
if|if
condition|(
name|name2
condition|)
block|{
name|len2
operator|=
name|strlen
argument_list|(
name|name2
argument_list|)
expr_stmt|;
if|if
condition|(
name|len2
operator|>
literal|0
operator|&&
name|name2
index|[
name|len2
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
name|len2
operator|--
expr_stmt|;
name|memcpy
argument_list|(
name|buffer2
argument_list|,
name|name2
argument_list|,
name|len2
argument_list|)
expr_stmt|;
name|buffer2
index|[
name|len2
operator|++
index|]
operator|=
literal|'/'
expr_stmt|;
block|}
for|for
control|(
name|i1
operator|=
name|i2
operator|=
literal|0
init|;
operator|!
name|ret
operator|&&
operator|(
name|i1
operator|<
name|p1
operator|.
name|nr
operator|||
name|i2
operator|<
name|p2
operator|.
name|nr
operator|)
condition|;
control|)
block|{
specifier|const
name|char
modifier|*
name|n1
decl_stmt|,
modifier|*
name|n2
decl_stmt|;
name|int
name|comp
decl_stmt|;
if|if
condition|(
name|i1
operator|==
name|p1
operator|.
name|nr
condition|)
name|comp
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|i2
operator|==
name|p2
operator|.
name|nr
condition|)
name|comp
operator|=
operator|-
literal|1
expr_stmt|;
else|else
name|comp
operator|=
name|strcmp
argument_list|(
name|p1
operator|.
name|items
index|[
name|i1
index|]
operator|.
name|string
argument_list|,
name|p2
operator|.
name|items
index|[
name|i2
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
if|if
condition|(
name|comp
operator|>
literal|0
condition|)
name|n1
operator|=
name|NULL
expr_stmt|;
else|else
block|{
name|n1
operator|=
name|buffer1
expr_stmt|;
name|strncpy
argument_list|(
name|buffer1
operator|+
name|len1
argument_list|,
name|p1
operator|.
name|items
index|[
name|i1
operator|++
index|]
operator|.
name|string
argument_list|,
name|PATH_MAX
operator|-
name|len1
argument_list|)
expr_stmt|;
name|buffer1
index|[
name|PATH_MAX
operator|-
literal|1
index|]
operator|=
literal|0
expr_stmt|;
block|}
if|if
condition|(
name|comp
operator|<
literal|0
condition|)
name|n2
operator|=
name|NULL
expr_stmt|;
else|else
block|{
name|n2
operator|=
name|buffer2
expr_stmt|;
name|strncpy
argument_list|(
name|buffer2
operator|+
name|len2
argument_list|,
name|p2
operator|.
name|items
index|[
name|i2
operator|++
index|]
operator|.
name|string
argument_list|,
name|PATH_MAX
operator|-
name|len2
argument_list|)
expr_stmt|;
name|buffer2
index|[
name|PATH_MAX
operator|-
literal|1
index|]
operator|=
literal|0
expr_stmt|;
block|}
name|ret
operator|=
name|queue_diff
argument_list|(
name|o
argument_list|,
name|n1
argument_list|,
name|n2
argument_list|)
expr_stmt|;
block|}
name|string_list_clear
argument_list|(
operator|&
name|p1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|p2
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
else|else
block|{
name|struct
name|diff_filespec
modifier|*
name|d1
decl_stmt|,
modifier|*
name|d2
decl_stmt|;
if|if
condition|(
name|DIFF_OPT_TST
argument_list|(
name|o
argument_list|,
name|REVERSE_DIFF
argument_list|)
condition|)
block|{
name|unsigned
name|tmp
decl_stmt|;
specifier|const
name|char
modifier|*
name|tmp_c
decl_stmt|;
name|tmp
operator|=
name|mode1
expr_stmt|;
name|mode1
operator|=
name|mode2
expr_stmt|;
name|mode2
operator|=
name|tmp
expr_stmt|;
name|tmp_c
operator|=
name|name1
expr_stmt|;
name|name1
operator|=
name|name2
expr_stmt|;
name|name2
operator|=
name|tmp_c
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|name1
condition|)
name|name1
operator|=
literal|"/dev/null"
expr_stmt|;
if|if
condition|(
operator|!
name|name2
condition|)
name|name2
operator|=
literal|"/dev/null"
expr_stmt|;
name|d1
operator|=
name|alloc_filespec
argument_list|(
name|name1
argument_list|)
expr_stmt|;
name|d2
operator|=
name|alloc_filespec
argument_list|(
name|name2
argument_list|)
expr_stmt|;
name|fill_filespec
argument_list|(
name|d1
argument_list|,
name|null_sha1
argument_list|,
name|mode1
argument_list|)
expr_stmt|;
name|fill_filespec
argument_list|(
name|d2
argument_list|,
name|null_sha1
argument_list|,
name|mode2
argument_list|)
expr_stmt|;
name|diff_queue
argument_list|(
operator|&
name|diff_queued_diff
argument_list|,
name|d1
argument_list|,
name|d2
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
block|}
end_function
begin_function
DECL|function|path_outside_repo
specifier|static
name|int
name|path_outside_repo
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|work_tree
decl_stmt|;
name|size_t
name|len
decl_stmt|;
if|if
condition|(
operator|!
name|is_absolute_path
argument_list|(
name|path
argument_list|)
condition|)
return|return
literal|0
return|;
name|work_tree
operator|=
name|get_git_work_tree
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|work_tree
condition|)
return|return
literal|1
return|;
name|len
operator|=
name|strlen
argument_list|(
name|work_tree
argument_list|)
expr_stmt|;
if|if
condition|(
name|strncmp
argument_list|(
name|path
argument_list|,
name|work_tree
argument_list|,
name|len
argument_list|)
operator|||
operator|(
name|path
index|[
name|len
index|]
operator|!=
literal|'\0'
operator|&&
name|path
index|[
name|len
index|]
operator|!=
literal|'/'
operator|)
condition|)
return|return
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|diff_no_index
name|void
name|diff_no_index
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
name|int
name|nongit
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|no_index
init|=
literal|0
decl_stmt|;
name|unsigned
name|options
init|=
literal|0
decl_stmt|;
comment|/* Were we asked to do --no-index explicitly? */
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
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
name|argv
index|[
name|i
index|]
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|i
operator|++
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"--no-index"
argument_list|)
condition|)
name|no_index
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|argv
index|[
name|i
index|]
index|[
literal|0
index|]
operator|!=
literal|'-'
condition|)
break|break;
block|}
if|if
condition|(
operator|!
name|no_index
operator|&&
operator|!
name|nongit
condition|)
block|{
comment|/* 		 * Inside a git repository, without --no-index.  Only 		 * when a path outside the repository is given, 		 * e.g. "git diff /var/tmp/[12]", or "git diff 		 * Makefile /var/tmp/Makefile", allow it to be used as 		 * a colourful "diff" replacement. 		 */
if|if
condition|(
operator|(
name|argc
operator|!=
name|i
operator|+
literal|2
operator|)
operator|||
operator|(
operator|!
name|path_outside_repo
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|)
operator|&&
operator|!
name|path_outside_repo
argument_list|(
name|argv
index|[
name|i
operator|+
literal|1
index|]
argument_list|)
operator|)
condition|)
return|return;
block|}
if|if
condition|(
name|argc
operator|!=
name|i
operator|+
literal|2
condition|)
name|usagef
argument_list|(
literal|"git diff %s<path><path>"
argument_list|,
name|no_index
condition|?
literal|"--no-index"
else|:
literal|"[--no-index]"
argument_list|)
expr_stmt|;
name|diff_setup
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
operator|-
literal|2
condition|;
control|)
block|{
name|int
name|j
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"--no-index"
argument_list|)
condition|)
name|i
operator|++
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"-q"
argument_list|)
condition|)
block|{
name|options
operator||=
name|DIFF_SILENT_ON_REMOVED
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"--"
argument_list|)
condition|)
name|i
operator|++
expr_stmt|;
else|else
block|{
name|j
operator|=
name|diff_opt_parse
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|argv
operator|+
name|i
argument_list|,
name|argc
operator|-
name|i
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|j
condition|)
name|die
argument_list|(
literal|"invalid diff option/value: %s"
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|i
operator|+=
name|j
expr_stmt|;
block|}
block|}
comment|/* 	 * If the user asked for our exit code then don't start a 	 * pager or we would end up reporting its exit code instead. 	 */
if|if
condition|(
operator|!
name|DIFF_OPT_TST
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|EXIT_WITH_STATUS
argument_list|)
condition|)
name|setup_pager
argument_list|()
expr_stmt|;
if|if
condition|(
name|prefix
condition|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|prefix
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|paths
index|[
literal|3
index|]
decl_stmt|;
name|memset
argument_list|(
name|paths
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|paths
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
literal|2
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|p
init|=
name|argv
index|[
name|argc
operator|-
literal|2
operator|+
name|i
index|]
decl_stmt|;
comment|/* 			 * stdin should be spelled as '-'; if you have 			 * path that is '-', spell it as ./-. 			 */
name|p
operator|=
operator|(
name|strcmp
argument_list|(
name|p
argument_list|,
literal|"-"
argument_list|)
condition|?
name|xstrdup
argument_list|(
name|prefix_filename
argument_list|(
name|prefix
argument_list|,
name|len
argument_list|,
name|p
argument_list|)
argument_list|)
else|:
name|p
operator|)
expr_stmt|;
name|paths
index|[
name|i
index|]
operator|=
name|p
expr_stmt|;
block|}
name|diff_tree_setup_paths
argument_list|(
name|paths
argument_list|,
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
block|}
else|else
name|diff_tree_setup_paths
argument_list|(
name|argv
operator|+
name|argc
operator|-
literal|2
argument_list|,
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|revs
operator|->
name|diffopt
operator|.
name|skip_stat_unmatch
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|revs
operator|->
name|diffopt
operator|.
name|output_format
condition|)
name|revs
operator|->
name|diffopt
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_PATCH
expr_stmt|;
name|DIFF_OPT_SET
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|EXIT_WITH_STATUS
argument_list|)
expr_stmt|;
name|DIFF_OPT_SET
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|NO_INDEX
argument_list|)
expr_stmt|;
name|revs
operator|->
name|max_count
operator|=
operator|-
literal|2
expr_stmt|;
if|if
condition|(
name|diff_setup_done
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"diff_setup_done failed"
argument_list|)
expr_stmt|;
if|if
condition|(
name|queue_diff
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|revs
operator|->
name|diffopt
operator|.
name|pathspec
operator|.
name|raw
index|[
literal|0
index|]
argument_list|,
name|revs
operator|->
name|diffopt
operator|.
name|pathspec
operator|.
name|raw
index|[
literal|1
index|]
argument_list|)
condition|)
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|diff_set_mnemonic_prefix
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
literal|"1/"
argument_list|,
literal|"2/"
argument_list|)
expr_stmt|;
name|diffcore_std
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
comment|/* 	 * The return code for --no-index imitates diff(1): 	 * 0 = no changes, 1 = changes, else error 	 */
name|exit
argument_list|(
name|revs
operator|->
name|diffopt
operator|.
name|found_changes
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
