begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|STRBUF_H
end_ifndef
begin_define
DECL|macro|STRBUF_H
define|#
directive|define
name|STRBUF_H
end_define
begin_comment
comment|/* See Documentation/technical/api-strbuf.txt */
end_comment
begin_decl_stmt
specifier|extern
name|char
name|strbuf_slopbuf
index|[]
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|strbuf
struct|struct
name|strbuf
block|{
DECL|member|alloc
name|size_t
name|alloc
decl_stmt|;
DECL|member|len
name|size_t
name|len
decl_stmt|;
DECL|member|buf
name|char
modifier|*
name|buf
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|STRBUF_INIT
define|#
directive|define
name|STRBUF_INIT
value|{ 0, 0, strbuf_slopbuf }
end_define
begin_comment
comment|/*----- strbuf life cycle -----*/
end_comment
begin_function_decl
specifier|extern
name|void
name|strbuf_init
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|size_t
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|strbuf_release
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|strbuf_detach
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|size_t
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|strbuf_attach
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|void
modifier|*
parameter_list|,
name|size_t
parameter_list|,
name|size_t
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|strbuf_swap
specifier|static
specifier|inline
name|void
name|strbuf_swap
parameter_list|(
name|struct
name|strbuf
modifier|*
name|a
parameter_list|,
name|struct
name|strbuf
modifier|*
name|b
parameter_list|)
block|{
name|struct
name|strbuf
name|tmp
init|=
operator|*
name|a
decl_stmt|;
operator|*
name|a
operator|=
operator|*
name|b
expr_stmt|;
operator|*
name|b
operator|=
name|tmp
expr_stmt|;
block|}
end_function
begin_comment
comment|/*----- strbuf size related -----*/
end_comment
begin_function
DECL|function|strbuf_avail
specifier|static
specifier|inline
name|size_t
name|strbuf_avail
parameter_list|(
specifier|const
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|)
block|{
return|return
name|sb
operator|->
name|alloc
condition|?
name|sb
operator|->
name|alloc
operator|-
name|sb
operator|->
name|len
operator|-
literal|1
else|:
literal|0
return|;
block|}
end_function
begin_function_decl
specifier|extern
name|void
name|strbuf_grow
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|size_t
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|strbuf_setlen
specifier|static
specifier|inline
name|void
name|strbuf_setlen
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
if|if
condition|(
name|len
operator|>
operator|(
name|sb
operator|->
name|alloc
condition|?
name|sb
operator|->
name|alloc
operator|-
literal|1
else|:
literal|0
operator|)
condition|)
name|die
argument_list|(
literal|"BUG: strbuf_setlen() beyond buffer"
argument_list|)
expr_stmt|;
name|sb
operator|->
name|len
operator|=
name|len
expr_stmt|;
name|sb
operator|->
name|buf
index|[
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
block|}
end_function
begin_define
DECL|macro|strbuf_reset
define|#
directive|define
name|strbuf_reset
parameter_list|(
name|sb
parameter_list|)
value|strbuf_setlen(sb, 0)
end_define
begin_comment
comment|/*----- content related -----*/
end_comment
begin_function_decl
specifier|extern
name|void
name|strbuf_trim
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|strbuf_rtrim
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|strbuf_ltrim
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|strbuf_cmp
parameter_list|(
specifier|const
name|struct
name|strbuf
modifier|*
parameter_list|,
specifier|const
name|struct
name|strbuf
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|struct
name|strbuf
modifier|*
modifier|*
name|strbuf_split_buf
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|,
name|size_t
parameter_list|,
name|int
name|delim
parameter_list|,
name|int
name|max
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|strbuf_split_str
specifier|static
specifier|inline
name|struct
name|strbuf
modifier|*
modifier|*
name|strbuf_split_str
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|int
name|delim
parameter_list|,
name|int
name|max
parameter_list|)
block|{
return|return
name|strbuf_split_buf
argument_list|(
name|str
argument_list|,
name|strlen
argument_list|(
name|str
argument_list|)
argument_list|,
name|delim
argument_list|,
name|max
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|strbuf_split_max
specifier|static
specifier|inline
name|struct
name|strbuf
modifier|*
modifier|*
name|strbuf_split_max
parameter_list|(
specifier|const
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|int
name|delim
parameter_list|,
name|int
name|max
parameter_list|)
block|{
return|return
name|strbuf_split_buf
argument_list|(
name|sb
operator|->
name|buf
argument_list|,
name|sb
operator|->
name|len
argument_list|,
name|delim
argument_list|,
name|max
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|strbuf_split
specifier|static
specifier|inline
name|struct
name|strbuf
modifier|*
modifier|*
name|strbuf_split
parameter_list|(
specifier|const
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|int
name|delim
parameter_list|)
block|{
return|return
name|strbuf_split_max
argument_list|(
name|sb
argument_list|,
name|delim
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
begin_function_decl
specifier|extern
name|void
name|strbuf_list_free
parameter_list|(
name|struct
name|strbuf
modifier|*
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*----- add data in your buffer -----*/
end_comment
begin_function
DECL|function|strbuf_addch
specifier|static
specifier|inline
name|void
name|strbuf_addch
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|int
name|c
parameter_list|)
block|{
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
operator|++
index|]
operator|=
name|c
expr_stmt|;
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
block|}
end_function
begin_function_decl
specifier|extern
name|void
name|strbuf_insert
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|size_t
name|pos
parameter_list|,
specifier|const
name|void
modifier|*
parameter_list|,
name|size_t
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|strbuf_remove
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|size_t
name|pos
parameter_list|,
name|size_t
name|len
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* splice pos..pos+len with given data */
end_comment
begin_function_decl
specifier|extern
name|void
name|strbuf_splice
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|size_t
name|pos
parameter_list|,
name|size_t
name|len
parameter_list|,
specifier|const
name|void
modifier|*
parameter_list|,
name|size_t
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|strbuf_add
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
specifier|const
name|void
modifier|*
parameter_list|,
name|size_t
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|strbuf_addstr
specifier|static
specifier|inline
name|void
name|strbuf_addstr
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|s
parameter_list|)
block|{
name|strbuf_add
argument_list|(
name|sb
argument_list|,
name|s
argument_list|,
name|strlen
argument_list|(
name|s
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|strbuf_addbuf
specifier|static
specifier|inline
name|void
name|strbuf_addbuf
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|struct
name|strbuf
modifier|*
name|sb2
parameter_list|)
block|{
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|sb2
operator|->
name|len
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
name|sb
argument_list|,
name|sb2
operator|->
name|buf
argument_list|,
name|sb2
operator|->
name|len
argument_list|)
expr_stmt|;
block|}
end_function
begin_function_decl
specifier|extern
name|void
name|strbuf_adddup
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
name|size_t
name|pos
parameter_list|,
name|size_t
name|len
parameter_list|)
function_decl|;
end_function_decl
begin_typedef
DECL|typedef|expand_fn_t
typedef|typedef
name|size_t
function_decl|(
modifier|*
name|expand_fn_t
function_decl|)
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|placeholder
parameter_list|,
name|void
modifier|*
name|context
parameter_list|)
function_decl|;
end_typedef
begin_function_decl
specifier|extern
name|void
name|strbuf_expand
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|format
parameter_list|,
name|expand_fn_t
name|fn
parameter_list|,
name|void
modifier|*
name|context
parameter_list|)
function_decl|;
end_function_decl
begin_struct
DECL|struct|strbuf_expand_dict_entry
struct|struct
name|strbuf_expand_dict_entry
block|{
DECL|member|placeholder
specifier|const
name|char
modifier|*
name|placeholder
decl_stmt|;
DECL|member|value
specifier|const
name|char
modifier|*
name|value
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|size_t
name|strbuf_expand_dict_cb
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|placeholder
parameter_list|,
name|void
modifier|*
name|context
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|strbuf_addbuf_percentquote
parameter_list|(
name|struct
name|strbuf
modifier|*
name|dst
parameter_list|,
specifier|const
name|struct
name|strbuf
modifier|*
name|src
parameter_list|)
function_decl|;
end_function_decl
begin_macro
name|__attribute__
argument_list|(
argument|(format (printf,
literal|2
argument|,
literal|3
argument|))
argument_list|)
end_macro
begin_function_decl
specifier|extern
name|void
name|strbuf_addf
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
function_decl|;
end_function_decl
begin_macro
name|__attribute__
argument_list|(
argument|(format (printf,
literal|2
argument|,
literal|0
argument|))
argument_list|)
end_macro
begin_function_decl
specifier|extern
name|void
name|strbuf_vaddf
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
name|va_list
name|ap
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|strbuf_add_lines
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
specifier|const
name|char
modifier|*
name|buf
parameter_list|,
name|size_t
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|strbuf_complete_line
specifier|static
specifier|inline
name|void
name|strbuf_complete_line
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|)
block|{
if|if
condition|(
name|sb
operator|->
name|len
operator|&&
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
operator|-
literal|1
index|]
operator|!=
literal|'\n'
condition|)
name|strbuf_addch
argument_list|(
name|sb
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
block|}
end_function
begin_function_decl
specifier|extern
name|size_t
name|strbuf_fread
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|size_t
parameter_list|,
name|FILE
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* XXX: if read fails, any partial read is undone */
end_comment
begin_function_decl
specifier|extern
name|ssize_t
name|strbuf_read
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|int
name|fd
parameter_list|,
name|size_t
name|hint
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|strbuf_read_file
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|size_t
name|hint
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|strbuf_readlink
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|size_t
name|hint
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|strbuf_getwholeline
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|FILE
modifier|*
parameter_list|,
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|strbuf_getline
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|,
name|FILE
modifier|*
parameter_list|,
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|stripspace
parameter_list|(
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|,
name|int
name|skip_comments
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|launch_editor
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|strbuf
modifier|*
name|buffer
parameter_list|,
specifier|const
name|char
modifier|*
specifier|const
modifier|*
name|env
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|strbuf_branchname
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|strbuf_check_branch_ref
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* STRBUF_H */
end_comment
end_unit
