begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|STRING_POOL_H_
end_ifndef
begin_define
DECL|macro|STRING_POOL_H_
define|#
directive|define
name|STRING_POOL_H_
end_define
begin_function_decl
name|uint32_t
name|pool_intern
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|const
name|char
modifier|*
name|pool_fetch
parameter_list|(
name|uint32_t
name|entry
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|uint32_t
name|pool_tok_r
parameter_list|(
name|char
modifier|*
name|str
parameter_list|,
specifier|const
name|char
modifier|*
name|delim
parameter_list|,
name|char
modifier|*
modifier|*
name|saveptr
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|pool_print_seq
parameter_list|(
name|uint32_t
name|len
parameter_list|,
specifier|const
name|uint32_t
modifier|*
name|seq
parameter_list|,
name|char
name|delim
parameter_list|,
name|FILE
modifier|*
name|stream
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|uint32_t
name|pool_tok_seq
parameter_list|(
name|uint32_t
name|sz
parameter_list|,
name|uint32_t
modifier|*
name|seq
parameter_list|,
specifier|const
name|char
modifier|*
name|delim
parameter_list|,
name|char
modifier|*
name|str
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|pool_reset
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
