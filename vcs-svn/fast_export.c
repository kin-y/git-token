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
file|"quote.h"
end_include
begin_include
include|#
directive|include
file|"fast_export.h"
end_include
begin_include
include|#
directive|include
file|"repo_tree.h"
end_include
begin_include
include|#
directive|include
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"svndiff.h"
end_include
begin_include
include|#
directive|include
file|"sliding_window.h"
end_include
begin_include
include|#
directive|include
file|"line_buffer.h"
end_include
begin_define
DECL|macro|MAX_GITSVN_LINE_LEN
define|#
directive|define
name|MAX_GITSVN_LINE_LEN
value|4096
end_define
begin_decl_stmt
DECL|variable|first_commit_done
specifier|static
name|uint32_t
name|first_commit_done
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|postimage
specifier|static
name|struct
name|line_buffer
name|postimage
init|=
name|LINE_BUFFER_INIT
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|report_buffer
specifier|static
name|struct
name|line_buffer
name|report_buffer
init|=
name|LINE_BUFFER_INIT
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* NEEDSWORK: move to fast_export_init() */
end_comment
begin_function
DECL|function|init_postimage
specifier|static
name|int
name|init_postimage
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
name|int
name|postimage_initialized
decl_stmt|;
if|if
condition|(
name|postimage_initialized
condition|)
return|return
literal|0
return|;
name|postimage_initialized
operator|=
literal|1
expr_stmt|;
return|return
name|buffer_tmpfile_init
argument_list|(
operator|&
name|postimage
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|fast_export_init
name|void
name|fast_export_init
parameter_list|(
name|int
name|fd
parameter_list|)
block|{
name|first_commit_done
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|buffer_fdinit
argument_list|(
operator|&
name|report_buffer
argument_list|,
name|fd
argument_list|)
condition|)
name|die_errno
argument_list|(
literal|"cannot read from file descriptor %d"
argument_list|,
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fast_export_deinit
name|void
name|fast_export_deinit
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|buffer_deinit
argument_list|(
operator|&
name|report_buffer
argument_list|)
condition|)
name|die_errno
argument_list|(
literal|"error closing fast-import feedback stream"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fast_export_reset
name|void
name|fast_export_reset
parameter_list|(
name|void
parameter_list|)
block|{
name|buffer_reset
argument_list|(
operator|&
name|report_buffer
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fast_export_delete
name|void
name|fast_export_delete
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|putchar
argument_list|(
literal|'D'
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|' '
argument_list|)
expr_stmt|;
name|quote_c_style
argument_list|(
name|path
argument_list|,
name|NULL
argument_list|,
name|stdout
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fast_export_truncate
specifier|static
name|void
name|fast_export_truncate
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|uint32_t
name|mode
parameter_list|)
block|{
name|fast_export_modify
argument_list|(
name|path
argument_list|,
name|mode
argument_list|,
literal|"inline"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"data 0\n\n"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fast_export_modify
name|void
name|fast_export_modify
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|uint32_t
name|mode
parameter_list|,
specifier|const
name|char
modifier|*
name|dataref
parameter_list|)
block|{
comment|/* Mode must be 100644, 100755, 120000, or 160000. */
if|if
condition|(
operator|!
name|dataref
condition|)
block|{
name|fast_export_truncate
argument_list|(
name|path
argument_list|,
name|mode
argument_list|)
expr_stmt|;
return|return;
block|}
name|printf
argument_list|(
literal|"M %06"
name|PRIo32
literal|" %s "
argument_list|,
name|mode
argument_list|,
name|dataref
argument_list|)
expr_stmt|;
name|quote_c_style
argument_list|(
name|path
argument_list|,
name|NULL
argument_list|,
name|stdout
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|variable|gitsvnline
specifier|static
name|char
name|gitsvnline
index|[
name|MAX_GITSVN_LINE_LEN
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|fast_export_begin_commit
name|void
name|fast_export_begin_commit
parameter_list|(
name|uint32_t
name|revision
parameter_list|,
specifier|const
name|char
modifier|*
name|author
parameter_list|,
specifier|const
name|struct
name|strbuf
modifier|*
name|log
parameter_list|,
specifier|const
name|char
modifier|*
name|uuid
parameter_list|,
specifier|const
name|char
modifier|*
name|url
parameter_list|,
name|unsigned
name|long
name|timestamp
parameter_list|)
block|{
specifier|static
specifier|const
name|struct
name|strbuf
name|empty
init|=
name|STRBUF_INIT
decl_stmt|;
if|if
condition|(
operator|!
name|log
condition|)
name|log
operator|=
operator|&
name|empty
expr_stmt|;
if|if
condition|(
operator|*
name|uuid
operator|&&
operator|*
name|url
condition|)
block|{
name|snprintf
argument_list|(
name|gitsvnline
argument_list|,
name|MAX_GITSVN_LINE_LEN
argument_list|,
literal|"\n\ngit-svn-id: %s@%"
name|PRIu32
literal|" %s\n"
argument_list|,
name|url
argument_list|,
name|revision
argument_list|,
name|uuid
argument_list|)
expr_stmt|;
block|}
else|else
block|{
operator|*
name|gitsvnline
operator|=
literal|'\0'
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"commit refs/heads/master\n"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"mark :%"
name|PRIu32
literal|"\n"
argument_list|,
name|revision
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"committer %s<%s@%s> %ld +0000\n"
argument_list|,
operator|*
name|author
condition|?
name|author
else|:
literal|"nobody"
argument_list|,
operator|*
name|author
condition|?
name|author
else|:
literal|"nobody"
argument_list|,
operator|*
name|uuid
condition|?
name|uuid
else|:
literal|"local"
argument_list|,
name|timestamp
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"data %"
name|PRIuMAX
literal|"\n"
argument_list|,
call|(
name|uintmax_t
call|)
argument_list|(
name|log
operator|->
name|len
operator|+
name|strlen
argument_list|(
name|gitsvnline
argument_list|)
argument_list|)
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
name|log
operator|->
name|buf
argument_list|,
name|log
operator|->
name|len
argument_list|,
literal|1
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|gitsvnline
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|first_commit_done
condition|)
block|{
if|if
condition|(
name|revision
operator|>
literal|1
condition|)
name|printf
argument_list|(
literal|"from :%"
name|PRIu32
literal|"\n"
argument_list|,
name|revision
operator|-
literal|1
argument_list|)
expr_stmt|;
name|first_commit_done
operator|=
literal|1
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|fast_export_end_commit
name|void
name|fast_export_end_commit
parameter_list|(
name|uint32_t
name|revision
parameter_list|)
block|{
name|printf
argument_list|(
literal|"progress Imported commit %"
name|PRIu32
literal|".\n\n"
argument_list|,
name|revision
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|ls_from_rev
specifier|static
name|void
name|ls_from_rev
parameter_list|(
name|uint32_t
name|rev
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
comment|/* ls :5 path/to/old/file */
name|printf
argument_list|(
literal|"ls :%"
name|PRIu32
literal|" "
argument_list|,
name|rev
argument_list|)
expr_stmt|;
name|quote_c_style
argument_list|(
name|path
argument_list|,
name|NULL
argument_list|,
name|stdout
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|ls_from_active_commit
specifier|static
name|void
name|ls_from_active_commit
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
comment|/* ls "path/to/file" */
name|printf
argument_list|(
literal|"ls \""
argument_list|)
expr_stmt|;
name|quote_c_style
argument_list|(
name|path
argument_list|,
name|NULL
argument_list|,
name|stdout
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"\"\n"
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_response_line
specifier|static
specifier|const
name|char
modifier|*
name|get_response_line
parameter_list|(
name|void
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|line
init|=
name|buffer_read_line
argument_list|(
operator|&
name|report_buffer
argument_list|)
decl_stmt|;
if|if
condition|(
name|line
condition|)
return|return
name|line
return|;
if|if
condition|(
name|buffer_ferror
argument_list|(
operator|&
name|report_buffer
argument_list|)
condition|)
name|die_errno
argument_list|(
literal|"error reading from fast-import"
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"unexpected end of fast-import feedback"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|die_short_read
specifier|static
name|void
name|die_short_read
parameter_list|(
name|struct
name|line_buffer
modifier|*
name|input
parameter_list|)
block|{
if|if
condition|(
name|buffer_ferror
argument_list|(
name|input
argument_list|)
condition|)
name|die_errno
argument_list|(
literal|"error reading dump file"
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"invalid dump: unexpected end of file"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|ends_with
specifier|static
name|int
name|ends_with
parameter_list|(
specifier|const
name|char
modifier|*
name|s
parameter_list|,
name|size_t
name|len
parameter_list|,
specifier|const
name|char
modifier|*
name|suffix
parameter_list|)
block|{
specifier|const
name|size_t
name|suffixlen
init|=
name|strlen
argument_list|(
name|suffix
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|<
name|suffixlen
condition|)
return|return
literal|0
return|;
return|return
operator|!
name|memcmp
argument_list|(
name|s
operator|+
name|len
operator|-
name|suffixlen
argument_list|,
name|suffix
argument_list|,
name|suffixlen
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|parse_cat_response_line
specifier|static
name|int
name|parse_cat_response_line
parameter_list|(
specifier|const
name|char
modifier|*
name|header
parameter_list|,
name|off_t
modifier|*
name|len
parameter_list|)
block|{
name|size_t
name|headerlen
init|=
name|strlen
argument_list|(
name|header
argument_list|)
decl_stmt|;
name|uintmax_t
name|n
decl_stmt|;
specifier|const
name|char
modifier|*
name|type
decl_stmt|;
specifier|const
name|char
modifier|*
name|end
decl_stmt|;
if|if
condition|(
name|ends_with
argument_list|(
name|header
argument_list|,
name|headerlen
argument_list|,
literal|" missing"
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"cat-blob reports missing blob: %s"
argument_list|,
name|header
argument_list|)
return|;
name|type
operator|=
name|memmem
argument_list|(
name|header
argument_list|,
name|headerlen
argument_list|,
literal|" blob "
argument_list|,
name|strlen
argument_list|(
literal|" blob "
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|type
condition|)
return|return
name|error
argument_list|(
literal|"cat-blob header has wrong object type: %s"
argument_list|,
name|header
argument_list|)
return|;
name|n
operator|=
name|strtoumax
argument_list|(
name|type
operator|+
name|strlen
argument_list|(
literal|" blob "
argument_list|)
argument_list|,
operator|(
name|char
operator|*
operator|*
operator|)
operator|&
name|end
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
name|end
operator|==
name|type
operator|+
name|strlen
argument_list|(
literal|" blob "
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"cat-blob header does not contain length: %s"
argument_list|,
name|header
argument_list|)
return|;
if|if
condition|(
name|memchr
argument_list|(
name|type
operator|+
name|strlen
argument_list|(
literal|" blob "
argument_list|)
argument_list|,
literal|'-'
argument_list|,
name|end
operator|-
name|type
operator|-
name|strlen
argument_list|(
literal|" blob "
argument_list|)
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"cat-blob header contains negative length: %s"
argument_list|,
name|header
argument_list|)
return|;
if|if
condition|(
name|n
operator|==
name|UINTMAX_MAX
operator|||
name|n
operator|>
name|maximum_signed_value_of_type
argument_list|(
name|off_t
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"blob too large for current definition of off_t"
argument_list|)
return|;
operator|*
name|len
operator|=
name|n
expr_stmt|;
if|if
condition|(
operator|*
name|end
condition|)
return|return
name|error
argument_list|(
literal|"cat-blob header contains garbage after length: %s"
argument_list|,
name|header
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|check_preimage_overflow
specifier|static
name|void
name|check_preimage_overflow
parameter_list|(
name|off_t
name|a
parameter_list|,
name|off_t
name|b
parameter_list|)
block|{
if|if
condition|(
name|signed_add_overflows
argument_list|(
name|a
argument_list|,
name|b
argument_list|)
condition|)
name|die
argument_list|(
literal|"blob too large for current definition of off_t"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|apply_delta
specifier|static
name|long
name|apply_delta
parameter_list|(
name|off_t
name|len
parameter_list|,
name|struct
name|line_buffer
modifier|*
name|input
parameter_list|,
specifier|const
name|char
modifier|*
name|old_data
parameter_list|,
name|uint32_t
name|old_mode
parameter_list|)
block|{
name|long
name|ret
decl_stmt|;
name|struct
name|sliding_view
name|preimage
init|=
name|SLIDING_VIEW_INIT
argument_list|(
operator|&
name|report_buffer
argument_list|,
literal|0
argument_list|)
decl_stmt|;
name|FILE
modifier|*
name|out
decl_stmt|;
if|if
condition|(
name|init_postimage
argument_list|()
operator|||
operator|!
operator|(
name|out
operator|=
name|buffer_tmpfile_rewind
argument_list|(
operator|&
name|postimage
argument_list|)
operator|)
condition|)
name|die
argument_list|(
literal|"cannot open temporary file for blob retrieval"
argument_list|)
expr_stmt|;
if|if
condition|(
name|old_data
condition|)
block|{
specifier|const
name|char
modifier|*
name|response
decl_stmt|;
name|printf
argument_list|(
literal|"cat-blob %s\n"
argument_list|,
name|old_data
argument_list|)
expr_stmt|;
name|fflush
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
name|response
operator|=
name|get_response_line
argument_list|()
expr_stmt|;
if|if
condition|(
name|parse_cat_response_line
argument_list|(
name|response
argument_list|,
operator|&
name|preimage
operator|.
name|max_off
argument_list|)
condition|)
name|die
argument_list|(
literal|"invalid cat-blob response: %s"
argument_list|,
name|response
argument_list|)
expr_stmt|;
name|check_preimage_overflow
argument_list|(
name|preimage
operator|.
name|max_off
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|old_mode
operator|==
name|REPO_MODE_LNK
condition|)
block|{
name|strbuf_addstr
argument_list|(
operator|&
name|preimage
operator|.
name|buf
argument_list|,
literal|"link "
argument_list|)
expr_stmt|;
name|check_preimage_overflow
argument_list|(
name|preimage
operator|.
name|max_off
argument_list|,
name|strlen
argument_list|(
literal|"link "
argument_list|)
argument_list|)
expr_stmt|;
name|preimage
operator|.
name|max_off
operator|+=
name|strlen
argument_list|(
literal|"link "
argument_list|)
expr_stmt|;
name|check_preimage_overflow
argument_list|(
name|preimage
operator|.
name|max_off
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|svndiff0_apply
argument_list|(
name|input
argument_list|,
name|len
argument_list|,
operator|&
name|preimage
argument_list|,
name|out
argument_list|)
condition|)
name|die
argument_list|(
literal|"cannot apply delta"
argument_list|)
expr_stmt|;
if|if
condition|(
name|old_data
condition|)
block|{
comment|/* Read the remainder of preimage and trailing newline. */
name|assert
argument_list|(
operator|!
name|signed_add_overflows
argument_list|(
name|preimage
operator|.
name|max_off
argument_list|,
literal|1
argument_list|)
argument_list|)
expr_stmt|;
name|preimage
operator|.
name|max_off
operator|++
expr_stmt|;
comment|/* room for newline */
if|if
condition|(
name|move_window
argument_list|(
operator|&
name|preimage
argument_list|,
name|preimage
operator|.
name|max_off
operator|-
literal|1
argument_list|,
literal|1
argument_list|)
condition|)
name|die
argument_list|(
literal|"cannot seek to end of input"
argument_list|)
expr_stmt|;
if|if
condition|(
name|preimage
operator|.
name|buf
operator|.
name|buf
index|[
literal|0
index|]
operator|!=
literal|'\n'
condition|)
name|die
argument_list|(
literal|"missing newline after cat-blob response"
argument_list|)
expr_stmt|;
block|}
name|ret
operator|=
name|buffer_tmpfile_prepare_to_read
argument_list|(
operator|&
name|postimage
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"cannot read temporary file for blob retrieval"
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|preimage
operator|.
name|buf
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|fast_export_data
name|void
name|fast_export_data
parameter_list|(
name|uint32_t
name|mode
parameter_list|,
name|uint32_t
name|len
parameter_list|,
name|struct
name|line_buffer
modifier|*
name|input
parameter_list|)
block|{
if|if
condition|(
name|mode
operator|==
name|REPO_MODE_LNK
condition|)
block|{
comment|/* svn symlink blobs start with "link " */
name|len
operator|-=
literal|5
expr_stmt|;
if|if
condition|(
name|buffer_skip_bytes
argument_list|(
name|input
argument_list|,
literal|5
argument_list|)
operator|!=
literal|5
condition|)
name|die_short_read
argument_list|(
name|input
argument_list|)
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"data %"
name|PRIu32
literal|"\n"
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|buffer_copy_bytes
argument_list|(
name|input
argument_list|,
name|len
argument_list|)
operator|!=
name|len
condition|)
name|die_short_read
argument_list|(
name|input
argument_list|)
expr_stmt|;
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|parse_ls_response
specifier|static
name|int
name|parse_ls_response
parameter_list|(
specifier|const
name|char
modifier|*
name|response
parameter_list|,
name|uint32_t
modifier|*
name|mode
parameter_list|,
name|struct
name|strbuf
modifier|*
name|dataref
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|tab
decl_stmt|;
specifier|const
name|char
modifier|*
name|response_end
decl_stmt|;
name|assert
argument_list|(
name|response
argument_list|)
expr_stmt|;
name|response_end
operator|=
name|response
operator|+
name|strlen
argument_list|(
name|response
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|response
operator|==
literal|'m'
condition|)
block|{
comment|/* Missing. */
name|errno
operator|=
name|ENOENT
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
comment|/* Mode. */
if|if
condition|(
name|response_end
operator|-
name|response
operator|<
name|strlen
argument_list|(
literal|"100644"
argument_list|)
operator|||
name|response
index|[
name|strlen
argument_list|(
literal|"100644"
argument_list|)
index|]
operator|!=
literal|' '
condition|)
name|die
argument_list|(
literal|"invalid ls response: missing mode: %s"
argument_list|,
name|response
argument_list|)
expr_stmt|;
operator|*
name|mode
operator|=
literal|0
expr_stmt|;
for|for
control|(
init|;
operator|*
name|response
operator|!=
literal|' '
condition|;
name|response
operator|++
control|)
block|{
name|char
name|ch
init|=
operator|*
name|response
decl_stmt|;
if|if
condition|(
name|ch
operator|<
literal|'0'
operator|||
name|ch
operator|>
literal|'7'
condition|)
name|die
argument_list|(
literal|"invalid ls response: mode is not octal: %s"
argument_list|,
name|response
argument_list|)
expr_stmt|;
operator|*
name|mode
operator|*=
literal|8
expr_stmt|;
operator|*
name|mode
operator|+=
name|ch
operator|-
literal|'0'
expr_stmt|;
block|}
comment|/* ' blob ' or ' tree ' */
if|if
condition|(
name|response_end
operator|-
name|response
operator|<
name|strlen
argument_list|(
literal|" blob "
argument_list|)
operator|||
operator|(
name|response
index|[
literal|1
index|]
operator|!=
literal|'b'
operator|&&
name|response
index|[
literal|1
index|]
operator|!=
literal|'t'
operator|)
condition|)
name|die
argument_list|(
literal|"unexpected ls response: not a tree or blob: %s"
argument_list|,
name|response
argument_list|)
expr_stmt|;
name|response
operator|+=
name|strlen
argument_list|(
literal|" blob "
argument_list|)
expr_stmt|;
comment|/* Dataref. */
name|tab
operator|=
name|memchr
argument_list|(
name|response
argument_list|,
literal|'\t'
argument_list|,
name|response_end
operator|-
name|response
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tab
condition|)
name|die
argument_list|(
literal|"invalid ls response: missing tab: %s"
argument_list|,
name|response
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
name|dataref
argument_list|,
name|response
argument_list|,
name|tab
operator|-
name|response
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|fast_export_ls_rev
name|int
name|fast_export_ls_rev
parameter_list|(
name|uint32_t
name|rev
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|uint32_t
modifier|*
name|mode
parameter_list|,
name|struct
name|strbuf
modifier|*
name|dataref
parameter_list|)
block|{
name|ls_from_rev
argument_list|(
name|rev
argument_list|,
name|path
argument_list|)
expr_stmt|;
return|return
name|parse_ls_response
argument_list|(
name|get_response_line
argument_list|()
argument_list|,
name|mode
argument_list|,
name|dataref
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|fast_export_ls
name|int
name|fast_export_ls
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|uint32_t
modifier|*
name|mode
parameter_list|,
name|struct
name|strbuf
modifier|*
name|dataref
parameter_list|)
block|{
name|ls_from_active_commit
argument_list|(
name|path
argument_list|)
expr_stmt|;
return|return
name|parse_ls_response
argument_list|(
name|get_response_line
argument_list|()
argument_list|,
name|mode
argument_list|,
name|dataref
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|fast_export_blob_delta
name|void
name|fast_export_blob_delta
parameter_list|(
name|uint32_t
name|mode
parameter_list|,
name|uint32_t
name|old_mode
parameter_list|,
specifier|const
name|char
modifier|*
name|old_data
parameter_list|,
name|uint32_t
name|len
parameter_list|,
name|struct
name|line_buffer
modifier|*
name|input
parameter_list|)
block|{
name|long
name|postimage_len
decl_stmt|;
if|if
condition|(
name|len
operator|>
name|maximum_signed_value_of_type
argument_list|(
name|off_t
argument_list|)
condition|)
name|die
argument_list|(
literal|"enormous delta"
argument_list|)
expr_stmt|;
name|postimage_len
operator|=
name|apply_delta
argument_list|(
operator|(
name|off_t
operator|)
name|len
argument_list|,
name|input
argument_list|,
name|old_data
argument_list|,
name|old_mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|mode
operator|==
name|REPO_MODE_LNK
condition|)
block|{
name|buffer_skip_bytes
argument_list|(
operator|&
name|postimage
argument_list|,
name|strlen
argument_list|(
literal|"link "
argument_list|)
argument_list|)
expr_stmt|;
name|postimage_len
operator|-=
name|strlen
argument_list|(
literal|"link "
argument_list|)
expr_stmt|;
block|}
name|printf
argument_list|(
literal|"data %ld\n"
argument_list|,
name|postimage_len
argument_list|)
expr_stmt|;
name|buffer_copy_bytes
argument_list|(
operator|&
name|postimage
argument_list|,
name|postimage_len
argument_list|)
expr_stmt|;
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
