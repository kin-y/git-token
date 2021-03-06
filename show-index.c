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
begin_decl_stmt
DECL|variable|show_index_usage
specifier|static
specifier|const
name|char
name|show_index_usage
index|[]
init|=
literal|"git show-index"
decl_stmt|;
end_decl_stmt
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
name|nr
decl_stmt|;
name|unsigned
name|int
name|version
decl_stmt|;
specifier|static
name|unsigned
name|int
name|top_index
index|[
literal|256
index|]
decl_stmt|;
name|git_setup_gettext
argument_list|()
expr_stmt|;
if|if
condition|(
name|argc
operator|!=
literal|1
condition|)
name|usage
argument_list|(
name|show_index_usage
argument_list|)
expr_stmt|;
if|if
condition|(
name|fread
argument_list|(
name|top_index
argument_list|,
literal|2
operator|*
literal|4
argument_list|,
literal|1
argument_list|,
name|stdin
argument_list|)
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"unable to read header"
argument_list|)
expr_stmt|;
if|if
condition|(
name|top_index
index|[
literal|0
index|]
operator|==
name|htonl
argument_list|(
name|PACK_IDX_SIGNATURE
argument_list|)
condition|)
block|{
name|version
operator|=
name|ntohl
argument_list|(
name|top_index
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|version
operator|<
literal|2
operator|||
name|version
operator|>
literal|2
condition|)
name|die
argument_list|(
literal|"unknown index version"
argument_list|)
expr_stmt|;
if|if
condition|(
name|fread
argument_list|(
name|top_index
argument_list|,
literal|256
operator|*
literal|4
argument_list|,
literal|1
argument_list|,
name|stdin
argument_list|)
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"unable to read index"
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|version
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|fread
argument_list|(
operator|&
name|top_index
index|[
literal|2
index|]
argument_list|,
literal|254
operator|*
literal|4
argument_list|,
literal|1
argument_list|,
name|stdin
argument_list|)
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"unable to read index"
argument_list|)
expr_stmt|;
block|}
name|nr
operator|=
literal|0
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
name|unsigned
name|n
init|=
name|ntohl
argument_list|(
name|top_index
index|[
name|i
index|]
argument_list|)
decl_stmt|;
if|if
condition|(
name|n
operator|<
name|nr
condition|)
name|die
argument_list|(
literal|"corrupt index file"
argument_list|)
expr_stmt|;
name|nr
operator|=
name|n
expr_stmt|;
block|}
if|if
condition|(
name|version
operator|==
literal|1
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|unsigned
name|int
name|offset
decl_stmt|,
name|entry
index|[
literal|6
index|]
decl_stmt|;
if|if
condition|(
name|fread
argument_list|(
name|entry
argument_list|,
literal|4
operator|+
literal|20
argument_list|,
literal|1
argument_list|,
name|stdin
argument_list|)
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"unable to read entry %u/%u"
argument_list|,
name|i
argument_list|,
name|nr
argument_list|)
expr_stmt|;
name|offset
operator|=
name|ntohl
argument_list|(
name|entry
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%u %s\n"
argument_list|,
name|offset
argument_list|,
name|sha1_to_hex
argument_list|(
operator|(
name|void
operator|*
operator|)
operator|(
name|entry
operator|+
literal|1
operator|)
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
else|else
block|{
name|unsigned
name|off64_nr
init|=
literal|0
decl_stmt|;
struct|struct
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|uint32_t
name|crc
decl_stmt|;
name|uint32_t
name|off
decl_stmt|;
block|}
modifier|*
name|entries
struct|;
name|ALLOC_ARRAY
argument_list|(
name|entries
argument_list|,
name|nr
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
name|nr
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|fread
argument_list|(
name|entries
index|[
name|i
index|]
operator|.
name|sha1
argument_list|,
literal|20
argument_list|,
literal|1
argument_list|,
name|stdin
argument_list|)
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"unable to read sha1 %u/%u"
argument_list|,
name|i
argument_list|,
name|nr
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
name|nr
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|fread
argument_list|(
operator|&
name|entries
index|[
name|i
index|]
operator|.
name|crc
argument_list|,
literal|4
argument_list|,
literal|1
argument_list|,
name|stdin
argument_list|)
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"unable to read crc %u/%u"
argument_list|,
name|i
argument_list|,
name|nr
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
name|nr
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|fread
argument_list|(
operator|&
name|entries
index|[
name|i
index|]
operator|.
name|off
argument_list|,
literal|4
argument_list|,
literal|1
argument_list|,
name|stdin
argument_list|)
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"unable to read 32b offset %u/%u"
argument_list|,
name|i
argument_list|,
name|nr
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
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|uint64_t
name|offset
decl_stmt|;
name|uint32_t
name|off
init|=
name|ntohl
argument_list|(
name|entries
index|[
name|i
index|]
operator|.
name|off
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|off
operator|&
literal|0x80000000
operator|)
condition|)
block|{
name|offset
operator|=
name|off
expr_stmt|;
block|}
else|else
block|{
name|uint32_t
name|off64
index|[
literal|2
index|]
decl_stmt|;
if|if
condition|(
operator|(
name|off
operator|&
literal|0x7fffffff
operator|)
operator|!=
name|off64_nr
condition|)
name|die
argument_list|(
literal|"inconsistent 64b offset index"
argument_list|)
expr_stmt|;
if|if
condition|(
name|fread
argument_list|(
name|off64
argument_list|,
literal|8
argument_list|,
literal|1
argument_list|,
name|stdin
argument_list|)
operator|!=
literal|1
condition|)
name|die
argument_list|(
literal|"unable to read 64b offset %u"
argument_list|,
name|off64_nr
argument_list|)
expr_stmt|;
name|offset
operator|=
operator|(
operator|(
operator|(
name|uint64_t
operator|)
name|ntohl
argument_list|(
name|off64
index|[
literal|0
index|]
argument_list|)
operator|)
operator|<<
literal|32
operator|)
operator||
name|ntohl
argument_list|(
name|off64
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|off64_nr
operator|++
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"%"
name|PRIuMAX
literal|" %s (%08"
name|PRIx32
literal|")\n"
argument_list|,
operator|(
name|uintmax_t
operator|)
name|offset
argument_list|,
name|sha1_to_hex
argument_list|(
name|entries
index|[
name|i
index|]
operator|.
name|sha1
argument_list|)
argument_list|,
name|ntohl
argument_list|(
name|entries
index|[
name|i
index|]
operator|.
name|crc
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|entries
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
