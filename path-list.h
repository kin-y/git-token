begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|PATH_LIST_H
end_ifndef
begin_define
DECL|macro|PATH_LIST_H
define|#
directive|define
name|PATH_LIST_H
end_define
begin_struct
DECL|struct|path_list_item
struct|struct
name|path_list_item
block|{
DECL|member|path
name|char
modifier|*
name|path
decl_stmt|;
DECL|member|util
name|void
modifier|*
name|util
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|path_list
struct|struct
name|path_list
block|{
DECL|member|items
name|struct
name|path_list_item
modifier|*
name|items
decl_stmt|;
DECL|member|nr
DECL|member|alloc
name|unsigned
name|int
name|nr
decl_stmt|,
name|alloc
decl_stmt|;
DECL|member|strdup_paths
name|unsigned
name|int
name|strdup_paths
range|:
literal|1
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
name|void
name|print_path_list
parameter_list|(
specifier|const
name|char
modifier|*
name|text
parameter_list|,
specifier|const
name|struct
name|path_list
modifier|*
name|p
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|path_list_has_path
parameter_list|(
specifier|const
name|struct
name|path_list
modifier|*
name|list
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|path_list_clear
parameter_list|(
name|struct
name|path_list
modifier|*
name|list
parameter_list|,
name|int
name|free_items
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|path_list_item
modifier|*
name|path_list_insert
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|path_list
modifier|*
name|list
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|path_list_item
modifier|*
name|path_list_lookup
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|path_list
modifier|*
name|list
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* PATH_LIST_H */
end_comment
end_unit
