begin_unit
begin_comment
comment|/*  * Whitespace rules  *  * Copyright (c) 2007 Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"attr.h"
end_include
begin_struct
DECL|struct|whitespace_rule
specifier|static
struct|struct
name|whitespace_rule
block|{
DECL|member|rule_name
specifier|const
name|char
modifier|*
name|rule_name
decl_stmt|;
DECL|member|rule_bits
name|unsigned
name|rule_bits
decl_stmt|;
block|}
DECL|variable|whitespace_rule_names
name|whitespace_rule_names
index|[]
init|=
block|{
block|{
literal|"trailing-space"
block|,
name|WS_TRAILING_SPACE
block|}
block|,
block|{
literal|"space-before-tab"
block|,
name|WS_SPACE_BEFORE_TAB
block|}
block|,
block|{
literal|"indent-with-non-tab"
block|,
name|WS_INDENT_WITH_NON_TAB
block|}
block|,
block|{
literal|"cr-at-eol"
block|,
name|WS_CR_AT_EOL
block|}
block|, }
struct|;
end_struct
begin_function
DECL|function|parse_whitespace_rule
name|unsigned
name|parse_whitespace_rule
parameter_list|(
specifier|const
name|char
modifier|*
name|string
parameter_list|)
block|{
name|unsigned
name|rule
init|=
name|WS_DEFAULT_RULE
decl_stmt|;
while|while
condition|(
name|string
condition|)
block|{
name|int
name|i
decl_stmt|;
name|size_t
name|len
decl_stmt|;
specifier|const
name|char
modifier|*
name|ep
decl_stmt|;
name|int
name|negated
init|=
literal|0
decl_stmt|;
name|string
operator|=
name|string
operator|+
name|strspn
argument_list|(
name|string
argument_list|,
literal|", \t\n\r"
argument_list|)
expr_stmt|;
name|ep
operator|=
name|strchr
argument_list|(
name|string
argument_list|,
literal|','
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ep
condition|)
name|len
operator|=
name|strlen
argument_list|(
name|string
argument_list|)
expr_stmt|;
else|else
name|len
operator|=
name|ep
operator|-
name|string
expr_stmt|;
if|if
condition|(
operator|*
name|string
operator|==
literal|'-'
condition|)
block|{
name|negated
operator|=
literal|1
expr_stmt|;
name|string
operator|++
expr_stmt|;
name|len
operator|--
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|len
condition|)
break|break;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ARRAY_SIZE
argument_list|(
name|whitespace_rule_names
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|strncmp
argument_list|(
name|whitespace_rule_names
index|[
name|i
index|]
operator|.
name|rule_name
argument_list|,
name|string
argument_list|,
name|len
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|negated
condition|)
name|rule
operator|&=
operator|~
name|whitespace_rule_names
index|[
name|i
index|]
operator|.
name|rule_bits
expr_stmt|;
else|else
name|rule
operator||=
name|whitespace_rule_names
index|[
name|i
index|]
operator|.
name|rule_bits
expr_stmt|;
break|break;
block|}
name|string
operator|=
name|ep
expr_stmt|;
block|}
return|return
name|rule
return|;
block|}
end_function
begin_function
DECL|function|setup_whitespace_attr_check
specifier|static
name|void
name|setup_whitespace_attr_check
parameter_list|(
name|struct
name|git_attr_check
modifier|*
name|check
parameter_list|)
block|{
specifier|static
name|struct
name|git_attr
modifier|*
name|attr_whitespace
decl_stmt|;
if|if
condition|(
operator|!
name|attr_whitespace
condition|)
name|attr_whitespace
operator|=
name|git_attr
argument_list|(
literal|"whitespace"
argument_list|,
literal|10
argument_list|)
expr_stmt|;
name|check
index|[
literal|0
index|]
operator|.
name|attr
operator|=
name|attr_whitespace
expr_stmt|;
block|}
end_function
begin_function
DECL|function|whitespace_rule
name|unsigned
name|whitespace_rule
parameter_list|(
specifier|const
name|char
modifier|*
name|pathname
parameter_list|)
block|{
name|struct
name|git_attr_check
name|attr_whitespace_rule
decl_stmt|;
name|setup_whitespace_attr_check
argument_list|(
operator|&
name|attr_whitespace_rule
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_checkattr
argument_list|(
name|pathname
argument_list|,
literal|1
argument_list|,
operator|&
name|attr_whitespace_rule
argument_list|)
condition|)
block|{
specifier|const
name|char
modifier|*
name|value
decl_stmt|;
name|value
operator|=
name|attr_whitespace_rule
operator|.
name|value
expr_stmt|;
if|if
condition|(
name|ATTR_TRUE
argument_list|(
name|value
argument_list|)
condition|)
block|{
comment|/* true (whitespace) */
name|unsigned
name|all_rule
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
name|i
operator|<
name|ARRAY_SIZE
argument_list|(
name|whitespace_rule_names
argument_list|)
condition|;
name|i
operator|++
control|)
name|all_rule
operator||=
name|whitespace_rule_names
index|[
name|i
index|]
operator|.
name|rule_bits
expr_stmt|;
return|return
name|all_rule
return|;
block|}
elseif|else
if|if
condition|(
name|ATTR_FALSE
argument_list|(
name|value
argument_list|)
condition|)
block|{
comment|/* false (-whitespace) */
return|return
literal|0
return|;
block|}
elseif|else
if|if
condition|(
name|ATTR_UNSET
argument_list|(
name|value
argument_list|)
condition|)
block|{
comment|/* reset to default (!whitespace) */
return|return
name|whitespace_rule_cfg
return|;
block|}
else|else
block|{
comment|/* string */
return|return
name|parse_whitespace_rule
argument_list|(
name|value
argument_list|)
return|;
block|}
block|}
else|else
block|{
return|return
name|whitespace_rule_cfg
return|;
block|}
block|}
end_function
begin_comment
comment|/* The returned string should be freed by the caller. */
end_comment
begin_function
DECL|function|whitespace_error_string
name|char
modifier|*
name|whitespace_error_string
parameter_list|(
name|unsigned
name|ws
parameter_list|)
block|{
name|struct
name|strbuf
name|err
decl_stmt|;
name|strbuf_init
argument_list|(
operator|&
name|err
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|ws
operator|&
name|WS_TRAILING_SPACE
condition|)
name|strbuf_addstr
argument_list|(
operator|&
name|err
argument_list|,
literal|"trailing whitespace"
argument_list|)
expr_stmt|;
if|if
condition|(
name|ws
operator|&
name|WS_SPACE_BEFORE_TAB
condition|)
block|{
if|if
condition|(
name|err
operator|.
name|len
condition|)
name|strbuf_addstr
argument_list|(
operator|&
name|err
argument_list|,
literal|", "
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|err
argument_list|,
literal|"space before tab in indent"
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|ws
operator|&
name|WS_INDENT_WITH_NON_TAB
condition|)
block|{
if|if
condition|(
name|err
operator|.
name|len
condition|)
name|strbuf_addstr
argument_list|(
operator|&
name|err
argument_list|,
literal|", "
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|err
argument_list|,
literal|"indent with spaces"
argument_list|)
expr_stmt|;
block|}
return|return
name|strbuf_detach
argument_list|(
operator|&
name|err
argument_list|,
name|NULL
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/* If stream is non-NULL, emits the line after checking. */
end_comment
begin_function
DECL|function|ws_check_emit_1
specifier|static
name|unsigned
name|ws_check_emit_1
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|ws_rule
parameter_list|,
name|FILE
modifier|*
name|stream
parameter_list|,
specifier|const
name|char
modifier|*
name|set
parameter_list|,
specifier|const
name|char
modifier|*
name|reset
parameter_list|,
specifier|const
name|char
modifier|*
name|ws
parameter_list|)
block|{
name|unsigned
name|result
init|=
literal|0
decl_stmt|;
name|int
name|written
init|=
literal|0
decl_stmt|;
name|int
name|trailing_whitespace
init|=
operator|-
literal|1
decl_stmt|;
name|int
name|trailing_newline
init|=
literal|0
decl_stmt|;
name|int
name|trailing_carriage_return
init|=
literal|0
decl_stmt|;
name|int
name|i
decl_stmt|;
comment|/* Logic is simpler if we temporarily ignore the trailing newline. */
if|if
condition|(
name|len
operator|>
literal|0
operator|&&
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
block|{
name|trailing_newline
operator|=
literal|1
expr_stmt|;
name|len
operator|--
expr_stmt|;
block|}
if|if
condition|(
operator|(
name|ws_rule
operator|&
name|WS_CR_AT_EOL
operator|)
operator|&&
name|len
operator|>
literal|0
operator|&&
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\r'
condition|)
block|{
name|trailing_carriage_return
operator|=
literal|1
expr_stmt|;
name|len
operator|--
expr_stmt|;
block|}
comment|/* Check for trailing whitespace. */
if|if
condition|(
name|ws_rule
operator|&
name|WS_TRAILING_SPACE
condition|)
block|{
for|for
control|(
name|i
operator|=
name|len
operator|-
literal|1
init|;
name|i
operator|>=
literal|0
condition|;
name|i
operator|--
control|)
block|{
if|if
condition|(
name|isspace
argument_list|(
name|line
index|[
name|i
index|]
argument_list|)
condition|)
block|{
name|trailing_whitespace
operator|=
name|i
expr_stmt|;
name|result
operator||=
name|WS_TRAILING_SPACE
expr_stmt|;
block|}
else|else
break|break;
block|}
block|}
comment|/* Check for space before tab in initial indent. */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|len
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|line
index|[
name|i
index|]
operator|==
literal|' '
condition|)
continue|continue;
if|if
condition|(
name|line
index|[
name|i
index|]
operator|!=
literal|'\t'
condition|)
break|break;
if|if
condition|(
operator|(
name|ws_rule
operator|&
name|WS_SPACE_BEFORE_TAB
operator|)
operator|&&
name|written
operator|<
name|i
condition|)
block|{
name|result
operator||=
name|WS_SPACE_BEFORE_TAB
expr_stmt|;
if|if
condition|(
name|stream
condition|)
block|{
name|fputs
argument_list|(
name|ws
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
name|line
operator|+
name|written
argument_list|,
name|i
operator|-
name|written
argument_list|,
literal|1
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fputs
argument_list|(
name|reset
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|stream
condition|)
name|fwrite
argument_list|(
name|line
operator|+
name|written
argument_list|,
name|i
operator|-
name|written
argument_list|,
literal|1
argument_list|,
name|stream
argument_list|)
expr_stmt|;
if|if
condition|(
name|stream
condition|)
name|fwrite
argument_list|(
name|line
operator|+
name|i
argument_list|,
literal|1
argument_list|,
literal|1
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|written
operator|=
name|i
operator|+
literal|1
expr_stmt|;
block|}
comment|/* Check for indent using non-tab. */
if|if
condition|(
operator|(
name|ws_rule
operator|&
name|WS_INDENT_WITH_NON_TAB
operator|)
operator|&&
name|i
operator|-
name|written
operator|>=
literal|8
condition|)
block|{
name|result
operator||=
name|WS_INDENT_WITH_NON_TAB
expr_stmt|;
if|if
condition|(
name|stream
condition|)
block|{
name|fputs
argument_list|(
name|ws
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
name|line
operator|+
name|written
argument_list|,
name|i
operator|-
name|written
argument_list|,
literal|1
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fputs
argument_list|(
name|reset
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
name|written
operator|=
name|i
expr_stmt|;
block|}
if|if
condition|(
name|stream
condition|)
block|{
comment|/* 		 * Now the rest of the line starts at "written". 		 * The non-highlighted part ends at "trailing_whitespace". 		 */
if|if
condition|(
name|trailing_whitespace
operator|==
operator|-
literal|1
condition|)
name|trailing_whitespace
operator|=
name|len
expr_stmt|;
comment|/* Emit non-highlighted (middle) segment. */
if|if
condition|(
name|trailing_whitespace
operator|-
name|written
operator|>
literal|0
condition|)
block|{
name|fputs
argument_list|(
name|set
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
name|line
operator|+
name|written
argument_list|,
name|trailing_whitespace
operator|-
name|written
argument_list|,
literal|1
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fputs
argument_list|(
name|reset
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
comment|/* Highlight errors in trailing whitespace. */
if|if
condition|(
name|trailing_whitespace
operator|!=
name|len
condition|)
block|{
name|fputs
argument_list|(
name|ws
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
name|line
operator|+
name|trailing_whitespace
argument_list|,
name|len
operator|-
name|trailing_whitespace
argument_list|,
literal|1
argument_list|,
name|stream
argument_list|)
expr_stmt|;
name|fputs
argument_list|(
name|reset
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|trailing_carriage_return
condition|)
name|fputc
argument_list|(
literal|'\r'
argument_list|,
name|stream
argument_list|)
expr_stmt|;
if|if
condition|(
name|trailing_newline
condition|)
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stream
argument_list|)
expr_stmt|;
block|}
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|ws_check_emit
name|void
name|ws_check_emit
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|ws_rule
parameter_list|,
name|FILE
modifier|*
name|stream
parameter_list|,
specifier|const
name|char
modifier|*
name|set
parameter_list|,
specifier|const
name|char
modifier|*
name|reset
parameter_list|,
specifier|const
name|char
modifier|*
name|ws
parameter_list|)
block|{
operator|(
name|void
operator|)
name|ws_check_emit_1
argument_list|(
name|line
argument_list|,
name|len
argument_list|,
name|ws_rule
argument_list|,
name|stream
argument_list|,
name|set
argument_list|,
name|reset
argument_list|,
name|ws
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|ws_check
name|unsigned
name|ws_check
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|ws_rule
parameter_list|)
block|{
return|return
name|ws_check_emit_1
argument_list|(
name|line
argument_list|,
name|len
argument_list|,
name|ws_rule
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|ws_blank_line
name|int
name|ws_blank_line
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|ws_rule
parameter_list|)
block|{
comment|/* 	 * We _might_ want to treat CR differently from other 	 * whitespace characters when ws_rule has WS_CR_AT_EOL, but 	 * for now we just use this stupid definition. 	 */
while|while
condition|(
name|len
operator|--
operator|>
literal|0
condition|)
block|{
if|if
condition|(
operator|!
name|isspace
argument_list|(
operator|*
name|line
argument_list|)
condition|)
return|return
literal|0
return|;
name|line
operator|++
expr_stmt|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_comment
comment|/* Copy the line to the buffer while fixing whitespaces */
end_comment
begin_function
DECL|function|ws_fix_copy
name|int
name|ws_fix_copy
parameter_list|(
name|char
modifier|*
name|dst
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|,
name|int
name|len
parameter_list|,
name|unsigned
name|ws_rule
parameter_list|,
name|int
modifier|*
name|error_count
parameter_list|)
block|{
comment|/* 	 * len is number of bytes to be copied from src, starting 	 * at src.  Typically src[len-1] is '\n', unless this is 	 * the incomplete last line. 	 */
name|int
name|i
decl_stmt|;
name|int
name|add_nl_to_tail
init|=
literal|0
decl_stmt|;
name|int
name|add_cr_to_tail
init|=
literal|0
decl_stmt|;
name|int
name|fixed
init|=
literal|0
decl_stmt|;
name|int
name|last_tab_in_indent
init|=
operator|-
literal|1
decl_stmt|;
name|int
name|last_space_in_indent
init|=
operator|-
literal|1
decl_stmt|;
name|int
name|need_fix_leading_space
init|=
literal|0
decl_stmt|;
name|char
modifier|*
name|buf
decl_stmt|;
comment|/* 	 * Strip trailing whitespace 	 */
if|if
condition|(
operator|(
name|ws_rule
operator|&
name|WS_TRAILING_SPACE
operator|)
operator|&&
operator|(
literal|2
operator|<=
name|len
operator|&&
name|isspace
argument_list|(
name|src
index|[
name|len
operator|-
literal|2
index|]
argument_list|)
operator|)
condition|)
block|{
if|if
condition|(
name|src
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
block|{
name|add_nl_to_tail
operator|=
literal|1
expr_stmt|;
name|len
operator|--
expr_stmt|;
if|if
condition|(
literal|1
operator|<
name|len
operator|&&
name|src
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\r'
condition|)
block|{
name|add_cr_to_tail
operator|=
operator|!
operator|!
operator|(
name|ws_rule
operator|&
name|WS_CR_AT_EOL
operator|)
expr_stmt|;
name|len
operator|--
expr_stmt|;
block|}
block|}
if|if
condition|(
literal|0
operator|<
name|len
operator|&&
name|isspace
argument_list|(
name|src
index|[
name|len
operator|-
literal|1
index|]
argument_list|)
condition|)
block|{
while|while
condition|(
literal|0
operator|<
name|len
operator|&&
name|isspace
argument_list|(
name|src
index|[
name|len
operator|-
literal|1
index|]
argument_list|)
condition|)
name|len
operator|--
expr_stmt|;
name|fixed
operator|=
literal|1
expr_stmt|;
block|}
block|}
comment|/* 	 * Check leading whitespaces (indent) 	 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|len
condition|;
name|i
operator|++
control|)
block|{
name|char
name|ch
init|=
name|src
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|ch
operator|==
literal|'\t'
condition|)
block|{
name|last_tab_in_indent
operator|=
name|i
expr_stmt|;
if|if
condition|(
operator|(
name|ws_rule
operator|&
name|WS_SPACE_BEFORE_TAB
operator|)
operator|&&
literal|0
operator|<=
name|last_space_in_indent
condition|)
name|need_fix_leading_space
operator|=
literal|1
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|ch
operator|==
literal|' '
condition|)
block|{
name|last_space_in_indent
operator|=
name|i
expr_stmt|;
if|if
condition|(
operator|(
name|ws_rule
operator|&
name|WS_INDENT_WITH_NON_TAB
operator|)
operator|&&
literal|8
operator|<=
name|i
operator|-
name|last_tab_in_indent
condition|)
name|need_fix_leading_space
operator|=
literal|1
expr_stmt|;
block|}
else|else
break|break;
block|}
name|buf
operator|=
name|dst
expr_stmt|;
if|if
condition|(
name|need_fix_leading_space
condition|)
block|{
comment|/* Process indent ourselves */
name|int
name|consecutive_spaces
init|=
literal|0
decl_stmt|;
name|int
name|last
init|=
name|last_tab_in_indent
operator|+
literal|1
decl_stmt|;
if|if
condition|(
name|ws_rule
operator|&
name|WS_INDENT_WITH_NON_TAB
condition|)
block|{
comment|/* have "last" point at one past the indent */
if|if
condition|(
name|last_tab_in_indent
operator|<
name|last_space_in_indent
condition|)
name|last
operator|=
name|last_space_in_indent
operator|+
literal|1
expr_stmt|;
else|else
name|last
operator|=
name|last_tab_in_indent
operator|+
literal|1
expr_stmt|;
block|}
comment|/* 		 * between src[0..last-1], strip the funny spaces, 		 * updating them to tab as needed. 		 */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|last
condition|;
name|i
operator|++
control|)
block|{
name|char
name|ch
init|=
name|src
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
name|ch
operator|!=
literal|' '
condition|)
block|{
name|consecutive_spaces
operator|=
literal|0
expr_stmt|;
operator|*
name|dst
operator|++
operator|=
name|ch
expr_stmt|;
block|}
else|else
block|{
name|consecutive_spaces
operator|++
expr_stmt|;
if|if
condition|(
name|consecutive_spaces
operator|==
literal|8
condition|)
block|{
operator|*
name|dst
operator|++
operator|=
literal|'\t'
expr_stmt|;
name|consecutive_spaces
operator|=
literal|0
expr_stmt|;
block|}
block|}
block|}
while|while
condition|(
literal|0
operator|<
name|consecutive_spaces
operator|--
condition|)
operator|*
name|dst
operator|++
operator|=
literal|' '
expr_stmt|;
name|len
operator|-=
name|last
expr_stmt|;
name|src
operator|+=
name|last
expr_stmt|;
name|fixed
operator|=
literal|1
expr_stmt|;
block|}
name|memcpy
argument_list|(
name|dst
argument_list|,
name|src
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|add_cr_to_tail
condition|)
name|dst
index|[
name|len
operator|++
index|]
operator|=
literal|'\r'
expr_stmt|;
if|if
condition|(
name|add_nl_to_tail
condition|)
name|dst
index|[
name|len
operator|++
index|]
operator|=
literal|'\n'
expr_stmt|;
if|if
condition|(
name|fixed
operator|&&
name|error_count
condition|)
operator|(
operator|*
name|error_count
operator|)
operator|++
expr_stmt|;
return|return
name|dst
operator|+
name|len
operator|-
name|buf
return|;
block|}
end_function
end_unit
