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
DECL|member|ref_name
name|char
modifier|*
name|ref_name
decl_stmt|;
DECL|member|orig_ref_name
name|char
modifier|*
name|orig_ref_name
decl_stmt|;
DECL|member|lk
name|struct
name|lock_file
modifier|*
name|lk
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
DECL|member|force_write
name|int
name|force_write
decl_stmt|;
block|}
struct|;
end_struct
begin_define
DECL|macro|REF_ISSYMREF
define|#
directive|define
name|REF_ISSYMREF
value|01
end_define
begin_define
DECL|macro|REF_ISPACKED
define|#
directive|define
name|REF_ISPACKED
value|02
end_define
begin_comment
comment|/*  * Calls the specified function for each ref file until it returns nonzero,  * and returns the value  */
end_comment
begin_typedef
DECL|typedef|each_ref_fn
typedef|typedef
name|int
name|each_ref_fn
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|flags
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
function_decl|;
end_typedef
begin_function_decl
specifier|extern
name|int
name|head_ref
parameter_list|(
name|each_ref_fn
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_ref
parameter_list|(
name|each_ref_fn
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_ref_in
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|,
name|each_ref_fn
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_tag_ref
parameter_list|(
name|each_ref_fn
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_branch_ref
parameter_list|(
name|each_ref_fn
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_remote_ref
parameter_list|(
name|each_ref_fn
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_replace_ref
parameter_list|(
name|each_ref_fn
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_glob_ref
parameter_list|(
name|each_ref_fn
parameter_list|,
specifier|const
name|char
modifier|*
name|pattern
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_glob_ref_in
parameter_list|(
name|each_ref_fn
parameter_list|,
specifier|const
name|char
modifier|*
name|pattern
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|head_ref_submodule
parameter_list|(
specifier|const
name|char
modifier|*
name|submodule
parameter_list|,
name|each_ref_fn
name|fn
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_ref_submodule
parameter_list|(
specifier|const
name|char
modifier|*
name|submodule
parameter_list|,
name|each_ref_fn
name|fn
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_ref_in_submodule
parameter_list|(
specifier|const
name|char
modifier|*
name|submodule
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|,
name|each_ref_fn
name|fn
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_tag_ref_submodule
parameter_list|(
specifier|const
name|char
modifier|*
name|submodule
parameter_list|,
name|each_ref_fn
name|fn
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_branch_ref_submodule
parameter_list|(
specifier|const
name|char
modifier|*
name|submodule
parameter_list|,
name|each_ref_fn
name|fn
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_remote_ref_submodule
parameter_list|(
specifier|const
name|char
modifier|*
name|submodule
parameter_list|,
name|each_ref_fn
name|fn
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|head_ref_namespaced
parameter_list|(
name|each_ref_fn
name|fn
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|for_each_namespaced_ref
parameter_list|(
name|each_ref_fn
name|fn
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|has_glob_specials
specifier|static
specifier|inline
specifier|const
name|char
modifier|*
name|has_glob_specials
parameter_list|(
specifier|const
name|char
modifier|*
name|pattern
parameter_list|)
block|{
return|return
name|strpbrk
argument_list|(
name|pattern
argument_list|,
literal|"?*["
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/* can be used to learn about broken ref and symref */
end_comment
begin_function_decl
specifier|extern
name|int
name|for_each_rawref
parameter_list|(
name|each_ref_fn
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|warn_dangling_symref
parameter_list|(
name|FILE
modifier|*
name|fp
parameter_list|,
specifier|const
name|char
modifier|*
name|msg_fmt
parameter_list|,
specifier|const
name|char
modifier|*
name|refname
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * Extra refs will be listed by for_each_ref() before any actual refs  * for the duration of this process or until clear_extra_refs() is  * called. Only extra refs added before for_each_ref() is called will  * be listed on a given call of for_each_ref().  */
end_comment
begin_function_decl
specifier|extern
name|void
name|add_extra_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|flags
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|clear_extra_refs
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|ref_exists
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|peel_ref
parameter_list|(
specifier|const
name|char
modifier|*
parameter_list|,
name|unsigned
name|char
modifier|*
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
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Locks any ref (for 'HEAD' type refs). */
end_comment
begin_define
DECL|macro|REF_NODEREF
define|#
directive|define
name|REF_NODEREF
value|0x01
end_define
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
name|flags
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Close the file descriptor owned by a lock and return the status */
end_comment
begin_function_decl
specifier|extern
name|int
name|close_ref
parameter_list|(
name|struct
name|ref_lock
modifier|*
name|lock
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Close and commit the ref locked by the lock */
end_comment
begin_function_decl
specifier|extern
name|int
name|commit_ref
parameter_list|(
name|struct
name|ref_lock
modifier|*
name|lock
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
comment|/** Setup reflog before using. **/
end_comment
begin_function_decl
name|int
name|log_ref_setup
parameter_list|(
specifier|const
name|char
modifier|*
name|ref_name
parameter_list|,
name|char
modifier|*
name|logfile
parameter_list|,
name|int
name|bufsize
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** Reads log for the value of ref during at_time. **/
end_comment
begin_function_decl
specifier|extern
name|int
name|read_ref_at
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
name|unsigned
name|long
name|at_time
parameter_list|,
name|int
name|cnt
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|char
modifier|*
modifier|*
name|msg
parameter_list|,
name|unsigned
name|long
modifier|*
name|cutoff_time
parameter_list|,
name|int
modifier|*
name|cutoff_tz
parameter_list|,
name|int
modifier|*
name|cutoff_cnt
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/* iterate over reflog entries */
end_comment
begin_typedef
DECL|typedef|each_reflog_ent_fn
typedef|typedef
name|int
name|each_reflog_ent_fn
parameter_list|(
name|unsigned
name|char
modifier|*
name|osha1
parameter_list|,
name|unsigned
name|char
modifier|*
name|nsha1
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|,
name|unsigned
name|long
parameter_list|,
name|int
parameter_list|,
specifier|const
name|char
modifier|*
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_typedef
begin_function_decl
name|int
name|for_each_reflog_ent
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
name|each_reflog_ent_fn
name|fn
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|int
name|for_each_recent_reflog_ent
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
name|each_reflog_ent_fn
name|fn
parameter_list|,
name|long
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * Calls the specified function for each reflog file until it returns nonzero,  * and returns the value  */
end_comment
begin_function_decl
specifier|extern
name|int
name|for_each_reflog
parameter_list|(
name|each_ref_fn
parameter_list|,
name|void
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|REFNAME_ALLOW_ONELEVEL
define|#
directive|define
name|REFNAME_ALLOW_ONELEVEL
value|1
end_define
begin_define
DECL|macro|REFNAME_REFSPEC_PATTERN
define|#
directive|define
name|REFNAME_REFSPEC_PATTERN
value|2
end_define
begin_define
DECL|macro|REFNAME_DOT_COMPONENT
define|#
directive|define
name|REFNAME_DOT_COMPONENT
value|4
end_define
begin_comment
comment|/*  * Return 0 iff ref has the correct format for a refname according to  * the rules described in Documentation/git-check-ref-format.txt.  If  * REFNAME_ALLOW_ONELEVEL is set in flags, then accept one-level  * reference names.  If REFNAME_REFSPEC_PATTERN is set in flags, then  * allow a "*" wildcard character in place of one of the name  * components.  No leading or repeated slashes are accepted.  If  * REFNAME_DOT_COMPONENT is set in flags, then allow refname  * components to start with "." (but not a whole component equal to  * "." or "..").  */
end_comment
begin_function_decl
specifier|extern
name|int
name|check_refname_format
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
name|int
name|flags
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
specifier|const
name|char
modifier|*
name|prettify_refname
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|char
modifier|*
name|shorten_unambiguous_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|ref
parameter_list|,
name|int
name|strict
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** rename ref, return 0 on success **/
end_comment
begin_function_decl
specifier|extern
name|int
name|rename_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|oldref
parameter_list|,
specifier|const
name|char
modifier|*
name|newref
parameter_list|,
specifier|const
name|char
modifier|*
name|logmsg
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** resolve ref in nested "gitlink" repository */
end_comment
begin_function_decl
specifier|extern
name|int
name|resolve_gitlink_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
name|unsigned
name|char
modifier|*
name|result
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/** lock a ref and then write its file */
end_comment
begin_enum
DECL|enum|action_on_err
DECL|enumerator|MSG_ON_ERR
DECL|enumerator|DIE_ON_ERR
DECL|enumerator|QUIET_ON_ERR
enum|enum
name|action_on_err
block|{
name|MSG_ON_ERR
block|,
name|DIE_ON_ERR
block|,
name|QUIET_ON_ERR
block|}
enum|;
end_enum
begin_function_decl
name|int
name|update_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|action
parameter_list|,
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|oldval
parameter_list|,
name|int
name|flags
parameter_list|,
name|enum
name|action_on_err
name|onerr
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
