begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"xdiff-interface.h"
end_include
begin_include
include|#
directive|include
file|"ll-merge.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_function
DECL|function|fill_mmfile_blob
specifier|static
name|int
name|fill_mmfile_blob
parameter_list|(
name|mmfile_t
modifier|*
name|f
parameter_list|,
name|struct
name|blob
modifier|*
name|obj
parameter_list|)
block|{
name|void
modifier|*
name|buf
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|buf
operator|=
name|read_sha1_file
argument_list|(
name|obj
operator|->
name|object
operator|.
name|sha1
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|type
operator|!=
name|OBJ_BLOB
condition|)
return|return
operator|-
literal|1
return|;
name|f
operator|->
name|ptr
operator|=
name|buf
expr_stmt|;
name|f
operator|->
name|size
operator|=
name|size
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|free_mmfile
specifier|static
name|void
name|free_mmfile
parameter_list|(
name|mmfile_t
modifier|*
name|f
parameter_list|)
block|{
name|free
argument_list|(
name|f
operator|->
name|ptr
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|three_way_filemerge
specifier|static
name|void
modifier|*
name|three_way_filemerge
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|mmfile_t
modifier|*
name|base
parameter_list|,
name|mmfile_t
modifier|*
name|our
parameter_list|,
name|mmfile_t
modifier|*
name|their
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
block|{
name|int
name|merge_status
decl_stmt|;
name|mmbuffer_t
name|res
decl_stmt|;
name|merge_status
operator|=
name|ll_merge
argument_list|(
operator|&
name|res
argument_list|,
name|path
argument_list|,
name|base
argument_list|,
name|NULL
argument_list|,
name|our
argument_list|,
literal|".our"
argument_list|,
name|their
argument_list|,
literal|".their"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|merge_status
operator|<
literal|0
condition|)
return|return
name|NULL
return|;
operator|*
name|size
operator|=
name|res
operator|.
name|size
expr_stmt|;
return|return
name|res
operator|.
name|ptr
return|;
block|}
end_function
begin_function
DECL|function|common_outf
specifier|static
name|int
name|common_outf
parameter_list|(
name|void
modifier|*
name|priv_
parameter_list|,
name|mmbuffer_t
modifier|*
name|mb
parameter_list|,
name|int
name|nbuf
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|mmfile_t
modifier|*
name|dst
init|=
name|priv_
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nbuf
condition|;
name|i
operator|++
control|)
block|{
name|memcpy
argument_list|(
name|dst
operator|->
name|ptr
operator|+
name|dst
operator|->
name|size
argument_list|,
name|mb
index|[
name|i
index|]
operator|.
name|ptr
argument_list|,
name|mb
index|[
name|i
index|]
operator|.
name|size
argument_list|)
expr_stmt|;
name|dst
operator|->
name|size
operator|+=
name|mb
index|[
name|i
index|]
operator|.
name|size
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|generate_common_file
specifier|static
name|int
name|generate_common_file
parameter_list|(
name|mmfile_t
modifier|*
name|res
parameter_list|,
name|mmfile_t
modifier|*
name|f1
parameter_list|,
name|mmfile_t
modifier|*
name|f2
parameter_list|)
block|{
name|unsigned
name|long
name|size
init|=
name|f1
operator|->
name|size
operator|<
name|f2
operator|->
name|size
condition|?
name|f1
operator|->
name|size
else|:
name|f2
operator|->
name|size
decl_stmt|;
name|void
modifier|*
name|ptr
init|=
name|xmalloc
argument_list|(
name|size
argument_list|)
decl_stmt|;
name|xpparam_t
name|xpp
decl_stmt|;
name|xdemitconf_t
name|xecfg
decl_stmt|;
name|xdemitcb_t
name|ecb
decl_stmt|;
name|memset
argument_list|(
operator|&
name|xpp
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|xpp
argument_list|)
argument_list|)
expr_stmt|;
name|xpp
operator|.
name|flags
operator|=
name|XDF_NEED_MINIMAL
expr_stmt|;
name|memset
argument_list|(
operator|&
name|xecfg
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|xecfg
argument_list|)
argument_list|)
expr_stmt|;
name|xecfg
operator|.
name|ctxlen
operator|=
literal|3
expr_stmt|;
name|xecfg
operator|.
name|flags
operator|=
name|XDL_EMIT_COMMON
expr_stmt|;
name|ecb
operator|.
name|outf
operator|=
name|common_outf
expr_stmt|;
name|res
operator|->
name|ptr
operator|=
name|ptr
expr_stmt|;
name|res
operator|->
name|size
operator|=
literal|0
expr_stmt|;
name|ecb
operator|.
name|priv
operator|=
name|res
expr_stmt|;
return|return
name|xdi_diff
argument_list|(
name|f1
argument_list|,
name|f2
argument_list|,
operator|&
name|xpp
argument_list|,
operator|&
name|xecfg
argument_list|,
operator|&
name|ecb
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|merge_file
name|void
modifier|*
name|merge_file
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|blob
modifier|*
name|base
parameter_list|,
name|struct
name|blob
modifier|*
name|our
parameter_list|,
name|struct
name|blob
modifier|*
name|their
parameter_list|,
name|unsigned
name|long
modifier|*
name|size
parameter_list|)
block|{
name|void
modifier|*
name|res
init|=
name|NULL
decl_stmt|;
name|mmfile_t
name|f1
decl_stmt|,
name|f2
decl_stmt|,
name|common
decl_stmt|;
comment|/* 	 * Removed in either branch? 	 * 	 * NOTE! This depends on the caller having done the 	 * proper warning about removing a file that got 	 * modified in the other branch! 	 */
if|if
condition|(
operator|!
name|our
operator|||
operator|!
name|their
condition|)
block|{
name|enum
name|object_type
name|type
decl_stmt|;
if|if
condition|(
name|base
condition|)
return|return
name|NULL
return|;
if|if
condition|(
operator|!
name|our
condition|)
name|our
operator|=
name|their
expr_stmt|;
return|return
name|read_sha1_file
argument_list|(
name|our
operator|->
name|object
operator|.
name|sha1
argument_list|,
operator|&
name|type
argument_list|,
name|size
argument_list|)
return|;
block|}
if|if
condition|(
name|fill_mmfile_blob
argument_list|(
operator|&
name|f1
argument_list|,
name|our
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|out_no_mmfile
goto|;
if|if
condition|(
name|fill_mmfile_blob
argument_list|(
operator|&
name|f2
argument_list|,
name|their
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|out_free_f1
goto|;
if|if
condition|(
name|base
condition|)
block|{
if|if
condition|(
name|fill_mmfile_blob
argument_list|(
operator|&
name|common
argument_list|,
name|base
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|out_free_f2_f1
goto|;
block|}
else|else
block|{
if|if
condition|(
name|generate_common_file
argument_list|(
operator|&
name|common
argument_list|,
operator|&
name|f1
argument_list|,
operator|&
name|f2
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|out_free_f2_f1
goto|;
block|}
name|res
operator|=
name|three_way_filemerge
argument_list|(
name|path
argument_list|,
operator|&
name|common
argument_list|,
operator|&
name|f1
argument_list|,
operator|&
name|f2
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|free_mmfile
argument_list|(
operator|&
name|common
argument_list|)
expr_stmt|;
name|out_free_f2_f1
label|:
name|free_mmfile
argument_list|(
operator|&
name|f2
argument_list|)
expr_stmt|;
name|out_free_f1
label|:
name|free_mmfile
argument_list|(
operator|&
name|f1
argument_list|)
expr_stmt|;
name|out_no_mmfile
label|:
return|return
name|res
return|;
block|}
end_function
end_unit
