begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|REFS_H
end_ifndef
begin_define
DECL|macro|REFS_H
define|#
directive|define
name|REFS_H
end_define
begin_struct
DECL|struct|ref_lock
struct|struct
name|ref_lock
block|{
DECL|member|ref_file
name|char
modifier|*
name|ref_file
decl_stmt|;
DECL|member|lock_file
name|char
modifier|*
name|lock_file
decl_stmt|;
DECL|member|old_sha1
name|unsigned
name|char
name|old_sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|lock_fd
name|int
name|lock_fd
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/*  * Calls the specified function for each ref file until it returns nonzero,  * and returns the value  */
end_comment
begin_function_decl
specifier|extern
name|int
name|head_ref
parameter_list|(
name|int
function_decl|(
modifier|*
name|fn
function_decl|)
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_ref
parameter_list|(
name|int
function_decl|(
modifier|*
name|fn
function_decl|)
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_tag_ref
parameter_list|(
name|int
function_decl|(
modifier|*
name|fn
function_decl|)
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_branch_ref
parameter_list|(
name|int
function_decl|(
modifier|*
name|fn
function_decl|)
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_remote_ref
parameter_list|(
name|int
function_decl|(
modifier|*
name|fn
function_decl|)
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Reads the refs file specified into sha1 **/
end_comment
begin_function_decl
specifier|extern
name|int
name|get_ref_sha1
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Locks a "refs/" ref returning the lock on success and NULL on failure. **/
end_comment
begin_function_decl
specifier|extern
name|struct
name|ref_lock
modifier|*
name|lock_ref_sha1
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|old_sha1
parameter_list|,
name|int
name|mustexist
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Locks any ref (for 'HEAD' type refs). */
end_comment
begin_function_decl
specifier|extern
name|struct
name|ref_lock
modifier|*
name|lock_any_ref_for_update
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|old_sha1
parameter_list|,
name|int
name|mustexist
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Release any lock taken but not written. **/
end_comment
begin_function_decl
specifier|extern
name|void
name|unlock_ref
parameter_list|(
name|struct
name|ref_lock
modifier|*
name|lock
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Writes sha1 into the ref specified by the lock. **/
end_comment
begin_function_decl
specifier|extern
name|int
name|write_ref_sha1
parameter_list|(
name|struct
name|ref_lock
modifier|*
name|lock
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Returns 0 if target has the right format for a ref. **/
end_comment
begin_function_decl
specifier|extern
name|int
name|check_ref_format
parameter_list|(
specifier|const
name|char
modifier|*
name|target
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* REFS_H */
end_comment
end_unit
