begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|CACHE_TREE_H
end_ifndef
begin_define
DECL|macro|CACHE_TREE_H
define|#
directive|define
name|CACHE_TREE_H
end_define
begin_struct_decl
struct_decl|struct
name|cache_tree
struct_decl|;
end_struct_decl
begin_struct
DECL|struct|cache_tree_sub
struct|struct
name|cache_tree_sub
block|{
DECL|member|cache_tree
name|struct
name|cache_tree
modifier|*
name|cache_tree
decl_stmt|;
DECL|member|namelen
name|int
name|namelen
decl_stmt|;
DECL|member|used
name|int
name|used
decl_stmt|;
DECL|member|name
name|char
name|name
index|[
name|FLEX_ARRAY
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|cache_tree
struct|struct
name|cache_tree
block|{
DECL|member|entry_count
name|int
name|entry_count
decl_stmt|;
comment|/* negative means "invalid" */
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|subtree_nr
name|int
name|subtree_nr
decl_stmt|;
DECL|member|subtree_alloc
name|int
name|subtree_alloc
decl_stmt|;
DECL|member|down
name|struct
name|cache_tree_sub
modifier|*
modifier|*
name|down
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
name|struct
name|cache_tree
modifier|*
name|cache_tree
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|cache_tree_free
parameter_list|(
name|struct
name|cache_tree
modifier|*
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|cache_tree_invalidate_path
parameter_list|(
name|struct
name|cache_tree
modifier|*
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
modifier|*
name|cache_tree_write
parameter_list|(
name|struct
name|cache_tree
modifier|*
name|root
parameter_list|,
name|unsigned
name|long
modifier|*
name|size_p
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|struct
name|cache_tree
modifier|*
name|cache_tree_read
parameter_list|(
specifier|const
name|char
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|cache_tree_fully_valid
parameter_list|(
name|struct
name|cache_tree
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|cache_tree_update
parameter_list|(
name|struct
name|cache_tree
modifier|*
parameter_list|,
name|struct
name|cache_entry
modifier|*
modifier|*
parameter_list|,
name|int
parameter_list|,
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
