begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|CSUM_FILE_H
end_ifndef
begin_define
DECL|macro|CSUM_FILE_H
define|#
directive|define
name|CSUM_FILE_H
end_define
begin_comment
comment|/* A SHA1-protected file */
end_comment
begin_struct
DECL|struct|sha1file
struct|struct
name|sha1file
block|{
DECL|member|fd
DECL|member|error
name|int
name|fd
decl_stmt|,
name|error
decl_stmt|;
DECL|member|offset
DECL|member|namelen
name|unsigned
name|int
name|offset
decl_stmt|,
name|namelen
decl_stmt|;
DECL|member|ctx
name|SHA_CTX
name|ctx
decl_stmt|;
DECL|member|name
name|char
name|name
index|[
name|PATH_MAX
index|]
decl_stmt|;
DECL|member|buffer
name|unsigned
name|char
name|buffer
index|[
literal|8192
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
specifier|extern
name|struct
name|sha1file
modifier|*
name|sha1fd
parameter_list|(
name|int
name|fd
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
name|struct
name|sha1file
modifier|*
name|sha1create
parameter_list|(
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|sha1close
parameter_list|(
name|struct
name|sha1file
modifier|*
parameter_list|,
name|unsigned
name|char
modifier|*
parameter_list|,
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|sha1write
parameter_list|(
name|struct
name|sha1file
modifier|*
parameter_list|,
name|void
modifier|*
parameter_list|,
name|unsigned
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|sha1write_compressed
parameter_list|(
name|struct
name|sha1file
modifier|*
parameter_list|,
name|void
modifier|*
parameter_list|,
name|unsigned
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
