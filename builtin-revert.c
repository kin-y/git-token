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
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"wt-status.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_include
include|#
directive|include
file|"utf8.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"cache-tree.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"rerere.h"
end_include
begin_include
include|#
directive|include
file|"merge-recursive.h"
end_include
begin_comment
comment|/*  * This implements the builtins revert and cherry-pick.  *  * Copyright (c) 2007 Johannes E. Schindelin  *  * Based on git-revert.sh, which is  *  * Copyright (c) 2005 Linus Torvalds  * Copyright (c) 2005 Junio C Hamano  */
end_comment
begin_decl_stmt
DECL|variable|revert_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|revert_usage
index|[]
init|=
block|{
literal|"git revert [options]<commit-ish>"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|cherry_pick_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|cherry_pick_usage
index|[]
init|=
block|{
literal|"git cherry-pick [options]<commit-ish>"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|edit
DECL|variable|no_replay
DECL|variable|no_commit
DECL|variable|mainline
DECL|variable|signoff
specifier|static
name|int
name|edit
decl_stmt|,
name|no_replay
decl_stmt|,
name|no_commit
decl_stmt|,
name|mainline
decl_stmt|,
name|signoff
decl_stmt|;
end_decl_stmt
begin_enum
DECL|enumerator|REVERT
DECL|enumerator|CHERRY_PICK
DECL|variable|action
specifier|static
enum|enum
block|{
name|REVERT
block|,
name|CHERRY_PICK
block|}
name|action
enum|;
end_enum
begin_decl_stmt
DECL|variable|commit
specifier|static
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|me
specifier|static
specifier|const
name|char
modifier|*
name|me
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|GIT_REFLOG_ACTION
define|#
directive|define
name|GIT_REFLOG_ACTION
value|"GIT_REFLOG_ACTION"
end_define
begin_function
DECL|function|parse_args
specifier|static
name|void
name|parse_args
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
specifier|const
name|char
modifier|*
specifier|const
modifier|*
name|usage_str
init|=
name|action
operator|==
name|REVERT
condition|?
name|revert_usage
else|:
name|cherry_pick_usage
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|arg
decl_stmt|;
name|int
name|noop
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_BOOLEAN
argument_list|(
literal|'n'
argument_list|,
literal|"no-commit"
argument_list|,
operator|&
name|no_commit
argument_list|,
literal|"don't automatically commit"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'e'
argument_list|,
literal|"edit"
argument_list|,
operator|&
name|edit
argument_list|,
literal|"edit the commit message"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'x'
argument_list|,
name|NULL
argument_list|,
operator|&
name|no_replay
argument_list|,
literal|"append commit name when cherry-picking"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'r'
argument_list|,
name|NULL
argument_list|,
operator|&
name|noop
argument_list|,
literal|"no-op (backward compatibility)"
argument_list|)
block|,
name|OPT_BOOLEAN
argument_list|(
literal|'s'
argument_list|,
literal|"signoff"
argument_list|,
operator|&
name|signoff
argument_list|,
literal|"add Signed-off-by:"
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|'m'
argument_list|,
literal|"mainline"
argument_list|,
operator|&
name|mainline
argument_list|,
literal|"parent number"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
if|if
condition|(
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|options
argument_list|,
name|usage_str
argument_list|,
literal|0
argument_list|)
operator|!=
literal|1
condition|)
name|usage_with_options
argument_list|(
name|usage_str
argument_list|,
name|options
argument_list|)
expr_stmt|;
name|arg
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Cannot find '%s'"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
name|commit
operator|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|parse_object
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
literal|"Could not find %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|type
operator|==
name|OBJ_TAG
condition|)
block|{
name|commit
operator|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|deref_tag
argument_list|(
operator|(
expr|struct
name|object
operator|*
operator|)
name|commit
argument_list|,
name|arg
argument_list|,
name|strlen
argument_list|(
name|arg
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|type
operator|!=
name|OBJ_COMMIT
condition|)
name|die
argument_list|(
literal|"'%s' does not point to a commit"
argument_list|,
name|arg
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_oneline
specifier|static
name|char
modifier|*
name|get_oneline
parameter_list|(
specifier|const
name|char
modifier|*
name|message
parameter_list|)
block|{
name|char
modifier|*
name|result
decl_stmt|;
specifier|const
name|char
modifier|*
name|p
init|=
name|message
decl_stmt|,
modifier|*
name|abbrev
decl_stmt|,
modifier|*
name|eol
decl_stmt|;
name|int
name|abbrev_len
decl_stmt|,
name|oneline_len
decl_stmt|;
if|if
condition|(
operator|!
name|p
condition|)
name|die
argument_list|(
literal|"Could not read commit message of %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
while|while
condition|(
operator|*
name|p
operator|&&
operator|(
operator|*
name|p
operator|!=
literal|'\n'
operator|||
name|p
index|[
literal|1
index|]
operator|!=
literal|'\n'
operator|)
condition|)
name|p
operator|++
expr_stmt|;
if|if
condition|(
operator|*
name|p
condition|)
block|{
name|p
operator|+=
literal|2
expr_stmt|;
for|for
control|(
name|eol
operator|=
name|p
operator|+
literal|1
init|;
operator|*
name|eol
operator|&&
operator|*
name|eol
operator|!=
literal|'\n'
condition|;
name|eol
operator|++
control|)
empty_stmt|;
comment|/* do nothing */
block|}
else|else
name|eol
operator|=
name|p
expr_stmt|;
name|abbrev
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
name|abbrev_len
operator|=
name|strlen
argument_list|(
name|abbrev
argument_list|)
expr_stmt|;
name|oneline_len
operator|=
name|eol
operator|-
name|p
expr_stmt|;
name|result
operator|=
name|xmalloc
argument_list|(
name|abbrev_len
operator|+
literal|5
operator|+
name|oneline_len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|result
argument_list|,
name|abbrev
argument_list|,
name|abbrev_len
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|result
operator|+
name|abbrev_len
argument_list|,
literal|"... "
argument_list|,
literal|4
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|result
operator|+
name|abbrev_len
operator|+
literal|4
argument_list|,
name|p
argument_list|,
name|oneline_len
argument_list|)
expr_stmt|;
name|result
index|[
name|abbrev_len
operator|+
literal|4
operator|+
name|oneline_len
index|]
operator|=
literal|'\0'
expr_stmt|;
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|get_encoding
specifier|static
name|char
modifier|*
name|get_encoding
parameter_list|(
specifier|const
name|char
modifier|*
name|message
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|p
init|=
name|message
decl_stmt|,
modifier|*
name|eol
decl_stmt|;
if|if
condition|(
operator|!
name|p
condition|)
name|die
argument_list|(
literal|"Could not read commit message of %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
while|while
condition|(
operator|*
name|p
operator|&&
operator|*
name|p
operator|!=
literal|'\n'
condition|)
block|{
for|for
control|(
name|eol
operator|=
name|p
operator|+
literal|1
init|;
operator|*
name|eol
operator|&&
operator|*
name|eol
operator|!=
literal|'\n'
condition|;
name|eol
operator|++
control|)
empty_stmt|;
comment|/* do nothing */
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|p
argument_list|,
literal|"encoding "
argument_list|)
condition|)
block|{
name|char
modifier|*
name|result
init|=
name|xmalloc
argument_list|(
name|eol
operator|-
literal|8
operator|-
name|p
argument_list|)
decl_stmt|;
name|strlcpy
argument_list|(
name|result
argument_list|,
name|p
operator|+
literal|9
argument_list|,
name|eol
operator|-
literal|8
operator|-
name|p
argument_list|)
expr_stmt|;
return|return
name|result
return|;
block|}
name|p
operator|=
name|eol
expr_stmt|;
if|if
condition|(
operator|*
name|p
operator|==
literal|'\n'
condition|)
name|p
operator|++
expr_stmt|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|msg_file
specifier|static
name|struct
name|lock_file
name|msg_file
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|msg_fd
specifier|static
name|int
name|msg_fd
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_to_msg
specifier|static
name|void
name|add_to_msg
parameter_list|(
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|string
argument_list|)
decl_stmt|;
if|if
condition|(
name|write_in_full
argument_list|(
name|msg_fd
argument_list|,
name|string
argument_list|,
name|len
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"Could not write to MERGE_MSG"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_message_to_msg
specifier|static
name|void
name|add_message_to_msg
parameter_list|(
specifier|const
name|char
modifier|*
name|message
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|p
init|=
name|message
decl_stmt|;
while|while
condition|(
operator|*
name|p
operator|&&
operator|(
operator|*
name|p
operator|!=
literal|'\n'
operator|||
name|p
index|[
literal|1
index|]
operator|!=
literal|'\n'
operator|)
condition|)
name|p
operator|++
expr_stmt|;
if|if
condition|(
operator|!
operator|*
name|p
condition|)
name|add_to_msg
argument_list|(
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|p
operator|+=
literal|2
expr_stmt|;
name|add_to_msg
argument_list|(
name|p
argument_list|)
expr_stmt|;
return|return;
block|}
end_function
begin_function
DECL|function|set_author_ident_env
specifier|static
name|void
name|set_author_ident_env
parameter_list|(
specifier|const
name|char
modifier|*
name|message
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|p
init|=
name|message
decl_stmt|;
if|if
condition|(
operator|!
name|p
condition|)
name|die
argument_list|(
literal|"Could not read commit message of %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
while|while
condition|(
operator|*
name|p
operator|&&
operator|*
name|p
operator|!=
literal|'\n'
condition|)
block|{
specifier|const
name|char
modifier|*
name|eol
decl_stmt|;
for|for
control|(
name|eol
operator|=
name|p
init|;
operator|*
name|eol
operator|&&
operator|*
name|eol
operator|!=
literal|'\n'
condition|;
name|eol
operator|++
control|)
empty_stmt|;
comment|/* do nothing */
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|p
argument_list|,
literal|"author "
argument_list|)
condition|)
block|{
name|char
modifier|*
name|line
decl_stmt|,
modifier|*
name|pend
decl_stmt|,
modifier|*
name|email
decl_stmt|,
modifier|*
name|timestamp
decl_stmt|;
name|p
operator|+=
literal|7
expr_stmt|;
name|line
operator|=
name|xmemdupz
argument_list|(
name|p
argument_list|,
name|eol
operator|-
name|p
argument_list|)
expr_stmt|;
name|email
operator|=
name|strchr
argument_list|(
name|line
argument_list|,
literal|'<'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|email
condition|)
name|die
argument_list|(
literal|"Could not extract author email from %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|email
operator|==
name|line
condition|)
name|pend
operator|=
name|line
expr_stmt|;
else|else
for|for
control|(
name|pend
operator|=
name|email
init|;
name|pend
operator|!=
name|line
operator|+
literal|1
operator|&&
name|isspace
argument_list|(
name|pend
index|[
operator|-
literal|1
index|]
argument_list|)
condition|;
name|pend
operator|--
control|)
empty_stmt|;
empty_stmt|;
comment|/* do nothing */
operator|*
name|pend
operator|=
literal|'\0'
expr_stmt|;
name|email
operator|++
expr_stmt|;
name|timestamp
operator|=
name|strchr
argument_list|(
name|email
argument_list|,
literal|'>'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|timestamp
condition|)
name|die
argument_list|(
literal|"Could not extract author time from %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
operator|*
name|timestamp
operator|=
literal|'\0'
expr_stmt|;
for|for
control|(
name|timestamp
operator|++
init|;
operator|*
name|timestamp
operator|&&
name|isspace
argument_list|(
operator|*
name|timestamp
argument_list|)
condition|;
name|timestamp
operator|++
control|)
empty_stmt|;
comment|/* do nothing */
name|setenv
argument_list|(
literal|"GIT_AUTHOR_NAME"
argument_list|,
name|line
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|setenv
argument_list|(
literal|"GIT_AUTHOR_EMAIL"
argument_list|,
name|email
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|setenv
argument_list|(
literal|"GIT_AUTHOR_DATE"
argument_list|,
name|timestamp
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|line
argument_list|)
expr_stmt|;
return|return;
block|}
name|p
operator|=
name|eol
expr_stmt|;
if|if
condition|(
operator|*
name|p
operator|==
literal|'\n'
condition|)
name|p
operator|++
expr_stmt|;
block|}
name|die
argument_list|(
literal|"No author information found in %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|help_msg
specifier|static
name|char
modifier|*
name|help_msg
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
specifier|static
name|char
name|helpbuf
index|[
literal|1024
index|]
decl_stmt|;
name|char
modifier|*
name|msg
init|=
name|getenv
argument_list|(
literal|"GIT_CHERRY_PICK_HELP"
argument_list|)
decl_stmt|;
if|if
condition|(
name|msg
condition|)
return|return
name|msg
return|;
name|strcpy
argument_list|(
name|helpbuf
argument_list|,
literal|"  After resolving the conflicts,\n"
literal|"mark the corrected paths with 'git add<paths>' "
literal|"or 'git rm<paths>' and commit the result."
argument_list|)
expr_stmt|;
if|if
condition|(
name|action
operator|==
name|CHERRY_PICK
condition|)
block|{
name|sprintf
argument_list|(
name|helpbuf
operator|+
name|strlen
argument_list|(
name|helpbuf
argument_list|)
argument_list|,
literal|"\nWhen commiting, use the option "
literal|"'-c %s' to retain authorship and message."
argument_list|,
name|find_unique_abbrev
argument_list|(
name|sha1
argument_list|,
name|DEFAULT_ABBREV
argument_list|)
argument_list|)
expr_stmt|;
block|}
return|return
name|helpbuf
return|;
block|}
end_function
begin_function
DECL|function|empty_tree
specifier|static
name|struct
name|tree
modifier|*
name|empty_tree
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|tree
modifier|*
name|tree
init|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|tree
argument_list|)
argument_list|)
decl_stmt|;
name|tree
operator|->
name|object
operator|.
name|parsed
operator|=
literal|1
expr_stmt|;
name|tree
operator|->
name|object
operator|.
name|type
operator|=
name|OBJ_TREE
expr_stmt|;
name|pretend_sha1_file
argument_list|(
name|NULL
argument_list|,
literal|0
argument_list|,
name|OBJ_TREE
argument_list|,
name|tree
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
return|return
name|tree
return|;
block|}
end_function
begin_function
DECL|function|revert_or_cherry_pick
specifier|static
name|int
name|revert_or_cherry_pick
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
name|unsigned
name|char
name|head
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|commit
modifier|*
name|base
decl_stmt|,
modifier|*
name|next
decl_stmt|,
modifier|*
name|parent
decl_stmt|;
name|int
name|i
decl_stmt|,
name|index_fd
decl_stmt|,
name|clean
decl_stmt|;
name|char
modifier|*
name|oneline
decl_stmt|,
modifier|*
name|reencoded_message
init|=
name|NULL
decl_stmt|;
specifier|const
name|char
modifier|*
name|message
decl_stmt|,
modifier|*
name|encoding
decl_stmt|;
name|char
modifier|*
name|defmsg
init|=
name|git_pathdup
argument_list|(
literal|"MERGE_MSG"
argument_list|)
decl_stmt|;
name|struct
name|merge_options
name|o
decl_stmt|;
name|struct
name|tree
modifier|*
name|result
decl_stmt|,
modifier|*
name|next_tree
decl_stmt|,
modifier|*
name|base_tree
decl_stmt|,
modifier|*
name|head_tree
decl_stmt|;
specifier|static
name|struct
name|lock_file
name|index_lock
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|me
operator|=
name|action
operator|==
name|REVERT
condition|?
literal|"revert"
else|:
literal|"cherry-pick"
expr_stmt|;
name|setenv
argument_list|(
name|GIT_REFLOG_ACTION
argument_list|,
name|me
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|parse_args
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|)
expr_stmt|;
comment|/* this is copied from the shell script, but it's never triggered... */
if|if
condition|(
name|action
operator|==
name|REVERT
operator|&&
operator|!
name|no_replay
condition|)
name|die
argument_list|(
literal|"revert is incompatible with replay"
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
literal|"git %s: failed to read the index"
argument_list|,
name|me
argument_list|)
expr_stmt|;
if|if
condition|(
name|no_commit
condition|)
block|{
comment|/* 		 * We do not intend to commit immediately.  We just want to 		 * merge the differences in, so let's compute the tree 		 * that represents the "current" state for merge-recursive 		 * to work on. 		 */
if|if
condition|(
name|write_cache_as_tree
argument_list|(
name|head
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|)
condition|)
name|die
argument_list|(
literal|"Your index file is unmerged."
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|get_sha1
argument_list|(
literal|"HEAD"
argument_list|,
name|head
argument_list|)
condition|)
name|die
argument_list|(
literal|"You do not have a valid HEAD"
argument_list|)
expr_stmt|;
if|if
condition|(
name|index_differs_from
argument_list|(
literal|"HEAD"
argument_list|,
literal|0
argument_list|)
condition|)
name|die
argument_list|(
literal|"Dirty index: cannot %s"
argument_list|,
name|me
argument_list|)
expr_stmt|;
block|}
name|discard_cache
argument_list|()
expr_stmt|;
name|index_fd
operator|=
name|hold_locked_index
argument_list|(
operator|&
name|index_lock
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
operator|->
name|parents
condition|)
block|{
if|if
condition|(
name|action
operator|==
name|REVERT
condition|)
name|die
argument_list|(
literal|"Cannot revert a root commit"
argument_list|)
expr_stmt|;
name|parent
operator|=
name|NULL
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|commit
operator|->
name|parents
operator|->
name|next
condition|)
block|{
comment|/* Reverting or cherry-picking a merge commit */
name|int
name|cnt
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
if|if
condition|(
operator|!
name|mainline
condition|)
name|die
argument_list|(
literal|"Commit %s is a merge but no -m option was given."
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
for|for
control|(
name|cnt
operator|=
literal|1
operator|,
name|p
operator|=
name|commit
operator|->
name|parents
init|;
name|cnt
operator|!=
name|mainline
operator|&&
name|p
condition|;
name|cnt
operator|++
control|)
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
if|if
condition|(
name|cnt
operator|!=
name|mainline
operator|||
operator|!
name|p
condition|)
name|die
argument_list|(
literal|"Commit %s does not have parent %d"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|,
name|mainline
argument_list|)
expr_stmt|;
name|parent
operator|=
name|p
operator|->
name|item
expr_stmt|;
block|}
elseif|else
if|if
condition|(
literal|0
operator|<
name|mainline
condition|)
name|die
argument_list|(
literal|"Mainline was specified but commit %s is not a merge."
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|parent
operator|=
name|commit
operator|->
name|parents
operator|->
name|item
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|message
operator|=
name|commit
operator|->
name|buffer
operator|)
condition|)
name|die
argument_list|(
literal|"Cannot get commit message for %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|parent
operator|&&
name|parse_commit
argument_list|(
name|parent
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"%s: cannot parse parent commit %s"
argument_list|,
name|me
argument_list|,
name|sha1_to_hex
argument_list|(
name|parent
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
comment|/* 	 * "commit" is an existing commit.  We would want to apply 	 * the difference it introduces since its first parent "prev" 	 * on top of the current HEAD if we are cherry-pick.  Or the 	 * reverse of it if we are revert. 	 */
name|msg_fd
operator|=
name|hold_lock_file_for_update
argument_list|(
operator|&
name|msg_file
argument_list|,
name|defmsg
argument_list|,
name|LOCK_DIE_ON_ERROR
argument_list|)
expr_stmt|;
name|encoding
operator|=
name|get_encoding
argument_list|(
name|message
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|encoding
condition|)
name|encoding
operator|=
literal|"UTF-8"
expr_stmt|;
if|if
condition|(
operator|!
name|git_commit_encoding
condition|)
name|git_commit_encoding
operator|=
literal|"UTF-8"
expr_stmt|;
if|if
condition|(
operator|(
name|reencoded_message
operator|=
name|reencode_string
argument_list|(
name|message
argument_list|,
name|git_commit_encoding
argument_list|,
name|encoding
argument_list|)
operator|)
condition|)
name|message
operator|=
name|reencoded_message
expr_stmt|;
name|oneline
operator|=
name|get_oneline
argument_list|(
name|message
argument_list|)
expr_stmt|;
if|if
condition|(
name|action
operator|==
name|REVERT
condition|)
block|{
name|char
modifier|*
name|oneline_body
init|=
name|strchr
argument_list|(
name|oneline
argument_list|,
literal|' '
argument_list|)
decl_stmt|;
name|base
operator|=
name|commit
expr_stmt|;
name|next
operator|=
name|parent
expr_stmt|;
name|add_to_msg
argument_list|(
literal|"Revert \""
argument_list|)
expr_stmt|;
name|add_to_msg
argument_list|(
name|oneline_body
operator|+
literal|1
argument_list|)
expr_stmt|;
name|add_to_msg
argument_list|(
literal|"\"\n\nThis reverts commit "
argument_list|)
expr_stmt|;
name|add_to_msg
argument_list|(
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit
operator|->
name|parents
operator|->
name|next
condition|)
block|{
name|add_to_msg
argument_list|(
literal|", reversing\nchanges made to "
argument_list|)
expr_stmt|;
name|add_to_msg
argument_list|(
name|sha1_to_hex
argument_list|(
name|parent
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|add_to_msg
argument_list|(
literal|".\n"
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|base
operator|=
name|parent
expr_stmt|;
name|next
operator|=
name|commit
expr_stmt|;
name|set_author_ident_env
argument_list|(
name|message
argument_list|)
expr_stmt|;
name|add_message_to_msg
argument_list|(
name|message
argument_list|)
expr_stmt|;
if|if
condition|(
name|no_replay
condition|)
block|{
name|add_to_msg
argument_list|(
literal|"(cherry picked from commit "
argument_list|)
expr_stmt|;
name|add_to_msg
argument_list|(
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|add_to_msg
argument_list|(
literal|")\n"
argument_list|)
expr_stmt|;
block|}
block|}
name|read_cache
argument_list|()
expr_stmt|;
name|init_merge_options
argument_list|(
operator|&
name|o
argument_list|)
expr_stmt|;
name|o
operator|.
name|branch1
operator|=
literal|"HEAD"
expr_stmt|;
name|o
operator|.
name|branch2
operator|=
name|oneline
expr_stmt|;
name|head_tree
operator|=
name|parse_tree_indirect
argument_list|(
name|head
argument_list|)
expr_stmt|;
name|next_tree
operator|=
name|next
condition|?
name|next
operator|->
name|tree
else|:
name|empty_tree
argument_list|()
expr_stmt|;
name|base_tree
operator|=
name|base
condition|?
name|base
operator|->
name|tree
else|:
name|empty_tree
argument_list|()
expr_stmt|;
name|clean
operator|=
name|merge_trees
argument_list|(
operator|&
name|o
argument_list|,
name|head_tree
argument_list|,
name|next_tree
argument_list|,
name|base_tree
argument_list|,
operator|&
name|result
argument_list|)
expr_stmt|;
if|if
condition|(
name|active_cache_changed
operator|&&
operator|(
name|write_cache
argument_list|(
name|index_fd
argument_list|,
name|active_cache
argument_list|,
name|active_nr
argument_list|)
operator|||
name|commit_locked_index
argument_list|(
operator|&
name|index_lock
argument_list|)
operator|)
condition|)
name|die
argument_list|(
literal|"%s: Unable to write new index file"
argument_list|,
name|me
argument_list|)
expr_stmt|;
name|rollback_lock_file
argument_list|(
operator|&
name|index_lock
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|clean
condition|)
block|{
name|add_to_msg
argument_list|(
literal|"\nConflicts:\n\n"
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
control|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
init|=
name|active_cache
index|[
name|i
operator|++
index|]
decl_stmt|;
if|if
condition|(
name|ce_stage
argument_list|(
name|ce
argument_list|)
condition|)
block|{
name|add_to_msg
argument_list|(
literal|"\t"
argument_list|)
expr_stmt|;
name|add_to_msg
argument_list|(
name|ce
operator|->
name|name
argument_list|)
expr_stmt|;
name|add_to_msg
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|active_nr
operator|&&
operator|!
name|strcmp
argument_list|(
name|ce
operator|->
name|name
argument_list|,
name|active_cache
index|[
name|i
index|]
operator|->
name|name
argument_list|)
condition|)
name|i
operator|++
expr_stmt|;
block|}
block|}
if|if
condition|(
name|commit_lock_file
argument_list|(
operator|&
name|msg_file
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"Error wrapping up %s"
argument_list|,
name|defmsg
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Automatic %s failed.%s\n"
argument_list|,
name|me
argument_list|,
name|help_msg
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|rerere
argument_list|()
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|commit_lock_file
argument_list|(
operator|&
name|msg_file
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"Error wrapping up %s"
argument_list|,
name|defmsg
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Finished one %s.\n"
argument_list|,
name|me
argument_list|)
expr_stmt|;
comment|/* 	 * 	 * If we are cherry-pick, and if the merge did not result in 	 * hand-editing, we will hit this commit and inherit the original 	 * author date and name. 	 * If we are revert, or if our cherry-pick results in a hand merge, 	 * we had better say that the current user is responsible for that. 	 */
if|if
condition|(
operator|!
name|no_commit
condition|)
block|{
comment|/* 6 is max possible length of our args array including NULL */
specifier|const
name|char
modifier|*
name|args
index|[
literal|6
index|]
decl_stmt|;
name|int
name|i
init|=
literal|0
decl_stmt|;
name|args
index|[
name|i
operator|++
index|]
operator|=
literal|"commit"
expr_stmt|;
name|args
index|[
name|i
operator|++
index|]
operator|=
literal|"-n"
expr_stmt|;
if|if
condition|(
name|signoff
condition|)
name|args
index|[
name|i
operator|++
index|]
operator|=
literal|"-s"
expr_stmt|;
if|if
condition|(
operator|!
name|edit
condition|)
block|{
name|args
index|[
name|i
operator|++
index|]
operator|=
literal|"-F"
expr_stmt|;
name|args
index|[
name|i
operator|++
index|]
operator|=
name|defmsg
expr_stmt|;
block|}
name|args
index|[
name|i
index|]
operator|=
name|NULL
expr_stmt|;
return|return
name|execv_git_cmd
argument_list|(
name|args
argument_list|)
return|;
block|}
name|free
argument_list|(
name|reencoded_message
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|defmsg
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|cmd_revert
name|int
name|cmd_revert
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
if|if
condition|(
name|isatty
argument_list|(
literal|0
argument_list|)
condition|)
name|edit
operator|=
literal|1
expr_stmt|;
name|no_replay
operator|=
literal|1
expr_stmt|;
name|action
operator|=
name|REVERT
expr_stmt|;
return|return
name|revert_or_cherry_pick
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|cmd_cherry_pick
name|int
name|cmd_cherry_pick
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
name|no_replay
operator|=
literal|0
expr_stmt|;
name|action
operator|=
name|CHERRY_PICK
expr_stmt|;
return|return
name|revert_or_cherry_pick
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|)
return|;
block|}
end_function
end_unit
