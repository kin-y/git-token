begin_unit
begin_include
include|#
directive|include
file|"rsh.h"
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<sys/types.h>
end_include
begin_include
include|#
directive|include
file|<sys/socket.h>
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_define
DECL|macro|COMMAND_SIZE
define|#
directive|define
name|COMMAND_SIZE
value|4096
end_define
begin_comment
comment|/*  * Write a shell-quoted version of a string into a buffer, and  * return bytes that ought to be output excluding final null.  */
end_comment
begin_function
DECL|function|shell_quote
specifier|static
name|int
name|shell_quote
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|int
name|nmax
parameter_list|,
specifier|const
name|char
modifier|*
name|str
parameter_list|)
block|{
name|char
name|ch
decl_stmt|;
name|int
name|nq
decl_stmt|;
name|int
name|oc
init|=
literal|0
decl_stmt|;
while|while
condition|(
operator|(
name|ch
operator|=
operator|*
name|str
operator|++
operator|)
condition|)
block|{
name|nq
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|strchr
argument_list|(
literal|" !\"#$%&\'()*;<=>?[\\]^`{|}"
argument_list|,
name|ch
argument_list|)
condition|)
name|nq
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|nq
condition|)
block|{
if|if
condition|(
name|nmax
operator|>
literal|1
condition|)
block|{
operator|*
name|buf
operator|++
operator|=
literal|'\\'
expr_stmt|;
name|nmax
operator|--
expr_stmt|;
block|}
name|oc
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|nmax
operator|>
literal|1
condition|)
block|{
operator|*
name|buf
operator|++
operator|=
name|ch
expr_stmt|;
name|nmax
operator|--
expr_stmt|;
block|}
name|oc
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|nmax
condition|)
operator|*
name|buf
operator|=
literal|'\0'
expr_stmt|;
return|return
name|oc
return|;
block|}
end_function
begin_comment
comment|/*  * Append a string to a string buffer, with or without quoting.  Return true  * if the buffer overflowed.  */
end_comment
begin_function
DECL|function|add_to_string
specifier|static
name|int
name|add_to_string
parameter_list|(
name|char
modifier|*
modifier|*
name|ptrp
parameter_list|,
name|int
modifier|*
name|sizep
parameter_list|,
specifier|const
name|char
modifier|*
name|str
parameter_list|,
name|int
name|quote
parameter_list|)
block|{
name|char
modifier|*
name|p
init|=
operator|*
name|ptrp
decl_stmt|;
name|int
name|size
init|=
operator|*
name|sizep
decl_stmt|;
name|int
name|oc
decl_stmt|;
name|int
name|err
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|quote
condition|)
block|{
name|oc
operator|=
name|shell_quote
argument_list|(
name|p
argument_list|,
name|size
argument_list|,
name|str
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|oc
operator|=
name|strlen
argument_list|(
name|str
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|p
argument_list|,
name|str
argument_list|,
operator|(
name|oc
operator|>=
name|size
operator|)
condition|?
name|size
operator|-
literal|1
else|:
name|oc
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|oc
operator|>=
name|size
condition|)
block|{
name|err
operator|=
literal|1
expr_stmt|;
name|oc
operator|=
name|size
operator|-
literal|1
expr_stmt|;
block|}
operator|*
name|ptrp
operator|+=
name|oc
expr_stmt|;
operator|*
operator|*
name|ptrp
operator|=
literal|'\0'
expr_stmt|;
operator|*
name|sizep
operator|-=
name|oc
expr_stmt|;
return|return
name|err
return|;
block|}
end_function
begin_function
DECL|function|setup_connection
name|int
name|setup_connection
parameter_list|(
name|int
modifier|*
name|fd_in
parameter_list|,
name|int
modifier|*
name|fd_out
parameter_list|,
specifier|const
name|char
modifier|*
name|remote_prog
parameter_list|,
name|char
modifier|*
name|url
parameter_list|,
name|int
name|rmt_argc
parameter_list|,
name|char
modifier|*
modifier|*
name|rmt_argv
parameter_list|)
block|{
name|char
modifier|*
name|host
decl_stmt|;
name|char
modifier|*
name|path
decl_stmt|;
name|int
name|sv
index|[
literal|2
index|]
decl_stmt|;
name|char
name|command
index|[
name|COMMAND_SIZE
index|]
decl_stmt|;
name|char
modifier|*
name|posn
decl_stmt|;
name|int
name|sizen
decl_stmt|;
name|int
name|of
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|url
argument_list|,
literal|"-"
argument_list|)
condition|)
block|{
operator|*
name|fd_in
operator|=
literal|0
expr_stmt|;
operator|*
name|fd_out
operator|=
literal|1
expr_stmt|;
return|return
literal|0
return|;
block|}
name|host
operator|=
name|strstr
argument_list|(
name|url
argument_list|,
literal|"//"
argument_list|)
expr_stmt|;
if|if
condition|(
name|host
condition|)
block|{
name|host
operator|+=
literal|2
expr_stmt|;
name|path
operator|=
name|strchr
argument_list|(
name|host
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|host
operator|=
name|url
expr_stmt|;
name|path
operator|=
name|strchr
argument_list|(
name|host
argument_list|,
literal|':'
argument_list|)
expr_stmt|;
if|if
condition|(
name|path
condition|)
operator|*
operator|(
name|path
operator|++
operator|)
operator|=
literal|'\0'
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|path
condition|)
block|{
return|return
name|error
argument_list|(
literal|"Bad URL: %s"
argument_list|,
name|url
argument_list|)
return|;
block|}
comment|/* $GIT_RSH<host> "env GIT_DIR=<path><remote_prog><args...>" */
name|sizen
operator|=
name|COMMAND_SIZE
expr_stmt|;
name|posn
operator|=
name|command
expr_stmt|;
name|of
operator|=
literal|0
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
literal|"env "
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
name|GIT_DIR_ENVIRONMENT
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
literal|"="
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
name|path
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
literal|" "
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
name|remote_prog
argument_list|,
literal|1
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|rmt_argc
condition|;
name|i
operator|++
control|)
block|{
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
literal|" "
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
name|rmt_argv
index|[
name|i
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
name|of
operator||=
name|add_to_string
argument_list|(
operator|&
name|posn
argument_list|,
operator|&
name|sizen
argument_list|,
literal|" -"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|of
condition|)
return|return
name|error
argument_list|(
literal|"Command line too long"
argument_list|)
return|;
if|if
condition|(
name|socketpair
argument_list|(
name|AF_UNIX
argument_list|,
name|SOCK_STREAM
argument_list|,
literal|0
argument_list|,
name|sv
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Couldn't create socket"
argument_list|)
return|;
if|if
condition|(
operator|!
name|fork
argument_list|()
condition|)
block|{
specifier|const
name|char
modifier|*
name|ssh
decl_stmt|,
modifier|*
name|ssh_basename
decl_stmt|;
name|ssh
operator|=
name|getenv
argument_list|(
literal|"GIT_SSH"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ssh
condition|)
name|ssh
operator|=
literal|"ssh"
expr_stmt|;
name|ssh_basename
operator|=
name|strrchr
argument_list|(
name|ssh
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ssh_basename
condition|)
name|ssh_basename
operator|=
name|ssh
expr_stmt|;
else|else
name|ssh_basename
operator|++
expr_stmt|;
name|close
argument_list|(
name|sv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|sv
index|[
literal|0
index|]
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|sv
index|[
literal|0
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|execlp
argument_list|(
name|ssh
argument_list|,
name|ssh_basename
argument_list|,
name|host
argument_list|,
name|command
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|sv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
operator|*
name|fd_in
operator|=
name|sv
index|[
literal|1
index|]
expr_stmt|;
operator|*
name|fd_out
operator|=
name|sv
index|[
literal|1
index|]
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
