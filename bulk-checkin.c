begin_unit
begin_comment
comment|/*  * Copyright (c) 2011, Google Inc.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"bulk-checkin.h"
end_include
begin_include
include|#
directive|include
file|"csum-file.h"
end_include
begin_include
include|#
directive|include
file|"pack.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_decl_stmt
DECL|variable|pack_compression_level
specifier|static
name|int
name|pack_compression_level
init|=
name|Z_DEFAULT_COMPRESSION
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|bulk_checkin_state
specifier|static
struct|struct
name|bulk_checkin_state
block|{
DECL|member|plugged
name|unsigned
name|plugged
range|:
literal|1
decl_stmt|;
DECL|member|pack_tmp_name
name|char
modifier|*
name|pack_tmp_name
decl_stmt|;
DECL|member|f
name|struct
name|sha1file
modifier|*
name|f
decl_stmt|;
DECL|member|offset
name|off_t
name|offset
decl_stmt|;
DECL|member|pack_idx_opts
name|struct
name|pack_idx_option
name|pack_idx_opts
decl_stmt|;
DECL|member|written
name|struct
name|pack_idx_entry
modifier|*
modifier|*
name|written
decl_stmt|;
DECL|member|alloc_written
name|uint32_t
name|alloc_written
decl_stmt|;
DECL|member|nr_written
name|uint32_t
name|nr_written
decl_stmt|;
block|}
DECL|variable|state
name|state
struct|;
end_struct
begin_function
DECL|function|finish_bulk_checkin
specifier|static
name|void
name|finish_bulk_checkin
parameter_list|(
name|struct
name|bulk_checkin_state
modifier|*
name|state
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
name|strbuf
name|packname
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|state
operator|->
name|f
condition|)
return|return;
if|if
condition|(
name|state
operator|->
name|nr_written
operator|==
literal|0
condition|)
block|{
name|close
argument_list|(
name|state
operator|->
name|f
operator|->
name|fd
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|state
operator|->
name|pack_tmp_name
argument_list|)
expr_stmt|;
goto|goto
name|clear_exit
goto|;
block|}
elseif|else
if|if
condition|(
name|state
operator|->
name|nr_written
operator|==
literal|1
condition|)
block|{
name|sha1close
argument_list|(
name|state
operator|->
name|f
argument_list|,
name|sha1
argument_list|,
name|CSUM_FSYNC
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|int
name|fd
init|=
name|sha1close
argument_list|(
name|state
operator|->
name|f
argument_list|,
name|sha1
argument_list|,
literal|0
argument_list|)
decl_stmt|;
name|fixup_pack_header_footer
argument_list|(
name|fd
argument_list|,
name|sha1
argument_list|,
name|state
operator|->
name|pack_tmp_name
argument_list|,
name|state
operator|->
name|nr_written
argument_list|,
name|sha1
argument_list|,
name|state
operator|->
name|offset
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
name|strbuf_addf
argument_list|(
operator|&
name|packname
argument_list|,
literal|"%s/pack/pack-"
argument_list|,
name|get_object_directory
argument_list|()
argument_list|)
expr_stmt|;
name|finish_tmp_packfile
argument_list|(
operator|&
name|packname
argument_list|,
name|state
operator|->
name|pack_tmp_name
argument_list|,
name|state
operator|->
name|written
argument_list|,
name|state
operator|->
name|nr_written
argument_list|,
operator|&
name|state
operator|->
name|pack_idx_opts
argument_list|,
name|sha1
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
name|state
operator|->
name|nr_written
condition|;
name|i
operator|++
control|)
name|free
argument_list|(
name|state
operator|->
name|written
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|clear_exit
label|:
name|free
argument_list|(
name|state
operator|->
name|written
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|state
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|state
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|packname
argument_list|)
expr_stmt|;
comment|/* Make objects we just wrote available to ourselves */
name|reprepare_packed_git
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|already_written
specifier|static
name|int
name|already_written
parameter_list|(
name|struct
name|bulk_checkin_state
modifier|*
name|state
parameter_list|,
name|unsigned
name|char
name|sha1
index|[]
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
comment|/* The object may already exist in the repository */
if|if
condition|(
name|has_sha1_file
argument_list|(
name|sha1
argument_list|)
condition|)
return|return
literal|1
return|;
comment|/* Might want to keep the list sorted */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|state
operator|->
name|nr_written
condition|;
name|i
operator|++
control|)
if|if
condition|(
operator|!
name|hashcmp
argument_list|(
name|state
operator|->
name|written
index|[
name|i
index|]
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
literal|1
return|;
comment|/* This is a new object we need to keep */
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * Read the contents from fd for size bytes, streaming it to the  * packfile in state while updating the hash in ctx. Signal a failure  * by returning a negative value when the resulting pack would exceed  * the pack size limit and this is not the first object in the pack,  * so that the caller can discard what we wrote from the current pack  * by truncating it and opening a new one. The caller will then call  * us again after rewinding the input fd.  *  * The already_hashed_to pointer is kept untouched by the caller to  * make sure we do not hash the same byte when we are called  * again. This way, the caller does not have to checkpoint its hash  * status before calling us just in case we ask it to call us again  * with a new pack.  */
end_comment
begin_function
DECL|function|stream_to_pack
specifier|static
name|int
name|stream_to_pack
parameter_list|(
name|struct
name|bulk_checkin_state
modifier|*
name|state
parameter_list|,
name|git_SHA_CTX
modifier|*
name|ctx
parameter_list|,
name|off_t
modifier|*
name|already_hashed_to
parameter_list|,
name|int
name|fd
parameter_list|,
name|size_t
name|size
parameter_list|,
name|enum
name|object_type
name|type
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|flags
parameter_list|)
block|{
name|git_zstream
name|s
decl_stmt|;
name|unsigned
name|char
name|obuf
index|[
literal|16384
index|]
decl_stmt|;
name|unsigned
name|hdrlen
decl_stmt|;
name|int
name|status
init|=
name|Z_OK
decl_stmt|;
name|int
name|write_object
init|=
operator|(
name|flags
operator|&
name|HASH_WRITE_OBJECT
operator|)
decl_stmt|;
name|off_t
name|offset
init|=
literal|0
decl_stmt|;
name|git_deflate_init
argument_list|(
operator|&
name|s
argument_list|,
name|pack_compression_level
argument_list|)
expr_stmt|;
name|hdrlen
operator|=
name|encode_in_pack_object_header
argument_list|(
name|type
argument_list|,
name|size
argument_list|,
name|obuf
argument_list|)
expr_stmt|;
name|s
operator|.
name|next_out
operator|=
name|obuf
operator|+
name|hdrlen
expr_stmt|;
name|s
operator|.
name|avail_out
operator|=
sizeof|sizeof
argument_list|(
name|obuf
argument_list|)
operator|-
name|hdrlen
expr_stmt|;
while|while
condition|(
name|status
operator|!=
name|Z_STREAM_END
condition|)
block|{
name|unsigned
name|char
name|ibuf
index|[
literal|16384
index|]
decl_stmt|;
if|if
condition|(
name|size
operator|&&
operator|!
name|s
operator|.
name|avail_in
condition|)
block|{
name|ssize_t
name|rsize
init|=
name|size
operator|<
sizeof|sizeof
argument_list|(
name|ibuf
argument_list|)
condition|?
name|size
else|:
sizeof|sizeof
argument_list|(
name|ibuf
argument_list|)
decl_stmt|;
if|if
condition|(
name|read_in_full
argument_list|(
name|fd
argument_list|,
name|ibuf
argument_list|,
name|rsize
argument_list|)
operator|!=
name|rsize
condition|)
name|die
argument_list|(
literal|"failed to read %d bytes from '%s'"
argument_list|,
operator|(
name|int
operator|)
name|rsize
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|offset
operator|+=
name|rsize
expr_stmt|;
if|if
condition|(
operator|*
name|already_hashed_to
operator|<
name|offset
condition|)
block|{
name|size_t
name|hsize
init|=
name|offset
operator|-
operator|*
name|already_hashed_to
decl_stmt|;
if|if
condition|(
name|rsize
operator|<
name|hsize
condition|)
name|hsize
operator|=
name|rsize
expr_stmt|;
if|if
condition|(
name|hsize
condition|)
name|git_SHA1_Update
argument_list|(
name|ctx
argument_list|,
name|ibuf
argument_list|,
name|hsize
argument_list|)
expr_stmt|;
operator|*
name|already_hashed_to
operator|=
name|offset
expr_stmt|;
block|}
name|s
operator|.
name|next_in
operator|=
name|ibuf
expr_stmt|;
name|s
operator|.
name|avail_in
operator|=
name|rsize
expr_stmt|;
name|size
operator|-=
name|rsize
expr_stmt|;
block|}
name|status
operator|=
name|git_deflate
argument_list|(
operator|&
name|s
argument_list|,
name|size
condition|?
literal|0
else|:
name|Z_FINISH
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|s
operator|.
name|avail_out
operator|||
name|status
operator|==
name|Z_STREAM_END
condition|)
block|{
if|if
condition|(
name|write_object
condition|)
block|{
name|size_t
name|written
init|=
name|s
operator|.
name|next_out
operator|-
name|obuf
decl_stmt|;
comment|/* would we bust the size limit? */
if|if
condition|(
name|state
operator|->
name|nr_written
operator|&&
name|pack_size_limit_cfg
operator|&&
name|pack_size_limit_cfg
operator|<
name|state
operator|->
name|offset
operator|+
name|written
condition|)
block|{
name|git_deflate_abort
argument_list|(
operator|&
name|s
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|sha1write
argument_list|(
name|state
operator|->
name|f
argument_list|,
name|obuf
argument_list|,
name|written
argument_list|)
expr_stmt|;
name|state
operator|->
name|offset
operator|+=
name|written
expr_stmt|;
block|}
name|s
operator|.
name|next_out
operator|=
name|obuf
expr_stmt|;
name|s
operator|.
name|avail_out
operator|=
sizeof|sizeof
argument_list|(
name|obuf
argument_list|)
expr_stmt|;
block|}
switch|switch
condition|(
name|status
condition|)
block|{
case|case
name|Z_OK
case|:
case|case
name|Z_BUF_ERROR
case|:
case|case
name|Z_STREAM_END
case|:
continue|continue;
default|default:
name|die
argument_list|(
literal|"unexpected deflate failure: %d"
argument_list|,
name|status
argument_list|)
expr_stmt|;
block|}
block|}
name|git_deflate_end
argument_list|(
operator|&
name|s
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/* Lazily create backing packfile for the state */
end_comment
begin_function
DECL|function|prepare_to_stream
specifier|static
name|void
name|prepare_to_stream
parameter_list|(
name|struct
name|bulk_checkin_state
modifier|*
name|state
parameter_list|,
name|unsigned
name|flags
parameter_list|)
block|{
if|if
condition|(
operator|!
operator|(
name|flags
operator|&
name|HASH_WRITE_OBJECT
operator|)
operator|||
name|state
operator|->
name|f
condition|)
return|return;
name|state
operator|->
name|f
operator|=
name|create_tmp_packfile
argument_list|(
operator|&
name|state
operator|->
name|pack_tmp_name
argument_list|)
expr_stmt|;
name|reset_pack_idx_option
argument_list|(
operator|&
name|state
operator|->
name|pack_idx_opts
argument_list|)
expr_stmt|;
comment|/* Pretend we are going to write only one object */
name|state
operator|->
name|offset
operator|=
name|write_pack_header
argument_list|(
name|state
operator|->
name|f
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|state
operator|->
name|offset
condition|)
name|die_errno
argument_list|(
literal|"unable to write pack header"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|deflate_to_pack
specifier|static
name|int
name|deflate_to_pack
parameter_list|(
name|struct
name|bulk_checkin_state
modifier|*
name|state
parameter_list|,
name|unsigned
name|char
name|result_sha1
index|[]
parameter_list|,
name|int
name|fd
parameter_list|,
name|size_t
name|size
parameter_list|,
name|enum
name|object_type
name|type
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|flags
parameter_list|)
block|{
name|off_t
name|seekback
decl_stmt|,
name|already_hashed_to
decl_stmt|;
name|git_SHA_CTX
name|ctx
decl_stmt|;
name|unsigned
name|char
name|obuf
index|[
literal|16384
index|]
decl_stmt|;
name|unsigned
name|header_len
decl_stmt|;
name|struct
name|sha1file_checkpoint
name|checkpoint
decl_stmt|;
name|struct
name|pack_idx_entry
modifier|*
name|idx
init|=
name|NULL
decl_stmt|;
name|seekback
operator|=
name|lseek
argument_list|(
name|fd
argument_list|,
literal|0
argument_list|,
name|SEEK_CUR
argument_list|)
expr_stmt|;
if|if
condition|(
name|seekback
operator|==
operator|(
name|off_t
operator|)
operator|-
literal|1
condition|)
return|return
name|error
argument_list|(
literal|"cannot find the current offset"
argument_list|)
return|;
name|header_len
operator|=
name|sprintf
argument_list|(
operator|(
name|char
operator|*
operator|)
name|obuf
argument_list|,
literal|"%s %"
name|PRIuMAX
argument_list|,
name|typename
argument_list|(
name|type
argument_list|)
argument_list|,
operator|(
name|uintmax_t
operator|)
name|size
argument_list|)
operator|+
literal|1
expr_stmt|;
name|git_SHA1_Init
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|git_SHA1_Update
argument_list|(
operator|&
name|ctx
argument_list|,
name|obuf
argument_list|,
name|header_len
argument_list|)
expr_stmt|;
comment|/* Note: idx is non-NULL when we are writing */
if|if
condition|(
operator|(
name|flags
operator|&
name|HASH_WRITE_OBJECT
operator|)
operator|!=
literal|0
condition|)
name|idx
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|idx
argument_list|)
argument_list|)
expr_stmt|;
name|already_hashed_to
operator|=
literal|0
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|prepare_to_stream
argument_list|(
name|state
argument_list|,
name|flags
argument_list|)
expr_stmt|;
if|if
condition|(
name|idx
condition|)
block|{
name|sha1file_checkpoint
argument_list|(
name|state
operator|->
name|f
argument_list|,
operator|&
name|checkpoint
argument_list|)
expr_stmt|;
name|idx
operator|->
name|offset
operator|=
name|state
operator|->
name|offset
expr_stmt|;
name|crc32_begin
argument_list|(
name|state
operator|->
name|f
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|stream_to_pack
argument_list|(
name|state
argument_list|,
operator|&
name|ctx
argument_list|,
operator|&
name|already_hashed_to
argument_list|,
name|fd
argument_list|,
name|size
argument_list|,
name|type
argument_list|,
name|path
argument_list|,
name|flags
argument_list|)
condition|)
break|break;
comment|/* 		 * Writing this object to the current pack will make 		 * it too big; we need to truncate it, start a new 		 * pack, and write into it. 		 */
if|if
condition|(
operator|!
name|idx
condition|)
name|die
argument_list|(
literal|"BUG: should not happen"
argument_list|)
expr_stmt|;
name|sha1file_truncate
argument_list|(
name|state
operator|->
name|f
argument_list|,
operator|&
name|checkpoint
argument_list|)
expr_stmt|;
name|state
operator|->
name|offset
operator|=
name|checkpoint
operator|.
name|offset
expr_stmt|;
name|finish_bulk_checkin
argument_list|(
name|state
argument_list|)
expr_stmt|;
if|if
condition|(
name|lseek
argument_list|(
name|fd
argument_list|,
name|seekback
argument_list|,
name|SEEK_SET
argument_list|)
operator|==
operator|(
name|off_t
operator|)
operator|-
literal|1
condition|)
return|return
name|error
argument_list|(
literal|"cannot seek back"
argument_list|)
return|;
block|}
name|git_SHA1_Final
argument_list|(
name|result_sha1
argument_list|,
operator|&
name|ctx
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|idx
condition|)
return|return
literal|0
return|;
name|idx
operator|->
name|crc32
operator|=
name|crc32_end
argument_list|(
name|state
operator|->
name|f
argument_list|)
expr_stmt|;
if|if
condition|(
name|already_written
argument_list|(
name|state
argument_list|,
name|result_sha1
argument_list|)
condition|)
block|{
name|sha1file_truncate
argument_list|(
name|state
operator|->
name|f
argument_list|,
operator|&
name|checkpoint
argument_list|)
expr_stmt|;
name|state
operator|->
name|offset
operator|=
name|checkpoint
operator|.
name|offset
expr_stmt|;
name|free
argument_list|(
name|idx
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|hashcpy
argument_list|(
name|idx
operator|->
name|sha1
argument_list|,
name|result_sha1
argument_list|)
expr_stmt|;
name|ALLOC_GROW
argument_list|(
name|state
operator|->
name|written
argument_list|,
name|state
operator|->
name|nr_written
operator|+
literal|1
argument_list|,
name|state
operator|->
name|alloc_written
argument_list|)
expr_stmt|;
name|state
operator|->
name|written
index|[
name|state
operator|->
name|nr_written
operator|++
index|]
operator|=
name|idx
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|index_bulk_checkin
name|int
name|index_bulk_checkin
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|fd
parameter_list|,
name|size_t
name|size
parameter_list|,
name|enum
name|object_type
name|type
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|unsigned
name|flags
parameter_list|)
block|{
name|int
name|status
init|=
name|deflate_to_pack
argument_list|(
operator|&
name|state
argument_list|,
name|sha1
argument_list|,
name|fd
argument_list|,
name|size
argument_list|,
name|type
argument_list|,
name|path
argument_list|,
name|flags
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|state
operator|.
name|plugged
condition|)
name|finish_bulk_checkin
argument_list|(
operator|&
name|state
argument_list|)
expr_stmt|;
return|return
name|status
return|;
block|}
end_function
begin_function
DECL|function|plug_bulk_checkin
name|void
name|plug_bulk_checkin
parameter_list|(
name|void
parameter_list|)
block|{
name|state
operator|.
name|plugged
operator|=
literal|1
expr_stmt|;
block|}
end_function
begin_function
DECL|function|unplug_bulk_checkin
name|void
name|unplug_bulk_checkin
parameter_list|(
name|void
parameter_list|)
block|{
name|state
operator|.
name|plugged
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|state
operator|.
name|f
condition|)
name|finish_bulk_checkin
argument_list|(
operator|&
name|state
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
