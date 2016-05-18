begin_unit
begin_comment
comment|/*  * ident.c  *  * create git identifier lines of the form "name<email> date"  *  * Copyright (C) 2005 Linus Torvalds  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_decl_stmt
DECL|variable|git_default_name
specifier|static
name|struct
name|strbuf
name|git_default_name
init|=
name|STRBUF_INIT
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|git_default_email
specifier|static
name|struct
name|strbuf
name|git_default_email
init|=
name|STRBUF_INIT
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|git_default_date
specifier|static
name|struct
name|strbuf
name|git_default_date
init|=
name|STRBUF_INIT
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|default_email_is_bogus
specifier|static
name|int
name|default_email_is_bogus
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|default_name_is_bogus
specifier|static
name|int
name|default_name_is_bogus
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|ident_use_config_only
specifier|static
name|int
name|ident_use_config_only
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|IDENT_NAME_GIVEN
define|#
directive|define
name|IDENT_NAME_GIVEN
value|01
end_define
begin_define
DECL|macro|IDENT_MAIL_GIVEN
define|#
directive|define
name|IDENT_MAIL_GIVEN
value|02
end_define
begin_define
DECL|macro|IDENT_ALL_GIVEN
define|#
directive|define
name|IDENT_ALL_GIVEN
value|(IDENT_NAME_GIVEN|IDENT_MAIL_GIVEN)
end_define
begin_decl_stmt
DECL|variable|committer_ident_explicitly_given
specifier|static
name|int
name|committer_ident_explicitly_given
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|author_ident_explicitly_given
specifier|static
name|int
name|author_ident_explicitly_given
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|ident_config_given
specifier|static
name|int
name|ident_config_given
decl_stmt|;
end_decl_stmt
begin_ifdef
ifdef|#
directive|ifdef
name|NO_GECOS_IN_PWENT
end_ifdef
begin_define
DECL|macro|get_gecos
define|#
directive|define
name|get_gecos
parameter_list|(
name|ignored
parameter_list|)
value|"&"
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|get_gecos
define|#
directive|define
name|get_gecos
parameter_list|(
name|struct_passwd
parameter_list|)
value|((struct_passwd)->pw_gecos)
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|xgetpwuid_self
specifier|static
name|struct
name|passwd
modifier|*
name|xgetpwuid_self
parameter_list|(
name|int
modifier|*
name|is_bogus
parameter_list|)
block|{
name|struct
name|passwd
modifier|*
name|pw
decl_stmt|;
name|errno
operator|=
literal|0
expr_stmt|;
name|pw
operator|=
name|getpwuid
argument_list|(
name|getuid
argument_list|()
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pw
condition|)
block|{
specifier|static
name|struct
name|passwd
name|fallback
decl_stmt|;
name|fallback
operator|.
name|pw_name
operator|=
literal|"unknown"
expr_stmt|;
ifndef|#
directive|ifndef
name|NO_GECOS_IN_PWENT
name|fallback
operator|.
name|pw_gecos
operator|=
literal|"Unknown"
expr_stmt|;
endif|#
directive|endif
name|pw
operator|=
operator|&
name|fallback
expr_stmt|;
if|if
condition|(
name|is_bogus
condition|)
operator|*
name|is_bogus
operator|=
literal|1
expr_stmt|;
block|}
return|return
name|pw
return|;
block|}
end_function
begin_function
DECL|function|copy_gecos
specifier|static
name|void
name|copy_gecos
parameter_list|(
specifier|const
name|struct
name|passwd
modifier|*
name|w
parameter_list|,
name|struct
name|strbuf
modifier|*
name|name
parameter_list|)
block|{
name|char
modifier|*
name|src
decl_stmt|;
comment|/* Traditionally GECOS field had office phone numbers etc, separated 	 * with commas.  Also& stands for capitalized form of the login name. 	 */
for|for
control|(
name|src
operator|=
name|get_gecos
argument_list|(
name|w
argument_list|)
init|;
operator|*
name|src
operator|&&
operator|*
name|src
operator|!=
literal|','
condition|;
name|src
operator|++
control|)
block|{
name|int
name|ch
init|=
operator|*
name|src
decl_stmt|;
if|if
condition|(
name|ch
operator|!=
literal|'&'
condition|)
name|strbuf_addch
argument_list|(
name|name
argument_list|,
name|ch
argument_list|)
expr_stmt|;
else|else
block|{
comment|/* Sorry, Mr. McDonald... */
name|strbuf_addch
argument_list|(
name|name
argument_list|,
name|toupper
argument_list|(
operator|*
name|w
operator|->
name|pw_name
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
name|name
argument_list|,
name|w
operator|->
name|pw_name
operator|+
literal|1
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|add_mailname_host
specifier|static
name|int
name|add_mailname_host
parameter_list|(
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|)
block|{
name|FILE
modifier|*
name|mailname
decl_stmt|;
name|struct
name|strbuf
name|mailnamebuf
init|=
name|STRBUF_INIT
decl_stmt|;
name|mailname
operator|=
name|fopen
argument_list|(
literal|"/etc/mailname"
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|mailname
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|ENOENT
condition|)
name|warning
argument_list|(
literal|"cannot open /etc/mailname: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|mailnamebuf
argument_list|,
name|mailname
argument_list|)
operator|==
name|EOF
condition|)
block|{
if|if
condition|(
name|ferror
argument_list|(
name|mailname
argument_list|)
condition|)
name|warning
argument_list|(
literal|"cannot read /etc/mailname: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|mailnamebuf
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|mailname
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
comment|/* success! */
name|strbuf_addbuf
argument_list|(
name|buf
argument_list|,
operator|&
name|mailnamebuf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|mailnamebuf
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|mailname
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|canonical_name
specifier|static
name|int
name|canonical_name
parameter_list|(
specifier|const
name|char
modifier|*
name|host
parameter_list|,
name|struct
name|strbuf
modifier|*
name|out
parameter_list|)
block|{
name|int
name|status
init|=
operator|-
literal|1
decl_stmt|;
ifndef|#
directive|ifndef
name|NO_IPV6
name|struct
name|addrinfo
name|hints
decl_stmt|,
modifier|*
name|ai
decl_stmt|;
name|memset
argument_list|(
operator|&
name|hints
argument_list|,
literal|'\0'
argument_list|,
sizeof|sizeof
argument_list|(
name|hints
argument_list|)
argument_list|)
expr_stmt|;
name|hints
operator|.
name|ai_flags
operator|=
name|AI_CANONNAME
expr_stmt|;
if|if
condition|(
operator|!
name|getaddrinfo
argument_list|(
name|host
argument_list|,
name|NULL
argument_list|,
operator|&
name|hints
argument_list|,
operator|&
name|ai
argument_list|)
condition|)
block|{
if|if
condition|(
name|ai
operator|&&
name|strchr
argument_list|(
name|ai
operator|->
name|ai_canonname
argument_list|,
literal|'.'
argument_list|)
condition|)
block|{
name|strbuf_addstr
argument_list|(
name|out
argument_list|,
name|ai
operator|->
name|ai_canonname
argument_list|)
expr_stmt|;
name|status
operator|=
literal|0
expr_stmt|;
block|}
name|freeaddrinfo
argument_list|(
name|ai
argument_list|)
expr_stmt|;
block|}
else|#
directive|else
name|struct
name|hostent
modifier|*
name|he
init|=
name|gethostbyname
argument_list|(
name|host
argument_list|)
decl_stmt|;
if|if
condition|(
name|he
operator|&&
name|strchr
argument_list|(
name|he
operator|->
name|h_name
argument_list|,
literal|'.'
argument_list|)
condition|)
block|{
name|strbuf_addstr
argument_list|(
name|out
argument_list|,
name|he
operator|->
name|h_name
argument_list|)
expr_stmt|;
name|status
operator|=
literal|0
expr_stmt|;
block|}
endif|#
directive|endif
comment|/* NO_IPV6 */
return|return
name|status
return|;
block|}
end_function
begin_function
DECL|function|add_domainname
specifier|static
name|void
name|add_domainname
parameter_list|(
name|struct
name|strbuf
modifier|*
name|out
parameter_list|,
name|int
modifier|*
name|is_bogus
parameter_list|)
block|{
name|char
name|buf
index|[
literal|1024
index|]
decl_stmt|;
if|if
condition|(
name|gethostname
argument_list|(
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|)
condition|)
block|{
name|warning
argument_list|(
literal|"cannot get host name: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
name|out
argument_list|,
literal|"(none)"
argument_list|)
expr_stmt|;
operator|*
name|is_bogus
operator|=
literal|1
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|strchr
argument_list|(
name|buf
argument_list|,
literal|'.'
argument_list|)
condition|)
name|strbuf_addstr
argument_list|(
name|out
argument_list|,
name|buf
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|canonical_name
argument_list|(
name|buf
argument_list|,
name|out
argument_list|)
operator|<
literal|0
condition|)
block|{
name|strbuf_addf
argument_list|(
name|out
argument_list|,
literal|"%s.(none)"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
operator|*
name|is_bogus
operator|=
literal|1
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|copy_email
specifier|static
name|void
name|copy_email
parameter_list|(
specifier|const
name|struct
name|passwd
modifier|*
name|pw
parameter_list|,
name|struct
name|strbuf
modifier|*
name|email
parameter_list|,
name|int
modifier|*
name|is_bogus
parameter_list|)
block|{
comment|/* 	 * Make up a fake email address 	 * (name + '@' + hostname [+ '.' + domainname]) 	 */
name|strbuf_addstr
argument_list|(
name|email
argument_list|,
name|pw
operator|->
name|pw_name
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
name|email
argument_list|,
literal|'@'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|add_mailname_host
argument_list|(
name|email
argument_list|)
condition|)
return|return;
comment|/* read from "/etc/mailname" (Debian) */
name|add_domainname
argument_list|(
name|email
argument_list|,
name|is_bogus
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|ident_default_name
specifier|const
name|char
modifier|*
name|ident_default_name
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_default_name
operator|.
name|len
condition|)
block|{
name|copy_gecos
argument_list|(
name|xgetpwuid_self
argument_list|(
operator|&
name|default_name_is_bogus
argument_list|)
argument_list|,
operator|&
name|git_default_name
argument_list|)
expr_stmt|;
name|strbuf_trim
argument_list|(
operator|&
name|git_default_name
argument_list|)
expr_stmt|;
block|}
return|return
name|git_default_name
operator|.
name|buf
return|;
block|}
end_function
begin_function
DECL|function|ident_default_email
specifier|const
name|char
modifier|*
name|ident_default_email
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_default_email
operator|.
name|len
condition|)
block|{
specifier|const
name|char
modifier|*
name|email
init|=
name|getenv
argument_list|(
literal|"EMAIL"
argument_list|)
decl_stmt|;
if|if
condition|(
name|email
operator|&&
name|email
index|[
literal|0
index|]
condition|)
block|{
name|strbuf_addstr
argument_list|(
operator|&
name|git_default_email
argument_list|,
name|email
argument_list|)
expr_stmt|;
name|committer_ident_explicitly_given
operator||=
name|IDENT_MAIL_GIVEN
expr_stmt|;
name|author_ident_explicitly_given
operator||=
name|IDENT_MAIL_GIVEN
expr_stmt|;
block|}
else|else
name|copy_email
argument_list|(
name|xgetpwuid_self
argument_list|(
operator|&
name|default_email_is_bogus
argument_list|)
argument_list|,
operator|&
name|git_default_email
argument_list|,
operator|&
name|default_email_is_bogus
argument_list|)
expr_stmt|;
name|strbuf_trim
argument_list|(
operator|&
name|git_default_email
argument_list|)
expr_stmt|;
block|}
return|return
name|git_default_email
operator|.
name|buf
return|;
block|}
end_function
begin_function
DECL|function|ident_default_date
specifier|static
specifier|const
name|char
modifier|*
name|ident_default_date
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|git_default_date
operator|.
name|len
condition|)
name|datestamp
argument_list|(
operator|&
name|git_default_date
argument_list|)
expr_stmt|;
return|return
name|git_default_date
operator|.
name|buf
return|;
block|}
end_function
begin_function
DECL|function|crud
specifier|static
name|int
name|crud
parameter_list|(
name|unsigned
name|char
name|c
parameter_list|)
block|{
return|return
name|c
operator|<=
literal|32
operator|||
name|c
operator|==
literal|'.'
operator|||
name|c
operator|==
literal|','
operator|||
name|c
operator|==
literal|':'
operator|||
name|c
operator|==
literal|';'
operator|||
name|c
operator|==
literal|'<'
operator|||
name|c
operator|==
literal|'>'
operator|||
name|c
operator|==
literal|'"'
operator|||
name|c
operator|==
literal|'\\'
operator|||
name|c
operator|==
literal|'\''
return|;
block|}
end_function
begin_comment
comment|/*  * Copy over a string to the destination, but avoid special  * characters ('\n', '<' and '>') and remove crud at the end  */
end_comment
begin_function
DECL|function|strbuf_addstr_without_crud
specifier|static
name|void
name|strbuf_addstr_without_crud
parameter_list|(
name|struct
name|strbuf
modifier|*
name|sb
parameter_list|,
specifier|const
name|char
modifier|*
name|src
parameter_list|)
block|{
name|size_t
name|i
decl_stmt|,
name|len
decl_stmt|;
name|unsigned
name|char
name|c
decl_stmt|;
comment|/* Remove crud from the beginning.. */
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|src
operator|)
operator|!=
literal|0
condition|)
block|{
if|if
condition|(
operator|!
name|crud
argument_list|(
name|c
argument_list|)
condition|)
break|break;
name|src
operator|++
expr_stmt|;
block|}
comment|/* Remove crud from the end.. */
name|len
operator|=
name|strlen
argument_list|(
name|src
argument_list|)
expr_stmt|;
while|while
condition|(
name|len
operator|>
literal|0
condition|)
block|{
name|c
operator|=
name|src
index|[
name|len
operator|-
literal|1
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|crud
argument_list|(
name|c
argument_list|)
condition|)
break|break;
operator|--
name|len
expr_stmt|;
block|}
comment|/* 	 * Copy the rest to the buffer, but avoid the special 	 * characters '\n' '<' and '>' that act as delimiters on 	 * an identification line. We can only remove crud, never add it, 	 * so 'len' is our maximum. 	 */
name|strbuf_grow
argument_list|(
name|sb
argument_list|,
name|len
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
name|len
condition|;
name|i
operator|++
control|)
block|{
name|c
operator|=
operator|*
name|src
operator|++
expr_stmt|;
switch|switch
condition|(
name|c
condition|)
block|{
case|case
literal|'\n'
case|:
case|case
literal|'<'
case|:
case|case
literal|'>'
case|:
continue|continue;
block|}
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
operator|++
index|]
operator|=
name|c
expr_stmt|;
block|}
name|sb
operator|->
name|buf
index|[
name|sb
operator|->
name|len
index|]
operator|=
literal|'\0'
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Reverse of fmt_ident(); given an ident line, split the fields  * to allow the caller to parse it.  * Signal a success by returning 0, but date/tz fields of the result  * can still be NULL if the input line only has the name/email part  * (e.g. reading from a reflog entry).  */
end_comment
begin_function
DECL|function|split_ident_line
name|int
name|split_ident_line
parameter_list|(
name|struct
name|ident_split
modifier|*
name|split
parameter_list|,
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|int
name|len
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|cp
decl_stmt|;
name|size_t
name|span
decl_stmt|;
name|int
name|status
init|=
operator|-
literal|1
decl_stmt|;
name|memset
argument_list|(
name|split
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|split
argument_list|)
argument_list|)
expr_stmt|;
name|split
operator|->
name|name_begin
operator|=
name|line
expr_stmt|;
for|for
control|(
name|cp
operator|=
name|line
init|;
operator|*
name|cp
operator|&&
name|cp
operator|<
name|line
operator|+
name|len
condition|;
name|cp
operator|++
control|)
if|if
condition|(
operator|*
name|cp
operator|==
literal|'<'
condition|)
block|{
name|split
operator|->
name|mail_begin
operator|=
name|cp
operator|+
literal|1
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|split
operator|->
name|mail_begin
condition|)
return|return
name|status
return|;
for|for
control|(
name|cp
operator|=
name|split
operator|->
name|mail_begin
operator|-
literal|2
init|;
name|line
operator|<=
name|cp
condition|;
name|cp
operator|--
control|)
if|if
condition|(
operator|!
name|isspace
argument_list|(
operator|*
name|cp
argument_list|)
condition|)
block|{
name|split
operator|->
name|name_end
operator|=
name|cp
operator|+
literal|1
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|split
operator|->
name|name_end
condition|)
block|{
comment|/* no human readable name */
name|split
operator|->
name|name_end
operator|=
name|split
operator|->
name|name_begin
expr_stmt|;
block|}
for|for
control|(
name|cp
operator|=
name|split
operator|->
name|mail_begin
init|;
name|cp
operator|<
name|line
operator|+
name|len
condition|;
name|cp
operator|++
control|)
if|if
condition|(
operator|*
name|cp
operator|==
literal|'>'
condition|)
block|{
name|split
operator|->
name|mail_end
operator|=
name|cp
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|split
operator|->
name|mail_end
condition|)
return|return
name|status
return|;
comment|/* 	 * Look from the end-of-line to find the trailing ">" of the mail 	 * address, even though we should already know it as split->mail_end. 	 * This can help in cases of broken idents with an extra ">" somewhere 	 * in the email address.  Note that we are assuming the timestamp will 	 * never have a ">" in it. 	 * 	 * Note that we will always find some ">" before going off the front of 	 * the string, because will always hit the split->mail_end closing 	 * bracket. 	 */
for|for
control|(
name|cp
operator|=
name|line
operator|+
name|len
operator|-
literal|1
init|;
operator|*
name|cp
operator|!=
literal|'>'
condition|;
name|cp
operator|--
control|)
empty_stmt|;
for|for
control|(
name|cp
operator|=
name|cp
operator|+
literal|1
init|;
name|cp
operator|<
name|line
operator|+
name|len
operator|&&
name|isspace
argument_list|(
operator|*
name|cp
argument_list|)
condition|;
name|cp
operator|++
control|)
empty_stmt|;
if|if
condition|(
name|line
operator|+
name|len
operator|<=
name|cp
condition|)
goto|goto
name|person_only
goto|;
name|split
operator|->
name|date_begin
operator|=
name|cp
expr_stmt|;
name|span
operator|=
name|strspn
argument_list|(
name|cp
argument_list|,
literal|"0123456789"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|span
condition|)
goto|goto
name|person_only
goto|;
name|split
operator|->
name|date_end
operator|=
name|split
operator|->
name|date_begin
operator|+
name|span
expr_stmt|;
for|for
control|(
name|cp
operator|=
name|split
operator|->
name|date_end
init|;
name|cp
operator|<
name|line
operator|+
name|len
operator|&&
name|isspace
argument_list|(
operator|*
name|cp
argument_list|)
condition|;
name|cp
operator|++
control|)
empty_stmt|;
if|if
condition|(
name|line
operator|+
name|len
operator|<=
name|cp
operator|||
operator|(
operator|*
name|cp
operator|!=
literal|'+'
operator|&&
operator|*
name|cp
operator|!=
literal|'-'
operator|)
condition|)
goto|goto
name|person_only
goto|;
name|split
operator|->
name|tz_begin
operator|=
name|cp
expr_stmt|;
name|span
operator|=
name|strspn
argument_list|(
name|cp
operator|+
literal|1
argument_list|,
literal|"0123456789"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|span
condition|)
goto|goto
name|person_only
goto|;
name|split
operator|->
name|tz_end
operator|=
name|split
operator|->
name|tz_begin
operator|+
literal|1
operator|+
name|span
expr_stmt|;
return|return
literal|0
return|;
name|person_only
label|:
name|split
operator|->
name|date_begin
operator|=
name|NULL
expr_stmt|;
name|split
operator|->
name|date_end
operator|=
name|NULL
expr_stmt|;
name|split
operator|->
name|tz_begin
operator|=
name|NULL
expr_stmt|;
name|split
operator|->
name|tz_end
operator|=
name|NULL
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|env_hint
specifier|static
specifier|const
name|char
modifier|*
name|env_hint
init|=
literal|"\n"
literal|"*** Please tell me who you are.\n"
literal|"\n"
literal|"Run\n"
literal|"\n"
literal|"  git config --global user.email \"you@example.com\"\n"
literal|"  git config --global user.name \"Your Name\"\n"
literal|"\n"
literal|"to set your account\'s default identity.\n"
literal|"Omit --global to set the identity only in this repository.\n"
literal|"\n"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|fmt_ident
specifier|const
name|char
modifier|*
name|fmt_ident
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|email
parameter_list|,
specifier|const
name|char
modifier|*
name|date_str
parameter_list|,
name|int
name|flag
parameter_list|)
block|{
specifier|static
name|struct
name|strbuf
name|ident
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|strict
init|=
operator|(
name|flag
operator|&
name|IDENT_STRICT
operator|)
decl_stmt|;
name|int
name|want_date
init|=
operator|!
operator|(
name|flag
operator|&
name|IDENT_NO_DATE
operator|)
decl_stmt|;
name|int
name|want_name
init|=
operator|!
operator|(
name|flag
operator|&
name|IDENT_NO_NAME
operator|)
decl_stmt|;
if|if
condition|(
name|want_name
condition|)
block|{
name|int
name|using_default
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|!
name|name
condition|)
block|{
if|if
condition|(
name|strict
operator|&&
name|ident_use_config_only
operator|&&
operator|!
operator|(
name|ident_config_given
operator|&
name|IDENT_NAME_GIVEN
operator|)
condition|)
block|{
name|fputs
argument_list|(
name|env_hint
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"no name was given and auto-detection is disabled"
argument_list|)
expr_stmt|;
block|}
name|name
operator|=
name|ident_default_name
argument_list|()
expr_stmt|;
name|using_default
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|strict
operator|&&
name|default_name_is_bogus
condition|)
block|{
name|fputs
argument_list|(
name|env_hint
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"unable to auto-detect name (got '%s')"
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
block|}
if|if
condition|(
operator|!
operator|*
name|name
condition|)
block|{
name|struct
name|passwd
modifier|*
name|pw
decl_stmt|;
if|if
condition|(
name|strict
condition|)
block|{
if|if
condition|(
name|using_default
condition|)
name|fputs
argument_list|(
name|env_hint
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"empty ident name (for<%s>) not allowed"
argument_list|,
name|email
argument_list|)
expr_stmt|;
block|}
name|pw
operator|=
name|xgetpwuid_self
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
name|name
operator|=
name|pw
operator|->
name|pw_name
expr_stmt|;
block|}
block|}
if|if
condition|(
operator|!
name|email
condition|)
block|{
if|if
condition|(
name|strict
operator|&&
name|ident_use_config_only
operator|&&
operator|!
operator|(
name|ident_config_given
operator|&
name|IDENT_MAIL_GIVEN
operator|)
condition|)
block|{
name|fputs
argument_list|(
name|env_hint
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"no email was given and auto-detection is disabled"
argument_list|)
expr_stmt|;
block|}
name|email
operator|=
name|ident_default_email
argument_list|()
expr_stmt|;
if|if
condition|(
name|strict
operator|&&
name|default_email_is_bogus
condition|)
block|{
name|fputs
argument_list|(
name|env_hint
argument_list|,
name|stderr
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"unable to auto-detect email address (got '%s')"
argument_list|,
name|email
argument_list|)
expr_stmt|;
block|}
block|}
name|strbuf_reset
argument_list|(
operator|&
name|ident
argument_list|)
expr_stmt|;
if|if
condition|(
name|want_name
condition|)
block|{
name|strbuf_addstr_without_crud
argument_list|(
operator|&
name|ident
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|ident
argument_list|,
literal|"<"
argument_list|)
expr_stmt|;
block|}
name|strbuf_addstr_without_crud
argument_list|(
operator|&
name|ident
argument_list|,
name|email
argument_list|)
expr_stmt|;
if|if
condition|(
name|want_name
condition|)
name|strbuf_addch
argument_list|(
operator|&
name|ident
argument_list|,
literal|'>'
argument_list|)
expr_stmt|;
if|if
condition|(
name|want_date
condition|)
block|{
name|strbuf_addch
argument_list|(
operator|&
name|ident
argument_list|,
literal|' '
argument_list|)
expr_stmt|;
if|if
condition|(
name|date_str
operator|&&
name|date_str
index|[
literal|0
index|]
condition|)
block|{
if|if
condition|(
name|parse_date
argument_list|(
name|date_str
argument_list|,
operator|&
name|ident
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"invalid date format: %s"
argument_list|,
name|date_str
argument_list|)
expr_stmt|;
block|}
else|else
name|strbuf_addstr
argument_list|(
operator|&
name|ident
argument_list|,
name|ident_default_date
argument_list|()
argument_list|)
expr_stmt|;
block|}
return|return
name|ident
operator|.
name|buf
return|;
block|}
end_function
begin_function
DECL|function|fmt_name
specifier|const
name|char
modifier|*
name|fmt_name
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|email
parameter_list|)
block|{
return|return
name|fmt_ident
argument_list|(
name|name
argument_list|,
name|email
argument_list|,
name|NULL
argument_list|,
name|IDENT_STRICT
operator||
name|IDENT_NO_DATE
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|git_author_info
specifier|const
name|char
modifier|*
name|git_author_info
parameter_list|(
name|int
name|flag
parameter_list|)
block|{
if|if
condition|(
name|getenv
argument_list|(
literal|"GIT_AUTHOR_NAME"
argument_list|)
condition|)
name|author_ident_explicitly_given
operator||=
name|IDENT_NAME_GIVEN
expr_stmt|;
if|if
condition|(
name|getenv
argument_list|(
literal|"GIT_AUTHOR_EMAIL"
argument_list|)
condition|)
name|author_ident_explicitly_given
operator||=
name|IDENT_MAIL_GIVEN
expr_stmt|;
return|return
name|fmt_ident
argument_list|(
name|getenv
argument_list|(
literal|"GIT_AUTHOR_NAME"
argument_list|)
argument_list|,
name|getenv
argument_list|(
literal|"GIT_AUTHOR_EMAIL"
argument_list|)
argument_list|,
name|getenv
argument_list|(
literal|"GIT_AUTHOR_DATE"
argument_list|)
argument_list|,
name|flag
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|git_committer_info
specifier|const
name|char
modifier|*
name|git_committer_info
parameter_list|(
name|int
name|flag
parameter_list|)
block|{
if|if
condition|(
name|getenv
argument_list|(
literal|"GIT_COMMITTER_NAME"
argument_list|)
condition|)
name|committer_ident_explicitly_given
operator||=
name|IDENT_NAME_GIVEN
expr_stmt|;
if|if
condition|(
name|getenv
argument_list|(
literal|"GIT_COMMITTER_EMAIL"
argument_list|)
condition|)
name|committer_ident_explicitly_given
operator||=
name|IDENT_MAIL_GIVEN
expr_stmt|;
return|return
name|fmt_ident
argument_list|(
name|getenv
argument_list|(
literal|"GIT_COMMITTER_NAME"
argument_list|)
argument_list|,
name|getenv
argument_list|(
literal|"GIT_COMMITTER_EMAIL"
argument_list|)
argument_list|,
name|getenv
argument_list|(
literal|"GIT_COMMITTER_DATE"
argument_list|)
argument_list|,
name|flag
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|ident_is_sufficient
specifier|static
name|int
name|ident_is_sufficient
parameter_list|(
name|int
name|user_ident_explicitly_given
parameter_list|)
block|{
ifndef|#
directive|ifndef
name|WINDOWS
return|return
operator|(
name|user_ident_explicitly_given
operator|&
name|IDENT_MAIL_GIVEN
operator|)
return|;
else|#
directive|else
return|return
operator|(
name|user_ident_explicitly_given
operator|==
name|IDENT_ALL_GIVEN
operator|)
return|;
endif|#
directive|endif
block|}
end_function
begin_function
DECL|function|committer_ident_sufficiently_given
name|int
name|committer_ident_sufficiently_given
parameter_list|(
name|void
parameter_list|)
block|{
return|return
name|ident_is_sufficient
argument_list|(
name|committer_ident_explicitly_given
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|author_ident_sufficiently_given
name|int
name|author_ident_sufficiently_given
parameter_list|(
name|void
parameter_list|)
block|{
return|return
name|ident_is_sufficient
argument_list|(
name|author_ident_explicitly_given
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|git_ident_config
name|int
name|git_ident_config
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
name|data
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|var
argument_list|,
literal|"user.useconfigonly"
argument_list|)
condition|)
block|{
name|ident_use_config_only
operator|=
name|git_config_bool
argument_list|(
name|var
argument_list|,
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
name|var
argument_list|,
literal|"user.name"
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
name|strbuf_reset
argument_list|(
operator|&
name|git_default_name
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|git_default_name
argument_list|,
name|value
argument_list|)
expr_stmt|;
name|committer_ident_explicitly_given
operator||=
name|IDENT_NAME_GIVEN
expr_stmt|;
name|author_ident_explicitly_given
operator||=
name|IDENT_NAME_GIVEN
expr_stmt|;
name|ident_config_given
operator||=
name|IDENT_NAME_GIVEN
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
name|var
argument_list|,
literal|"user.email"
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
name|strbuf_reset
argument_list|(
operator|&
name|git_default_email
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|git_default_email
argument_list|,
name|value
argument_list|)
expr_stmt|;
name|committer_ident_explicitly_given
operator||=
name|IDENT_MAIL_GIVEN
expr_stmt|;
name|author_ident_explicitly_given
operator||=
name|IDENT_MAIL_GIVEN
expr_stmt|;
name|ident_config_given
operator||=
name|IDENT_MAIL_GIVEN
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
DECL|function|buf_cmp
specifier|static
name|int
name|buf_cmp
parameter_list|(
specifier|const
name|char
modifier|*
name|a_begin
parameter_list|,
specifier|const
name|char
modifier|*
name|a_end
parameter_list|,
specifier|const
name|char
modifier|*
name|b_begin
parameter_list|,
specifier|const
name|char
modifier|*
name|b_end
parameter_list|)
block|{
name|int
name|a_len
init|=
name|a_end
operator|-
name|a_begin
decl_stmt|;
name|int
name|b_len
init|=
name|b_end
operator|-
name|b_begin
decl_stmt|;
name|int
name|min
init|=
name|a_len
operator|<
name|b_len
condition|?
name|a_len
else|:
name|b_len
decl_stmt|;
name|int
name|cmp
decl_stmt|;
name|cmp
operator|=
name|memcmp
argument_list|(
name|a_begin
argument_list|,
name|b_begin
argument_list|,
name|min
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmp
condition|)
return|return
name|cmp
return|;
return|return
name|a_len
operator|-
name|b_len
return|;
block|}
end_function
begin_function
DECL|function|ident_cmp
name|int
name|ident_cmp
parameter_list|(
specifier|const
name|struct
name|ident_split
modifier|*
name|a
parameter_list|,
specifier|const
name|struct
name|ident_split
modifier|*
name|b
parameter_list|)
block|{
name|int
name|cmp
decl_stmt|;
name|cmp
operator|=
name|buf_cmp
argument_list|(
name|a
operator|->
name|mail_begin
argument_list|,
name|a
operator|->
name|mail_end
argument_list|,
name|b
operator|->
name|mail_begin
argument_list|,
name|b
operator|->
name|mail_end
argument_list|)
expr_stmt|;
if|if
condition|(
name|cmp
condition|)
return|return
name|cmp
return|;
return|return
name|buf_cmp
argument_list|(
name|a
operator|->
name|name_begin
argument_list|,
name|a
operator|->
name|name_end
argument_list|,
name|b
operator|->
name|name_begin
argument_list|,
name|b
operator|->
name|name_end
argument_list|)
return|;
block|}
end_function
end_unit
