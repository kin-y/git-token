begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  */
end_comment
begin_ifndef
ifndef|#
directive|ifndef
name|DIFF_H
end_ifndef
begin_define
DECL|macro|DIFF_H
define|#
directive|define
name|DIFF_H
end_define
begin_function_decl
specifier|extern
name|void
name|diff_addremove
parameter_list|(
name|int
name|addremove
parameter_list|,
name|unsigned
name|mode
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
name|base
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|diff_change
parameter_list|(
name|unsigned
name|mode1
parameter_list|,
name|unsigned
name|mode2
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
name|sha2
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|diff_helper_input
parameter_list|(
name|unsigned
name|mode1
parameter_list|,
name|unsigned
name|mode2
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
name|sha2
parameter_list|,
specifier|const
name|char
modifier|*
name|path1
parameter_list|,
name|int
name|status
parameter_list|,
name|int
name|score
parameter_list|,
specifier|const
name|char
modifier|*
name|path2
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|diff_unmerge
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|diff_scoreopt_parse
parameter_list|(
specifier|const
name|char
modifier|*
name|opt
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|DIFF_SETUP_REVERSE
define|#
directive|define
name|DIFF_SETUP_REVERSE
value|1
end_define
begin_define
DECL|macro|DIFF_SETUP_USE_CACHE
define|#
directive|define
name|DIFF_SETUP_USE_CACHE
value|2
end_define
begin_define
DECL|macro|DIFF_SETUP_USE_SIZE_CACHE
define|#
directive|define
name|DIFF_SETUP_USE_SIZE_CACHE
value|4
end_define
begin_function_decl
specifier|extern
name|void
name|diff_setup
parameter_list|(
name|int
name|flags
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|DIFF_DETECT_RENAME
define|#
directive|define
name|DIFF_DETECT_RENAME
value|1
end_define
begin_define
DECL|macro|DIFF_DETECT_COPY
define|#
directive|define
name|DIFF_DETECT_COPY
value|2
end_define
begin_function_decl
specifier|extern
name|void
name|diffcore_rename
parameter_list|(
name|int
name|rename_copy
parameter_list|,
name|int
name|minimum_score
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|DIFF_PICKAXE_ALL
define|#
directive|define
name|DIFF_PICKAXE_ALL
value|1
end_define
begin_function_decl
specifier|extern
name|void
name|diffcore_pickaxe
parameter_list|(
specifier|const
name|char
modifier|*
name|needle
parameter_list|,
name|int
name|opts
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|diffcore_pathspec
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|pathspec
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|diffcore_break
parameter_list|(
name|int
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|void
name|diffcore_std
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|paths
parameter_list|,
name|int
name|detect_rename
parameter_list|,
name|int
name|rename_score
parameter_list|,
specifier|const
name|char
modifier|*
name|pickaxe
parameter_list|,
name|int
name|pickaxe_opts
parameter_list|,
name|int
name|break_opt
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|extern
name|int
name|diff_queue_is_empty
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_define
DECL|macro|DIFF_FORMAT_HUMAN
define|#
directive|define
name|DIFF_FORMAT_HUMAN
value|0
end_define
begin_define
DECL|macro|DIFF_FORMAT_MACHINE
define|#
directive|define
name|DIFF_FORMAT_MACHINE
value|1
end_define
begin_define
DECL|macro|DIFF_FORMAT_PATCH
define|#
directive|define
name|DIFF_FORMAT_PATCH
value|2
end_define
begin_define
DECL|macro|DIFF_FORMAT_NO_OUTPUT
define|#
directive|define
name|DIFF_FORMAT_NO_OUTPUT
value|3
end_define
begin_function_decl
specifier|extern
name|void
name|diff_flush
parameter_list|(
name|int
name|output_style
parameter_list|,
name|int
name|resolve_rename_copy
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
begin_comment
comment|/* DIFF_H */
end_comment
end_unit
