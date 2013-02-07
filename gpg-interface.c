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
file|"strbuf.h"
end_include
begin_include
include|#
directive|include
file|"gpg-interface.h"
end_include
begin_include
include|#
directive|include
file|"sigchain.h"
end_include
begin_decl_stmt
DECL|variable|configured_signing_key
specifier|static
name|char
modifier|*
name|configured_signing_key
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|gpg_program
specifier|static
specifier|const
name|char
modifier|*
name|gpg_program
init|=
literal|"gpg"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|set_signing_key
name|void
name|set_signing_key
parameter_list|(
specifier|const
name|char
modifier|*
name|key
parameter_list|)
block|{
name|free
argument_list|(
name|configured_signing_key
argument_list|)
expr_stmt|;
name|configured_signing_key
operator|=
name|xstrdup
argument_list|(
name|key
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|git_gpg_config
name|int
name|git_gpg_config
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
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"user.signingkey"
argument_list|)
condition|)
block|{
name|set_signing_key
argument_list|(
name|value
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"gpg.program"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|value
condition|)
return|return
name|config_error_nonbool
argument_list|(
name|var
argument_list|)
return|;
name|gpg_program
operator|=
name|xstrdup
argument_list|(
name|value
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_signing_key
specifier|const
name|char
modifier|*
name|get_signing_key
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|configured_signing_key
condition|)
return|return
name|configured_signing_key
return|;
return|return
name|git_committer_info
argument_list|(
name|IDENT_STRICT
operator||
name|IDENT_NO_DATE
argument_list|)
return|;
block|}
end_function
begin_comment
comment|/*  * Create a detached signature for the contents of "buffer" and append  * it after "signature"; "buffer" and "signature" can be the same  * strbuf instance, which would cause the detached signature appended  * at the end.  */
end_comment
begin_function
DECL|function|sign_buffer
name|int
name|sign_buffer
parameter_list|(
name|struct
name|strbuf
modifier|*
name|buffer
parameter_list|,
name|struct
name|strbuf
modifier|*
name|signature
parameter_list|,
specifier|const
name|char
modifier|*
name|signing_key
parameter_list|)
block|{
name|struct
name|child_process
name|gpg
decl_stmt|;
specifier|const
name|char
modifier|*
name|args
index|[
literal|4
index|]
decl_stmt|;
name|ssize_t
name|len
decl_stmt|;
name|size_t
name|i
decl_stmt|,
name|j
decl_stmt|,
name|bottom
decl_stmt|;
name|memset
argument_list|(
operator|&
name|gpg
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|gpg
argument_list|)
argument_list|)
expr_stmt|;
name|gpg
operator|.
name|argv
operator|=
name|args
expr_stmt|;
name|gpg
operator|.
name|in
operator|=
operator|-
literal|1
expr_stmt|;
name|gpg
operator|.
name|out
operator|=
operator|-
literal|1
expr_stmt|;
name|args
index|[
literal|0
index|]
operator|=
name|gpg_program
expr_stmt|;
name|args
index|[
literal|1
index|]
operator|=
literal|"-bsau"
expr_stmt|;
name|args
index|[
literal|2
index|]
operator|=
name|signing_key
expr_stmt|;
name|args
index|[
literal|3
index|]
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|gpg
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"could not run gpg."
argument_list|)
argument_list|)
return|;
comment|/* 	 * When the username signingkey is bad, program could be terminated 	 * because gpg exits without reading and then write gets SIGPIPE. 	 */
name|sigchain_push
argument_list|(
name|SIGPIPE
argument_list|,
name|SIG_IGN
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_in_full
argument_list|(
name|gpg
operator|.
name|in
argument_list|,
name|buffer
operator|->
name|buf
argument_list|,
name|buffer
operator|->
name|len
argument_list|)
operator|!=
name|buffer
operator|->
name|len
condition|)
block|{
name|close
argument_list|(
name|gpg
operator|.
name|in
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|gpg
operator|.
name|out
argument_list|)
expr_stmt|;
name|finish_command
argument_list|(
operator|&
name|gpg
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"gpg did not accept the data"
argument_list|)
argument_list|)
return|;
block|}
name|close
argument_list|(
name|gpg
operator|.
name|in
argument_list|)
expr_stmt|;
name|bottom
operator|=
name|signature
operator|->
name|len
expr_stmt|;
name|len
operator|=
name|strbuf_read
argument_list|(
name|signature
argument_list|,
name|gpg
operator|.
name|out
argument_list|,
literal|1024
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|gpg
operator|.
name|out
argument_list|)
expr_stmt|;
name|sigchain_pop
argument_list|(
name|SIGPIPE
argument_list|)
expr_stmt|;
if|if
condition|(
name|finish_command
argument_list|(
operator|&
name|gpg
argument_list|)
operator|||
operator|!
name|len
operator|||
name|len
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"gpg failed to sign the data"
argument_list|)
argument_list|)
return|;
comment|/* Strip CR from the line endings, in case we are on Windows. */
for|for
control|(
name|i
operator|=
name|j
operator|=
name|bottom
init|;
name|i
operator|<
name|signature
operator|->
name|len
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|signature
operator|->
name|buf
index|[
name|i
index|]
operator|!=
literal|'\r'
condition|)
block|{
if|if
condition|(
name|i
operator|!=
name|j
condition|)
name|signature
operator|->
name|buf
index|[
name|j
index|]
operator|=
name|signature
operator|->
name|buf
index|[
name|i
index|]
expr_stmt|;
name|j
operator|++
expr_stmt|;
block|}
name|strbuf_setlen
argument_list|(
name|signature
argument_list|,
name|j
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * Run "gpg" to see if the payload matches the detached signature.  * gpg_output, when set, receives the diagnostic output from GPG.  */
end_comment
begin_function
DECL|function|verify_signed_buffer
name|int
name|verify_signed_buffer
parameter_list|(
specifier|const
name|char
modifier|*
name|payload
parameter_list|,
name|size_t
name|payload_size
parameter_list|,
specifier|const
name|char
modifier|*
name|signature
parameter_list|,
name|size_t
name|signature_size
parameter_list|,
name|struct
name|strbuf
modifier|*
name|gpg_output
parameter_list|)
block|{
name|struct
name|child_process
name|gpg
decl_stmt|;
specifier|const
name|char
modifier|*
name|args_gpg
index|[]
init|=
block|{
name|NULL
block|,
literal|"--verify"
block|,
literal|"FILE"
block|,
literal|"-"
block|,
name|NULL
block|}
decl_stmt|;
name|char
name|path
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|int
name|fd
decl_stmt|,
name|ret
decl_stmt|;
name|args_gpg
index|[
literal|0
index|]
operator|=
name|gpg_program
expr_stmt|;
name|fd
operator|=
name|git_mkstemp
argument_list|(
name|path
argument_list|,
name|PATH_MAX
argument_list|,
literal|".git_vtag_tmpXXXXXX"
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"could not create temporary file '%s': %s"
argument_list|)
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
if|if
condition|(
name|write_in_full
argument_list|(
name|fd
argument_list|,
name|signature
argument_list|,
name|signature_size
argument_list|)
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"failed writing detached signature to '%s': %s"
argument_list|)
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|gpg
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|gpg
argument_list|)
argument_list|)
expr_stmt|;
name|gpg
operator|.
name|argv
operator|=
name|args_gpg
expr_stmt|;
name|gpg
operator|.
name|in
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|gpg_output
condition|)
name|gpg
operator|.
name|err
operator|=
operator|-
literal|1
expr_stmt|;
name|args_gpg
index|[
literal|2
index|]
operator|=
name|path
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|gpg
argument_list|)
condition|)
block|{
name|unlink
argument_list|(
name|path
argument_list|)
expr_stmt|;
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"could not run gpg."
argument_list|)
argument_list|)
return|;
block|}
name|write_in_full
argument_list|(
name|gpg
operator|.
name|in
argument_list|,
name|payload
argument_list|,
name|payload_size
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|gpg
operator|.
name|in
argument_list|)
expr_stmt|;
if|if
condition|(
name|gpg_output
condition|)
block|{
name|strbuf_read
argument_list|(
name|gpg_output
argument_list|,
name|gpg
operator|.
name|err
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|gpg
operator|.
name|err
argument_list|)
expr_stmt|;
block|}
name|ret
operator|=
name|finish_command
argument_list|(
operator|&
name|gpg
argument_list|)
expr_stmt|;
name|unlink_or_warn
argument_list|(
name|path
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
end_unit
