begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"notes-utils.h"
end_include
begin_function
DECL|function|create_notes_commit
name|void
name|create_notes_commit
parameter_list|(
name|struct
name|notes_tree
modifier|*
name|t
parameter_list|,
name|struct
name|commit_list
modifier|*
name|parents
parameter_list|,
specifier|const
name|struct
name|strbuf
modifier|*
name|msg
parameter_list|,
name|unsigned
name|char
modifier|*
name|result_sha1
parameter_list|)
block|{
name|unsigned
name|char
name|tree_sha1
index|[
literal|20
index|]
decl_stmt|;
name|assert
argument_list|(
name|t
operator|->
name|initialized
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_notes_tree
argument_list|(
name|t
argument_list|,
name|tree_sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"Failed to write notes tree to database"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|parents
condition|)
block|{
comment|/* Deduce parent commit from t->ref */
name|unsigned
name|char
name|parent_sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|read_ref
argument_list|(
name|t
operator|->
name|ref
argument_list|,
name|parent_sha1
argument_list|)
condition|)
block|{
name|struct
name|commit
modifier|*
name|parent
init|=
name|lookup_commit
argument_list|(
name|parent_sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|parent
operator|||
name|parse_commit
argument_list|(
name|parent
argument_list|)
condition|)
name|die
argument_list|(
literal|"Failed to find/parse commit %s"
argument_list|,
name|t
operator|->
name|ref
argument_list|)
expr_stmt|;
name|commit_list_insert
argument_list|(
name|parent
argument_list|,
operator|&
name|parents
argument_list|)
expr_stmt|;
block|}
comment|/* else: t->ref points to nothing, assume root/orphan commit */
block|}
if|if
condition|(
name|commit_tree
argument_list|(
name|msg
argument_list|,
name|tree_sha1
argument_list|,
name|parents
argument_list|,
name|result_sha1
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
condition|)
name|die
argument_list|(
literal|"Failed to commit notes tree to database"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|commit_notes
name|void
name|commit_notes
parameter_list|(
name|struct
name|notes_tree
modifier|*
name|t
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|unsigned
name|char
name|commit_sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|t
condition|)
name|t
operator|=
operator|&
name|default_notes_tree
expr_stmt|;
if|if
condition|(
operator|!
name|t
operator|->
name|initialized
operator|||
operator|!
name|t
operator|->
name|ref
operator|||
operator|!
operator|*
name|t
operator|->
name|ref
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Cannot commit uninitialized/unreferenced notes tree"
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|t
operator|->
name|dirty
condition|)
return|return;
comment|/* don't have to commit an unchanged tree */
comment|/* Prepare commit message and reflog message */
name|strbuf_addstr
argument_list|(
operator|&
name|buf
argument_list|,
name|msg
argument_list|)
expr_stmt|;
if|if
condition|(
name|buf
operator|.
name|buf
index|[
name|buf
operator|.
name|len
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
name|strbuf_addch
argument_list|(
operator|&
name|buf
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
comment|/* Make sure msg ends with newline */
name|create_notes_commit
argument_list|(
name|t
argument_list|,
name|NULL
argument_list|,
operator|&
name|buf
argument_list|,
name|commit_sha1
argument_list|)
expr_stmt|;
name|strbuf_insert
argument_list|(
operator|&
name|buf
argument_list|,
literal|0
argument_list|,
literal|"notes: "
argument_list|,
literal|7
argument_list|)
expr_stmt|;
comment|/* commit message starts at index 7 */
name|update_ref
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
name|t
operator|->
name|ref
argument_list|,
name|commit_sha1
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
name|DIE_ON_ERR
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
DECL|function|parse_combine_notes_fn
specifier|static
name|combine_notes_fn
name|parse_combine_notes_fn
parameter_list|(
specifier|const
name|char
modifier|*
name|v
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|v
argument_list|,
literal|"overwrite"
argument_list|)
condition|)
return|return
name|combine_notes_overwrite
return|;
elseif|else
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|v
argument_list|,
literal|"ignore"
argument_list|)
condition|)
return|return
name|combine_notes_ignore
return|;
elseif|else
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|v
argument_list|,
literal|"concatenate"
argument_list|)
condition|)
return|return
name|combine_notes_concatenate
return|;
elseif|else
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|v
argument_list|,
literal|"cat_sort_uniq"
argument_list|)
condition|)
return|return
name|combine_notes_cat_sort_uniq
return|;
else|else
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|notes_rewrite_config
specifier|static
name|int
name|notes_rewrite_config
parameter_list|(
specifier|const
name|char
modifier|*
name|k
parameter_list|,
specifier|const
name|char
modifier|*
name|v
parameter_list|,
name|void
modifier|*
name|cb
parameter_list|)
block|{
name|struct
name|notes_rewrite_cfg
modifier|*
name|c
init|=
name|cb
decl_stmt|;
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|k
argument_list|,
literal|"notes.rewrite."
argument_list|)
operator|&&
operator|!
name|strcmp
argument_list|(
name|k
operator|+
literal|14
argument_list|,
name|c
operator|->
name|cmd
argument_list|)
condition|)
block|{
name|c
operator|->
name|enabled
operator|=
name|git_config_bool
argument_list|(
name|k
argument_list|,
name|v
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
elseif|else
if|if
condition|(
operator|!
name|c
operator|->
name|mode_from_env
operator|&&
operator|!
name|strcmp
argument_list|(
name|k
argument_list|,
literal|"notes.rewritemode"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|v
condition|)
name|config_error_nonbool
argument_list|(
name|k
argument_list|)
expr_stmt|;
name|c
operator|->
name|combine
operator|=
name|parse_combine_notes_fn
argument_list|(
name|v
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|c
operator|->
name|combine
condition|)
block|{
name|error
argument_list|(
name|_
argument_list|(
literal|"Bad notes.rewriteMode value: '%s'"
argument_list|)
argument_list|,
name|v
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
elseif|else
if|if
condition|(
operator|!
name|c
operator|->
name|refs_from_env
operator|&&
operator|!
name|strcmp
argument_list|(
name|k
argument_list|,
literal|"notes.rewriteref"
argument_list|)
condition|)
block|{
comment|/* note that a refs/ prefix is implied in the 		 * underlying for_each_glob_ref */
if|if
condition|(
operator|!
name|prefixcmp
argument_list|(
name|v
argument_list|,
literal|"refs/notes/"
argument_list|)
condition|)
name|string_list_add_refs_by_glob
argument_list|(
name|c
operator|->
name|refs
argument_list|,
name|v
argument_list|)
expr_stmt|;
else|else
name|warning
argument_list|(
name|_
argument_list|(
literal|"Refusing to rewrite notes in %s"
literal|" (outside of refs/notes/)"
argument_list|)
argument_list|,
name|v
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|init_copy_notes_for_rewrite
name|struct
name|notes_rewrite_cfg
modifier|*
name|init_copy_notes_for_rewrite
parameter_list|(
specifier|const
name|char
modifier|*
name|cmd
parameter_list|)
block|{
name|struct
name|notes_rewrite_cfg
modifier|*
name|c
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|notes_rewrite_cfg
argument_list|)
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|rewrite_mode_env
init|=
name|getenv
argument_list|(
name|GIT_NOTES_REWRITE_MODE_ENVIRONMENT
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|rewrite_refs_env
init|=
name|getenv
argument_list|(
name|GIT_NOTES_REWRITE_REF_ENVIRONMENT
argument_list|)
decl_stmt|;
name|c
operator|->
name|cmd
operator|=
name|cmd
expr_stmt|;
name|c
operator|->
name|enabled
operator|=
literal|1
expr_stmt|;
name|c
operator|->
name|combine
operator|=
name|combine_notes_concatenate
expr_stmt|;
name|c
operator|->
name|refs
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|string_list
argument_list|)
argument_list|)
expr_stmt|;
name|c
operator|->
name|refs
operator|->
name|strdup_strings
operator|=
literal|1
expr_stmt|;
name|c
operator|->
name|refs_from_env
operator|=
literal|0
expr_stmt|;
name|c
operator|->
name|mode_from_env
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|rewrite_mode_env
condition|)
block|{
name|c
operator|->
name|mode_from_env
operator|=
literal|1
expr_stmt|;
name|c
operator|->
name|combine
operator|=
name|parse_combine_notes_fn
argument_list|(
name|rewrite_mode_env
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|c
operator|->
name|combine
condition|)
comment|/* TRANSLATORS: The first %s is the name of the 			   environment variable, the second %s is its value */
name|error
argument_list|(
name|_
argument_list|(
literal|"Bad %s value: '%s'"
argument_list|)
argument_list|,
name|GIT_NOTES_REWRITE_MODE_ENVIRONMENT
argument_list|,
name|rewrite_mode_env
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|rewrite_refs_env
condition|)
block|{
name|c
operator|->
name|refs_from_env
operator|=
literal|1
expr_stmt|;
name|string_list_add_refs_from_colon_sep
argument_list|(
name|c
operator|->
name|refs
argument_list|,
name|rewrite_refs_env
argument_list|)
expr_stmt|;
block|}
name|git_config
argument_list|(
name|notes_rewrite_config
argument_list|,
name|c
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|c
operator|->
name|enabled
operator|||
operator|!
name|c
operator|->
name|refs
operator|->
name|nr
condition|)
block|{
name|string_list_clear
argument_list|(
name|c
operator|->
name|refs
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|c
operator|->
name|refs
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|c
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|c
operator|->
name|trees
operator|=
name|load_notes_trees
argument_list|(
name|c
operator|->
name|refs
argument_list|)
expr_stmt|;
name|string_list_clear
argument_list|(
name|c
operator|->
name|refs
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|c
operator|->
name|refs
argument_list|)
expr_stmt|;
return|return
name|c
return|;
block|}
end_function
begin_function
DECL|function|copy_note_for_rewrite
name|int
name|copy_note_for_rewrite
parameter_list|(
name|struct
name|notes_rewrite_cfg
modifier|*
name|c
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|from_obj
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|to_obj
parameter_list|)
block|{
name|int
name|ret
init|=
literal|0
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
name|c
operator|->
name|trees
index|[
name|i
index|]
condition|;
name|i
operator|++
control|)
name|ret
operator|=
name|copy_note
argument_list|(
name|c
operator|->
name|trees
index|[
name|i
index|]
argument_list|,
name|from_obj
argument_list|,
name|to_obj
argument_list|,
literal|1
argument_list|,
name|c
operator|->
name|combine
argument_list|)
operator|||
name|ret
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|finish_copy_notes_for_rewrite
name|void
name|finish_copy_notes_for_rewrite
parameter_list|(
name|struct
name|notes_rewrite_cfg
modifier|*
name|c
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
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
name|c
operator|->
name|trees
index|[
name|i
index|]
condition|;
name|i
operator|++
control|)
block|{
name|commit_notes
argument_list|(
name|c
operator|->
name|trees
index|[
name|i
index|]
argument_list|,
name|msg
argument_list|)
expr_stmt|;
name|free_notes
argument_list|(
name|c
operator|->
name|trees
index|[
name|i
index|]
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|c
operator|->
name|trees
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|c
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
