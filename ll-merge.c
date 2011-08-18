begin_unit
begin_comment
comment|/*  * Low level 3-way in-core file merge.  *  * Copyright (c) 2007 Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"attr.h"
end_include
begin_include
include|#
directive|include
file|"xdiff-interface.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"ll-merge.h"
end_include
begin_struct_decl
struct_decl|struct
name|ll_merge_driver
struct_decl|;
end_struct_decl
begin_typedef
DECL|typedef|ll_merge_fn
typedef|typedef
name|int
function_decl|(
modifier|*
name|ll_merge_fn
function_decl|)
parameter_list|(
specifier|const
name|struct
name|ll_merge_driver
modifier|*
parameter_list|,
name|mmbuffer_t
modifier|*
name|result
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|mmfile_t
modifier|*
name|orig
parameter_list|,
specifier|const
name|char
modifier|*
name|orig_name
parameter_list|,
name|mmfile_t
modifier|*
name|src1
parameter_list|,
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
name|mmfile_t
modifier|*
name|src2
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|,
specifier|const
name|struct
name|ll_merge_options
modifier|*
name|opts
parameter_list|,
name|int
name|marker_size
parameter_list|)
function_decl|;
end_typedef
begin_struct
DECL|struct|ll_merge_driver
struct|struct
name|ll_merge_driver
block|{
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|description
specifier|const
name|char
modifier|*
name|description
decl_stmt|;
DECL|member|fn
name|ll_merge_fn
name|fn
decl_stmt|;
DECL|member|recursive
specifier|const
name|char
modifier|*
name|recursive
decl_stmt|;
DECL|member|next
name|struct
name|ll_merge_driver
modifier|*
name|next
decl_stmt|;
DECL|member|cmdline
name|char
modifier|*
name|cmdline
decl_stmt|;
block|}
struct|;
end_struct
begin_comment
comment|/*  * Built-in low-levels  */
end_comment
begin_function
DECL|function|ll_binary_merge
specifier|static
name|int
name|ll_binary_merge
parameter_list|(
specifier|const
name|struct
name|ll_merge_driver
modifier|*
name|drv_unused
parameter_list|,
name|mmbuffer_t
modifier|*
name|result
parameter_list|,
specifier|const
name|char
modifier|*
name|path_unused
parameter_list|,
name|mmfile_t
modifier|*
name|orig
parameter_list|,
specifier|const
name|char
modifier|*
name|orig_name
parameter_list|,
name|mmfile_t
modifier|*
name|src1
parameter_list|,
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
name|mmfile_t
modifier|*
name|src2
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|,
specifier|const
name|struct
name|ll_merge_options
modifier|*
name|opts
parameter_list|,
name|int
name|marker_size
parameter_list|)
block|{
name|mmfile_t
modifier|*
name|stolen
decl_stmt|;
name|assert
argument_list|(
name|opts
argument_list|)
expr_stmt|;
comment|/* 	 * The tentative merge result is "ours" for the final round, 	 * or common ancestor for an internal merge.  Still return 	 * "conflicted merge" status. 	 */
name|stolen
operator|=
name|opts
operator|->
name|virtual_ancestor
condition|?
name|orig
else|:
name|src1
expr_stmt|;
name|result
operator|->
name|ptr
operator|=
name|stolen
operator|->
name|ptr
expr_stmt|;
name|result
operator|->
name|size
operator|=
name|stolen
operator|->
name|size
expr_stmt|;
name|stolen
operator|->
name|ptr
operator|=
name|NULL
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|ll_xdl_merge
specifier|static
name|int
name|ll_xdl_merge
parameter_list|(
specifier|const
name|struct
name|ll_merge_driver
modifier|*
name|drv_unused
parameter_list|,
name|mmbuffer_t
modifier|*
name|result
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|mmfile_t
modifier|*
name|orig
parameter_list|,
specifier|const
name|char
modifier|*
name|orig_name
parameter_list|,
name|mmfile_t
modifier|*
name|src1
parameter_list|,
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
name|mmfile_t
modifier|*
name|src2
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|,
specifier|const
name|struct
name|ll_merge_options
modifier|*
name|opts
parameter_list|,
name|int
name|marker_size
parameter_list|)
block|{
name|xmparam_t
name|xmp
decl_stmt|;
name|assert
argument_list|(
name|opts
argument_list|)
expr_stmt|;
if|if
condition|(
name|buffer_is_binary
argument_list|(
name|orig
operator|->
name|ptr
argument_list|,
name|orig
operator|->
name|size
argument_list|)
operator|||
name|buffer_is_binary
argument_list|(
name|src1
operator|->
name|ptr
argument_list|,
name|src1
operator|->
name|size
argument_list|)
operator|||
name|buffer_is_binary
argument_list|(
name|src2
operator|->
name|ptr
argument_list|,
name|src2
operator|->
name|size
argument_list|)
condition|)
block|{
name|warning
argument_list|(
literal|"Cannot merge binary files: %s (%s vs. %s)\n"
argument_list|,
name|path
argument_list|,
name|name1
argument_list|,
name|name2
argument_list|)
expr_stmt|;
return|return
name|ll_binary_merge
argument_list|(
name|drv_unused
argument_list|,
name|result
argument_list|,
name|path
argument_list|,
name|orig
argument_list|,
name|orig_name
argument_list|,
name|src1
argument_list|,
name|name1
argument_list|,
name|src2
argument_list|,
name|name2
argument_list|,
name|opts
argument_list|,
name|marker_size
argument_list|)
return|;
block|}
name|memset
argument_list|(
operator|&
name|xmp
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|xmp
argument_list|)
argument_list|)
expr_stmt|;
name|xmp
operator|.
name|level
operator|=
name|XDL_MERGE_ZEALOUS
expr_stmt|;
name|xmp
operator|.
name|favor
operator|=
name|opts
operator|->
name|variant
expr_stmt|;
name|xmp
operator|.
name|xpp
operator|.
name|flags
operator|=
name|opts
operator|->
name|xdl_opts
expr_stmt|;
if|if
condition|(
name|git_xmerge_style
operator|>=
literal|0
condition|)
name|xmp
operator|.
name|style
operator|=
name|git_xmerge_style
expr_stmt|;
if|if
condition|(
name|marker_size
operator|>
literal|0
condition|)
name|xmp
operator|.
name|marker_size
operator|=
name|marker_size
expr_stmt|;
name|xmp
operator|.
name|ancestor
operator|=
name|orig_name
expr_stmt|;
name|xmp
operator|.
name|file1
operator|=
name|name1
expr_stmt|;
name|xmp
operator|.
name|file2
operator|=
name|name2
expr_stmt|;
return|return
name|xdl_merge
argument_list|(
name|orig
argument_list|,
name|src1
argument_list|,
name|src2
argument_list|,
operator|&
name|xmp
argument_list|,
name|result
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|ll_union_merge
specifier|static
name|int
name|ll_union_merge
parameter_list|(
specifier|const
name|struct
name|ll_merge_driver
modifier|*
name|drv_unused
parameter_list|,
name|mmbuffer_t
modifier|*
name|result
parameter_list|,
specifier|const
name|char
modifier|*
name|path_unused
parameter_list|,
name|mmfile_t
modifier|*
name|orig
parameter_list|,
specifier|const
name|char
modifier|*
name|orig_name
parameter_list|,
name|mmfile_t
modifier|*
name|src1
parameter_list|,
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
name|mmfile_t
modifier|*
name|src2
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|,
specifier|const
name|struct
name|ll_merge_options
modifier|*
name|opts
parameter_list|,
name|int
name|marker_size
parameter_list|)
block|{
comment|/* Use union favor */
name|struct
name|ll_merge_options
name|o
decl_stmt|;
name|assert
argument_list|(
name|opts
argument_list|)
expr_stmt|;
name|o
operator|=
operator|*
name|opts
expr_stmt|;
name|o
operator|.
name|variant
operator|=
name|XDL_MERGE_FAVOR_UNION
expr_stmt|;
return|return
name|ll_xdl_merge
argument_list|(
name|drv_unused
argument_list|,
name|result
argument_list|,
name|path_unused
argument_list|,
name|orig
argument_list|,
name|NULL
argument_list|,
name|src1
argument_list|,
name|NULL
argument_list|,
name|src2
argument_list|,
name|NULL
argument_list|,
operator|&
name|o
argument_list|,
name|marker_size
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|LL_BINARY_MERGE
define|#
directive|define
name|LL_BINARY_MERGE
value|0
end_define
begin_define
DECL|macro|LL_TEXT_MERGE
define|#
directive|define
name|LL_TEXT_MERGE
value|1
end_define
begin_define
DECL|macro|LL_UNION_MERGE
define|#
directive|define
name|LL_UNION_MERGE
value|2
end_define
begin_decl_stmt
DECL|variable|ll_merge_drv
specifier|static
name|struct
name|ll_merge_driver
name|ll_merge_drv
index|[]
init|=
block|{
block|{
literal|"binary"
block|,
literal|"built-in binary merge"
block|,
name|ll_binary_merge
block|}
block|,
block|{
literal|"text"
block|,
literal|"built-in 3-way text merge"
block|,
name|ll_xdl_merge
block|}
block|,
block|{
literal|"union"
block|,
literal|"built-in union merge"
block|,
name|ll_union_merge
block|}
block|, }
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|create_temp
specifier|static
name|void
name|create_temp
parameter_list|(
name|mmfile_t
modifier|*
name|src
parameter_list|,
name|char
modifier|*
name|path
parameter_list|)
block|{
name|int
name|fd
decl_stmt|;
name|strcpy
argument_list|(
name|path
argument_list|,
literal|".merge_file_XXXXXX"
argument_list|)
expr_stmt|;
name|fd
operator|=
name|xmkstemp
argument_list|(
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_in_full
argument_list|(
name|fd
argument_list|,
name|src
operator|->
name|ptr
argument_list|,
name|src
operator|->
name|size
argument_list|)
operator|!=
name|src
operator|->
name|size
condition|)
name|die_errno
argument_list|(
literal|"unable to write temp-file"
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * User defined low-level merge driver support.  */
end_comment
begin_function
DECL|function|ll_ext_merge
specifier|static
name|int
name|ll_ext_merge
parameter_list|(
specifier|const
name|struct
name|ll_merge_driver
modifier|*
name|fn
parameter_list|,
name|mmbuffer_t
modifier|*
name|result
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|mmfile_t
modifier|*
name|orig
parameter_list|,
specifier|const
name|char
modifier|*
name|orig_name
parameter_list|,
name|mmfile_t
modifier|*
name|src1
parameter_list|,
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
name|mmfile_t
modifier|*
name|src2
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|,
specifier|const
name|struct
name|ll_merge_options
modifier|*
name|opts
parameter_list|,
name|int
name|marker_size
parameter_list|)
block|{
name|char
name|temp
index|[
literal|4
index|]
index|[
literal|50
index|]
decl_stmt|;
name|struct
name|strbuf
name|cmd
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf_expand_dict_entry
name|dict
index|[
literal|5
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|args
index|[]
init|=
block|{
name|NULL
block|,
name|NULL
block|}
decl_stmt|;
name|int
name|status
decl_stmt|,
name|fd
decl_stmt|,
name|i
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|assert
argument_list|(
name|opts
argument_list|)
expr_stmt|;
name|dict
index|[
literal|0
index|]
operator|.
name|placeholder
operator|=
literal|"O"
expr_stmt|;
name|dict
index|[
literal|0
index|]
operator|.
name|value
operator|=
name|temp
index|[
literal|0
index|]
expr_stmt|;
name|dict
index|[
literal|1
index|]
operator|.
name|placeholder
operator|=
literal|"A"
expr_stmt|;
name|dict
index|[
literal|1
index|]
operator|.
name|value
operator|=
name|temp
index|[
literal|1
index|]
expr_stmt|;
name|dict
index|[
literal|2
index|]
operator|.
name|placeholder
operator|=
literal|"B"
expr_stmt|;
name|dict
index|[
literal|2
index|]
operator|.
name|value
operator|=
name|temp
index|[
literal|2
index|]
expr_stmt|;
name|dict
index|[
literal|3
index|]
operator|.
name|placeholder
operator|=
literal|"L"
expr_stmt|;
name|dict
index|[
literal|3
index|]
operator|.
name|value
operator|=
name|temp
index|[
literal|3
index|]
expr_stmt|;
name|dict
index|[
literal|4
index|]
operator|.
name|placeholder
operator|=
name|NULL
expr_stmt|;
name|dict
index|[
literal|4
index|]
operator|.
name|value
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|fn
operator|->
name|cmdline
operator|==
name|NULL
condition|)
name|die
argument_list|(
literal|"custom merge driver %s lacks command line."
argument_list|,
name|fn
operator|->
name|name
argument_list|)
expr_stmt|;
name|result
operator|->
name|ptr
operator|=
name|NULL
expr_stmt|;
name|result
operator|->
name|size
operator|=
literal|0
expr_stmt|;
name|create_temp
argument_list|(
name|orig
argument_list|,
name|temp
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|create_temp
argument_list|(
name|src1
argument_list|,
name|temp
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|create_temp
argument_list|(
name|src2
argument_list|,
name|temp
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|temp
index|[
literal|3
index|]
argument_list|,
literal|"%d"
argument_list|,
name|marker_size
argument_list|)
expr_stmt|;
name|strbuf_expand
argument_list|(
operator|&
name|cmd
argument_list|,
name|fn
operator|->
name|cmdline
argument_list|,
name|strbuf_expand_dict_cb
argument_list|,
operator|&
name|dict
argument_list|)
expr_stmt|;
name|args
index|[
literal|0
index|]
operator|=
name|cmd
operator|.
name|buf
expr_stmt|;
name|status
operator|=
name|run_command_v_opt
argument_list|(
name|args
argument_list|,
name|RUN_USING_SHELL
argument_list|)
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|temp
index|[
literal|1
index|]
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
goto|goto
name|bad
goto|;
if|if
condition|(
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|st
argument_list|)
condition|)
goto|goto
name|close_bad
goto|;
name|result
operator|->
name|size
operator|=
name|st
operator|.
name|st_size
expr_stmt|;
name|result
operator|->
name|ptr
operator|=
name|xmalloc
argument_list|(
name|result
operator|->
name|size
operator|+
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|read_in_full
argument_list|(
name|fd
argument_list|,
name|result
operator|->
name|ptr
argument_list|,
name|result
operator|->
name|size
argument_list|)
operator|!=
name|result
operator|->
name|size
condition|)
block|{
name|free
argument_list|(
name|result
operator|->
name|ptr
argument_list|)
expr_stmt|;
name|result
operator|->
name|ptr
operator|=
name|NULL
expr_stmt|;
name|result
operator|->
name|size
operator|=
literal|0
expr_stmt|;
block|}
name|close_bad
label|:
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|bad
label|:
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|3
condition|;
name|i
operator|++
control|)
name|unlink_or_warn
argument_list|(
name|temp
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|cmd
argument_list|)
expr_stmt|;
return|return
name|status
return|;
block|}
end_function
begin_comment
comment|/*  * merge.default and merge.driver configuration items  */
end_comment
begin_decl_stmt
DECL|variable|ll_user_merge
DECL|variable|ll_user_merge_tail
specifier|static
name|struct
name|ll_merge_driver
modifier|*
name|ll_user_merge
decl_stmt|,
modifier|*
modifier|*
name|ll_user_merge_tail
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|default_ll_merge
specifier|static
specifier|const
name|char
modifier|*
name|default_ll_merge
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|read_merge_config
specifier|static
name|int
name|read_merge_config
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|,
name|void
modifier|*
name|cb
parameter_list|)
block|{
name|struct
name|ll_merge_driver
modifier|*
name|fn
decl_stmt|;
specifier|const
name|char
modifier|*
name|ep
decl_stmt|,
modifier|*
name|name
decl_stmt|;
name|int
name|namelen
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"merge.default"
argument_list|)
condition|)
block|{
if|if
condition|(
name|value
condition|)
name|default_ll_merge
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
comment|/* 	 * We are not interested in anything but "merge.<name>.variable"; 	 * especially, we do not want to look at variables such as 	 * "merge.summary", "merge.tool", and "merge.verbosity". 	 */
if|if
condition|(
name|prefixcmp
argument_list|(
name|var
argument_list|,
literal|"merge."
argument_list|)
operator|||
operator|(
name|ep
operator|=
name|strrchr
argument_list|(
name|var
argument_list|,
literal|'.'
argument_list|)
operator|)
operator|==
name|var
operator|+
literal|5
condition|)
return|return
literal|0
return|;
comment|/* 	 * Find existing one as we might be processing merge.<name>.var2 	 * after seeing merge.<name>.var1. 	 */
name|name
operator|=
name|var
operator|+
literal|6
expr_stmt|;
name|namelen
operator|=
name|ep
operator|-
name|name
expr_stmt|;
for|for
control|(
name|fn
operator|=
name|ll_user_merge
init|;
name|fn
condition|;
name|fn
operator|=
name|fn
operator|->
name|next
control|)
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|fn
operator|->
name|name
argument_list|,
name|name
argument_list|,
name|namelen
argument_list|)
operator|&&
operator|!
name|fn
operator|->
name|name
index|[
name|namelen
index|]
condition|)
break|break;
if|if
condition|(
operator|!
name|fn
condition|)
block|{
name|fn
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|ll_merge_driver
argument_list|)
argument_list|)
expr_stmt|;
name|fn
operator|->
name|name
operator|=
name|xmemdupz
argument_list|(
name|name
argument_list|,
name|namelen
argument_list|)
expr_stmt|;
name|fn
operator|->
name|fn
operator|=
name|ll_ext_merge
expr_stmt|;
operator|*
name|ll_user_merge_tail
operator|=
name|fn
expr_stmt|;
name|ll_user_merge_tail
operator|=
operator|&
operator|(
name|fn
operator|->
name|next
operator|)
expr_stmt|;
block|}
name|ep
operator|++
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"name"
argument_list|,
name|ep
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|error
argument_list|(
literal|"%s: lacks value"
argument_list|,
name|var
argument_list|)
return|;
name|fn
operator|->
name|description
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"driver"
argument_list|,
name|ep
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|error
argument_list|(
literal|"%s: lacks value"
argument_list|,
name|var
argument_list|)
return|;
comment|/* 		 * merge.<name>.driver specifies the command line: 		 * 		 *	command-line 		 * 		 * The command-line will be interpolated with the following 		 * tokens and is given to the shell: 		 * 		 *    %O - temporary file name for the merge base. 		 *    %A - temporary file name for our version. 		 *    %B - temporary file name for the other branches' version. 		 *    %L - conflict marker length 		 * 		 * The external merge driver should write the results in the 		 * file named by %A, and signal that it has done with zero exit 		 * status. 		 */
name|fn
operator|->
name|cmdline
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"recursive"
argument_list|,
name|ep
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|error
argument_list|(
literal|"%s: lacks value"
argument_list|,
name|var
argument_list|)
return|;
name|fn
operator|->
name|recursive
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|initialize_ll_merge
specifier|static
name|void
name|initialize_ll_merge
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|ll_user_merge_tail
condition|)
return|return;
name|ll_user_merge_tail
operator|=
operator|&
name|ll_user_merge
expr_stmt|;
name|git_config
argument_list|(
name|read_merge_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|find_ll_merge_driver
specifier|static
specifier|const
name|struct
name|ll_merge_driver
modifier|*
name|find_ll_merge_driver
parameter_list|(
specifier|const
name|char
modifier|*
name|merge_attr
parameter_list|)
block|{
name|struct
name|ll_merge_driver
modifier|*
name|fn
decl_stmt|;
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
name|int
name|i
decl_stmt|;
name|initialize_ll_merge
argument_list|()
expr_stmt|;
if|if
condition|(
name|ATTR_TRUE
argument_list|(
name|merge_attr
argument_list|)
condition|)
return|return
operator|&
name|ll_merge_drv
index|[
name|LL_TEXT_MERGE
index|]
return|;
elseif|else
if|if
condition|(
name|ATTR_FALSE
argument_list|(
name|merge_attr
argument_list|)
condition|)
return|return
operator|&
name|ll_merge_drv
index|[
name|LL_BINARY_MERGE
index|]
return|;
elseif|else
if|if
condition|(
name|ATTR_UNSET
argument_list|(
name|merge_attr
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|default_ll_merge
condition|)
return|return
operator|&
name|ll_merge_drv
index|[
name|LL_TEXT_MERGE
index|]
return|;
else|else
name|name
operator|=
name|default_ll_merge
expr_stmt|;
block|}
else|else
name|name
operator|=
name|merge_attr
expr_stmt|;
for|for
control|(
name|fn
operator|=
name|ll_user_merge
init|;
name|fn
condition|;
name|fn
operator|=
name|fn
operator|->
name|next
control|)
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|fn
operator|->
name|name
argument_list|,
name|name
argument_list|)
condition|)
return|return
name|fn
return|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ARRAY_SIZE
argument_list|(
name|ll_merge_drv
argument_list|)
condition|;
name|i
operator|++
control|)
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|ll_merge_drv
index|[
name|i
index|]
operator|.
name|name
argument_list|,
name|name
argument_list|)
condition|)
return|return
operator|&
name|ll_merge_drv
index|[
name|i
index|]
return|;
comment|/* default to the 3-way */
return|return
operator|&
name|ll_merge_drv
index|[
name|LL_TEXT_MERGE
index|]
return|;
block|}
end_function
begin_function
DECL|function|git_path_check_merge
specifier|static
name|int
name|git_path_check_merge
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|git_attr_check
name|check
index|[
literal|2
index|]
parameter_list|)
block|{
if|if
condition|(
operator|!
name|check
index|[
literal|0
index|]
operator|.
name|attr
condition|)
block|{
name|check
index|[
literal|0
index|]
operator|.
name|attr
operator|=
name|git_attr
argument_list|(
literal|"merge"
argument_list|)
expr_stmt|;
name|check
index|[
literal|1
index|]
operator|.
name|attr
operator|=
name|git_attr
argument_list|(
literal|"conflict-marker-size"
argument_list|)
expr_stmt|;
block|}
return|return
name|git_check_attr
argument_list|(
name|path
argument_list|,
literal|2
argument_list|,
name|check
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|normalize_file
specifier|static
name|void
name|normalize_file
parameter_list|(
name|mmfile_t
modifier|*
name|mm
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|struct
name|strbuf
name|strbuf
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
name|renormalize_buffer
argument_list|(
name|path
argument_list|,
name|mm
operator|->
name|ptr
argument_list|,
name|mm
operator|->
name|size
argument_list|,
operator|&
name|strbuf
argument_list|)
condition|)
block|{
name|free
argument_list|(
name|mm
operator|->
name|ptr
argument_list|)
expr_stmt|;
name|mm
operator|->
name|size
operator|=
name|strbuf
operator|.
name|len
expr_stmt|;
name|mm
operator|->
name|ptr
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|strbuf
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|ll_merge
name|int
name|ll_merge
parameter_list|(
name|mmbuffer_t
modifier|*
name|result_buf
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|mmfile_t
modifier|*
name|ancestor
parameter_list|,
specifier|const
name|char
modifier|*
name|ancestor_label
parameter_list|,
name|mmfile_t
modifier|*
name|ours
parameter_list|,
specifier|const
name|char
modifier|*
name|our_label
parameter_list|,
name|mmfile_t
modifier|*
name|theirs
parameter_list|,
specifier|const
name|char
modifier|*
name|their_label
parameter_list|,
specifier|const
name|struct
name|ll_merge_options
modifier|*
name|opts
parameter_list|)
block|{
specifier|static
name|struct
name|git_attr_check
name|check
index|[
literal|2
index|]
decl_stmt|;
specifier|static
specifier|const
name|struct
name|ll_merge_options
name|default_opts
decl_stmt|;
specifier|const
name|char
modifier|*
name|ll_driver_name
init|=
name|NULL
decl_stmt|;
name|int
name|marker_size
init|=
name|DEFAULT_CONFLICT_MARKER_SIZE
decl_stmt|;
specifier|const
name|struct
name|ll_merge_driver
modifier|*
name|driver
decl_stmt|;
if|if
condition|(
operator|!
name|opts
condition|)
name|opts
operator|=
operator|&
name|default_opts
expr_stmt|;
if|if
condition|(
name|opts
operator|->
name|renormalize
condition|)
block|{
name|normalize_file
argument_list|(
name|ancestor
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|normalize_file
argument_list|(
name|ours
argument_list|,
name|path
argument_list|)
expr_stmt|;
name|normalize_file
argument_list|(
name|theirs
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|git_path_check_merge
argument_list|(
name|path
argument_list|,
name|check
argument_list|)
condition|)
block|{
name|ll_driver_name
operator|=
name|check
index|[
literal|0
index|]
operator|.
name|value
expr_stmt|;
if|if
condition|(
name|check
index|[
literal|1
index|]
operator|.
name|value
condition|)
block|{
name|marker_size
operator|=
name|atoi
argument_list|(
name|check
index|[
literal|1
index|]
operator|.
name|value
argument_list|)
expr_stmt|;
if|if
condition|(
name|marker_size
operator|<=
literal|0
condition|)
name|marker_size
operator|=
name|DEFAULT_CONFLICT_MARKER_SIZE
expr_stmt|;
block|}
block|}
name|driver
operator|=
name|find_ll_merge_driver
argument_list|(
name|ll_driver_name
argument_list|)
expr_stmt|;
if|if
condition|(
name|opts
operator|->
name|virtual_ancestor
operator|&&
name|driver
operator|->
name|recursive
condition|)
name|driver
operator|=
name|find_ll_merge_driver
argument_list|(
name|driver
operator|->
name|recursive
argument_list|)
expr_stmt|;
return|return
name|driver
operator|->
name|fn
argument_list|(
name|driver
argument_list|,
name|result_buf
argument_list|,
name|path
argument_list|,
name|ancestor
argument_list|,
name|ancestor_label
argument_list|,
name|ours
argument_list|,
name|our_label
argument_list|,
name|theirs
argument_list|,
name|their_label
argument_list|,
name|opts
argument_list|,
name|marker_size
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|ll_merge_marker_size
name|int
name|ll_merge_marker_size
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
specifier|static
name|struct
name|git_attr_check
name|check
decl_stmt|;
name|int
name|marker_size
init|=
name|DEFAULT_CONFLICT_MARKER_SIZE
decl_stmt|;
if|if
condition|(
operator|!
name|check
operator|.
name|attr
condition|)
name|check
operator|.
name|attr
operator|=
name|git_attr
argument_list|(
literal|"conflict-marker-size"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|git_check_attr
argument_list|(
name|path
argument_list|,
literal|1
argument_list|,
operator|&
name|check
argument_list|)
operator|&&
name|check
operator|.
name|value
condition|)
block|{
name|marker_size
operator|=
name|atoi
argument_list|(
name|check
operator|.
name|value
argument_list|)
expr_stmt|;
if|if
condition|(
name|marker_size
operator|<=
literal|0
condition|)
name|marker_size
operator|=
name|DEFAULT_CONFLICT_MARKER_SIZE
expr_stmt|;
block|}
return|return
name|marker_size
return|;
block|}
end_function
end_unit
