begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|FAST_EXPORT_H_
end_ifndef
begin_define
DECL|macro|FAST_EXPORT_H_
define|#
directive|define
name|FAST_EXPORT_H_
end_define
begin_include
include|#
directive|include
file|"line_buffer.h"
end_include
begin_function_decl
name|void
name|fast_export_init
parameter_list|(
name|int
name|fd
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|fast_export_deinit
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|fast_export_reset
parameter_list|(
name|void
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|fast_export_delete
parameter_list|(
name|uint32_t
name|depth
parameter_list|,
name|uint32_t
modifier|*
name|path
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|fast_export_modify
parameter_list|(
name|uint32_t
name|depth
parameter_list|,
name|uint32_t
modifier|*
name|path
parameter_list|,
name|uint32_t
name|mode
parameter_list|,
name|uint32_t
name|mark
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|fast_export_begin_commit
parameter_list|(
name|uint32_t
name|revision
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|fast_export_commit
parameter_list|(
name|uint32_t
name|revision
parameter_list|,
name|uint32_t
name|author
parameter_list|,
name|char
modifier|*
name|log
parameter_list|,
name|uint32_t
name|uuid
parameter_list|,
name|uint32_t
name|url
parameter_list|,
name|unsigned
name|long
name|timestamp
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
name|void
name|fast_export_blob
parameter_list|(
name|uint32_t
name|mode
parameter_list|,
name|uint32_t
name|mark
parameter_list|,
name|uint32_t
name|len
parameter_list|,
name|struct
name|line_buffer
modifier|*
name|input
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
