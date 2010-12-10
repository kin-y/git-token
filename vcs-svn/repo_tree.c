begin_unit
begin_comment
comment|/*  * Licensed under a two-clause BSD-style license.  * See LICENSE for details.  */
end_comment
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"repo_tree.h"
end_include
begin_include
include|#
directive|include
file|"fast_export.h"
end_include
begin_function
DECL|function|repo_read_path
specifier|const
name|char
modifier|*
name|repo_read_path
parameter_list|(
specifier|const
name|uint32_t
modifier|*
name|path
parameter_list|)
block|{
name|int
name|err
decl_stmt|;
name|uint32_t
name|dummy
decl_stmt|;
specifier|static
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
name|err
operator|=
name|fast_export_ls
argument_list|(
name|REPO_MAX_PATH_DEPTH
argument_list|,
name|path
argument_list|,
operator|&
name|dummy
argument_list|,
operator|&
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|ENOENT
condition|)
name|die_errno
argument_list|(
literal|"BUG: unexpected fast_export_ls error"
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
return|return
name|buf
operator|.
name|buf
return|;
block|}
end_function
begin_function
DECL|function|repo_read_mode
name|uint32_t
name|repo_read_mode
parameter_list|(
specifier|const
name|uint32_t
modifier|*
name|path
parameter_list|)
block|{
name|int
name|err
decl_stmt|;
name|uint32_t
name|result
decl_stmt|;
specifier|static
name|struct
name|strbuf
name|dummy
init|=
name|STRBUF_INIT
decl_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|dummy
argument_list|)
expr_stmt|;
name|err
operator|=
name|fast_export_ls
argument_list|(
name|REPO_MAX_PATH_DEPTH
argument_list|,
name|path
argument_list|,
operator|&
name|result
argument_list|,
operator|&
name|dummy
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|ENOENT
condition|)
name|die_errno
argument_list|(
literal|"BUG: unexpected fast_export_ls error"
argument_list|)
expr_stmt|;
comment|/* Treat missing paths as directories. */
return|return
name|REPO_MODE_DIR
return|;
block|}
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|repo_copy
name|void
name|repo_copy
parameter_list|(
name|uint32_t
name|revision
parameter_list|,
specifier|const
name|uint32_t
modifier|*
name|src
parameter_list|,
specifier|const
name|uint32_t
modifier|*
name|dst
parameter_list|)
block|{
name|int
name|err
decl_stmt|;
name|uint32_t
name|mode
decl_stmt|;
specifier|static
name|struct
name|strbuf
name|data
init|=
name|STRBUF_INIT
decl_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|data
argument_list|)
expr_stmt|;
name|err
operator|=
name|fast_export_ls_rev
argument_list|(
name|revision
argument_list|,
name|REPO_MAX_PATH_DEPTH
argument_list|,
name|src
argument_list|,
operator|&
name|mode
argument_list|,
operator|&
name|data
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|ENOENT
condition|)
name|die_errno
argument_list|(
literal|"BUG: unexpected fast_export_ls_rev error"
argument_list|)
expr_stmt|;
name|fast_export_delete
argument_list|(
name|REPO_MAX_PATH_DEPTH
argument_list|,
name|dst
argument_list|)
expr_stmt|;
return|return;
block|}
name|fast_export_modify
argument_list|(
name|REPO_MAX_PATH_DEPTH
argument_list|,
name|dst
argument_list|,
name|mode
argument_list|,
name|data
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|repo_delete
name|void
name|repo_delete
parameter_list|(
name|uint32_t
modifier|*
name|path
parameter_list|)
block|{
name|fast_export_delete
argument_list|(
name|REPO_MAX_PATH_DEPTH
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
