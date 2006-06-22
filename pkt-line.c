begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"pkt-line.h"
end_include
begin_comment
comment|/*  * Write a packetized stream, where each line is preceded by  * its length (including the header) as a 4-byte hex number.  * A length of 'zero' means end of stream (and a length of 1-3  * would be an error).   *  * This is all pretty stupid, but we use this packetized line  * format to make a streaming format possible without ever  * over-running the read buffers. That way we'll never read  * into what might be the pack data (which should go to another  * process entirely).  *  * The writing side could use stdio, but since the reading  * side can't, we stay with pure read/write interfaces.  */
end_comment
begin_function
DECL|function|safe_write
name|ssize_t
name|safe_write
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|void
modifier|*
name|buf
parameter_list|,
name|ssize_t
name|n
parameter_list|)
block|{
name|ssize_t
name|nn
init|=
name|n
decl_stmt|;
while|while
condition|(
name|n
condition|)
block|{
name|int
name|ret
init|=
name|xwrite
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|n
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
operator|>
literal|0
condition|)
block|{
name|buf
operator|=
operator|(
name|char
operator|*
operator|)
name|buf
operator|+
name|ret
expr_stmt|;
name|n
operator|-=
name|ret
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"write error (disk full?)"
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"write error (%s)"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
return|return
name|nn
return|;
block|}
end_function
begin_comment
comment|/*  * If we buffered things up above (we don't, but we should),  * we'd flush it here  */
end_comment
begin_function
DECL|function|packet_flush
name|void
name|packet_flush
parameter_list|(
name|int
name|fd
parameter_list|)
block|{
name|safe_write
argument_list|(
name|fd
argument_list|,
literal|"0000"
argument_list|,
literal|4
argument_list|)
expr_stmt|;
block|}
end_function
begin_define
DECL|macro|hex
define|#
directive|define
name|hex
parameter_list|(
name|a
parameter_list|)
value|(hexchar[(a)& 15])
end_define
begin_function
DECL|function|packet_write
name|void
name|packet_write
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
specifier|static
name|char
name|buffer
index|[
literal|1000
index|]
decl_stmt|;
specifier|static
name|char
name|hexchar
index|[]
init|=
literal|"0123456789abcdef"
decl_stmt|;
name|va_list
name|args
decl_stmt|;
name|unsigned
name|n
decl_stmt|;
name|va_start
argument_list|(
name|args
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|n
operator|=
name|vsnprintf
argument_list|(
name|buffer
operator|+
literal|4
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
operator|-
literal|4
argument_list|,
name|fmt
argument_list|,
name|args
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|args
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
operator|>=
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
operator|-
literal|4
condition|)
name|die
argument_list|(
literal|"protocol error: impossibly long line"
argument_list|)
expr_stmt|;
name|n
operator|+=
literal|4
expr_stmt|;
name|buffer
index|[
literal|0
index|]
operator|=
name|hex
argument_list|(
name|n
operator|>>
literal|12
argument_list|)
expr_stmt|;
name|buffer
index|[
literal|1
index|]
operator|=
name|hex
argument_list|(
name|n
operator|>>
literal|8
argument_list|)
expr_stmt|;
name|buffer
index|[
literal|2
index|]
operator|=
name|hex
argument_list|(
name|n
operator|>>
literal|4
argument_list|)
expr_stmt|;
name|buffer
index|[
literal|3
index|]
operator|=
name|hex
argument_list|(
name|n
argument_list|)
expr_stmt|;
name|safe_write
argument_list|(
name|fd
argument_list|,
name|buffer
argument_list|,
name|n
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|safe_read
specifier|static
name|void
name|safe_read
parameter_list|(
name|int
name|fd
parameter_list|,
name|void
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|size
parameter_list|)
block|{
name|int
name|n
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|n
operator|<
name|size
condition|)
block|{
name|int
name|ret
init|=
name|xread
argument_list|(
name|fd
argument_list|,
operator|(
name|char
operator|*
operator|)
name|buffer
operator|+
name|n
argument_list|,
name|size
operator|-
name|n
argument_list|)
decl_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"read error (%s)"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"unexpected EOF"
argument_list|)
expr_stmt|;
name|n
operator|+=
name|ret
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|packet_read_line
name|int
name|packet_read_line
parameter_list|(
name|int
name|fd
parameter_list|,
name|char
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|size
parameter_list|)
block|{
name|int
name|n
decl_stmt|;
name|unsigned
name|len
decl_stmt|;
name|char
name|linelen
index|[
literal|4
index|]
decl_stmt|;
name|safe_read
argument_list|(
name|fd
argument_list|,
name|linelen
argument_list|,
literal|4
argument_list|)
expr_stmt|;
name|len
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|n
operator|=
literal|0
init|;
name|n
operator|<
literal|4
condition|;
name|n
operator|++
control|)
block|{
name|unsigned
name|char
name|c
init|=
name|linelen
index|[
name|n
index|]
decl_stmt|;
name|len
operator|<<=
literal|4
expr_stmt|;
if|if
condition|(
name|c
operator|>=
literal|'0'
operator|&&
name|c
operator|<=
literal|'9'
condition|)
block|{
name|len
operator|+=
name|c
operator|-
literal|'0'
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|c
operator|>=
literal|'a'
operator|&&
name|c
operator|<=
literal|'f'
condition|)
block|{
name|len
operator|+=
name|c
operator|-
literal|'a'
operator|+
literal|10
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
name|c
operator|>=
literal|'A'
operator|&&
name|c
operator|<=
literal|'F'
condition|)
block|{
name|len
operator|+=
name|c
operator|-
literal|'A'
operator|+
literal|10
expr_stmt|;
continue|continue;
block|}
name|die
argument_list|(
literal|"protocol error: bad line length character"
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|len
condition|)
return|return
literal|0
return|;
name|len
operator|-=
literal|4
expr_stmt|;
if|if
condition|(
name|len
operator|>=
name|size
condition|)
name|die
argument_list|(
literal|"protocol error: bad line length %d"
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|safe_read
argument_list|(
name|fd
argument_list|,
name|buffer
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|buffer
index|[
name|len
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|len
return|;
block|}
end_function
end_unit
