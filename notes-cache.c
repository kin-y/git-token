begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"notes-cache.h"
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
begin_function
DECL|function|notes_cache_match_validity
specifier|static
name|int
name|notes_cache_match_validity
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
specifier|const
name|char
modifier|*
name|validity
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|struct
name|pretty_print_context
name|pretty_ctx
decl_stmt|;
name|struct
name|strbuf
name|msg
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|ret
decl_stmt|;
if|if
condition|(
name|read_ref
argument_list|(
name|ref
argument_list|,
name|sha1
argument_list|)
operator|<
literal|0
condition|)
return|return
literal|0
return|;
name|commit
operator|=
name|lookup_commit_reference_gently
argument_list|(
name|sha1
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
return|return
literal|0
return|;
name|memset
argument_list|(
operator|&
name|pretty_ctx
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|pretty_ctx
argument_list|)
argument_list|)
expr_stmt|;
name|format_commit_message
argument_list|(
name|commit
argument_list|,
literal|"%s"
argument_list|,
operator|&
name|msg
argument_list|,
operator|&
name|pretty_ctx
argument_list|)
expr_stmt|;
name|strbuf_trim
argument_list|(
operator|&
name|msg
argument_list|)
expr_stmt|;
name|ret
operator|=
operator|!
name|strcmp
argument_list|(
name|msg
operator|.
name|buf
argument_list|,
name|validity
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|msg
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|notes_cache_init
name|void
name|notes_cache_init
parameter_list|(
name|struct
name|notes_cache
modifier|*
name|c
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|validity
parameter_list|)
block|{
name|struct
name|strbuf
name|ref
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|flags
init|=
name|NOTES_INIT_WRITABLE
decl_stmt|;
name|memset
argument_list|(
name|c
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|c
argument_list|)
argument_list|)
expr_stmt|;
name|c
operator|->
name|validity
operator|=
name|xstrdup
argument_list|(
name|validity
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|ref
argument_list|,
literal|"refs/notes/%s"
argument_list|,
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|notes_cache_match_validity
argument_list|(
name|ref
operator|.
name|buf
argument_list|,
name|validity
argument_list|)
condition|)
name|flags
operator||=
name|NOTES_INIT_EMPTY
expr_stmt|;
name|init_notes
argument_list|(
operator|&
name|c
operator|->
name|tree
argument_list|,
name|ref
operator|.
name|buf
argument_list|,
name|combine_notes_overwrite
argument_list|,
name|flags
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|ref
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|notes_cache_write
name|int
name|notes_cache_write
parameter_list|(
name|struct
name|notes_cache
modifier|*
name|c
parameter_list|)
block|{
name|unsigned
name|char
name|tree_sha1
index|[
literal|20
index|]
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
name|c
operator|||
operator|!
name|c
operator|->
name|tree
operator|.
name|initialized
operator|||
operator|!
name|c
operator|->
name|tree
operator|.
name|update_ref
operator|||
operator|!
operator|*
name|c
operator|->
name|tree
operator|.
name|update_ref
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|c
operator|->
name|tree
operator|.
name|dirty
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|write_notes_tree
argument_list|(
operator|&
name|c
operator|->
name|tree
argument_list|,
name|tree_sha1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|commit_tree
argument_list|(
name|c
operator|->
name|validity
argument_list|,
name|strlen
argument_list|(
name|c
operator|->
name|validity
argument_list|)
argument_list|,
name|tree_sha1
argument_list|,
name|NULL
argument_list|,
name|commit_sha1
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|update_ref
argument_list|(
literal|"update notes cache"
argument_list|,
name|c
operator|->
name|tree
operator|.
name|update_ref
argument_list|,
name|commit_sha1
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
name|UPDATE_REFS_QUIET_ON_ERR
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|notes_cache_get
name|char
modifier|*
name|notes_cache_get
parameter_list|(
name|struct
name|notes_cache
modifier|*
name|c
parameter_list|,
name|unsigned
name|char
name|key_sha1
index|[
literal|20
index|]
parameter_list|,
name|size_t
modifier|*
name|outsize
parameter_list|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|value_sha1
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|char
modifier|*
name|value
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|value_sha1
operator|=
name|get_note
argument_list|(
operator|&
name|c
operator|->
name|tree
argument_list|,
name|key_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|value_sha1
condition|)
return|return
name|NULL
return|;
name|value
operator|=
name|read_sha1_file
argument_list|(
name|value_sha1
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
operator|*
name|outsize
operator|=
name|size
expr_stmt|;
return|return
name|value
return|;
block|}
end_function
begin_function
DECL|function|notes_cache_put
name|int
name|notes_cache_put
parameter_list|(
name|struct
name|notes_cache
modifier|*
name|c
parameter_list|,
name|unsigned
name|char
name|key_sha1
index|[
literal|20
index|]
parameter_list|,
specifier|const
name|char
modifier|*
name|data
parameter_list|,
name|size_t
name|size
parameter_list|)
block|{
name|unsigned
name|char
name|value_sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|write_sha1_file
argument_list|(
name|data
argument_list|,
name|size
argument_list|,
literal|"blob"
argument_list|,
name|value_sha1
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
return|return
name|add_note
argument_list|(
operator|&
name|c
operator|->
name|tree
argument_list|,
name|key_sha1
argument_list|,
name|value_sha1
argument_list|,
name|NULL
argument_list|)
return|;
block|}
end_function
end_unit
