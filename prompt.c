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
file|"prompt.h"
end_include
begin_function
DECL|function|git_getpass
name|char
modifier|*
name|git_getpass
parameter_list|(
specifier|const
name|char
modifier|*
name|prompt
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|askpass
decl_stmt|;
name|struct
name|child_process
name|pass
decl_stmt|;
specifier|const
name|char
modifier|*
name|args
index|[
literal|3
index|]
decl_stmt|;
specifier|static
name|struct
name|strbuf
name|buffer
init|=
name|STRBUF_INIT
decl_stmt|;
name|askpass
operator|=
name|getenv
argument_list|(
literal|"GIT_ASKPASS"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|askpass
condition|)
name|askpass
operator|=
name|askpass_program
expr_stmt|;
if|if
condition|(
operator|!
name|askpass
condition|)
name|askpass
operator|=
name|getenv
argument_list|(
literal|"SSH_ASKPASS"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|askpass
operator|||
operator|!
operator|(
operator|*
name|askpass
operator|)
condition|)
block|{
name|char
modifier|*
name|result
init|=
name|getpass
argument_list|(
name|prompt
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|result
condition|)
name|die_errno
argument_list|(
literal|"Could not read password"
argument_list|)
expr_stmt|;
return|return
name|result
return|;
block|}
name|args
index|[
literal|0
index|]
operator|=
name|askpass
expr_stmt|;
name|args
index|[
literal|1
index|]
operator|=
name|prompt
expr_stmt|;
name|args
index|[
literal|2
index|]
operator|=
name|NULL
expr_stmt|;
name|memset
argument_list|(
operator|&
name|pass
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|pass
argument_list|)
argument_list|)
expr_stmt|;
name|pass
operator|.
name|argv
operator|=
name|args
expr_stmt|;
name|pass
operator|.
name|out
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|pass
argument_list|)
condition|)
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|strbuf_reset
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
if|if
condition|(
name|strbuf_read
argument_list|(
operator|&
name|buffer
argument_list|,
name|pass
operator|.
name|out
argument_list|,
literal|20
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"failed to read password from %s\n"
argument_list|,
name|askpass
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|pass
operator|.
name|out
argument_list|)
expr_stmt|;
if|if
condition|(
name|finish_command
argument_list|(
operator|&
name|pass
argument_list|)
condition|)
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|strbuf_setlen
argument_list|(
operator|&
name|buffer
argument_list|,
name|strcspn
argument_list|(
name|buffer
operator|.
name|buf
argument_list|,
literal|"\r\n"
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|buffer
operator|.
name|buf
return|;
block|}
end_function
end_unit
