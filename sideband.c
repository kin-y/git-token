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
begin_include
include|#
directive|include
file|"sideband.h"
end_include
begin_comment
comment|/*  * Receive multiplexed output stream over git native protocol.  * in_stream is the input stream from the remote, which carries data  * in pkt_line format with band designator.  Demultiplex it into out  * and err and return error appropriately.  Band #1 carries the  * primary payload.  Things coming over band #2 is not necessarily  * error; they are usually informative message on the standard error  * stream, aka "verbose").  A message over band #3 is a signal that  * the remote died unexpectedly.  A flush() concludes the stream.  */
end_comment
begin_define
DECL|macro|PREFIX
define|#
directive|define
name|PREFIX
value|"remote:"
end_define
begin_define
DECL|macro|ANSI_SUFFIX
define|#
directive|define
name|ANSI_SUFFIX
value|"\033[K"
end_define
begin_define
DECL|macro|DUMB_SUFFIX
define|#
directive|define
name|DUMB_SUFFIX
value|"        "
end_define
begin_define
DECL|macro|FIX_SIZE
define|#
directive|define
name|FIX_SIZE
value|10
end_define
begin_comment
DECL|macro|FIX_SIZE
comment|/* large enough for any of the above */
end_comment
begin_function
DECL|function|recv_sideband
name|int
name|recv_sideband
parameter_list|(
specifier|const
name|char
modifier|*
name|me
parameter_list|,
name|int
name|in_stream
parameter_list|,
name|int
name|out
parameter_list|)
block|{
name|unsigned
name|pf
init|=
name|strlen
argument_list|(
name|PREFIX
argument_list|)
decl_stmt|;
name|unsigned
name|sf
decl_stmt|;
name|char
name|buf
index|[
name|LARGE_PACKET_MAX
operator|+
literal|2
operator|*
name|FIX_SIZE
index|]
decl_stmt|;
name|char
modifier|*
name|suffix
decl_stmt|,
modifier|*
name|term
decl_stmt|;
name|int
name|skip_pf
init|=
literal|0
decl_stmt|;
name|memcpy
argument_list|(
name|buf
argument_list|,
name|PREFIX
argument_list|,
name|pf
argument_list|)
expr_stmt|;
name|term
operator|=
name|getenv
argument_list|(
literal|"TERM"
argument_list|)
expr_stmt|;
if|if
condition|(
name|isatty
argument_list|(
literal|2
argument_list|)
operator|&&
name|term
operator|&&
name|strcmp
argument_list|(
name|term
argument_list|,
literal|"dumb"
argument_list|)
condition|)
name|suffix
operator|=
name|ANSI_SUFFIX
expr_stmt|;
else|else
name|suffix
operator|=
name|DUMB_SUFFIX
expr_stmt|;
name|sf
operator|=
name|strlen
argument_list|(
name|suffix
argument_list|)
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|int
name|band
decl_stmt|,
name|len
decl_stmt|;
name|len
operator|=
name|packet_read
argument_list|(
name|in_stream
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
name|buf
operator|+
name|pf
argument_list|,
name|LARGE_PACKET_MAX
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|==
literal|0
condition|)
break|break;
if|if
condition|(
name|len
operator|<
literal|1
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s: protocol error: no band designator\n"
argument_list|,
name|me
argument_list|)
expr_stmt|;
return|return
name|SIDEBAND_PROTOCOL_ERROR
return|;
block|}
name|band
operator|=
name|buf
index|[
name|pf
index|]
operator|&
literal|0xff
expr_stmt|;
name|len
operator|--
expr_stmt|;
switch|switch
condition|(
name|band
condition|)
block|{
case|case
literal|3
case|:
name|buf
index|[
name|pf
index|]
operator|=
literal|' '
expr_stmt|;
name|buf
index|[
name|pf
operator|+
literal|1
operator|+
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s\n"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
return|return
name|SIDEBAND_REMOTE_ERROR
return|;
case|case
literal|2
case|:
name|buf
index|[
name|pf
index|]
operator|=
literal|' '
expr_stmt|;
do|do
block|{
name|char
modifier|*
name|b
init|=
name|buf
decl_stmt|;
name|int
name|brk
init|=
literal|0
decl_stmt|;
comment|/* 				 * If the last buffer didn't end with a line 				 * break then we should not print a prefix 				 * this time around. 				 */
if|if
condition|(
name|skip_pf
condition|)
block|{
name|b
operator|+=
name|pf
operator|+
literal|1
expr_stmt|;
block|}
else|else
block|{
name|len
operator|+=
name|pf
operator|+
literal|1
expr_stmt|;
name|brk
operator|+=
name|pf
operator|+
literal|1
expr_stmt|;
block|}
comment|/* Look for a line break. */
for|for
control|(
init|;
condition|;
control|)
block|{
name|brk
operator|++
expr_stmt|;
if|if
condition|(
name|brk
operator|>
name|len
condition|)
block|{
name|brk
operator|=
literal|0
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|b
index|[
name|brk
operator|-
literal|1
index|]
operator|==
literal|'\n'
operator|||
name|b
index|[
name|brk
operator|-
literal|1
index|]
operator|==
literal|'\r'
condition|)
break|break;
block|}
comment|/* 				 * Let's insert a suffix to clear the end 				 * of the screen line if a line break was 				 * found.  Also, if we don't skip the 				 * prefix, then a non-empty string must be 				 * present too. 				 */
if|if
condition|(
name|brk
operator|>
operator|(
name|skip_pf
condition|?
literal|0
else|:
operator|(
name|pf
operator|+
literal|1
operator|+
literal|1
operator|)
operator|)
condition|)
block|{
name|char
name|save
index|[
name|FIX_SIZE
index|]
decl_stmt|;
name|memcpy
argument_list|(
name|save
argument_list|,
name|b
operator|+
name|brk
argument_list|,
name|sf
argument_list|)
expr_stmt|;
name|b
index|[
name|brk
operator|+
name|sf
operator|-
literal|1
index|]
operator|=
name|b
index|[
name|brk
operator|-
literal|1
index|]
expr_stmt|;
name|memcpy
argument_list|(
name|b
operator|+
name|brk
operator|-
literal|1
argument_list|,
name|suffix
argument_list|,
name|sf
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%.*s"
argument_list|,
name|brk
operator|+
name|sf
argument_list|,
name|b
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|b
operator|+
name|brk
argument_list|,
name|save
argument_list|,
name|sf
argument_list|)
expr_stmt|;
name|len
operator|-=
name|brk
expr_stmt|;
block|}
else|else
block|{
name|int
name|l
init|=
name|brk
condition|?
name|brk
else|:
name|len
decl_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%.*s"
argument_list|,
name|l
argument_list|,
name|b
argument_list|)
expr_stmt|;
name|len
operator|-=
name|l
expr_stmt|;
block|}
name|skip_pf
operator|=
operator|!
name|brk
expr_stmt|;
name|memmove
argument_list|(
name|buf
operator|+
name|pf
operator|+
literal|1
argument_list|,
name|b
operator|+
name|brk
argument_list|,
name|len
argument_list|)
expr_stmt|;
block|}
do|while
condition|(
name|len
condition|)
do|;
continue|continue;
case|case
literal|1
case|:
name|write_or_die
argument_list|(
name|out
argument_list|,
name|buf
operator|+
name|pf
operator|+
literal|1
argument_list|,
name|len
argument_list|)
expr_stmt|;
continue|continue;
default|default:
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s: protocol error: bad band #%d\n"
argument_list|,
name|me
argument_list|,
name|band
argument_list|)
expr_stmt|;
return|return
name|SIDEBAND_PROTOCOL_ERROR
return|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * fd is connected to the remote side; send the sideband data  * over multiplexed packet stream.  */
end_comment
begin_function
DECL|function|send_sideband
name|void
name|send_sideband
parameter_list|(
name|int
name|fd
parameter_list|,
name|int
name|band
parameter_list|,
specifier|const
name|char
modifier|*
name|data
parameter_list|,
name|ssize_t
name|sz
parameter_list|,
name|int
name|packet_max
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|p
init|=
name|data
decl_stmt|;
while|while
condition|(
name|sz
condition|)
block|{
name|unsigned
name|n
decl_stmt|;
name|char
name|hdr
index|[
literal|5
index|]
decl_stmt|;
name|n
operator|=
name|sz
expr_stmt|;
if|if
condition|(
name|packet_max
operator|-
literal|5
operator|<
name|n
condition|)
name|n
operator|=
name|packet_max
operator|-
literal|5
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|band
condition|)
block|{
name|xsnprintf
argument_list|(
name|hdr
argument_list|,
sizeof|sizeof
argument_list|(
name|hdr
argument_list|)
argument_list|,
literal|"%04x"
argument_list|,
name|n
operator|+
literal|5
argument_list|)
expr_stmt|;
name|hdr
index|[
literal|4
index|]
operator|=
name|band
expr_stmt|;
name|write_or_die
argument_list|(
name|fd
argument_list|,
name|hdr
argument_list|,
literal|5
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|xsnprintf
argument_list|(
name|hdr
argument_list|,
sizeof|sizeof
argument_list|(
name|hdr
argument_list|)
argument_list|,
literal|"%04x"
argument_list|,
name|n
operator|+
literal|4
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|fd
argument_list|,
name|hdr
argument_list|,
literal|4
argument_list|)
expr_stmt|;
block|}
name|write_or_die
argument_list|(
name|fd
argument_list|,
name|p
argument_list|,
name|n
argument_list|)
expr_stmt|;
name|p
operator|+=
name|n
expr_stmt|;
name|sz
operator|-=
name|n
expr_stmt|;
block|}
block|}
end_function
end_unit
