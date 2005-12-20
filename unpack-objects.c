begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_include
include|#
directive|include
file|"delta.h"
end_include
begin_include
include|#
directive|include
file|"pack.h"
end_include
begin_include
include|#
directive|include
file|<sys/time.h>
end_include
begin_decl_stmt
DECL|variable|dry_run
DECL|variable|quiet
specifier|static
name|int
name|dry_run
decl_stmt|,
name|quiet
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|unpack_usage
specifier|static
specifier|const
name|char
name|unpack_usage
index|[]
init|=
literal|"git-unpack-objects [-n] [-q]< pack-file"
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* We always read in 4kB chunks. */
end_comment
begin_decl_stmt
DECL|variable|buffer
specifier|static
name|unsigned
name|char
name|buffer
index|[
literal|4096
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|offset
DECL|variable|len
DECL|variable|eof
specifier|static
name|unsigned
name|long
name|offset
decl_stmt|,
name|len
decl_stmt|,
name|eof
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|ctx
specifier|static
name|SHA_CTX
name|ctx
decl_stmt|;
end_decl_stmt
begin_comment
comment|/*  * Make sure at least "min" bytes are available in the buffer, and  * return the pointer to the buffer.  */
end_comment
begin_function
DECL|function|fill
specifier|static
name|void
modifier|*
name|fill
parameter_list|(
name|int
name|min
parameter_list|)
block|{
if|if
condition|(
name|min
operator|<=
name|len
condition|)
return|return
name|buffer
operator|+
name|offset
return|;
if|if
condition|(
name|eof
condition|)
name|die
argument_list|(
literal|"unable to fill input"
argument_list|)
expr_stmt|;
if|if
condition|(
name|min
operator|>
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
condition|)
name|die
argument_list|(
literal|"cannot fill %d bytes"
argument_list|,
name|min
argument_list|)
expr_stmt|;
if|if
condition|(
name|offset
condition|)
block|{
name|SHA1_Update
argument_list|(
operator|&
name|ctx
argument_list|,
name|buffer
argument_list|,
name|offset
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|buffer
argument_list|,
name|buffer
operator|+
name|offset
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|offset
operator|=
literal|0
expr_stmt|;
block|}
do|do
block|{
name|int
name|ret
init|=
name|xread
argument_list|(
literal|0
argument_list|,
name|buffer
operator|+
name|len
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
operator|-
name|len
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
operator|<=
literal|0
condition|)
block|{
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"early EOF"
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"read error on input: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|len
operator|+=
name|ret
expr_stmt|;
block|}
do|while
condition|(
name|len
operator|<
name|min
condition|)
do|;
return|return
name|buffer
return|;
block|}
end_function
begin_function
DECL|function|use
specifier|static
name|void
name|use
parameter_list|(
name|int
name|bytes
parameter_list|)
block|{
if|if
condition|(
name|bytes
operator|>
name|len
condition|)
name|die
argument_list|(
literal|"used more bytes than were available"
argument_list|)
expr_stmt|;
name|len
operator|-=
name|bytes
expr_stmt|;
name|offset
operator|+=
name|bytes
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_data
specifier|static
name|void
modifier|*
name|get_data
parameter_list|(
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|z_stream
name|stream
decl_stmt|;
name|void
modifier|*
name|buf
init|=
name|xmalloc
argument_list|(
name|size
argument_list|)
decl_stmt|;
name|memset
argument_list|(
operator|&
name|stream
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|stream
argument_list|)
argument_list|)
expr_stmt|;
name|stream
operator|.
name|next_out
operator|=
name|buf
expr_stmt|;
name|stream
operator|.
name|avail_out
operator|=
name|size
expr_stmt|;
name|stream
operator|.
name|next_in
operator|=
name|fill
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|stream
operator|.
name|avail_in
operator|=
name|len
expr_stmt|;
name|inflateInit
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|ret
init|=
name|inflate
argument_list|(
operator|&
name|stream
argument_list|,
literal|0
argument_list|)
decl_stmt|;
name|use
argument_list|(
name|len
operator|-
name|stream
operator|.
name|avail_in
argument_list|)
expr_stmt|;
if|if
condition|(
name|stream
operator|.
name|total_out
operator|==
name|size
operator|&&
name|ret
operator|==
name|Z_STREAM_END
condition|)
break|break;
if|if
condition|(
name|ret
operator|!=
name|Z_OK
condition|)
name|die
argument_list|(
literal|"inflate returned %d\n"
argument_list|,
name|ret
argument_list|)
expr_stmt|;
name|stream
operator|.
name|next_in
operator|=
name|fill
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|stream
operator|.
name|avail_in
operator|=
name|len
expr_stmt|;
block|}
name|inflateEnd
argument_list|(
operator|&
name|stream
argument_list|)
expr_stmt|;
return|return
name|buf
return|;
block|}
end_function
begin_struct
DECL|struct|delta_info
struct|struct
name|delta_info
block|{
DECL|member|base_sha1
name|unsigned
name|char
name|base_sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|size
name|unsigned
name|long
name|size
decl_stmt|;
DECL|member|delta
name|void
modifier|*
name|delta
decl_stmt|;
DECL|member|next
name|struct
name|delta_info
modifier|*
name|next
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|delta_list
specifier|static
name|struct
name|delta_info
modifier|*
name|delta_list
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_delta_to_list
specifier|static
name|void
name|add_delta_to_list
parameter_list|(
name|unsigned
name|char
modifier|*
name|base_sha1
parameter_list|,
name|void
modifier|*
name|delta
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|struct
name|delta_info
modifier|*
name|info
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|info
argument_list|)
argument_list|)
decl_stmt|;
name|memcpy
argument_list|(
name|info
operator|->
name|base_sha1
argument_list|,
name|base_sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|info
operator|->
name|size
operator|=
name|size
expr_stmt|;
name|info
operator|->
name|delta
operator|=
name|delta
expr_stmt|;
name|info
operator|->
name|next
operator|=
name|delta_list
expr_stmt|;
name|delta_list
operator|=
name|info
expr_stmt|;
block|}
end_function
begin_function_decl
specifier|static
name|void
name|added_object
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|,
name|void
modifier|*
name|data
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|write_object
specifier|static
name|void
name|write_object
parameter_list|(
name|void
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|)
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
name|write_sha1_file
argument_list|(
name|buf
argument_list|,
name|size
argument_list|,
name|type
argument_list|,
name|sha1
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"failed to write object"
argument_list|)
expr_stmt|;
name|added_object
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
name|buf
argument_list|,
name|size
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|resolve_delta
specifier|static
name|int
name|resolve_delta
parameter_list|(
specifier|const
name|char
modifier|*
name|type
parameter_list|,
name|void
modifier|*
name|base
parameter_list|,
name|unsigned
name|long
name|base_size
parameter_list|,
name|void
modifier|*
name|delta
parameter_list|,
name|unsigned
name|long
name|delta_size
parameter_list|)
block|{
name|void
modifier|*
name|result
decl_stmt|;
name|unsigned
name|long
name|result_size
decl_stmt|;
name|result
operator|=
name|patch_delta
argument_list|(
name|base
argument_list|,
name|base_size
argument_list|,
name|delta
argument_list|,
name|delta_size
argument_list|,
operator|&
name|result_size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|result
condition|)
name|die
argument_list|(
literal|"failed to apply delta"
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|delta
argument_list|)
expr_stmt|;
name|write_object
argument_list|(
name|result
argument_list|,
name|result_size
argument_list|,
name|type
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|result
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|added_object
specifier|static
name|void
name|added_object
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|type
parameter_list|,
name|void
modifier|*
name|data
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|struct
name|delta_info
modifier|*
modifier|*
name|p
init|=
operator|&
name|delta_list
decl_stmt|;
name|struct
name|delta_info
modifier|*
name|info
decl_stmt|;
while|while
condition|(
operator|(
name|info
operator|=
operator|*
name|p
operator|)
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|info
operator|->
name|base_sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
condition|)
block|{
operator|*
name|p
operator|=
name|info
operator|->
name|next
expr_stmt|;
name|p
operator|=
operator|&
name|delta_list
expr_stmt|;
name|resolve_delta
argument_list|(
name|type
argument_list|,
name|data
argument_list|,
name|size
argument_list|,
name|info
operator|->
name|delta
argument_list|,
name|info
operator|->
name|size
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|info
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|p
operator|=
operator|&
name|info
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|unpack_non_delta_entry
specifier|static
name|int
name|unpack_non_delta_entry
parameter_list|(
name|enum
name|object_type
name|kind
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|void
modifier|*
name|buf
init|=
name|get_data
argument_list|(
name|size
argument_list|)
decl_stmt|;
specifier|const
name|char
modifier|*
name|type
decl_stmt|;
switch|switch
condition|(
name|kind
condition|)
block|{
case|case
name|OBJ_COMMIT
case|:
name|type
operator|=
literal|"commit"
expr_stmt|;
break|break;
case|case
name|OBJ_TREE
case|:
name|type
operator|=
literal|"tree"
expr_stmt|;
break|break;
case|case
name|OBJ_BLOB
case|:
name|type
operator|=
literal|"blob"
expr_stmt|;
break|break;
case|case
name|OBJ_TAG
case|:
name|type
operator|=
literal|"tag"
expr_stmt|;
break|break;
default|default:
name|die
argument_list|(
literal|"bad type %d"
argument_list|,
name|kind
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|dry_run
condition|)
name|write_object
argument_list|(
name|buf
argument_list|,
name|size
argument_list|,
name|type
argument_list|)
expr_stmt|;
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
DECL|function|unpack_delta_entry
specifier|static
name|int
name|unpack_delta_entry
parameter_list|(
name|unsigned
name|long
name|delta_size
parameter_list|)
block|{
name|void
modifier|*
name|delta_data
decl_stmt|,
modifier|*
name|base
decl_stmt|;
name|unsigned
name|long
name|base_size
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|char
name|base_sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|result
decl_stmt|;
name|memcpy
argument_list|(
name|base_sha1
argument_list|,
name|fill
argument_list|(
literal|20
argument_list|)
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|use
argument_list|(
literal|20
argument_list|)
expr_stmt|;
name|delta_data
operator|=
name|get_data
argument_list|(
name|delta_size
argument_list|)
expr_stmt|;
if|if
condition|(
name|dry_run
condition|)
block|{
name|free
argument_list|(
name|delta_data
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|has_sha1_file
argument_list|(
name|base_sha1
argument_list|)
condition|)
block|{
name|add_delta_to_list
argument_list|(
name|base_sha1
argument_list|,
name|delta_data
argument_list|,
name|delta_size
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|base
operator|=
name|read_sha1_file
argument_list|(
name|base_sha1
argument_list|,
name|type
argument_list|,
operator|&
name|base_size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|base
condition|)
name|die
argument_list|(
literal|"failed to read delta-pack base object %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|base_sha1
argument_list|)
argument_list|)
expr_stmt|;
name|result
operator|=
name|resolve_delta
argument_list|(
name|type
argument_list|,
name|base
argument_list|,
name|base_size
argument_list|,
name|delta_data
argument_list|,
name|delta_size
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|base
argument_list|)
expr_stmt|;
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|unpack_one
specifier|static
name|void
name|unpack_one
parameter_list|(
name|unsigned
name|nr
parameter_list|,
name|unsigned
name|total
parameter_list|)
block|{
name|unsigned
name|shift
decl_stmt|;
name|unsigned
name|char
modifier|*
name|pack
decl_stmt|,
name|c
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|pack
operator|=
name|fill
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|c
operator|=
operator|*
name|pack
expr_stmt|;
name|use
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|type
operator|=
operator|(
name|c
operator|>>
literal|4
operator|)
operator|&
literal|7
expr_stmt|;
name|size
operator|=
operator|(
name|c
operator|&
literal|15
operator|)
expr_stmt|;
name|shift
operator|=
literal|4
expr_stmt|;
while|while
condition|(
name|c
operator|&
literal|0x80
condition|)
block|{
name|pack
operator|=
name|fill
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|c
operator|=
operator|*
name|pack
operator|++
expr_stmt|;
name|use
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|size
operator|+=
operator|(
name|c
operator|&
literal|0x7f
operator|)
operator|<<
name|shift
expr_stmt|;
name|shift
operator|+=
literal|7
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|quiet
condition|)
block|{
specifier|static
name|unsigned
name|long
name|last_sec
decl_stmt|;
specifier|static
name|unsigned
name|last_percent
decl_stmt|;
name|struct
name|timeval
name|now
decl_stmt|;
name|unsigned
name|percentage
init|=
operator|(
name|nr
operator|*
literal|100
operator|)
operator|/
name|total
decl_stmt|;
name|gettimeofday
argument_list|(
operator|&
name|now
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|percentage
operator|!=
name|last_percent
operator|||
name|now
operator|.
name|tv_sec
operator|!=
name|last_sec
condition|)
block|{
name|last_sec
operator|=
name|now
operator|.
name|tv_sec
expr_stmt|;
name|last_percent
operator|=
name|percentage
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%4u%% (%u/%u) done\r"
argument_list|,
name|percentage
argument_list|,
name|nr
argument_list|,
name|total
argument_list|)
expr_stmt|;
block|}
block|}
switch|switch
condition|(
name|type
condition|)
block|{
case|case
name|OBJ_COMMIT
case|:
case|case
name|OBJ_TREE
case|:
case|case
name|OBJ_BLOB
case|:
case|case
name|OBJ_TAG
case|:
name|unpack_non_delta_entry
argument_list|(
name|type
argument_list|,
name|size
argument_list|)
expr_stmt|;
return|return;
case|case
name|OBJ_DELTA
case|:
name|unpack_delta_entry
argument_list|(
name|size
argument_list|)
expr_stmt|;
return|return;
default|default:
name|die
argument_list|(
literal|"bad object type %d"
argument_list|,
name|type
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_comment
comment|/*  * We unpack from the end, older files first. Now, usually  * there are deltas etc, so we'll not actually write the  * objects in that order, but we might as well try..  */
end_comment
begin_function
DECL|function|unpack_all
specifier|static
name|void
name|unpack_all
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|struct
name|pack_header
modifier|*
name|hdr
init|=
name|fill
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|pack_header
argument_list|)
argument_list|)
decl_stmt|;
name|unsigned
name|version
init|=
name|ntohl
argument_list|(
name|hdr
operator|->
name|hdr_version
argument_list|)
decl_stmt|;
name|unsigned
name|nr_objects
init|=
name|ntohl
argument_list|(
name|hdr
operator|->
name|hdr_entries
argument_list|)
decl_stmt|;
if|if
condition|(
name|ntohl
argument_list|(
name|hdr
operator|->
name|hdr_signature
argument_list|)
operator|!=
name|PACK_SIGNATURE
condition|)
name|die
argument_list|(
literal|"bad pack file"
argument_list|)
expr_stmt|;
if|if
condition|(
name|version
operator|!=
name|PACK_VERSION
condition|)
name|die
argument_list|(
literal|"unable to handle pack file version %d"
argument_list|,
name|version
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Unpacking %d objects\n"
argument_list|,
name|nr_objects
argument_list|)
expr_stmt|;
name|use
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|pack_header
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
name|nr_objects
condition|;
name|i
operator|++
control|)
name|unpack_one
argument_list|(
name|i
operator|+
literal|1
argument_list|,
name|nr_objects
argument_list|)
expr_stmt|;
if|if
condition|(
name|delta_list
condition|)
name|die
argument_list|(
literal|"unresolved deltas left after unpacking"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|setup_git_directory
argument_list|()
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
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|*
name|arg
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"-n"
argument_list|)
condition|)
block|{
name|dry_run
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
literal|"-q"
argument_list|)
condition|)
block|{
name|quiet
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|unpack_usage
argument_list|)
expr_stmt|;
block|}
comment|/* We don't take any non-flag arguments now.. Maybe some day */
name|usage
argument_list|(
name|unpack_usage
argument_list|)
expr_stmt|;
block|}
name|SHA1_Init
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|unpack_all
argument_list|()
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|ctx
argument_list|,
name|buffer
argument_list|,
name|offset
argument_list|)
expr_stmt|;
name|SHA1_Final
argument_list|(
name|sha1
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|fill
argument_list|(
literal|20
argument_list|)
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
condition|)
name|die
argument_list|(
literal|"final sha1 did not match"
argument_list|)
expr_stmt|;
name|use
argument_list|(
literal|20
argument_list|)
expr_stmt|;
comment|/* Write the last part of the buffer to stdout */
while|while
condition|(
name|len
condition|)
block|{
name|int
name|ret
init|=
name|xwrite
argument_list|(
literal|1
argument_list|,
name|buffer
operator|+
name|offset
argument_list|,
name|len
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
operator|<=
literal|0
condition|)
break|break;
name|len
operator|-=
name|ret
expr_stmt|;
name|offset
operator|+=
name|ret
expr_stmt|;
block|}
comment|/* All done */
if|if
condition|(
operator|!
name|quiet
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"\n"
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
