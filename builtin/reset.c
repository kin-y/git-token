begin_unit
begin_comment
comment|/*  * "git reset" builtin command  *  * Copyright (c) 2007 Carlos Rica  *  * Based on git-reset.sh, which is  *  * Copyright (c) 2005, 2006 Linus Torvalds and Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
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
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"branch.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"unpack-trees.h"
end_include
begin_include
include|#
directive|include
file|"cache-tree.h"
end_include
begin_decl_stmt
DECL|variable|git_reset_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|git_reset_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git reset [--mixed | --soft | --hard | --merge | --keep] [-q] [<commit>]"
argument_list|)
block|,
name|N_
argument_list|(
literal|"git reset [-q]<tree-ish> [--]<paths>..."
argument_list|)
block|,
name|N_
argument_list|(
literal|"git reset --patch [<tree-ish>] [--] [<paths>...]"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_enum
DECL|enum|reset_type
DECL|enumerator|MIXED
DECL|enumerator|SOFT
DECL|enumerator|HARD
DECL|enumerator|MERGE
DECL|enumerator|KEEP
DECL|enumerator|NONE
enum|enum
name|reset_type
block|{
name|MIXED
block|,
name|SOFT
block|,
name|HARD
block|,
name|MERGE
block|,
name|KEEP
block|,
name|NONE
block|}
enum|;
end_enum
begin_decl_stmt
DECL|variable|reset_type_names
specifier|static
specifier|const
name|char
modifier|*
name|reset_type_names
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"mixed"
argument_list|)
block|,
name|N_
argument_list|(
literal|"soft"
argument_list|)
block|,
name|N_
argument_list|(
literal|"hard"
argument_list|)
block|,
name|N_
argument_list|(
literal|"merge"
argument_list|)
block|,
name|N_
argument_list|(
literal|"keep"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|is_merge
specifier|static
specifier|inline
name|int
name|is_merge
parameter_list|(
name|void
parameter_list|)
block|{
return|return
operator|!
name|access
argument_list|(
name|git_path
argument_list|(
literal|"MERGE_HEAD"
argument_list|)
argument_list|,
name|F_OK
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|reset_index
specifier|static
name|int
name|reset_index
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|reset_type
parameter_list|,
name|int
name|quiet
parameter_list|)
block|{
name|int
name|nr
init|=
literal|1
decl_stmt|;
name|struct
name|tree_desc
name|desc
index|[
literal|2
index|]
decl_stmt|;
name|struct
name|tree
modifier|*
name|tree
decl_stmt|;
name|struct
name|unpack_trees_options
name|opts
decl_stmt|;
name|memset
argument_list|(
operator|&
name|opts
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|opts
argument_list|)
argument_list|)
expr_stmt|;
name|opts
operator|.
name|head_idx
operator|=
literal|1
expr_stmt|;
name|opts
operator|.
name|src_index
operator|=
operator|&
name|the_index
expr_stmt|;
name|opts
operator|.
name|dst_index
operator|=
operator|&
name|the_index
expr_stmt|;
name|opts
operator|.
name|fn
operator|=
name|oneway_merge
expr_stmt|;
name|opts
operator|.
name|merge
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|quiet
condition|)
name|opts
operator|.
name|verbose_update
operator|=
literal|1
expr_stmt|;
switch|switch
condition|(
name|reset_type
condition|)
block|{
case|case
name|KEEP
case|:
case|case
name|MERGE
case|:
name|opts
operator|.
name|update
operator|=
literal|1
expr_stmt|;
break|break;
case|case
name|HARD
case|:
name|opts
operator|.
name|update
operator|=
literal|1
expr_stmt|;
comment|/* fallthrough */
default|default:
name|opts
operator|.
name|reset
operator|=
literal|1
expr_stmt|;
block|}
name|read_cache_unmerged
argument_list|()
expr_stmt|;
if|if
condition|(
name|reset_type
operator|==
name|KEEP
condition|)
block|{
name|unsigned
name|char
name|head_sha1
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
name|head_sha1
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"You do not have a valid HEAD."
argument_list|)
argument_list|)
return|;
if|if
condition|(
operator|!
name|fill_tree_descriptor
argument_list|(
name|desc
argument_list|,
name|head_sha1
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"Failed to find tree of HEAD."
argument_list|)
argument_list|)
return|;
name|nr
operator|++
expr_stmt|;
name|opts
operator|.
name|fn
operator|=
name|twoway_merge
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|fill_tree_descriptor
argument_list|(
name|desc
operator|+
name|nr
operator|-
literal|1
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"Failed to find tree of %s."
argument_list|)
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|unpack_trees
argument_list|(
name|nr
argument_list|,
name|desc
argument_list|,
operator|&
name|opts
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|reset_type
operator|==
name|MIXED
operator|||
name|reset_type
operator|==
name|HARD
condition|)
block|{
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
name|prime_cache_tree
argument_list|(
operator|&
name|active_cache_tree
argument_list|,
name|tree
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|print_new_head_line
specifier|static
name|void
name|print_new_head_line
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|hex
decl_stmt|,
modifier|*
name|body
decl_stmt|;
name|char
modifier|*
name|msg
decl_stmt|;
name|hex
operator|=
name|find_unique_abbrev
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|DEFAULT_ABBREV
argument_list|)
expr_stmt|;
name|printf
argument_list|(
name|_
argument_list|(
literal|"HEAD is now at %s"
argument_list|)
argument_list|,
name|hex
argument_list|)
expr_stmt|;
name|msg
operator|=
name|logmsg_reencode
argument_list|(
name|commit
argument_list|,
name|NULL
argument_list|,
name|get_log_output_encoding
argument_list|()
argument_list|)
expr_stmt|;
name|body
operator|=
name|strstr
argument_list|(
name|msg
argument_list|,
literal|"\n\n"
argument_list|)
expr_stmt|;
if|if
condition|(
name|body
condition|)
block|{
specifier|const
name|char
modifier|*
name|eol
decl_stmt|;
name|size_t
name|len
decl_stmt|;
name|body
operator|+=
literal|2
expr_stmt|;
name|eol
operator|=
name|strchr
argument_list|(
name|body
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
name|len
operator|=
name|eol
condition|?
name|eol
operator|-
name|body
else|:
name|strlen
argument_list|(
name|body
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|" %.*s\n"
argument_list|,
operator|(
name|int
operator|)
name|len
argument_list|,
name|body
argument_list|)
expr_stmt|;
block|}
else|else
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
name|logmsg_free
argument_list|(
name|msg
argument_list|,
name|commit
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|update_index_from_diff
specifier|static
name|void
name|update_index_from_diff
parameter_list|(
name|struct
name|diff_queue_struct
modifier|*
name|q
parameter_list|,
name|struct
name|diff_options
modifier|*
name|opt
parameter_list|,
name|void
modifier|*
name|data
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
name|q
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|diff_filespec
modifier|*
name|one
init|=
name|q
operator|->
name|queue
index|[
name|i
index|]
operator|->
name|one
decl_stmt|;
if|if
condition|(
name|one
operator|->
name|mode
operator|&&
operator|!
name|is_null_sha1
argument_list|(
name|one
operator|->
name|sha1
argument_list|)
condition|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
name|ce
operator|=
name|make_cache_entry
argument_list|(
name|one
operator|->
name|mode
argument_list|,
name|one
operator|->
name|sha1
argument_list|,
name|one
operator|->
name|path
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ce
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"make_cache_entry failed for path '%s'"
argument_list|)
argument_list|,
name|one
operator|->
name|path
argument_list|)
expr_stmt|;
name|add_cache_entry
argument_list|(
name|ce
argument_list|,
name|ADD_CACHE_OK_TO_ADD
operator||
name|ADD_CACHE_OK_TO_REPLACE
argument_list|)
expr_stmt|;
block|}
else|else
name|remove_file_from_cache
argument_list|(
name|one
operator|->
name|path
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|read_from_tree
specifier|static
name|int
name|read_from_tree
parameter_list|(
specifier|const
name|struct
name|pathspec
modifier|*
name|pathspec
parameter_list|,
name|unsigned
name|char
modifier|*
name|tree_sha1
parameter_list|)
block|{
name|struct
name|diff_options
name|opt
decl_stmt|;
name|memset
argument_list|(
operator|&
name|opt
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|opt
argument_list|)
argument_list|)
expr_stmt|;
name|copy_pathspec
argument_list|(
operator|&
name|opt
operator|.
name|pathspec
argument_list|,
name|pathspec
argument_list|)
expr_stmt|;
name|opt
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_CALLBACK
expr_stmt|;
name|opt
operator|.
name|format_callback
operator|=
name|update_index_from_diff
expr_stmt|;
if|if
condition|(
name|do_diff_cache
argument_list|(
name|tree_sha1
argument_list|,
operator|&
name|opt
argument_list|)
condition|)
return|return
literal|1
return|;
name|diffcore_std
argument_list|(
operator|&
name|opt
argument_list|)
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|opt
argument_list|)
expr_stmt|;
name|free_pathspec
argument_list|(
operator|&
name|opt
operator|.
name|pathspec
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|set_reflog_message
specifier|static
name|void
name|set_reflog_message
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|action
parameter_list|,
specifier|const
name|char
modifier|*
name|rev
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|rla
init|=
name|getenv
argument_list|(
literal|"GIT_REFLOG_ACTION"
argument_list|)
decl_stmt|;
name|strbuf_reset
argument_list|(
name|sb
argument_list|)
expr_stmt|;
if|if
condition|(
name|rla
condition|)
name|strbuf_addf
argument_list|(
name|sb
argument_list|,
literal|"%s: %s"
argument_list|,
name|rla
argument_list|,
name|action
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|rev
condition|)
name|strbuf_addf
argument_list|(
name|sb
argument_list|,
literal|"reset: moving to %s"
argument_list|,
name|rev
argument_list|)
expr_stmt|;
else|else
name|strbuf_addf
argument_list|(
name|sb
argument_list|,
literal|"reset: %s"
argument_list|,
name|action
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|die_if_unmerged_cache
specifier|static
name|void
name|die_if_unmerged_cache
parameter_list|(
name|int
name|reset_type
parameter_list|)
block|{
if|if
condition|(
name|is_merge
argument_list|()
operator|||
name|unmerged_cache
argument_list|()
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Cannot do a %s reset in the middle of a merge."
argument_list|)
argument_list|,
name|_
argument_list|(
name|reset_type_names
index|[
name|reset_type
index|]
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|parse_args
specifier|static
name|void
name|parse_args
parameter_list|(
name|struct
name|pathspec
modifier|*
name|pathspec
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
parameter_list|,
name|int
name|patch_mode
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|rev_ret
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|rev
init|=
literal|"HEAD"
decl_stmt|;
name|unsigned
name|char
name|unused
index|[
literal|20
index|]
decl_stmt|;
comment|/* 	 * Possible arguments are: 	 * 	 * git reset [-opts] [<rev>] 	 * git reset [-opts]<tree> [<paths>...] 	 * git reset [-opts]<tree> -- [<paths>...] 	 * git reset [-opts] -- [<paths>...] 	 * git reset [-opts]<paths>... 	 * 	 * At this point, argv points immediately after [-opts]. 	 */
if|if
condition|(
name|argv
index|[
literal|0
index|]
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|argv
operator|++
expr_stmt|;
comment|/* reset to HEAD, possibly with paths */
block|}
elseif|else
if|if
condition|(
name|argv
index|[
literal|1
index|]
operator|&&
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|rev
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
name|argv
operator|+=
literal|2
expr_stmt|;
block|}
comment|/* 		 * Otherwise, argv[0] could be either<rev> or<paths> and 		 * has to be unambiguous. If there is a single argument, it 		 * can not be a tree 		 */
elseif|else
if|if
condition|(
operator|(
operator|!
name|argv
index|[
literal|1
index|]
operator|&&
operator|!
name|get_sha1_committish
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|unused
argument_list|)
operator|)
operator|||
operator|(
name|argv
index|[
literal|1
index|]
operator|&&
operator|!
name|get_sha1_treeish
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|,
name|unused
argument_list|)
operator|)
condition|)
block|{
comment|/* 			 * Ok, argv[0] looks like a commit/tree; it should not 			 * be a filename. 			 */
name|verify_non_filename
argument_list|(
name|prefix
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|rev
operator|=
operator|*
name|argv
operator|++
expr_stmt|;
block|}
else|else
block|{
comment|/* Otherwise we treat this as a filename */
name|verify_filename
argument_list|(
name|prefix
argument_list|,
name|argv
index|[
literal|0
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
block|}
operator|*
name|rev_ret
operator|=
name|rev
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
name|pathspec
argument_list|,
literal|0
argument_list|,
name|PATHSPEC_PREFER_FULL
operator||
name|PATHSPEC_STRIP_SUBMODULE_SLASH_CHEAP
operator||
operator|(
name|patch_mode
condition|?
name|PATHSPEC_PREFIX_ORIGIN
else|:
literal|0
operator|)
argument_list|,
name|prefix
argument_list|,
name|argv
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|reset_refs
specifier|static
name|int
name|reset_refs
parameter_list|(
specifier|const
name|char
modifier|*
name|rev
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|update_ref_status
decl_stmt|;
name|struct
name|strbuf
name|msg
init|=
name|STRBUF_INIT
decl_stmt|;
name|unsigned
name|char
modifier|*
name|orig
init|=
name|NULL
decl_stmt|,
name|sha1_orig
index|[
literal|20
index|]
decl_stmt|,
modifier|*
name|old_orig
init|=
name|NULL
decl_stmt|,
name|sha1_old_orig
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|get_sha1
argument_list|(
literal|"ORIG_HEAD"
argument_list|,
name|sha1_old_orig
argument_list|)
condition|)
name|old_orig
operator|=
name|sha1_old_orig
expr_stmt|;
if|if
condition|(
operator|!
name|get_sha1
argument_list|(
literal|"HEAD"
argument_list|,
name|sha1_orig
argument_list|)
condition|)
block|{
name|orig
operator|=
name|sha1_orig
expr_stmt|;
name|set_reflog_message
argument_list|(
operator|&
name|msg
argument_list|,
literal|"updating ORIG_HEAD"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|update_ref
argument_list|(
name|msg
operator|.
name|buf
argument_list|,
literal|"ORIG_HEAD"
argument_list|,
name|orig
argument_list|,
name|old_orig
argument_list|,
literal|0
argument_list|,
name|MSG_ON_ERR
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|old_orig
condition|)
name|delete_ref
argument_list|(
literal|"ORIG_HEAD"
argument_list|,
name|old_orig
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|set_reflog_message
argument_list|(
operator|&
name|msg
argument_list|,
literal|"updating HEAD"
argument_list|,
name|rev
argument_list|)
expr_stmt|;
name|update_ref_status
operator|=
name|update_ref
argument_list|(
name|msg
operator|.
name|buf
argument_list|,
literal|"HEAD"
argument_list|,
name|sha1
argument_list|,
name|orig
argument_list|,
literal|0
argument_list|,
name|MSG_ON_ERR
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|msg
argument_list|)
expr_stmt|;
return|return
name|update_ref_status
return|;
block|}
end_function
begin_function
DECL|function|cmd_reset
name|int
name|cmd_reset
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
name|reset_type
init|=
name|NONE
decl_stmt|,
name|update_ref_status
init|=
literal|0
decl_stmt|,
name|quiet
init|=
literal|0
decl_stmt|;
name|int
name|patch_mode
init|=
literal|0
decl_stmt|,
name|unborn
decl_stmt|;
specifier|const
name|char
modifier|*
name|rev
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|pathspec
name|pathspec
decl_stmt|;
specifier|const
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
argument_list|,
name|N_
argument_list|(
literal|"be quiet, only report errors"
argument_list|)
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"mixed"
argument_list|,
operator|&
name|reset_type
argument_list|,
name|N_
argument_list|(
literal|"reset HEAD and index"
argument_list|)
argument_list|,
name|MIXED
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"soft"
argument_list|,
operator|&
name|reset_type
argument_list|,
name|N_
argument_list|(
literal|"reset only HEAD"
argument_list|)
argument_list|,
name|SOFT
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"hard"
argument_list|,
operator|&
name|reset_type
argument_list|,
name|N_
argument_list|(
literal|"reset HEAD, index and working tree"
argument_list|)
argument_list|,
name|HARD
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"merge"
argument_list|,
operator|&
name|reset_type
argument_list|,
name|N_
argument_list|(
literal|"reset HEAD, index and working tree"
argument_list|)
argument_list|,
name|MERGE
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"keep"
argument_list|,
operator|&
name|reset_type
argument_list|,
name|N_
argument_list|(
literal|"reset HEAD but keep local changes"
argument_list|)
argument_list|,
name|KEEP
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|'p'
argument_list|,
literal|"patch"
argument_list|,
operator|&
name|patch_mode
argument_list|,
name|N_
argument_list|(
literal|"select hunks interactively"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
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
name|options
argument_list|,
name|git_reset_usage
argument_list|,
name|PARSE_OPT_KEEP_DASHDASH
argument_list|)
expr_stmt|;
name|parse_args
argument_list|(
operator|&
name|pathspec
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|patch_mode
argument_list|,
operator|&
name|rev
argument_list|)
expr_stmt|;
name|unborn
operator|=
operator|!
name|strcmp
argument_list|(
name|rev
argument_list|,
literal|"HEAD"
argument_list|)
operator|&&
name|get_sha1
argument_list|(
literal|"HEAD"
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|unborn
condition|)
block|{
comment|/* reset on unborn branch: treat as reset to empty tree */
name|hashcpy
argument_list|(
name|sha1
argument_list|,
name|EMPTY_TREE_SHA1_BIN
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|pathspec
operator|.
name|nr
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
if|if
condition|(
name|get_sha1_committish
argument_list|(
name|rev
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Failed to resolve '%s' as a valid revision."
argument_list|)
argument_list|,
name|rev
argument_list|)
expr_stmt|;
name|commit
operator|=
name|lookup_commit_reference
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Could not parse object '%s'."
argument_list|)
argument_list|,
name|rev
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|sha1
argument_list|,
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|struct
name|tree
modifier|*
name|tree
decl_stmt|;
if|if
condition|(
name|get_sha1_treeish
argument_list|(
name|rev
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Failed to resolve '%s' as a valid tree."
argument_list|)
argument_list|,
name|rev
argument_list|)
expr_stmt|;
name|tree
operator|=
name|parse_tree_indirect
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Could not parse object '%s'."
argument_list|)
argument_list|,
name|rev
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|sha1
argument_list|,
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|patch_mode
condition|)
block|{
if|if
condition|(
name|reset_type
operator|!=
name|NONE
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"--patch is incompatible with --{hard,mixed,soft}"
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|run_add_interactive
argument_list|(
name|rev
argument_list|,
literal|"--patch=reset"
argument_list|,
operator|&
name|pathspec
argument_list|)
return|;
block|}
comment|/* git reset tree [--] paths... can be used to 	 * load chosen paths from the tree into the index without 	 * affecting the working tree nor HEAD. */
if|if
condition|(
name|pathspec
operator|.
name|nr
condition|)
block|{
if|if
condition|(
name|reset_type
operator|==
name|MIXED
condition|)
name|warning
argument_list|(
name|_
argument_list|(
literal|"--mixed with paths is deprecated; use 'git reset --<paths>' instead."
argument_list|)
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|reset_type
operator|!=
name|NONE
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Cannot do %s reset with paths."
argument_list|)
argument_list|,
name|_
argument_list|(
name|reset_type_names
index|[
name|reset_type
index|]
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|reset_type
operator|==
name|NONE
condition|)
name|reset_type
operator|=
name|MIXED
expr_stmt|;
comment|/* by default */
if|if
condition|(
name|reset_type
operator|!=
name|SOFT
operator|&&
operator|(
name|reset_type
operator|!=
name|MIXED
operator|||
name|get_git_work_tree
argument_list|()
operator|)
condition|)
name|setup_work_tree
argument_list|()
expr_stmt|;
if|if
condition|(
name|reset_type
operator|==
name|MIXED
operator|&&
name|is_bare_repository
argument_list|()
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"%s reset is not allowed in a bare repository"
argument_list|)
argument_list|,
name|_
argument_list|(
name|reset_type_names
index|[
name|reset_type
index|]
argument_list|)
argument_list|)
expr_stmt|;
comment|/* Soft reset does not touch the index file nor the working tree 	 * at all, but requires them in a good order.  Other resets reset 	 * the index file to the tree object we are switching to. */
if|if
condition|(
name|reset_type
operator|==
name|SOFT
operator|||
name|reset_type
operator|==
name|KEEP
condition|)
name|die_if_unmerged_cache
argument_list|(
name|reset_type
argument_list|)
expr_stmt|;
if|if
condition|(
name|reset_type
operator|!=
name|SOFT
condition|)
block|{
name|struct
name|lock_file
modifier|*
name|lock
init|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|lock
argument_list|)
argument_list|)
decl_stmt|;
name|int
name|newfd
init|=
name|hold_locked_index
argument_list|(
name|lock
argument_list|,
literal|1
argument_list|)
decl_stmt|;
if|if
condition|(
name|reset_type
operator|==
name|MIXED
condition|)
block|{
name|int
name|flags
init|=
name|quiet
condition|?
name|REFRESH_QUIET
else|:
name|REFRESH_IN_PORCELAIN
decl_stmt|;
if|if
condition|(
name|read_from_tree
argument_list|(
operator|&
name|pathspec
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
literal|1
return|;
if|if
condition|(
name|get_git_work_tree
argument_list|()
condition|)
name|refresh_index
argument_list|(
operator|&
name|the_index
argument_list|,
name|flags
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
name|_
argument_list|(
literal|"Unstaged changes after reset:"
argument_list|)
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|int
name|err
init|=
name|reset_index
argument_list|(
name|sha1
argument_list|,
name|reset_type
argument_list|,
name|quiet
argument_list|)
decl_stmt|;
if|if
condition|(
name|reset_type
operator|==
name|KEEP
operator|&&
operator|!
name|err
condition|)
name|err
operator|=
name|reset_index
argument_list|(
name|sha1
argument_list|,
name|MIXED
argument_list|,
name|quiet
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Could not reset index file to revision '%s'."
argument_list|)
argument_list|,
name|rev
argument_list|)
expr_stmt|;
block|}
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
name|lock
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Could not write new index file."
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|pathspec
operator|.
name|nr
operator|&&
operator|!
name|unborn
condition|)
block|{
comment|/* Any resets without paths update HEAD to the head being 		 * switched to, saving the previous head in ORIG_HEAD before. */
name|update_ref_status
operator|=
name|reset_refs
argument_list|(
name|rev
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|reset_type
operator|==
name|HARD
operator|&&
operator|!
name|update_ref_status
operator|&&
operator|!
name|quiet
condition|)
name|print_new_head_line
argument_list|(
name|lookup_commit_reference
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|pathspec
operator|.
name|nr
condition|)
name|remove_branch_state
argument_list|()
expr_stmt|;
return|return
name|update_ref_status
return|;
block|}
end_function
end_unit
