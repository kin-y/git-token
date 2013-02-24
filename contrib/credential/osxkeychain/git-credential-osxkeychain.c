begin_unit
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_include
include|#
directive|include
file|<Security/Security.h>
end_include
begin_decl_stmt
DECL|variable|protocol
specifier|static
name|SecProtocolType
name|protocol
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|host
specifier|static
name|char
modifier|*
name|host
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|path
specifier|static
name|char
modifier|*
name|path
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|username
specifier|static
name|char
modifier|*
name|username
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|password
specifier|static
name|char
modifier|*
name|password
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|port
specifier|static
name|UInt16
name|port
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|die
specifier|static
name|void
name|die
parameter_list|(
specifier|const
name|char
modifier|*
name|err
parameter_list|,
modifier|...
parameter_list|)
block|{
name|char
name|msg
index|[
literal|4096
index|]
decl_stmt|;
name|va_list
name|params
decl_stmt|;
name|va_start
argument_list|(
name|params
argument_list|,
name|err
argument_list|)
expr_stmt|;
name|vsnprintf
argument_list|(
name|msg
argument_list|,
sizeof|sizeof
argument_list|(
name|msg
argument_list|)
argument_list|,
name|err
argument_list|,
name|params
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%s\n"
argument_list|,
name|msg
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|params
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|xstrdup
specifier|static
name|void
modifier|*
name|xstrdup
parameter_list|(
specifier|const
name|char
modifier|*
name|s1
parameter_list|)
block|{
name|void
modifier|*
name|ret
init|=
name|strdup
argument_list|(
name|s1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory"
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_define
DECL|macro|KEYCHAIN_ITEM
define|#
directive|define
name|KEYCHAIN_ITEM
parameter_list|(
name|x
parameter_list|)
value|(x ? strlen(x) : 0), x
end_define
begin_define
DECL|macro|KEYCHAIN_ARGS
define|#
directive|define
name|KEYCHAIN_ARGS
define|\
value|NULL,
comment|/* default keychain */
value|\ 	KEYCHAIN_ITEM(host), \ 	0, NULL,
comment|/* account domain */
value|\ 	KEYCHAIN_ITEM(username), \ 	KEYCHAIN_ITEM(path), \ 	port, \ 	protocol, \ 	kSecAuthenticationTypeDefault
end_define
begin_function
DECL|function|write_item
specifier|static
name|void
name|write_item
parameter_list|(
specifier|const
name|char
modifier|*
name|what
parameter_list|,
specifier|const
name|char
modifier|*
name|buf
parameter_list|,
name|int
name|len
parameter_list|)
block|{
name|printf
argument_list|(
literal|"%s="
argument_list|,
name|what
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
name|buf
argument_list|,
literal|1
argument_list|,
name|len
argument_list|,
name|stdout
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
DECL|function|find_username_in_item
specifier|static
name|void
name|find_username_in_item
parameter_list|(
name|SecKeychainItemRef
name|item
parameter_list|)
block|{
name|SecKeychainAttributeList
name|list
decl_stmt|;
name|SecKeychainAttribute
name|attr
decl_stmt|;
name|list
operator|.
name|count
operator|=
literal|1
expr_stmt|;
name|list
operator|.
name|attr
operator|=
operator|&
name|attr
expr_stmt|;
name|attr
operator|.
name|tag
operator|=
name|kSecAccountItemAttr
expr_stmt|;
if|if
condition|(
name|SecKeychainItemCopyContent
argument_list|(
name|item
argument_list|,
name|NULL
argument_list|,
operator|&
name|list
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
condition|)
return|return;
name|write_item
argument_list|(
literal|"username"
argument_list|,
name|attr
operator|.
name|data
argument_list|,
name|attr
operator|.
name|length
argument_list|)
expr_stmt|;
name|SecKeychainItemFreeContent
argument_list|(
operator|&
name|list
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|find_internet_password
specifier|static
name|void
name|find_internet_password
parameter_list|(
name|void
parameter_list|)
block|{
name|void
modifier|*
name|buf
decl_stmt|;
name|UInt32
name|len
decl_stmt|;
name|SecKeychainItemRef
name|item
decl_stmt|;
if|if
condition|(
name|SecKeychainFindInternetPassword
argument_list|(
name|KEYCHAIN_ARGS
argument_list|,
operator|&
name|len
argument_list|,
operator|&
name|buf
argument_list|,
operator|&
name|item
argument_list|)
condition|)
return|return;
name|write_item
argument_list|(
literal|"password"
argument_list|,
name|buf
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|username
condition|)
name|find_username_in_item
argument_list|(
name|item
argument_list|)
expr_stmt|;
name|SecKeychainItemFreeContent
argument_list|(
name|NULL
argument_list|,
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|delete_internet_password
specifier|static
name|void
name|delete_internet_password
parameter_list|(
name|void
parameter_list|)
block|{
name|SecKeychainItemRef
name|item
decl_stmt|;
comment|/* 	 * Require at least a protocol and host for removal, which is what git 	 * will give us; if you want to do something more fancy, use the 	 * Keychain manager. 	 */
if|if
condition|(
operator|!
name|protocol
operator|||
operator|!
name|host
condition|)
return|return;
if|if
condition|(
name|SecKeychainFindInternetPassword
argument_list|(
name|KEYCHAIN_ARGS
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|,
operator|&
name|item
argument_list|)
condition|)
return|return;
name|SecKeychainItemDelete
argument_list|(
name|item
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|add_internet_password
specifier|static
name|void
name|add_internet_password
parameter_list|(
name|void
parameter_list|)
block|{
comment|/* Only store complete credentials */
if|if
condition|(
operator|!
name|protocol
operator|||
operator|!
name|host
operator|||
operator|!
name|username
operator|||
operator|!
name|password
condition|)
return|return;
if|if
condition|(
name|SecKeychainAddInternetPassword
argument_list|(
name|KEYCHAIN_ARGS
argument_list|,
name|KEYCHAIN_ITEM
argument_list|(
name|password
argument_list|)
argument_list|,
name|NULL
argument_list|)
condition|)
return|return;
block|}
end_function
begin_function
DECL|function|read_credential
specifier|static
name|void
name|read_credential
parameter_list|(
name|void
parameter_list|)
block|{
name|char
name|buf
index|[
literal|1024
index|]
decl_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|,
name|stdin
argument_list|)
condition|)
block|{
name|char
modifier|*
name|v
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"\n"
argument_list|)
condition|)
break|break;
name|buf
index|[
name|strlen
argument_list|(
name|buf
argument_list|)
operator|-
literal|1
index|]
operator|=
literal|'\0'
expr_stmt|;
name|v
operator|=
name|strchr
argument_list|(
name|buf
argument_list|,
literal|'='
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|v
condition|)
name|die
argument_list|(
literal|"bad input: %s"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
operator|*
name|v
operator|++
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"protocol"
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|v
argument_list|,
literal|"https"
argument_list|)
condition|)
name|protocol
operator|=
name|kSecProtocolTypeHTTPS
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|v
argument_list|,
literal|"http"
argument_list|)
condition|)
name|protocol
operator|=
name|kSecProtocolTypeHTTP
expr_stmt|;
else|else
comment|/* we don't yet handle other protocols */
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"host"
argument_list|)
condition|)
block|{
name|char
modifier|*
name|colon
init|=
name|strchr
argument_list|(
name|v
argument_list|,
literal|':'
argument_list|)
decl_stmt|;
if|if
condition|(
name|colon
condition|)
block|{
operator|*
name|colon
operator|++
operator|=
literal|'\0'
expr_stmt|;
name|port
operator|=
name|atoi
argument_list|(
name|colon
argument_list|)
expr_stmt|;
block|}
name|host
operator|=
name|xstrdup
argument_list|(
name|v
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"path"
argument_list|)
condition|)
name|path
operator|=
name|xstrdup
argument_list|(
name|v
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"username"
argument_list|)
condition|)
name|username
operator|=
name|xstrdup
argument_list|(
name|v
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"password"
argument_list|)
condition|)
name|password
operator|=
name|xstrdup
argument_list|(
name|v
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
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|usage
init|=
literal|"usage: git credential-osxkeychain<get|store|erase>"
decl_stmt|;
if|if
condition|(
operator|!
name|argv
index|[
literal|1
index|]
condition|)
name|die
argument_list|(
name|usage
argument_list|)
expr_stmt|;
name|read_credential
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"get"
argument_list|)
condition|)
name|find_internet_password
argument_list|()
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"store"
argument_list|)
condition|)
name|add_internet_password
argument_list|()
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"erase"
argument_list|)
condition|)
name|delete_internet_password
argument_list|()
expr_stmt|;
comment|/* otherwise, ignore unknown action */
return|return
literal|0
return|;
block|}
end_function
end_unit
