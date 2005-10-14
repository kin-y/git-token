begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"pkt-line.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_decl_stmt
DECL|variable|upload_pack_usage
specifier|static
specifier|const
name|char
name|upload_pack_usage
index|[]
init|=
literal|"git-upload-pack<dir>"
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|MAX_HAS
define|#
directive|define
name|MAX_HAS
value|(16)
end_define
begin_define
DECL|macro|MAX_NEEDS
define|#
directive|define
name|MAX_NEEDS
value|(256)
end_define
begin_decl_stmt
DECL|variable|nr_has
DECL|variable|nr_needs
specifier|static
name|int
name|nr_has
init|=
literal|0
decl_stmt|,
name|nr_needs
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|has_sha1
specifier|static
name|unsigned
name|char
name|has_sha1
index|[
name|MAX_HAS
index|]
index|[
literal|20
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|needs_sha1
specifier|static
name|unsigned
name|char
name|needs_sha1
index|[
name|MAX_NEEDS
index|]
index|[
literal|20
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|strip
specifier|static
name|int
name|strip
parameter_list|(
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|)
block|{
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
return|return
name|len
return|;
block|}
end_function
begin_function
DECL|function|create_pack_file
specifier|static
name|void
name|create_pack_file
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|fd
index|[
literal|2
index|]
decl_stmt|;
name|pid_t
name|pid
decl_stmt|;
if|if
condition|(
name|pipe
argument_list|(
name|fd
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"git-upload-pack: unable to create pipe"
argument_list|)
expr_stmt|;
name|pid
operator|=
name|fork
argument_list|()
expr_stmt|;
if|if
condition|(
name|pid
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"git-upload-pack: unable to fork git-rev-list"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pid
condition|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|args
decl_stmt|;
name|char
modifier|*
modifier|*
name|argv
decl_stmt|;
name|char
modifier|*
name|buf
decl_stmt|;
name|char
modifier|*
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|MAX_NEEDS
operator|<=
name|nr_needs
condition|)
name|args
operator|=
name|nr_has
operator|+
literal|10
expr_stmt|;
else|else
name|args
operator|=
name|nr_has
operator|+
name|nr_needs
operator|+
literal|5
expr_stmt|;
name|argv
operator|=
name|xmalloc
argument_list|(
name|args
operator|*
sizeof|sizeof
argument_list|(
name|char
operator|*
argument_list|)
argument_list|)
expr_stmt|;
name|buf
operator|=
name|xmalloc
argument_list|(
name|args
operator|*
literal|45
argument_list|)
expr_stmt|;
name|p
operator|=
name|argv
expr_stmt|;
name|dup2
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|close
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
operator|*
name|p
operator|++
operator|=
literal|"git-rev-list"
expr_stmt|;
operator|*
name|p
operator|++
operator|=
literal|"--objects"
expr_stmt|;
if|if
condition|(
name|MAX_NEEDS
operator|<=
name|nr_needs
condition|)
operator|*
name|p
operator|++
operator|=
literal|"--all"
expr_stmt|;
else|else
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_needs
condition|;
name|i
operator|++
control|)
block|{
operator|*
name|p
operator|++
operator|=
name|buf
expr_stmt|;
name|memcpy
argument_list|(
name|buf
argument_list|,
name|sha1_to_hex
argument_list|(
name|needs_sha1
index|[
name|i
index|]
argument_list|)
argument_list|,
literal|41
argument_list|)
expr_stmt|;
name|buf
operator|+=
literal|41
expr_stmt|;
block|}
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|nr_has
condition|;
name|i
operator|++
control|)
block|{
operator|*
name|p
operator|++
operator|=
name|buf
expr_stmt|;
operator|*
name|buf
operator|++
operator|=
literal|'^'
expr_stmt|;
name|memcpy
argument_list|(
name|buf
argument_list|,
name|sha1_to_hex
argument_list|(
name|has_sha1
index|[
name|i
index|]
argument_list|)
argument_list|,
literal|41
argument_list|)
expr_stmt|;
name|buf
operator|+=
literal|41
expr_stmt|;
block|}
operator|*
name|p
operator|++
operator|=
name|NULL
expr_stmt|;
name|execvp
argument_list|(
literal|"git-rev-list"
argument_list|,
name|argv
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"git-upload-pack: unable to exec git-rev-list"
argument_list|)
expr_stmt|;
block|}
name|dup2
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|execlp
argument_list|(
literal|"git-pack-objects"
argument_list|,
literal|"git-pack-objects"
argument_list|,
literal|"--stdout"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"git-upload-pack: unable to exec git-pack-objects"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|got_sha1
specifier|static
name|int
name|got_sha1
parameter_list|(
name|char
modifier|*
name|hex
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|nr
decl_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|hex
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"git-upload-pack: expected SHA1 object, got '%s'"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|has_sha1_file
argument_list|(
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
name|nr
operator|=
name|nr_has
expr_stmt|;
if|if
condition|(
name|nr
operator|<
name|MAX_HAS
condition|)
block|{
name|memcpy
argument_list|(
name|has_sha1
index|[
name|nr
index|]
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|nr_has
operator|=
name|nr
operator|+
literal|1
expr_stmt|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|get_common_commits
specifier|static
name|int
name|get_common_commits
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
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|len
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
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
operator|!
name|len
condition|)
block|{
name|packet_write
argument_list|(
literal|1
argument_list|,
literal|"NAK\n"
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|len
operator|=
name|strip
argument_list|(
name|line
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|line
argument_list|,
literal|"have "
argument_list|,
literal|5
argument_list|)
condition|)
block|{
if|if
condition|(
name|got_sha1
argument_list|(
name|line
operator|+
literal|5
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|packet_write
argument_list|(
literal|1
argument_list|,
literal|"ACK %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
break|break;
block|}
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|line
argument_list|,
literal|"done"
argument_list|)
condition|)
block|{
name|packet_write
argument_list|(
literal|1
argument_list|,
literal|"NAK\n"
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|die
argument_list|(
literal|"git-upload-pack: expected SHA1 list, got '%s'"
argument_list|,
name|line
argument_list|)
expr_stmt|;
block|}
for|for
control|(
init|;
condition|;
control|)
block|{
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
operator|!
name|len
condition|)
continue|continue;
name|len
operator|=
name|strip
argument_list|(
name|line
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|line
argument_list|,
literal|"have "
argument_list|,
literal|5
argument_list|)
condition|)
block|{
name|got_sha1
argument_list|(
name|line
operator|+
literal|5
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|line
argument_list|,
literal|"done"
argument_list|)
condition|)
break|break;
name|die
argument_list|(
literal|"git-upload-pack: expected SHA1 list, got '%s'"
argument_list|,
name|line
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|receive_needs
specifier|static
name|int
name|receive_needs
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
decl_stmt|,
name|needs
decl_stmt|;
name|needs
operator|=
literal|0
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|unsigned
name|char
name|dummy
index|[
literal|20
index|]
decl_stmt|,
modifier|*
name|sha1_buf
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
operator|!
name|len
condition|)
return|return
name|needs
return|;
name|sha1_buf
operator|=
name|dummy
expr_stmt|;
if|if
condition|(
name|needs
operator|==
name|MAX_NEEDS
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"warning: supporting only a max of %d requests. "
literal|"sending everything instead.\n"
argument_list|,
name|MAX_NEEDS
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|needs
operator|<
name|MAX_NEEDS
condition|)
name|sha1_buf
operator|=
name|needs_sha1
index|[
name|needs
index|]
expr_stmt|;
if|if
condition|(
name|strncmp
argument_list|(
literal|"want "
argument_list|,
name|line
argument_list|,
literal|5
argument_list|)
operator|||
name|get_sha1_hex
argument_list|(
name|line
operator|+
literal|5
argument_list|,
name|sha1_buf
argument_list|)
condition|)
name|die
argument_list|(
literal|"git-upload-pack: protocol error, "
literal|"expected to get sha, not '%s'"
argument_list|,
name|line
argument_list|)
expr_stmt|;
name|needs
operator|++
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|send_ref
specifier|static
name|int
name|send_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|refname
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|o
init|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|packet_write
argument_list|(
literal|1
argument_list|,
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|refname
argument_list|)
expr_stmt|;
if|if
condition|(
name|o
operator|->
name|type
operator|==
name|tag_type
condition|)
block|{
name|o
operator|=
name|deref_tag
argument_list|(
name|o
argument_list|)
expr_stmt|;
name|packet_write
argument_list|(
literal|1
argument_list|,
literal|"%s %s^{}\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|o
operator|->
name|sha1
argument_list|)
argument_list|,
name|refname
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|upload_pack
specifier|static
name|int
name|upload_pack
parameter_list|(
name|void
parameter_list|)
block|{
name|head_ref
argument_list|(
name|send_ref
argument_list|)
expr_stmt|;
name|for_each_ref
argument_list|(
name|send_ref
argument_list|)
expr_stmt|;
name|packet_flush
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|nr_needs
operator|=
name|receive_needs
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|nr_needs
condition|)
return|return
literal|0
return|;
name|get_common_commits
argument_list|()
expr_stmt|;
name|create_pack_file
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
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
specifier|const
name|char
modifier|*
name|dir
decl_stmt|;
if|if
condition|(
name|argc
operator|!=
literal|2
condition|)
name|usage
argument_list|(
name|upload_pack_usage
argument_list|)
expr_stmt|;
name|dir
operator|=
name|argv
index|[
literal|1
index|]
expr_stmt|;
comment|/* chdir to the directory. If that fails, try appending ".git" */
if|if
condition|(
name|chdir
argument_list|(
name|dir
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|chdir
argument_list|(
name|mkpath
argument_list|(
literal|"%s.git"
argument_list|,
name|dir
argument_list|)
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"git-upload-pack unable to chdir to %s"
argument_list|,
name|dir
argument_list|)
expr_stmt|;
block|}
name|chdir
argument_list|(
literal|".git"
argument_list|)
expr_stmt|;
if|if
condition|(
name|access
argument_list|(
literal|"objects"
argument_list|,
name|X_OK
argument_list|)
operator|||
name|access
argument_list|(
literal|"refs"
argument_list|,
name|X_OK
argument_list|)
condition|)
name|die
argument_list|(
literal|"git-upload-pack: %s doesn't seem to be a git archive"
argument_list|,
name|dir
argument_list|)
expr_stmt|;
name|putenv
argument_list|(
literal|"GIT_DIR=."
argument_list|)
expr_stmt|;
name|upload_pack
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
