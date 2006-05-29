begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"pack.h"
end_include
begin_function
DECL|function|verify_packfile
specifier|static
name|int
name|verify_packfile
parameter_list|(
name|struct
name|packed_git
modifier|*
name|p
parameter_list|)
block|{
name|unsigned
name|long
name|index_size
init|=
name|p
operator|->
name|index_size
decl_stmt|;
name|void
modifier|*
name|index_base
init|=
name|p
operator|->
name|index_base
decl_stmt|;
name|SHA_CTX
name|ctx
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|long
name|pack_size
init|=
name|p
operator|->
name|pack_size
decl_stmt|;
name|void
modifier|*
name|pack_base
decl_stmt|;
name|struct
name|pack_header
modifier|*
name|hdr
decl_stmt|;
name|int
name|nr_objects
decl_stmt|,
name|err
decl_stmt|,
name|i
decl_stmt|;
comment|/* Header consistency check */
name|hdr
operator|=
name|p
operator|->
name|pack_base
expr_stmt|;
if|if
condition|(
name|hdr
operator|->
name|hdr_signature
operator|!=
name|htonl
argument_list|(
name|PACK_SIGNATURE
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Packfile %s signature mismatch"
argument_list|,
name|p
operator|->
name|pack_name
argument_list|)
return|;
if|if
condition|(
operator|!
name|pack_version_ok
argument_list|(
name|hdr
operator|->
name|hdr_version
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Packfile version %d unsupported"
argument_list|,
name|ntohl
argument_list|(
name|hdr
operator|->
name|hdr_version
argument_list|)
argument_list|)
return|;
name|nr_objects
operator|=
name|ntohl
argument_list|(
name|hdr
operator|->
name|hdr_entries
argument_list|)
expr_stmt|;
if|if
condition|(
name|num_packed_objects
argument_list|(
name|p
argument_list|)
operator|!=
name|nr_objects
condition|)
return|return
name|error
argument_list|(
literal|"Packfile claims to have %d objects, "
literal|"while idx size expects %d"
argument_list|,
name|nr_objects
argument_list|,
name|num_packed_objects
argument_list|(
name|p
argument_list|)
argument_list|)
return|;
name|SHA1_Init
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|pack_base
operator|=
name|p
operator|->
name|pack_base
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|ctx
argument_list|,
name|pack_base
argument_list|,
name|pack_size
operator|-
literal|20
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
name|sha1
argument_list|,
name|pack_base
operator|+
name|pack_size
operator|-
literal|20
argument_list|,
literal|20
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Packfile %s SHA1 mismatch with itself"
argument_list|,
name|p
operator|->
name|pack_name
argument_list|)
return|;
if|if
condition|(
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|index_base
operator|+
name|index_size
operator|-
literal|40
argument_list|,
literal|20
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Packfile %s SHA1 mismatch with idx"
argument_list|,
name|p
operator|->
name|pack_name
argument_list|)
return|;
comment|/* Make sure everything reachable from idx is valid.  Since we 	 * have verified that nr_objects matches between idx and pack, 	 * we do not do scan-streaming check on the pack file. 	 */
for|for
control|(
name|i
operator|=
name|err
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
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|pack_entry
name|e
decl_stmt|;
name|void
modifier|*
name|data
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
if|if
condition|(
name|nth_packed_object_sha1
argument_list|(
name|p
argument_list|,
name|i
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"internal error pack-check nth-packed-object"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|find_pack_entry_one
argument_list|(
name|sha1
argument_list|,
operator|&
name|e
argument_list|,
name|p
argument_list|)
condition|)
name|die
argument_list|(
literal|"internal error pack-check find-pack-entry-one"
argument_list|)
expr_stmt|;
name|data
operator|=
name|unpack_entry_gently
argument_list|(
operator|&
name|e
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|data
condition|)
block|{
name|err
operator|=
name|error
argument_list|(
literal|"cannot unpack %s from %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|p
operator|->
name|pack_name
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|check_sha1_signature
argument_list|(
name|sha1
argument_list|,
name|data
argument_list|,
name|size
argument_list|,
name|type
argument_list|)
condition|)
block|{
name|err
operator|=
name|error
argument_list|(
literal|"packed %s from %s is corrupt"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|p
operator|->
name|pack_name
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|data
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|free
argument_list|(
name|data
argument_list|)
expr_stmt|;
block|}
return|return
name|err
return|;
block|}
end_function
begin_define
DECL|macro|MAX_CHAIN
define|#
directive|define
name|MAX_CHAIN
value|40
end_define
begin_function
DECL|function|show_pack_info
specifier|static
name|void
name|show_pack_info
parameter_list|(
name|struct
name|packed_git
modifier|*
name|p
parameter_list|)
block|{
name|struct
name|pack_header
modifier|*
name|hdr
decl_stmt|;
name|int
name|nr_objects
decl_stmt|,
name|i
decl_stmt|;
name|unsigned
name|int
name|chain_histogram
index|[
name|MAX_CHAIN
index|]
decl_stmt|;
name|hdr
operator|=
name|p
operator|->
name|pack_base
expr_stmt|;
name|nr_objects
operator|=
name|ntohl
argument_list|(
name|hdr
operator|->
name|hdr_entries
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|chain_histogram
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|chain_histogram
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
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|,
name|base_sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|pack_entry
name|e
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|unsigned
name|long
name|store_size
decl_stmt|;
name|unsigned
name|int
name|delta_chain_length
decl_stmt|;
if|if
condition|(
name|nth_packed_object_sha1
argument_list|(
name|p
argument_list|,
name|i
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"internal error pack-check nth-packed-object"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|find_pack_entry_one
argument_list|(
name|sha1
argument_list|,
operator|&
name|e
argument_list|,
name|p
argument_list|)
condition|)
name|die
argument_list|(
literal|"internal error pack-check find-pack-entry-one"
argument_list|)
expr_stmt|;
name|packed_object_info_detail
argument_list|(
operator|&
name|e
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|,
operator|&
name|store_size
argument_list|,
operator|&
name|delta_chain_length
argument_list|,
name|base_sha1
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s "
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|delta_chain_length
condition|)
name|printf
argument_list|(
literal|"%-6s %lu %u\n"
argument_list|,
name|type
argument_list|,
name|size
argument_list|,
name|e
operator|.
name|offset
argument_list|)
expr_stmt|;
else|else
block|{
name|printf
argument_list|(
literal|"%-6s %lu %u %u %s\n"
argument_list|,
name|type
argument_list|,
name|size
argument_list|,
name|e
operator|.
name|offset
argument_list|,
name|delta_chain_length
argument_list|,
name|sha1_to_hex
argument_list|(
name|base_sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|delta_chain_length
operator|<
name|MAX_CHAIN
condition|)
name|chain_histogram
index|[
name|delta_chain_length
index|]
operator|++
expr_stmt|;
else|else
name|chain_histogram
index|[
literal|0
index|]
operator|++
expr_stmt|;
block|}
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|MAX_CHAIN
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|chain_histogram
index|[
name|i
index|]
condition|)
continue|continue;
name|printf
argument_list|(
literal|"chain length %s %d: %d object%s\n"
argument_list|,
name|i
condition|?
literal|"="
else|:
literal|">="
argument_list|,
name|i
condition|?
name|i
else|:
name|MAX_CHAIN
argument_list|,
name|chain_histogram
index|[
name|i
index|]
argument_list|,
literal|1
operator|<
name|chain_histogram
index|[
name|i
index|]
condition|?
literal|"s"
else|:
literal|""
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|verify_pack
name|int
name|verify_pack
parameter_list|(
name|struct
name|packed_git
modifier|*
name|p
parameter_list|,
name|int
name|verbose
parameter_list|)
block|{
name|unsigned
name|long
name|index_size
init|=
name|p
operator|->
name|index_size
decl_stmt|;
name|void
modifier|*
name|index_base
init|=
name|p
operator|->
name|index_base
decl_stmt|;
name|SHA_CTX
name|ctx
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|ret
operator|=
literal|0
expr_stmt|;
comment|/* Verify SHA1 sum of the index file */
name|SHA1_Init
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
name|SHA1_Update
argument_list|(
operator|&
name|ctx
argument_list|,
name|index_base
argument_list|,
name|index_size
operator|-
literal|20
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
name|sha1
argument_list|,
name|index_base
operator|+
name|index_size
operator|-
literal|20
argument_list|,
literal|20
argument_list|)
condition|)
name|ret
operator|=
name|error
argument_list|(
literal|"Packfile index for %s SHA1 mismatch"
argument_list|,
name|p
operator|->
name|pack_name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
block|{
comment|/* Verify pack file */
name|use_packed_git
argument_list|(
name|p
argument_list|)
expr_stmt|;
name|ret
operator|=
name|verify_packfile
argument_list|(
name|p
argument_list|)
expr_stmt|;
name|unuse_packed_git
argument_list|(
name|p
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|verbose
condition|)
block|{
if|if
condition|(
name|ret
condition|)
name|printf
argument_list|(
literal|"%s: bad\n"
argument_list|,
name|p
operator|->
name|pack_name
argument_list|)
expr_stmt|;
else|else
block|{
name|use_packed_git
argument_list|(
name|p
argument_list|)
expr_stmt|;
name|show_pack_info
argument_list|(
name|p
argument_list|)
expr_stmt|;
name|unuse_packed_git
argument_list|(
name|p
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s: ok\n"
argument_list|,
name|p
operator|->
name|pack_name
argument_list|)
expr_stmt|;
block|}
block|}
return|return
name|ret
return|;
block|}
end_function
end_unit
