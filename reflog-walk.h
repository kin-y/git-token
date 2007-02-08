begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|REFLOG_WALK_H
end_ifndef
begin_define
DECL|macro|REFLOG_WALK_H
define|#
directive|define
name|REFLOG_WALK_H
end_define
begin_function_decl
specifier|extern
name|void
name|init_reflog_walk
parameter_list|(
name|struct
name|reflog_walk_info
modifier|*
modifier|*
name|info
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|add_reflog_for_walk
parameter_list|(
name|struct
name|reflog_walk_info
modifier|*
name|info
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
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
name|void
name|fake_reflog_parent
parameter_list|(
name|struct
name|reflog_walk_info
modifier|*
name|info
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|show_reflog_message
parameter_list|(
name|struct
name|reflog_walk_info
modifier|*
name|info
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
