begin_unit
begin_ifndef
ifndef|#
directive|ifndef
name|SEND_PACK_H
end_ifndef
begin_define
DECL|macro|SEND_PACK_H
define|#
directive|define
name|SEND_PACK_H
end_define
begin_struct
DECL|struct|send_pack_args
struct|struct
name|send_pack_args
block|{
DECL|member|verbose
name|unsigned
name|verbose
range|:
literal|1
decl_stmt|,
DECL|member|quiet
name|quiet
range|:
literal|1
decl_stmt|,
DECL|member|porcelain
name|porcelain
range|:
literal|1
decl_stmt|,
DECL|member|progress
name|progress
range|:
literal|1
decl_stmt|,
DECL|member|send_mirror
name|send_mirror
range|:
literal|1
decl_stmt|,
DECL|member|force_update
name|force_update
range|:
literal|1
decl_stmt|,
DECL|member|use_thin_pack
name|use_thin_pack
range|:
literal|1
decl_stmt|,
DECL|member|use_ofs_delta
name|use_ofs_delta
range|:
literal|1
decl_stmt|,
DECL|member|dry_run
name|dry_run
range|:
literal|1
decl_stmt|,
DECL|member|push_cert
name|push_cert
range|:
literal|1
decl_stmt|,
DECL|member|stateless_rpc
name|stateless_rpc
range|:
literal|1
decl_stmt|;
block|}
struct|;
end_struct
begin_function_decl
name|int
name|send_pack
parameter_list|(
name|struct
name|send_pack_args
modifier|*
name|args
parameter_list|,
name|int
name|fd
index|[]
parameter_list|,
name|struct
name|child_process
modifier|*
name|conn
parameter_list|,
name|struct
name|ref
modifier|*
name|remote_refs
parameter_list|,
name|struct
name|sha1_array
modifier|*
name|extra_have
parameter_list|)
function_decl|;
end_function_decl
begin_endif
endif|#
directive|endif
end_endif
end_unit
