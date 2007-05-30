begin_unit
begin_comment
comment|/*  * Builtin "git count-objects".  *  * Copyright (c) 2006 Junio C Hamano  */
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
begin_decl_stmt
DECL|variable|count_objects_usage
specifier|static
specifier|const
name|char
name|count_objects_usage
index|[]
init|=
literal|"git-count-objects [-v]"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|count_objects
specifier|static
name|void
name|count_objects
parameter_list|(
name|DIR
modifier|*
name|d
parameter_list|,
name|char
modifier|*
name|path
parameter_list|,
name|int
name|len
parameter_list|,
name|int
name|verbose
parameter_list|,
name|unsigned
name|long
modifier|*
name|loose
parameter_list|,
name|unsigned
name|long
modifier|*
name|loose_size
parameter_list|,
name|unsigned
name|long
modifier|*
name|packed_loose
parameter_list|,
name|unsigned
name|long
modifier|*
name|garbage
parameter_list|)
block|{
name|struct
name|dirent
modifier|*
name|ent
decl_stmt|;
while|while
condition|(
operator|(
name|ent
operator|=
name|readdir
argument_list|(
name|d
argument_list|)
operator|)
operator|!=
name|NULL
condition|)
block|{
name|char
name|hex
index|[
literal|41
index|]
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
name|cp
decl_stmt|;
name|int
name|bad
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|(
name|ent
operator|->
name|d_name
index|[
literal|0
index|]
operator|==
literal|'.'
operator|)
operator|&&
operator|(
name|ent
operator|->
name|d_name
index|[
literal|1
index|]
operator|==
literal|0
operator|||
operator|(
operator|(
name|ent
operator|->
name|d_name
index|[
literal|1
index|]
operator|==
literal|'.'
operator|)
operator|&&
operator|(
name|ent
operator|->
name|d_name
index|[
literal|2
index|]
operator|==
literal|0
operator|)
operator|)
operator|)
condition|)
continue|continue;
for|for
control|(
name|cp
operator|=
name|ent
operator|->
name|d_name
init|;
operator|*
name|cp
condition|;
name|cp
operator|++
control|)
block|{
name|int
name|ch
init|=
operator|*
name|cp
decl_stmt|;
if|if
condition|(
operator|(
literal|'0'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'9'
operator|)
operator|||
operator|(
literal|'a'
operator|<=
name|ch
operator|&&
name|ch
operator|<=
literal|'f'
operator|)
condition|)
continue|continue;
name|bad
operator|=
literal|1
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|cp
operator|-
name|ent
operator|->
name|d_name
operator|!=
literal|38
condition|)
name|bad
operator|=
literal|1
expr_stmt|;
else|else
block|{
name|struct
name|stat
name|st
decl_stmt|;
name|memcpy
argument_list|(
name|path
operator|+
name|len
operator|+
literal|3
argument_list|,
name|ent
operator|->
name|d_name
argument_list|,
literal|38
argument_list|)
expr_stmt|;
name|path
index|[
name|len
operator|+
literal|2
index|]
operator|=
literal|'/'
expr_stmt|;
name|path
index|[
name|len
operator|+
literal|41
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|path
argument_list|,
operator|&
name|st
argument_list|)
operator|||
operator|!
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
name|bad
operator|=
literal|1
expr_stmt|;
else|else
operator|(
operator|*
name|loose_size
operator|)
operator|+=
name|xsize_t
argument_list|(
name|st
operator|.
name|st_blocks
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|bad
condition|)
block|{
if|if
condition|(
name|verbose
condition|)
block|{
name|error
argument_list|(
literal|"garbage found: %.*s/%s"
argument_list|,
name|len
operator|+
literal|2
argument_list|,
name|path
argument_list|,
name|ent
operator|->
name|d_name
argument_list|)
expr_stmt|;
operator|(
operator|*
name|garbage
operator|)
operator|++
expr_stmt|;
block|}
continue|continue;
block|}
operator|(
operator|*
name|loose
operator|)
operator|++
expr_stmt|;
if|if
condition|(
operator|!
name|verbose
condition|)
continue|continue;
name|memcpy
argument_list|(
name|hex
argument_list|,
name|path
operator|+
name|len
argument_list|,
literal|2
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|hex
operator|+
literal|2
argument_list|,
name|ent
operator|->
name|d_name
argument_list|,
literal|38
argument_list|)
expr_stmt|;
name|hex
index|[
literal|40
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|hex
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"internal error"
argument_list|)
expr_stmt|;
if|if
condition|(
name|has_sha1_pack
argument_list|(
name|sha1
argument_list|,
name|NULL
argument_list|)
condition|)
operator|(
operator|*
name|packed_loose
operator|)
operator|++
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|cmd_count_objects
name|int
name|cmd_count_objects
parameter_list|(
name|int
name|ac
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|av
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
name|verbose
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|objdir
init|=
name|get_object_directory
argument_list|()
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|objdir
argument_list|)
decl_stmt|;
name|char
modifier|*
name|path
init|=
name|xmalloc
argument_list|(
name|len
operator|+
literal|50
argument_list|)
decl_stmt|;
name|unsigned
name|long
name|loose
init|=
literal|0
decl_stmt|,
name|packed
init|=
literal|0
decl_stmt|,
name|packed_loose
init|=
literal|0
decl_stmt|,
name|garbage
init|=
literal|0
decl_stmt|;
name|unsigned
name|long
name|loose_size
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|ac
condition|;
name|i
operator|++
control|)
block|{
specifier|const
name|char
modifier|*
name|arg
init|=
name|av
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|*
name|arg
operator|!=
literal|'-'
condition|)
break|break;
elseif|else
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
name|verbose
operator|=
literal|1
expr_stmt|;
else|else
name|usage
argument_list|(
name|count_objects_usage
argument_list|)
expr_stmt|;
block|}
comment|/* we do not take arguments other than flags for now */
if|if
condition|(
name|i
operator|<
name|ac
condition|)
name|usage
argument_list|(
name|count_objects_usage
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|path
argument_list|,
name|objdir
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|&&
name|objdir
index|[
name|len
operator|-
literal|1
index|]
operator|!=
literal|'/'
condition|)
name|path
index|[
name|len
operator|++
index|]
operator|=
literal|'/'
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|256
condition|;
name|i
operator|++
control|)
block|{
name|DIR
modifier|*
name|d
decl_stmt|;
name|sprintf
argument_list|(
name|path
operator|+
name|len
argument_list|,
literal|"%02x"
argument_list|,
name|i
argument_list|)
expr_stmt|;
name|d
operator|=
name|opendir
argument_list|(
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|d
condition|)
continue|continue;
name|count_objects
argument_list|(
name|d
argument_list|,
name|path
argument_list|,
name|len
argument_list|,
name|verbose
argument_list|,
operator|&
name|loose
argument_list|,
operator|&
name|loose_size
argument_list|,
operator|&
name|packed_loose
argument_list|,
operator|&
name|garbage
argument_list|)
expr_stmt|;
name|closedir
argument_list|(
name|d
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|verbose
condition|)
block|{
name|struct
name|packed_git
modifier|*
name|p
decl_stmt|;
name|unsigned
name|long
name|num_pack
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|!
name|packed_git
condition|)
name|prepare_packed_git
argument_list|()
expr_stmt|;
for|for
control|(
name|p
operator|=
name|packed_git
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
if|if
condition|(
operator|!
name|p
operator|->
name|pack_local
condition|)
continue|continue;
if|if
condition|(
name|open_pack_index
argument_list|(
name|p
argument_list|)
condition|)
continue|continue;
name|packed
operator|+=
name|p
operator|->
name|num_objects
expr_stmt|;
name|num_pack
operator|++
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"count: %lu\n"
argument_list|,
name|loose
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"size: %lu\n"
argument_list|,
name|loose_size
operator|/
literal|2
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"in-pack: %lu\n"
argument_list|,
name|packed
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"packs: %lu\n"
argument_list|,
name|num_pack
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"prune-packable: %lu\n"
argument_list|,
name|packed_loose
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"garbage: %lu\n"
argument_list|,
name|garbage
argument_list|)
expr_stmt|;
block|}
else|else
name|printf
argument_list|(
literal|"%lu objects, %lu kilobytes\n"
argument_list|,
name|loose
argument_list|,
name|loose_size
operator|/
literal|2
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
