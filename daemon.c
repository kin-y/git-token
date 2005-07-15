begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"pkt-line.h"
end_include
begin_include
include|#
directive|include
file|<sys/socket.h>
end_include
begin_include
include|#
directive|include
file|<netinet/in.h>
end_include
begin_decl_stmt
DECL|variable|daemon_usage
specifier|static
specifier|const
name|char
name|daemon_usage
index|[]
init|=
literal|"git-daemon [--port=n]"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|upload
specifier|static
name|int
name|upload
parameter_list|(
name|char
modifier|*
name|dir
parameter_list|,
name|int
name|dirlen
parameter_list|)
block|{
if|if
condition|(
name|chdir
argument_list|(
name|dir
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
name|chdir
argument_list|(
literal|".git"
argument_list|)
expr_stmt|;
comment|/* 	 * Security on the cheap. 	 * 	 * We want a readable HEAD, usable "objects" directory, and  	 * a "git-daemon-export-ok" flag that says that the other side 	 * is ok with us doing this. 	 */
if|if
condition|(
name|access
argument_list|(
literal|"git-daemon-export-ok"
argument_list|,
name|F_OK
argument_list|)
operator|||
name|access
argument_list|(
literal|"objects/00"
argument_list|,
name|X_OK
argument_list|)
operator|||
name|access
argument_list|(
literal|"HEAD"
argument_list|,
name|R_OK
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
comment|/* git-upload-pack only ever reads stuff, so this is safe */
name|execlp
argument_list|(
literal|"git-upload-pack"
argument_list|,
literal|"git-upload-pack"
argument_list|,
literal|"."
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|execute
specifier|static
name|int
name|execute
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
name|char
name|line
index|[
literal|1000
index|]
decl_stmt|;
name|int
name|len
decl_stmt|;
name|len
operator|=
name|packet_read_line
argument_list|(
literal|0
argument_list|,
name|line
argument_list|,
sizeof|sizeof
argument_list|(
name|line
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|len
operator|&&
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|line
index|[
operator|--
name|len
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
literal|"git-upload-pack /"
argument_list|,
name|line
argument_list|,
literal|17
argument_list|)
condition|)
return|return
name|upload
argument_list|(
name|line
operator|+
literal|16
argument_list|,
name|len
operator|-
literal|16
argument_list|)
return|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"got bad connection '%s'\n"
argument_list|,
name|line
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|handle
specifier|static
name|void
name|handle
parameter_list|(
name|int
name|incoming
parameter_list|,
name|struct
name|sockaddr_in
modifier|*
name|addr
parameter_list|,
name|int
name|addrlen
parameter_list|)
block|{
if|if
condition|(
name|fork
argument_list|()
condition|)
block|{
name|close
argument_list|(
name|incoming
argument_list|)
expr_stmt|;
return|return;
block|}
name|dup2
argument_list|(
name|incoming
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|incoming
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|incoming
argument_list|)
expr_stmt|;
name|exit
argument_list|(
name|execute
argument_list|()
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|serve
specifier|static
name|int
name|serve
parameter_list|(
name|int
name|port
parameter_list|)
block|{
name|int
name|sockfd
decl_stmt|;
name|struct
name|sockaddr_in
name|addr
decl_stmt|;
name|sockfd
operator|=
name|socket
argument_list|(
name|PF_INET
argument_list|,
name|SOCK_STREAM
argument_list|,
name|IPPROTO_IP
argument_list|)
expr_stmt|;
if|if
condition|(
name|sockfd
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to open socket (%s)"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|memset
argument_list|(
operator|&
name|addr
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|addr
argument_list|)
argument_list|)
expr_stmt|;
name|addr
operator|.
name|sin_port
operator|=
name|htons
argument_list|(
name|port
argument_list|)
expr_stmt|;
name|addr
operator|.
name|sin_family
operator|=
name|AF_INET
expr_stmt|;
if|if
condition|(
name|bind
argument_list|(
name|sockfd
argument_list|,
operator|(
name|void
operator|*
operator|)
operator|&
name|addr
argument_list|,
sizeof|sizeof
argument_list|(
name|addr
argument_list|)
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to bind to port %d (%s)"
argument_list|,
name|port
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|listen
argument_list|(
name|sockfd
argument_list|,
literal|5
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to listen to port %d (%s)"
argument_list|,
name|port
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|struct
name|sockaddr_in
name|in
decl_stmt|;
name|socklen_t
name|addrlen
init|=
sizeof|sizeof
argument_list|(
name|in
argument_list|)
decl_stmt|;
name|int
name|incoming
init|=
name|accept
argument_list|(
name|sockfd
argument_list|,
operator|(
name|void
operator|*
operator|)
operator|&
name|in
argument_list|,
operator|&
name|addrlen
argument_list|)
decl_stmt|;
if|if
condition|(
name|incoming
operator|<
literal|0
condition|)
block|{
switch|switch
condition|(
name|errno
condition|)
block|{
case|case
name|EAGAIN
case|:
case|case
name|EINTR
case|:
case|case
name|ECONNABORTED
case|:
continue|continue;
default|default:
name|die
argument_list|(
literal|"accept returned %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
name|handle
argument_list|(
name|incoming
argument_list|,
operator|&
name|in
argument_list|,
name|addrlen
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|port
init|=
name|DEFAULT_GIT_PORT
decl_stmt|;
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
literal|"--port="
argument_list|,
literal|7
argument_list|)
condition|)
block|{
name|char
modifier|*
name|end
decl_stmt|;
name|unsigned
name|long
name|n
decl_stmt|;
name|n
operator|=
name|strtoul
argument_list|(
name|arg
operator|+
literal|7
argument_list|,
operator|&
name|end
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|arg
index|[
literal|7
index|]
operator|&&
operator|!
operator|*
name|end
condition|)
block|{
name|port
operator|=
name|n
expr_stmt|;
continue|continue;
block|}
block|}
name|usage
argument_list|(
name|daemon_usage
argument_list|)
expr_stmt|;
block|}
return|return
name|serve
argument_list|(
name|port
argument_list|)
return|;
block|}
end_function
end_unit
