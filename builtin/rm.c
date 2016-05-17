begin_unit
begin_comment
comment|/*  * "git rm" builtin command  *  * Copyright (C) Linus Torvalds 2006  */
end_comment
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"lockfile.h"
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
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"submodule.h"
end_include
begin_include
include|#
directive|include
file|"pathspec.h"
end_include
begin_decl_stmt
DECL|variable|builtin_rm_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|builtin_rm_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git rm [<options>] [--]<file>..."
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_struct
specifier|static
struct|struct
block|{
DECL|member|nr
DECL|member|alloc
name|int
name|nr
decl_stmt|,
name|alloc
decl_stmt|;
struct|struct
block|{
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|is_submodule
name|char
name|is_submodule
decl_stmt|;
block|}
DECL|member|entry
modifier|*
name|entry
struct|;
block|}
DECL|variable|list
name|list
struct|;
end_struct
begin_function
DECL|function|get_ours_cache_pos
specifier|static
name|int
name|get_ours_cache_pos
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|pos
parameter_list|)
block|{
name|int
name|i
init|=
operator|-
name|pos
operator|-
literal|1
decl_stmt|;
while|while
condition|(
operator|(
name|i
operator|<
name|active_nr
operator|)
operator|&&
operator|!
name|strcmp
argument_list|(
name|active_cache
index|[
name|i
index|]
operator|->
name|name
argument_list|,
name|path
argument_list|)
condition|)
block|{
if|if
condition|(
name|ce_stage
argument_list|(
name|active_cache
index|[
name|i
index|]
argument_list|)
operator|==
literal|2
condition|)
return|return
name|i
return|;
name|i
operator|++
expr_stmt|;
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|print_error_files
specifier|static
name|void
name|print_error_files
parameter_list|(
name|struct
name|string_list
modifier|*
name|files_list
parameter_list|,
specifier|const
name|char
modifier|*
name|main_msg
parameter_list|,
specifier|const
name|char
modifier|*
name|hints_msg
parameter_list|,
name|int
modifier|*
name|errs
parameter_list|)
block|{
if|if
condition|(
name|files_list
operator|->
name|nr
condition|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|strbuf
name|err_msg
init|=
name|STRBUF_INIT
decl_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|err_msg
argument_list|,
name|main_msg
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
name|files_list
operator|->
name|nr
condition|;
name|i
operator|++
control|)
name|strbuf_addf
argument_list|(
operator|&
name|err_msg
argument_list|,
literal|"\n    %s"
argument_list|,
name|files_list
operator|->
name|items
index|[
name|i
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
if|if
condition|(
name|advice_rm_hints
condition|)
name|strbuf_addstr
argument_list|(
operator|&
name|err_msg
argument_list|,
name|hints_msg
argument_list|)
expr_stmt|;
operator|*
name|errs
operator|=
name|error
argument_list|(
literal|"%s"
argument_list|,
name|err_msg
operator|.
name|buf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|err_msg
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|error_removing_concrete_submodules
specifier|static
name|void
name|error_removing_concrete_submodules
parameter_list|(
name|struct
name|string_list
modifier|*
name|files
parameter_list|,
name|int
modifier|*
name|errs
parameter_list|)
block|{
name|print_error_files
argument_list|(
name|files
argument_list|,
name|Q_
argument_list|(
literal|"the following submodule (or one of its nested "
literal|"submodules)\n"
literal|"uses a .git directory:"
argument_list|,
literal|"the following submodules (or one of their nested "
literal|"submodules)\n"
literal|"use a .git directory:"
argument_list|,
name|files
operator|->
name|nr
argument_list|)
argument_list|,
name|_
argument_list|(
literal|"\n(use 'rm -rf' if you really want to remove "
literal|"it including all of its history)"
argument_list|)
argument_list|,
name|errs
argument_list|)
expr_stmt|;
name|string_list_clear
argument_list|(
name|files
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|check_submodules_use_gitfiles
specifier|static
name|int
name|check_submodules_use_gitfiles
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|errs
init|=
literal|0
decl_stmt|;
name|struct
name|string_list
name|files
init|=
name|STRING_LIST_INIT_NODUP
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|list
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|name
init|=
name|list
operator|.
name|entry
index|[
name|i
index|]
operator|.
name|name
decl_stmt|;
name|int
name|pos
decl_stmt|;
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|name
argument_list|,
name|strlen
argument_list|(
name|name
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
block|{
name|pos
operator|=
name|get_ours_cache_pos
argument_list|(
name|name
argument_list|,
name|pos
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
continue|continue;
block|}
name|ce
operator|=
name|active_cache
index|[
name|pos
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|S_ISGITLINK
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
operator|||
operator|!
name|file_exists
argument_list|(
name|ce
operator|->
name|name
argument_list|)
operator|||
name|is_empty_dir
argument_list|(
name|name
argument_list|)
condition|)
continue|continue;
if|if
condition|(
operator|!
name|submodule_uses_gitfile
argument_list|(
name|name
argument_list|)
condition|)
name|string_list_append
argument_list|(
operator|&
name|files
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
name|error_removing_concrete_submodules
argument_list|(
operator|&
name|files
argument_list|,
operator|&
name|errs
argument_list|)
expr_stmt|;
return|return
name|errs
return|;
block|}
end_function
begin_function
DECL|function|check_local_mod
specifier|static
name|int
name|check_local_mod
parameter_list|(
name|unsigned
name|char
modifier|*
name|head
parameter_list|,
name|int
name|index_only
parameter_list|)
block|{
comment|/* 	 * Items in list are already sorted in the cache order, 	 * so we could do this a lot more efficiently by using 	 * tree_desc based traversal if we wanted to, but I am 	 * lazy, and who cares if removal of files is a tad 	 * slower than the theoretical maximum speed? 	 */
name|int
name|i
decl_stmt|,
name|no_head
decl_stmt|;
name|int
name|errs
init|=
literal|0
decl_stmt|;
name|struct
name|string_list
name|files_staged
init|=
name|STRING_LIST_INIT_NODUP
decl_stmt|;
name|struct
name|string_list
name|files_cached
init|=
name|STRING_LIST_INIT_NODUP
decl_stmt|;
name|struct
name|string_list
name|files_submodule
init|=
name|STRING_LIST_INIT_NODUP
decl_stmt|;
name|struct
name|string_list
name|files_local
init|=
name|STRING_LIST_INIT_NODUP
decl_stmt|;
name|no_head
operator|=
name|is_null_sha1
argument_list|(
name|head
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
name|list
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
name|int
name|pos
decl_stmt|;
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
init|=
name|list
operator|.
name|entry
index|[
name|i
index|]
operator|.
name|name
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|mode
decl_stmt|;
name|int
name|local_changes
init|=
literal|0
decl_stmt|;
name|int
name|staged_changes
init|=
literal|0
decl_stmt|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|name
argument_list|,
name|strlen
argument_list|(
name|name
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
block|{
comment|/* 			 * Skip unmerged entries except for populated submodules 			 * that could lose history when removed. 			 */
name|pos
operator|=
name|get_ours_cache_pos
argument_list|(
name|name
argument_list|,
name|pos
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
continue|continue;
if|if
condition|(
operator|!
name|S_ISGITLINK
argument_list|(
name|active_cache
index|[
name|pos
index|]
operator|->
name|ce_mode
argument_list|)
operator|||
name|is_empty_dir
argument_list|(
name|name
argument_list|)
condition|)
continue|continue;
block|}
name|ce
operator|=
name|active_cache
index|[
name|pos
index|]
expr_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|ce
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|ENOENT
operator|&&
name|errno
operator|!=
name|ENOTDIR
condition|)
name|warning_errno
argument_list|(
name|_
argument_list|(
literal|"failed to stat '%s'"
argument_list|)
argument_list|,
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
comment|/* It already vanished from the working tree */
continue|continue;
block|}
elseif|else
if|if
condition|(
name|S_ISDIR
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
comment|/* if a file was removed and it is now a 			 * directory, that is the same as ENOENT as 			 * far as git is concerned; we do not track 			 * directories unless they are submodules. 			 */
if|if
condition|(
operator|!
name|S_ISGITLINK
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
condition|)
continue|continue;
block|}
comment|/* 		 * "rm" of a path that has changes need to be treated 		 * carefully not to allow losing local changes 		 * accidentally.  A local change could be (1) file in 		 * work tree is different since the index; and/or (2) 		 * the user staged a content that is different from 		 * the current commit in the index. 		 * 		 * In such a case, you would need to --force the 		 * removal.  However, "rm --cached" (remove only from 		 * the index) is safe if the index matches the file in 		 * the work tree or the HEAD commit, as it means that 		 * the content being removed is available elsewhere. 		 */
comment|/* 		 * Is the index different from the file in the work tree? 		 * If it's a submodule, is its work tree modified? 		 */
if|if
condition|(
name|ce_match_stat
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|,
literal|0
argument_list|)
operator|||
operator|(
name|S_ISGITLINK
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
operator|&&
operator|!
name|ok_to_remove_submodule
argument_list|(
name|ce
operator|->
name|name
argument_list|)
operator|)
condition|)
name|local_changes
operator|=
literal|1
expr_stmt|;
comment|/* 		 * Is the index different from the HEAD commit?  By 		 * definition, before the very initial commit, 		 * anything staged in the index is treated by the same 		 * way as changed from the HEAD. 		 */
if|if
condition|(
name|no_head
operator|||
name|get_tree_entry
argument_list|(
name|head
argument_list|,
name|name
argument_list|,
name|sha1
argument_list|,
operator|&
name|mode
argument_list|)
operator|||
name|ce
operator|->
name|ce_mode
operator|!=
name|create_ce_mode
argument_list|(
name|mode
argument_list|)
operator|||
name|hashcmp
argument_list|(
name|ce
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|)
condition|)
name|staged_changes
operator|=
literal|1
expr_stmt|;
comment|/* 		 * If the index does not match the file in the work 		 * tree and if it does not match the HEAD commit 		 * either, (1) "git rm" without --cached definitely 		 * will lose information; (2) "git rm --cached" will 		 * lose information unless it is about removing an 		 * "intent to add" entry. 		 */
if|if
condition|(
name|local_changes
operator|&&
name|staged_changes
condition|)
block|{
if|if
condition|(
operator|!
name|index_only
operator|||
operator|!
name|ce_intent_to_add
argument_list|(
name|ce
argument_list|)
condition|)
name|string_list_append
argument_list|(
operator|&
name|files_staged
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|index_only
condition|)
block|{
if|if
condition|(
name|staged_changes
condition|)
name|string_list_append
argument_list|(
operator|&
name|files_cached
argument_list|,
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|local_changes
condition|)
block|{
if|if
condition|(
name|S_ISGITLINK
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
operator|&&
operator|!
name|submodule_uses_gitfile
argument_list|(
name|name
argument_list|)
condition|)
name|string_list_append
argument_list|(
operator|&
name|files_submodule
argument_list|,
name|name
argument_list|)
expr_stmt|;
else|else
name|string_list_append
argument_list|(
operator|&
name|files_local
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
block|}
block|}
name|print_error_files
argument_list|(
operator|&
name|files_staged
argument_list|,
name|Q_
argument_list|(
literal|"the following file has staged content different "
literal|"from both the\nfile and the HEAD:"
argument_list|,
literal|"the following files have staged content different"
literal|" from both the\nfile and the HEAD:"
argument_list|,
name|files_staged
operator|.
name|nr
argument_list|)
argument_list|,
name|_
argument_list|(
literal|"\n(use -f to force removal)"
argument_list|)
argument_list|,
operator|&
name|errs
argument_list|)
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|files_staged
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|print_error_files
argument_list|(
operator|&
name|files_cached
argument_list|,
name|Q_
argument_list|(
literal|"the following file has changes "
literal|"staged in the index:"
argument_list|,
literal|"the following files have changes "
literal|"staged in the index:"
argument_list|,
name|files_cached
operator|.
name|nr
argument_list|)
argument_list|,
name|_
argument_list|(
literal|"\n(use --cached to keep the file,"
literal|" or -f to force removal)"
argument_list|)
argument_list|,
operator|&
name|errs
argument_list|)
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|files_cached
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|error_removing_concrete_submodules
argument_list|(
operator|&
name|files_submodule
argument_list|,
operator|&
name|errs
argument_list|)
expr_stmt|;
name|print_error_files
argument_list|(
operator|&
name|files_local
argument_list|,
name|Q_
argument_list|(
literal|"the following file has local modifications:"
argument_list|,
literal|"the following files have local modifications:"
argument_list|,
name|files_local
operator|.
name|nr
argument_list|)
argument_list|,
name|_
argument_list|(
literal|"\n(use --cached to keep the file,"
literal|" or -f to force removal)"
argument_list|)
argument_list|,
operator|&
name|errs
argument_list|)
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|files_local
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
name|errs
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
begin_decl_stmt
DECL|variable|show_only
DECL|variable|force
DECL|variable|index_only
DECL|variable|recursive
DECL|variable|quiet
specifier|static
name|int
name|show_only
init|=
literal|0
decl_stmt|,
name|force
init|=
literal|0
decl_stmt|,
name|index_only
init|=
literal|0
decl_stmt|,
name|recursive
init|=
literal|0
decl_stmt|,
name|quiet
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|ignore_unmatch
specifier|static
name|int
name|ignore_unmatch
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|builtin_rm_options
specifier|static
name|struct
name|option
name|builtin_rm_options
index|[]
init|=
block|{
name|OPT__DRY_RUN
argument_list|(
operator|&
name|show_only
argument_list|,
name|N_
argument_list|(
literal|"dry run"
argument_list|)
argument_list|)
block|,
name|OPT__QUIET
argument_list|(
operator|&
name|quiet
argument_list|,
name|N_
argument_list|(
literal|"do not list removed files"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"cached"
argument_list|,
operator|&
name|index_only
argument_list|,
name|N_
argument_list|(
literal|"only remove from the index"
argument_list|)
argument_list|)
block|,
name|OPT__FORCE
argument_list|(
operator|&
name|force
argument_list|,
name|N_
argument_list|(
literal|"override the up-to-date check"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'r'
argument_list|,
name|NULL
argument_list|,
operator|&
name|recursive
argument_list|,
name|N_
argument_list|(
literal|"allow recursive removal"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"ignore-unmatch"
argument_list|,
operator|&
name|ignore_unmatch
argument_list|,
name|N_
argument_list|(
literal|"exit with a zero status even if nothing matched"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, }
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_rm
name|int
name|cmd_rm
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
decl_stmt|;
name|struct
name|pathspec
name|pathspec
decl_stmt|;
name|char
modifier|*
name|seen
decl_stmt|;
name|gitmodules_config
argument_list|()
expr_stmt|;
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
name|builtin_rm_options
argument_list|,
name|builtin_rm_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|argc
condition|)
name|usage_with_options
argument_list|(
name|builtin_rm_usage
argument_list|,
name|builtin_rm_options
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|index_only
condition|)
name|setup_work_tree
argument_list|()
expr_stmt|;
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
name|parse_pathspec
argument_list|(
operator|&
name|pathspec
argument_list|,
literal|0
argument_list|,
name|PATHSPEC_PREFER_CWD
operator||
name|PATHSPEC_STRIP_SUBMODULE_SLASH_CHEAP
argument_list|,
name|prefix
argument_list|,
name|argv
argument_list|)
expr_stmt|;
name|refresh_index
argument_list|(
operator|&
name|the_index
argument_list|,
name|REFRESH_QUIET
argument_list|,
operator|&
name|pathspec
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|seen
operator|=
name|xcalloc
argument_list|(
name|pathspec
operator|.
name|nr
argument_list|,
literal|1
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
name|active_nr
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|ce_path_match
argument_list|(
name|ce
argument_list|,
operator|&
name|pathspec
argument_list|,
name|seen
argument_list|)
condition|)
continue|continue;
name|ALLOC_GROW
argument_list|(
name|list
operator|.
name|entry
argument_list|,
name|list
operator|.
name|nr
operator|+
literal|1
argument_list|,
name|list
operator|.
name|alloc
argument_list|)
expr_stmt|;
name|list
operator|.
name|entry
index|[
name|list
operator|.
name|nr
index|]
operator|.
name|name
operator|=
name|xstrdup
argument_list|(
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
name|list
operator|.
name|entry
index|[
name|list
operator|.
name|nr
index|]
operator|.
name|is_submodule
operator|=
name|S_ISGITLINK
argument_list|(
name|ce
operator|->
name|ce_mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|list
operator|.
name|entry
index|[
name|list
operator|.
name|nr
operator|++
index|]
operator|.
name|is_submodule
operator|&&
operator|!
name|is_staging_gitmodules_ok
argument_list|()
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Please stage your changes to .gitmodules or stash them to proceed"
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|pathspec
operator|.
name|nr
condition|)
block|{
specifier|const
name|char
modifier|*
name|original
decl_stmt|;
name|int
name|seen_any
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
name|pathspec
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|original
operator|=
name|pathspec
operator|.
name|items
index|[
name|i
index|]
operator|.
name|original
expr_stmt|;
if|if
condition|(
operator|!
name|seen
index|[
name|i
index|]
condition|)
block|{
if|if
condition|(
operator|!
name|ignore_unmatch
condition|)
block|{
name|die
argument_list|(
name|_
argument_list|(
literal|"pathspec '%s' did not match any files"
argument_list|)
argument_list|,
name|original
argument_list|)
expr_stmt|;
block|}
block|}
else|else
block|{
name|seen_any
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|recursive
operator|&&
name|seen
index|[
name|i
index|]
operator|==
name|MATCHED_RECURSIVELY
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"not removing '%s' recursively without -r"
argument_list|)
argument_list|,
operator|*
name|original
condition|?
name|original
else|:
literal|"."
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|seen_any
condition|)
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * If not forced, the file, the index and the HEAD (if exists) 	 * must match; but the file can already been removed, since 	 * this sequence is a natural "novice" way: 	 * 	 *	rm F; git rm F 	 * 	 * Further, if HEAD commit exists, "diff-index --cached" must 	 * report no changes unless forced. 	 */
if|if
condition|(
operator|!
name|force
condition|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
literal|"HEAD"
argument_list|,
name|sha1
argument_list|)
condition|)
name|hashclr
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|check_local_mod
argument_list|(
name|sha1
argument_list|,
name|index_only
argument_list|)
condition|)
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|index_only
condition|)
block|{
if|if
condition|(
name|check_submodules_use_gitfiles
argument_list|()
condition|)
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * First remove the names from the index: we won't commit 	 * the index unless all of them succeed. 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|list
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|path
init|=
name|list
operator|.
name|entry
index|[
name|i
index|]
operator|.
name|name
decl_stmt|;
if|if
condition|(
operator|!
name|quiet
condition|)
name|printf
argument_list|(
literal|"rm '%s'\n"
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|remove_file_from_cache
argument_list|(
name|path
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"git rm: unable to remove %s"
argument_list|)
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|show_only
condition|)
return|return
literal|0
return|;
comment|/* 	 * Then, unless we used "--cached", remove the filenames from 	 * the workspace. If we fail to remove the first one, we 	 * abort the "git rm" (but once we've successfully removed 	 * any file at all, we'll go ahead and commit to it all: 	 * by then we've already committed ourselves and can't fail 	 * in the middle) 	 */
if|if
condition|(
operator|!
name|index_only
condition|)
block|{
name|int
name|removed
init|=
literal|0
decl_stmt|,
name|gitmodules_modified
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
name|list
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|path
init|=
name|list
operator|.
name|entry
index|[
name|i
index|]
operator|.
name|name
decl_stmt|;
if|if
condition|(
name|list
operator|.
name|entry
index|[
name|i
index|]
operator|.
name|is_submodule
condition|)
block|{
if|if
condition|(
name|is_empty_dir
argument_list|(
name|path
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|rmdir
argument_list|(
name|path
argument_list|)
condition|)
block|{
name|removed
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|remove_path_from_gitmodules
argument_list|(
name|path
argument_list|)
condition|)
name|gitmodules_modified
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
block|}
else|else
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|buf
argument_list|,
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|remove_dir_recursively
argument_list|(
operator|&
name|buf
argument_list|,
literal|0
argument_list|)
condition|)
block|{
name|removed
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|remove_path_from_gitmodules
argument_list|(
name|path
argument_list|)
condition|)
name|gitmodules_modified
operator|=
literal|1
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
continue|continue;
block|}
elseif|else
if|if
condition|(
operator|!
name|file_exists
argument_list|(
name|path
argument_list|)
condition|)
comment|/* Submodule was removed by user */
if|if
condition|(
operator|!
name|remove_path_from_gitmodules
argument_list|(
name|path
argument_list|)
condition|)
name|gitmodules_modified
operator|=
literal|1
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
comment|/* Fallthrough and let remove_path() fail. */
block|}
block|}
if|if
condition|(
operator|!
name|remove_path
argument_list|(
name|path
argument_list|)
condition|)
block|{
name|removed
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|removed
condition|)
name|die_errno
argument_list|(
literal|"git rm: '%s'"
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|gitmodules_modified
condition|)
name|stage_updated_gitmodules
argument_list|()
expr_stmt|;
block|}
if|if
condition|(
name|active_cache_changed
condition|)
block|{
if|if
condition|(
name|write_locked_index
argument_list|(
operator|&
name|the_index
argument_list|,
operator|&
name|lock_file
argument_list|,
name|COMMIT_LOCK
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
