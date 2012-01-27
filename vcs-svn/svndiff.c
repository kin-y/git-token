begin_unit
begin_comment
comment|/*  * Licensed under a two-clause BSD-style license.  * See LICENSE for details.  */
end_comment
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"sliding_window.h"
end_include
begin_include
include|#
directive|include
file|"line_buffer.h"
end_include
begin_include
include|#
directive|include
file|"svndiff.h"
end_include
begin_comment
comment|/*  * svndiff0 applier  *  * See http://svn.apache.org/repos/asf/subversion/trunk/notes/svndiff.  *  * svndiff0 ::= 'SVN\0' window*  * window ::= int int int int int instructions inline_data;  * instructions ::= instruction*;  * instruction ::= view_selector int int  *   | copyfrom_data int  *   | packed_view_selector int  *   | packed_copyfrom_data  *   ;  * view_selector ::= copyfrom_source  *   | copyfrom_target  *   ;  * copyfrom_source ::= # binary 00 000000;  * copyfrom_target ::= # binary 01 000000;  * copyfrom_data ::= # binary 10 000000;  * packed_view_selector ::= # view_selector OR-ed with 6 bit value;  * packed_copyfrom_data ::= # copyfrom_data OR-ed with 6 bit value;  * int ::= highdigit* lowdigit;  * highdigit ::= # binary 1000 0000 OR-ed with 7 bit value;  * lowdigit ::= # 7 bit value;  */
end_comment
begin_define
DECL|macro|INSN_MASK
define|#
directive|define
name|INSN_MASK
value|0xc0
end_define
begin_define
DECL|macro|INSN_COPYFROM_SOURCE
define|#
directive|define
name|INSN_COPYFROM_SOURCE
value|0x00
end_define
begin_define
DECL|macro|INSN_COPYFROM_TARGET
define|#
directive|define
name|INSN_COPYFROM_TARGET
value|0x40
end_define
begin_define
DECL|macro|INSN_COPYFROM_DATA
define|#
directive|define
name|INSN_COPYFROM_DATA
value|0x80
end_define
begin_define
DECL|macro|OPERAND_MASK
define|#
directive|define
name|OPERAND_MASK
value|0x3f
end_define
begin_define
DECL|macro|VLI_CONTINUE
define|#
directive|define
name|VLI_CONTINUE
value|0x80
end_define
begin_define
DECL|macro|VLI_DIGIT_MASK
define|#
directive|define
name|VLI_DIGIT_MASK
value|0x7f
end_define
begin_define
DECL|macro|VLI_BITS_PER_DIGIT
define|#
directive|define
name|VLI_BITS_PER_DIGIT
value|7
end_define
begin_struct
DECL|struct|window
struct|struct
name|window
block|{
DECL|member|in
name|struct
name|sliding_view
modifier|*
name|in
decl_stmt|;
DECL|member|out
name|struct
name|strbuf
name|out
decl_stmt|;
DECL|member|instructions
name|struct
name|strbuf
name|instructions
decl_stmt|;
DECL|member|data
name|struct
name|strbuf
name|data
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|WINDOW_INIT
define|#
directive|define
name|WINDOW_INIT
parameter_list|(
name|w
parameter_list|)
value|{ (w), STRBUF_INIT, STRBUF_INIT, STRBUF_INIT }
end_define
begin_function
DECL|function|window_release
specifier|static
name|void
name|window_release
parameter_list|(
name|struct
name|window
modifier|*
name|ctx
parameter_list|)
block|{
name|strbuf_release
argument_list|(
operator|&
name|ctx
operator|->
name|out
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|ctx
operator|->
name|instructions
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|ctx
operator|->
name|data
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|write_strbuf
specifier|static
name|int
name|write_strbuf
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|FILE
modifier|*
name|out
parameter_list|)
block|{
if|if
condition|(
name|fwrite
argument_list|(
name|sb
operator|->
name|buf
argument_list|,
literal|1
argument_list|,
name|sb
operator|->
name|len
argument_list|,
name|out
argument_list|)
operator|==
name|sb
operator|->
name|len
condition|)
comment|/* Success. */
return|return
literal|0
return|;
return|return
name|error
argument_list|(
literal|"cannot write delta postimage: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|error_short_read
specifier|static
name|int
name|error_short_read
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|input
parameter_list|)
block|{
if|if
condition|(
name|buffer_ferror
argument_list|(
name|input
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"error reading delta: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
return|return
name|error
argument_list|(
literal|"invalid delta: unexpected end of file"
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|read_chunk
specifier|static
name|int
name|read_chunk
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|delta
parameter_list|,
name|off_t
modifier|*
name|delta_len
parameter_list|,
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
name|strbuf_reset
argument_list|(
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|>
operator|*
name|delta_len
operator|||
name|buffer_read_binary
argument_list|(
name|delta
argument_list|,
name|buf
argument_list|,
name|len
argument_list|)
operator|!=
name|len
condition|)
return|return
name|error_short_read
argument_list|(
name|delta
argument_list|)
return|;
operator|*
name|delta_len
operator|-=
name|buf
operator|->
name|len
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_magic
specifier|static
name|int
name|read_magic
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|in
parameter_list|,
name|off_t
modifier|*
name|len
parameter_list|)
block|{
specifier|static
specifier|const
name|char
name|magic
index|[]
init|=
block|{
literal|'S'
block|,
literal|'V'
block|,
literal|'N'
block|,
literal|'\0'
block|}
decl_stmt|;
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
name|read_chunk
argument_list|(
name|in
argument_list|,
name|len
argument_list|,
operator|&
name|sb
argument_list|,
sizeof|sizeof
argument_list|(
name|magic
argument_list|)
argument_list|)
condition|)
block|{
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|memcmp
argument_list|(
name|sb
operator|.
name|buf
argument_list|,
name|magic
argument_list|,
sizeof|sizeof
argument_list|(
name|magic
argument_list|)
argument_list|)
condition|)
block|{
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
literal|"invalid delta: unrecognized file type"
argument_list|)
return|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_int
specifier|static
name|int
name|read_int
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|in
parameter_list|,
name|uintmax_t
modifier|*
name|result
parameter_list|,
name|off_t
modifier|*
name|len
parameter_list|)
block|{
name|uintmax_t
name|rv
init|=
literal|0
decl_stmt|;
name|off_t
name|sz
decl_stmt|;
for|for
control|(
name|sz
operator|=
operator|*
name|len
init|;
name|sz
condition|;
name|sz
operator|--
control|)
block|{
specifier|const
name|int
name|ch
init|=
name|buffer_read_char
argument_list|(
name|in
argument_list|)
decl_stmt|;
if|if
condition|(
name|ch
operator|==
name|EOF
condition|)
break|break;
name|rv
operator|<<=
name|VLI_BITS_PER_DIGIT
expr_stmt|;
name|rv
operator|+=
operator|(
name|ch
operator|&
name|VLI_DIGIT_MASK
operator|)
expr_stmt|;
if|if
condition|(
name|ch
operator|&
name|VLI_CONTINUE
condition|)
continue|continue;
operator|*
name|result
operator|=
name|rv
expr_stmt|;
operator|*
name|len
operator|=
name|sz
operator|-
literal|1
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
name|error_short_read
argument_list|(
name|in
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|parse_int
specifier|static
name|int
name|parse_int
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|buf
parameter_list|,
name|size_t
modifier|*
name|result
parameter_list|,
specifier|const
name|char
modifier|*
name|end
parameter_list|)
block|{
name|size_t
name|rv
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|pos
decl_stmt|;
for|for
control|(
name|pos
operator|=
operator|*
name|buf
init|;
name|pos
operator|!=
name|end
condition|;
name|pos
operator|++
control|)
block|{
name|unsigned
name|char
name|ch
init|=
operator|*
name|pos
decl_stmt|;
name|rv
operator|<<=
name|VLI_BITS_PER_DIGIT
expr_stmt|;
name|rv
operator|+=
operator|(
name|ch
operator|&
name|VLI_DIGIT_MASK
operator|)
expr_stmt|;
if|if
condition|(
name|ch
operator|&
name|VLI_CONTINUE
condition|)
continue|continue;
operator|*
name|result
operator|=
name|rv
expr_stmt|;
operator|*
name|buf
operator|=
name|pos
operator|+
literal|1
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
name|error
argument_list|(
literal|"invalid delta: unexpected end of instructions section"
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|read_offset
specifier|static
name|int
name|read_offset
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|in
parameter_list|,
name|off_t
modifier|*
name|result
parameter_list|,
name|off_t
modifier|*
name|len
parameter_list|)
block|{
name|uintmax_t
name|val
decl_stmt|;
if|if
condition|(
name|read_int
argument_list|(
name|in
argument_list|,
operator|&
name|val
argument_list|,
name|len
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|val
operator|>
name|maximum_signed_value_of_type
argument_list|(
name|off_t
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"unrepresentable offset in delta: %"
name|PRIuMAX
literal|""
argument_list|,
name|val
argument_list|)
return|;
operator|*
name|result
operator|=
name|val
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|read_length
specifier|static
name|int
name|read_length
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|in
parameter_list|,
name|size_t
modifier|*
name|result
parameter_list|,
name|off_t
modifier|*
name|len
parameter_list|)
block|{
name|uintmax_t
name|val
decl_stmt|;
if|if
condition|(
name|read_int
argument_list|(
name|in
argument_list|,
operator|&
name|val
argument_list|,
name|len
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|val
operator|>
name|SIZE_MAX
condition|)
return|return
name|error
argument_list|(
literal|"unrepresentable length in delta: %"
name|PRIuMAX
literal|""
argument_list|,
name|val
argument_list|)
return|;
operator|*
name|result
operator|=
name|val
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|copyfrom_source
specifier|static
name|int
name|copyfrom_source
parameter_list|(
name|struct
name|window
modifier|*
name|ctx
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|instructions
parameter_list|,
name|size_t
name|nbytes
parameter_list|,
specifier|const
name|char
modifier|*
name|insns_end
parameter_list|)
block|{
name|size_t
name|offset
decl_stmt|;
if|if
condition|(
name|parse_int
argument_list|(
name|instructions
argument_list|,
operator|&
name|offset
argument_list|,
name|insns_end
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|unsigned_add_overflows
argument_list|(
name|offset
argument_list|,
name|nbytes
argument_list|)
operator|||
name|offset
operator|+
name|nbytes
operator|>
name|ctx
operator|->
name|in
operator|->
name|width
condition|)
return|return
name|error
argument_list|(
literal|"invalid delta: copies source data outside view"
argument_list|)
return|;
name|strbuf_add
argument_list|(
operator|&
name|ctx
operator|->
name|out
argument_list|,
name|ctx
operator|->
name|in
operator|->
name|buf
operator|.
name|buf
operator|+
name|offset
argument_list|,
name|nbytes
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|copyfrom_target
specifier|static
name|int
name|copyfrom_target
parameter_list|(
name|struct
name|window
modifier|*
name|ctx
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|instructions
parameter_list|,
name|size_t
name|nbytes
parameter_list|,
specifier|const
name|char
modifier|*
name|instructions_end
parameter_list|)
block|{
name|size_t
name|offset
decl_stmt|;
if|if
condition|(
name|parse_int
argument_list|(
name|instructions
argument_list|,
operator|&
name|offset
argument_list|,
name|instructions_end
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|offset
operator|>=
name|ctx
operator|->
name|out
operator|.
name|len
condition|)
return|return
name|error
argument_list|(
literal|"invalid delta: copies from the future"
argument_list|)
return|;
for|for
control|(
init|;
name|nbytes
operator|>
literal|0
condition|;
name|nbytes
operator|--
control|)
name|strbuf_addch
argument_list|(
operator|&
name|ctx
operator|->
name|out
argument_list|,
name|ctx
operator|->
name|out
operator|.
name|buf
index|[
name|offset
operator|++
index|]
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|copyfrom_data
specifier|static
name|int
name|copyfrom_data
parameter_list|(
name|struct
name|window
modifier|*
name|ctx
parameter_list|,
name|size_t
modifier|*
name|data_pos
parameter_list|,
name|size_t
name|nbytes
parameter_list|)
block|{
specifier|const
name|size_t
name|pos
init|=
operator|*
name|data_pos
decl_stmt|;
if|if
condition|(
name|unsigned_add_overflows
argument_list|(
name|pos
argument_list|,
name|nbytes
argument_list|)
operator|||
name|pos
operator|+
name|nbytes
operator|>
name|ctx
operator|->
name|data
operator|.
name|len
condition|)
return|return
name|error
argument_list|(
literal|"invalid delta: copies unavailable inline data"
argument_list|)
return|;
name|strbuf_add
argument_list|(
operator|&
name|ctx
operator|->
name|out
argument_list|,
name|ctx
operator|->
name|data
operator|.
name|buf
operator|+
name|pos
argument_list|,
name|nbytes
argument_list|)
expr_stmt|;
operator|*
name|data_pos
operator|+=
name|nbytes
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|parse_first_operand
specifier|static
name|int
name|parse_first_operand
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|buf
parameter_list|,
name|size_t
modifier|*
name|out
parameter_list|,
specifier|const
name|char
modifier|*
name|end
parameter_list|)
block|{
name|size_t
name|result
init|=
operator|(
name|unsigned
name|char
operator|)
operator|*
operator|(
operator|*
name|buf
operator|)
operator|++
operator|&
name|OPERAND_MASK
decl_stmt|;
if|if
condition|(
name|result
condition|)
block|{
comment|/* immediate operand */
operator|*
name|out
operator|=
name|result
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
name|parse_int
argument_list|(
name|buf
argument_list|,
name|out
argument_list|,
name|end
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|execute_one_instruction
specifier|static
name|int
name|execute_one_instruction
parameter_list|(
name|struct
name|window
modifier|*
name|ctx
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|instructions
parameter_list|,
name|size_t
modifier|*
name|data_pos
parameter_list|)
block|{
name|unsigned
name|int
name|instruction
decl_stmt|;
specifier|const
name|char
modifier|*
name|insns_end
init|=
name|ctx
operator|->
name|instructions
operator|.
name|buf
operator|+
name|ctx
operator|->
name|instructions
operator|.
name|len
decl_stmt|;
name|size_t
name|nbytes
decl_stmt|;
name|assert
argument_list|(
name|ctx
argument_list|)
expr_stmt|;
name|assert
argument_list|(
name|instructions
operator|&&
operator|*
name|instructions
argument_list|)
expr_stmt|;
name|assert
argument_list|(
name|data_pos
argument_list|)
expr_stmt|;
name|instruction
operator|=
operator|(
name|unsigned
name|char
operator|)
operator|*
operator|*
name|instructions
expr_stmt|;
if|if
condition|(
name|parse_first_operand
argument_list|(
name|instructions
argument_list|,
operator|&
name|nbytes
argument_list|,
name|insns_end
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
switch|switch
condition|(
name|instruction
operator|&
name|INSN_MASK
condition|)
block|{
case|case
name|INSN_COPYFROM_SOURCE
case|:
return|return
name|copyfrom_source
argument_list|(
name|ctx
argument_list|,
name|instructions
argument_list|,
name|nbytes
argument_list|,
name|insns_end
argument_list|)
return|;
case|case
name|INSN_COPYFROM_TARGET
case|:
return|return
name|copyfrom_target
argument_list|(
name|ctx
argument_list|,
name|instructions
argument_list|,
name|nbytes
argument_list|,
name|insns_end
argument_list|)
return|;
case|case
name|INSN_COPYFROM_DATA
case|:
return|return
name|copyfrom_data
argument_list|(
name|ctx
argument_list|,
name|data_pos
argument_list|,
name|nbytes
argument_list|)
return|;
default|default:
return|return
name|error
argument_list|(
literal|"invalid delta: unrecognized instruction"
argument_list|)
return|;
block|}
block|}
end_function
begin_function
DECL|function|apply_window_in_core
specifier|static
name|int
name|apply_window_in_core
parameter_list|(
name|struct
name|window
modifier|*
name|ctx
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|instructions
decl_stmt|;
name|size_t
name|data_pos
init|=
literal|0
decl_stmt|;
comment|/* 	 * Fill ctx->out.buf using data from the source, target, 	 * and inline data views. 	 */
for|for
control|(
name|instructions
operator|=
name|ctx
operator|->
name|instructions
operator|.
name|buf
init|;
name|instructions
operator|!=
name|ctx
operator|->
name|instructions
operator|.
name|buf
operator|+
name|ctx
operator|->
name|instructions
operator|.
name|len
condition|;
control|)
if|if
condition|(
name|execute_one_instruction
argument_list|(
name|ctx
argument_list|,
operator|&
name|instructions
argument_list|,
operator|&
name|data_pos
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|data_pos
operator|!=
name|ctx
operator|->
name|data
operator|.
name|len
condition|)
return|return
name|error
argument_list|(
literal|"invalid delta: does not copy all inline data"
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|apply_one_window
specifier|static
name|int
name|apply_one_window
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|delta
parameter_list|,
name|off_t
modifier|*
name|delta_len
parameter_list|,
name|struct
name|sliding_view
modifier|*
name|preimage
parameter_list|,
name|FILE
modifier|*
name|out
parameter_list|)
block|{
name|struct
name|window
name|ctx
init|=
name|WINDOW_INIT
argument_list|(
name|preimage
argument_list|)
decl_stmt|;
name|size_t
name|out_len
decl_stmt|;
name|size_t
name|instructions_len
decl_stmt|;
name|size_t
name|data_len
decl_stmt|;
name|assert
argument_list|(
name|delta_len
argument_list|)
expr_stmt|;
comment|/* "source view" offset and length already handled; */
if|if
condition|(
name|read_length
argument_list|(
name|delta
argument_list|,
operator|&
name|out_len
argument_list|,
name|delta_len
argument_list|)
operator|||
name|read_length
argument_list|(
name|delta
argument_list|,
operator|&
name|instructions_len
argument_list|,
name|delta_len
argument_list|)
operator|||
name|read_length
argument_list|(
name|delta
argument_list|,
operator|&
name|data_len
argument_list|,
name|delta_len
argument_list|)
operator|||
name|read_chunk
argument_list|(
name|delta
argument_list|,
name|delta_len
argument_list|,
operator|&
name|ctx
operator|.
name|instructions
argument_list|,
name|instructions_len
argument_list|)
operator|||
name|read_chunk
argument_list|(
name|delta
argument_list|,
name|delta_len
argument_list|,
operator|&
name|ctx
operator|.
name|data
argument_list|,
name|data_len
argument_list|)
condition|)
goto|goto
name|error_out
goto|;
name|strbuf_grow
argument_list|(
operator|&
name|ctx
operator|.
name|out
argument_list|,
name|out_len
argument_list|)
expr_stmt|;
if|if
condition|(
name|apply_window_in_core
argument_list|(
operator|&
name|ctx
argument_list|)
condition|)
goto|goto
name|error_out
goto|;
if|if
condition|(
name|ctx
operator|.
name|out
operator|.
name|len
operator|!=
name|out_len
condition|)
block|{
name|error
argument_list|(
literal|"invalid delta: incorrect postimage length"
argument_list|)
expr_stmt|;
goto|goto
name|error_out
goto|;
block|}
if|if
condition|(
name|write_strbuf
argument_list|(
operator|&
name|ctx
operator|.
name|out
argument_list|,
name|out
argument_list|)
condition|)
goto|goto
name|error_out
goto|;
name|window_release
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
name|error_out
label|:
name|window_release
argument_list|(
operator|&
name|ctx
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|svndiff0_apply
name|int
name|svndiff0_apply
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|delta
parameter_list|,
name|off_t
name|delta_len
parameter_list|,
name|struct
name|sliding_view
modifier|*
name|preimage
parameter_list|,
name|FILE
modifier|*
name|postimage
parameter_list|)
block|{
name|assert
argument_list|(
name|delta
operator|&&
name|preimage
operator|&&
name|postimage
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_magic
argument_list|(
name|delta
argument_list|,
operator|&
name|delta_len
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
while|while
condition|(
name|delta_len
condition|)
block|{
comment|/* For each window: */
name|off_t
name|pre_off
init|=
name|pre_off
decl_stmt|;
comment|/* stupid GCC... */
name|size_t
name|pre_len
decl_stmt|;
if|if
condition|(
name|read_offset
argument_list|(
name|delta
argument_list|,
operator|&
name|pre_off
argument_list|,
operator|&
name|delta_len
argument_list|)
operator|||
name|read_length
argument_list|(
name|delta
argument_list|,
operator|&
name|pre_len
argument_list|,
operator|&
name|delta_len
argument_list|)
operator|||
name|move_window
argument_list|(
name|preimage
argument_list|,
name|pre_off
argument_list|,
name|pre_len
argument_list|)
operator|||
name|apply_one_window
argument_list|(
name|delta
argument_list|,
operator|&
name|delta_len
argument_list|,
name|preimage
argument_list|,
name|postimage
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
