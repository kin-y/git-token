begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"urlmatch.h"
end_include
begin_define
DECL|macro|URL_ALPHA
define|#
directive|define
name|URL_ALPHA
value|"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
end_define
begin_define
DECL|macro|URL_DIGIT
define|#
directive|define
name|URL_DIGIT
value|"0123456789"
end_define
begin_define
DECL|macro|URL_ALPHADIGIT
define|#
directive|define
name|URL_ALPHADIGIT
value|URL_ALPHA URL_DIGIT
end_define
begin_define
DECL|macro|URL_SCHEME_CHARS
define|#
directive|define
name|URL_SCHEME_CHARS
value|URL_ALPHADIGIT "+.-"
end_define
begin_define
DECL|macro|URL_HOST_CHARS
define|#
directive|define
name|URL_HOST_CHARS
value|URL_ALPHADIGIT ".-[:]"
end_define
begin_comment
DECL|macro|URL_HOST_CHARS
comment|/* IPv6 literals need [:] */
end_comment
begin_define
DECL|macro|URL_UNSAFE_CHARS
define|#
directive|define
name|URL_UNSAFE_CHARS
value|"<>\"%{}|\\^`"
end_define
begin_comment
DECL|macro|URL_UNSAFE_CHARS
comment|/* plus 0x00-0x1F,0x7F-0xFF */
end_comment
begin_define
DECL|macro|URL_GEN_RESERVED
define|#
directive|define
name|URL_GEN_RESERVED
value|":/?#[]@"
end_define
begin_define
DECL|macro|URL_SUB_RESERVED
define|#
directive|define
name|URL_SUB_RESERVED
value|"!$&'()*+,;="
end_define
begin_define
DECL|macro|URL_RESERVED
define|#
directive|define
name|URL_RESERVED
value|URL_GEN_RESERVED URL_SUB_RESERVED
end_define
begin_comment
DECL|macro|URL_RESERVED
comment|/* only allowed delims */
end_comment
begin_function
DECL|function|append_normalized_escapes
specifier|static
name|int
name|append_normalized_escapes
parameter_list|(
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|,
specifier|const
name|char
modifier|*
name|from
parameter_list|,
name|size_t
name|from_len
parameter_list|,
specifier|const
name|char
modifier|*
name|esc_extra
parameter_list|,
specifier|const
name|char
modifier|*
name|esc_ok
parameter_list|)
block|{
comment|/* 	 * Append to strbuf 'buf' characters from string 'from' with length 	 * 'from_len' while unescaping characters that do not need to be escaped 	 * and escaping characters that do.  The set of characters to escape 	 * (the complement of which is unescaped) starts out as the RFC 3986 	 * unsafe characters (0x00-0x1F,0x7F-0xFF,"<>\"#%{}|\\^`").  If 	 * 'esc_extra' is not NULL, those additional characters will also always 	 * be escaped.  If 'esc_ok' is not NULL, those characters will be left 	 * escaped if found that way, but will not be unescaped otherwise (used 	 * for delimiters).  If a %-escape sequence is encountered that is not 	 * followed by 2 hexadecimal digits, the sequence is invalid and 	 * false (0) will be returned.  Otherwise true (1) will be returned for 	 * success. 	 * 	 * Note that all %-escape sequences will be normalized to UPPERCASE 	 * as indicated in RFC 3986.  Unless included in esc_extra or esc_ok 	 * alphanumerics and "-._~" will always be unescaped as per RFC 3986. 	 */
while|while
condition|(
name|from_len
condition|)
block|{
name|int
name|ch
init|=
operator|*
name|from
operator|++
decl_stmt|;
name|int
name|was_esc
init|=
literal|0
decl_stmt|;
name|from_len
operator|--
expr_stmt|;
if|if
condition|(
name|ch
operator|==
literal|'%'
condition|)
block|{
if|if
condition|(
name|from_len
operator|<
literal|2
operator|||
operator|!
name|isxdigit
argument_list|(
name|from
index|[
literal|0
index|]
argument_list|)
operator|||
operator|!
name|isxdigit
argument_list|(
name|from
index|[
literal|1
index|]
argument_list|)
condition|)
return|return
literal|0
return|;
name|ch
operator|=
name|hexval
argument_list|(
operator|*
name|from
operator|++
argument_list|)
operator|<<
literal|4
expr_stmt|;
name|ch
operator||=
name|hexval
argument_list|(
operator|*
name|from
operator|++
argument_list|)
expr_stmt|;
name|from_len
operator|-=
literal|2
expr_stmt|;
name|was_esc
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
operator|(
name|unsigned
name|char
operator|)
name|ch
operator|<=
literal|0x1F
operator|||
operator|(
name|unsigned
name|char
operator|)
name|ch
operator|>=
literal|0x7F
operator|||
name|strchr
argument_list|(
name|URL_UNSAFE_CHARS
argument_list|,
name|ch
argument_list|)
operator|||
operator|(
name|esc_extra
operator|&&
name|strchr
argument_list|(
name|esc_extra
argument_list|,
name|ch
argument_list|)
operator|)
operator|||
operator|(
name|was_esc
operator|&&
name|strchr
argument_list|(
name|esc_ok
argument_list|,
name|ch
argument_list|)
operator|)
condition|)
name|strbuf_addf
argument_list|(
name|buf
argument_list|,
literal|"%%%02X"
argument_list|,
operator|(
name|unsigned
name|char
operator|)
name|ch
argument_list|)
expr_stmt|;
else|else
name|strbuf_addch
argument_list|(
name|buf
argument_list|,
name|ch
argument_list|)
expr_stmt|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|url_normalize
name|char
modifier|*
name|url_normalize
parameter_list|(
specifier|const
name|char
modifier|*
name|url
parameter_list|,
name|struct
name|url_info
modifier|*
name|out_info
parameter_list|)
block|{
comment|/* 	 * Normalize NUL-terminated url using the following rules: 	 * 	 * 1. Case-insensitive parts of url will be converted to lower case 	 * 2. %-encoded characters that do not need to be will be unencoded 	 * 3. Characters that are not %-encoded and must be will be encoded 	 * 4. All %-encodings will be converted to upper case hexadecimal 	 * 5. Leading 0s are removed from port numbers 	 * 6. If the default port for the scheme is given it will be removed 	 * 7. A path part (including empty) not starting with '/' has one added 	 * 8. Any dot segments (. or ..) in the path are resolved and removed 	 * 9. IPv6 host literals are allowed (but not normalized or validated) 	 * 	 * The rules are based on information in RFC 3986. 	 * 	 * Please note this function requires a full URL including a scheme 	 * and host part (except for file: URLs which may have an empty host). 	 * 	 * The return value is a newly allocated string that must be freed 	 * or NULL if the url is not valid. 	 * 	 * If out_info is non-NULL, the url and err fields therein will always 	 * be set.  If a non-NULL value is returned, it will be stored in 	 * out_info->url as well, out_info->err will be set to NULL and the 	 * other fields of *out_info will also be filled in.  If a NULL value 	 * is returned, NULL will be stored in out_info->url and out_info->err 	 * will be set to a brief, translated, error message, but no other 	 * fields will be filled in. 	 * 	 * This is NOT a URL validation function.  Full URL validation is NOT 	 * performed.  Some invalid host names are passed through this function 	 * undetected.  However, most all other problems that make a URL invalid 	 * will be detected (including a missing host for non file: URLs). 	 */
name|size_t
name|url_len
init|=
name|strlen
argument_list|(
name|url
argument_list|)
decl_stmt|;
name|struct
name|strbuf
name|norm
decl_stmt|;
name|size_t
name|spanned
decl_stmt|;
name|size_t
name|scheme_len
decl_stmt|,
name|user_off
init|=
literal|0
decl_stmt|,
name|user_len
init|=
literal|0
decl_stmt|,
name|passwd_off
init|=
literal|0
decl_stmt|,
name|passwd_len
init|=
literal|0
decl_stmt|;
name|size_t
name|host_off
init|=
literal|0
decl_stmt|,
name|host_len
init|=
literal|0
decl_stmt|,
name|port_len
init|=
literal|0
decl_stmt|,
name|path_off
decl_stmt|,
name|path_len
decl_stmt|,
name|result_len
decl_stmt|;
specifier|const
name|char
modifier|*
name|slash_ptr
decl_stmt|,
modifier|*
name|at_ptr
decl_stmt|,
modifier|*
name|colon_ptr
decl_stmt|,
modifier|*
name|path_start
decl_stmt|;
name|char
modifier|*
name|result
decl_stmt|;
comment|/* 	 * Copy lowercased scheme and :// suffix, %-escapes are not allowed 	 * First character of scheme must be URL_ALPHA 	 */
name|spanned
operator|=
name|strspn
argument_list|(
name|url
argument_list|,
name|URL_SCHEME_CHARS
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|spanned
operator|||
operator|!
name|isalpha
argument_list|(
name|url
index|[
literal|0
index|]
argument_list|)
operator|||
name|spanned
operator|+
literal|3
operator|>
name|url_len
operator|||
name|url
index|[
name|spanned
index|]
operator|!=
literal|':'
operator|||
name|url
index|[
name|spanned
operator|+
literal|1
index|]
operator|!=
literal|'/'
operator|||
name|url
index|[
name|spanned
operator|+
literal|2
index|]
operator|!=
literal|'/'
condition|)
block|{
if|if
condition|(
name|out_info
condition|)
block|{
name|out_info
operator|->
name|url
operator|=
name|NULL
expr_stmt|;
name|out_info
operator|->
name|err
operator|=
name|_
argument_list|(
literal|"invalid URL scheme name or missing '://' suffix"
argument_list|)
expr_stmt|;
block|}
return|return
name|NULL
return|;
comment|/* Bad scheme and/or missing "://" part */
block|}
name|strbuf_init
argument_list|(
operator|&
name|norm
argument_list|,
name|url_len
argument_list|)
expr_stmt|;
name|scheme_len
operator|=
name|spanned
expr_stmt|;
name|spanned
operator|+=
literal|3
expr_stmt|;
name|url_len
operator|-=
name|spanned
expr_stmt|;
while|while
condition|(
name|spanned
operator|--
condition|)
name|strbuf_addch
argument_list|(
operator|&
name|norm
argument_list|,
name|tolower
argument_list|(
operator|*
name|url
operator|++
argument_list|)
argument_list|)
expr_stmt|;
comment|/* 	 * Copy any username:password if present normalizing %-escapes 	 */
name|at_ptr
operator|=
name|strchr
argument_list|(
name|url
argument_list|,
literal|'@'
argument_list|)
expr_stmt|;
name|slash_ptr
operator|=
name|url
operator|+
name|strcspn
argument_list|(
name|url
argument_list|,
literal|"/?#"
argument_list|)
expr_stmt|;
if|if
condition|(
name|at_ptr
operator|&&
name|at_ptr
operator|<
name|slash_ptr
condition|)
block|{
name|user_off
operator|=
name|norm
operator|.
name|len
expr_stmt|;
if|if
condition|(
name|at_ptr
operator|>
name|url
condition|)
block|{
if|if
condition|(
operator|!
name|append_normalized_escapes
argument_list|(
operator|&
name|norm
argument_list|,
name|url
argument_list|,
name|at_ptr
operator|-
name|url
argument_list|,
literal|""
argument_list|,
name|URL_RESERVED
argument_list|)
condition|)
block|{
if|if
condition|(
name|out_info
condition|)
block|{
name|out_info
operator|->
name|url
operator|=
name|NULL
expr_stmt|;
name|out_info
operator|->
name|err
operator|=
name|_
argument_list|(
literal|"invalid %XX escape sequence"
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|norm
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|colon_ptr
operator|=
name|strchr
argument_list|(
name|norm
operator|.
name|buf
operator|+
name|scheme_len
operator|+
literal|3
argument_list|,
literal|':'
argument_list|)
expr_stmt|;
if|if
condition|(
name|colon_ptr
condition|)
block|{
name|passwd_off
operator|=
operator|(
name|colon_ptr
operator|+
literal|1
operator|)
operator|-
name|norm
operator|.
name|buf
expr_stmt|;
name|passwd_len
operator|=
name|norm
operator|.
name|len
operator|-
name|passwd_off
expr_stmt|;
name|user_len
operator|=
operator|(
name|passwd_off
operator|-
literal|1
operator|)
operator|-
operator|(
name|scheme_len
operator|+
literal|3
operator|)
expr_stmt|;
block|}
else|else
block|{
name|user_len
operator|=
name|norm
operator|.
name|len
operator|-
operator|(
name|scheme_len
operator|+
literal|3
operator|)
expr_stmt|;
block|}
block|}
name|strbuf_addch
argument_list|(
operator|&
name|norm
argument_list|,
literal|'@'
argument_list|)
expr_stmt|;
name|url_len
operator|-=
operator|(
operator|++
name|at_ptr
operator|-
name|url
operator|)
expr_stmt|;
name|url
operator|=
name|at_ptr
expr_stmt|;
block|}
comment|/* 	 * Copy the host part excluding any port part, no %-escapes allowed 	 */
if|if
condition|(
operator|!
name|url_len
operator|||
name|strchr
argument_list|(
literal|":/?#"
argument_list|,
operator|*
name|url
argument_list|)
condition|)
block|{
comment|/* Missing host invalid for all URL schemes except file */
if|if
condition|(
name|strncmp
argument_list|(
name|norm
operator|.
name|buf
argument_list|,
literal|"file:"
argument_list|,
literal|5
argument_list|)
condition|)
block|{
if|if
condition|(
name|out_info
condition|)
block|{
name|out_info
operator|->
name|url
operator|=
name|NULL
expr_stmt|;
name|out_info
operator|->
name|err
operator|=
name|_
argument_list|(
literal|"missing host and scheme is not 'file:'"
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|norm
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
block|}
else|else
block|{
name|host_off
operator|=
name|norm
operator|.
name|len
expr_stmt|;
block|}
name|colon_ptr
operator|=
name|slash_ptr
operator|-
literal|1
expr_stmt|;
while|while
condition|(
name|colon_ptr
operator|>
name|url
operator|&&
operator|*
name|colon_ptr
operator|!=
literal|':'
operator|&&
operator|*
name|colon_ptr
operator|!=
literal|']'
condition|)
name|colon_ptr
operator|--
expr_stmt|;
if|if
condition|(
operator|*
name|colon_ptr
operator|!=
literal|':'
condition|)
block|{
name|colon_ptr
operator|=
name|slash_ptr
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|host_off
operator|&&
name|colon_ptr
operator|<
name|slash_ptr
operator|&&
name|colon_ptr
operator|+
literal|1
operator|!=
name|slash_ptr
condition|)
block|{
comment|/* file: URLs may not have a port number */
if|if
condition|(
name|out_info
condition|)
block|{
name|out_info
operator|->
name|url
operator|=
name|NULL
expr_stmt|;
name|out_info
operator|->
name|err
operator|=
name|_
argument_list|(
literal|"a 'file:' URL may not have a port number"
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|norm
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|spanned
operator|=
name|strspn
argument_list|(
name|url
argument_list|,
name|URL_HOST_CHARS
argument_list|)
expr_stmt|;
if|if
condition|(
name|spanned
operator|<
name|colon_ptr
operator|-
name|url
condition|)
block|{
comment|/* Host name has invalid characters */
if|if
condition|(
name|out_info
condition|)
block|{
name|out_info
operator|->
name|url
operator|=
name|NULL
expr_stmt|;
name|out_info
operator|->
name|err
operator|=
name|_
argument_list|(
literal|"invalid characters in host name"
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|norm
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
while|while
condition|(
name|url
operator|<
name|colon_ptr
condition|)
block|{
name|strbuf_addch
argument_list|(
operator|&
name|norm
argument_list|,
name|tolower
argument_list|(
operator|*
name|url
operator|++
argument_list|)
argument_list|)
expr_stmt|;
name|url_len
operator|--
expr_stmt|;
block|}
comment|/* 	 * Check the port part and copy if not the default (after removing any 	 * leading 0s); no %-escapes allowed 	 */
if|if
condition|(
name|colon_ptr
operator|<
name|slash_ptr
condition|)
block|{
comment|/* skip the ':' and leading 0s but not the last one if all 0s */
name|url
operator|++
expr_stmt|;
name|url
operator|+=
name|strspn
argument_list|(
name|url
argument_list|,
literal|"0"
argument_list|)
expr_stmt|;
if|if
condition|(
name|url
operator|==
name|slash_ptr
operator|&&
name|url
index|[
operator|-
literal|1
index|]
operator|==
literal|'0'
condition|)
name|url
operator|--
expr_stmt|;
if|if
condition|(
name|url
operator|==
name|slash_ptr
condition|)
block|{
comment|/* Skip ":" port with no number, it's same as default */
block|}
elseif|else
if|if
condition|(
name|slash_ptr
operator|-
name|url
operator|==
literal|2
operator|&&
operator|!
name|strncmp
argument_list|(
name|norm
operator|.
name|buf
argument_list|,
literal|"http:"
argument_list|,
literal|5
argument_list|)
operator|&&
operator|!
name|strncmp
argument_list|(
name|url
argument_list|,
literal|"80"
argument_list|,
literal|2
argument_list|)
condition|)
block|{
comment|/* Skip http :80 as it's the default */
block|}
elseif|else
if|if
condition|(
name|slash_ptr
operator|-
name|url
operator|==
literal|3
operator|&&
operator|!
name|strncmp
argument_list|(
name|norm
operator|.
name|buf
argument_list|,
literal|"https:"
argument_list|,
literal|6
argument_list|)
operator|&&
operator|!
name|strncmp
argument_list|(
name|url
argument_list|,
literal|"443"
argument_list|,
literal|3
argument_list|)
condition|)
block|{
comment|/* Skip https :443 as it's the default */
block|}
else|else
block|{
comment|/* 			 * Port number must be all digits with leading 0s removed 			 * and since all the protocols we deal with have a 16-bit 			 * port number it must also be in the range 1..65535 			 * 0 is not allowed because that means "next available" 			 * on just about every system and therefore cannot be used 			 */
name|unsigned
name|long
name|pnum
init|=
literal|0
decl_stmt|;
name|spanned
operator|=
name|strspn
argument_list|(
name|url
argument_list|,
name|URL_DIGIT
argument_list|)
expr_stmt|;
if|if
condition|(
name|spanned
operator|<
name|slash_ptr
operator|-
name|url
condition|)
block|{
comment|/* port number has invalid characters */
if|if
condition|(
name|out_info
condition|)
block|{
name|out_info
operator|->
name|url
operator|=
name|NULL
expr_stmt|;
name|out_info
operator|->
name|err
operator|=
name|_
argument_list|(
literal|"invalid port number"
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|norm
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
if|if
condition|(
name|slash_ptr
operator|-
name|url
operator|<=
literal|5
condition|)
name|pnum
operator|=
name|strtoul
argument_list|(
name|url
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
if|if
condition|(
name|pnum
operator|==
literal|0
operator|||
name|pnum
operator|>
literal|65535
condition|)
block|{
comment|/* port number not in range 1..65535 */
if|if
condition|(
name|out_info
condition|)
block|{
name|out_info
operator|->
name|url
operator|=
name|NULL
expr_stmt|;
name|out_info
operator|->
name|err
operator|=
name|_
argument_list|(
literal|"invalid port number"
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|norm
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|strbuf_addch
argument_list|(
operator|&
name|norm
argument_list|,
literal|':'
argument_list|)
expr_stmt|;
name|strbuf_add
argument_list|(
operator|&
name|norm
argument_list|,
name|url
argument_list|,
name|slash_ptr
operator|-
name|url
argument_list|)
expr_stmt|;
name|port_len
operator|=
name|slash_ptr
operator|-
name|url
expr_stmt|;
block|}
name|url_len
operator|-=
name|slash_ptr
operator|-
name|colon_ptr
expr_stmt|;
name|url
operator|=
name|slash_ptr
expr_stmt|;
block|}
if|if
condition|(
name|host_off
condition|)
name|host_len
operator|=
name|norm
operator|.
name|len
operator|-
name|host_off
expr_stmt|;
comment|/* 	 * Now copy the path resolving any . and .. segments being careful not 	 * to corrupt the URL by unescaping any delimiters, but do add an 	 * initial '/' if it's missing and do normalize any %-escape sequences. 	 */
name|path_off
operator|=
name|norm
operator|.
name|len
expr_stmt|;
name|path_start
operator|=
name|norm
operator|.
name|buf
operator|+
name|path_off
expr_stmt|;
name|strbuf_addch
argument_list|(
operator|&
name|norm
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|*
name|url
operator|==
literal|'/'
condition|)
block|{
name|url
operator|++
expr_stmt|;
name|url_len
operator|--
expr_stmt|;
block|}
for|for
control|(
init|;
condition|;
control|)
block|{
specifier|const
name|char
modifier|*
name|seg_start
decl_stmt|;
name|size_t
name|seg_start_off
init|=
name|norm
operator|.
name|len
decl_stmt|;
specifier|const
name|char
modifier|*
name|next_slash
init|=
name|url
operator|+
name|strcspn
argument_list|(
name|url
argument_list|,
literal|"/?#"
argument_list|)
decl_stmt|;
name|int
name|skip_add_slash
init|=
literal|0
decl_stmt|;
comment|/* 		 * RFC 3689 indicates that any . or .. segments should be 		 * unescaped before being checked for. 		 */
if|if
condition|(
operator|!
name|append_normalized_escapes
argument_list|(
operator|&
name|norm
argument_list|,
name|url
argument_list|,
name|next_slash
operator|-
name|url
argument_list|,
literal|""
argument_list|,
name|URL_RESERVED
argument_list|)
condition|)
block|{
if|if
condition|(
name|out_info
condition|)
block|{
name|out_info
operator|->
name|url
operator|=
name|NULL
expr_stmt|;
name|out_info
operator|->
name|err
operator|=
name|_
argument_list|(
literal|"invalid %XX escape sequence"
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|norm
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
name|seg_start
operator|=
name|norm
operator|.
name|buf
operator|+
name|seg_start_off
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|seg_start
argument_list|,
literal|"."
argument_list|)
condition|)
block|{
comment|/* ignore a . segment; be careful not to remove initial '/' */
if|if
condition|(
name|seg_start
operator|==
name|path_start
operator|+
literal|1
condition|)
block|{
name|strbuf_setlen
argument_list|(
operator|&
name|norm
argument_list|,
name|norm
operator|.
name|len
operator|-
literal|1
argument_list|)
expr_stmt|;
name|skip_add_slash
operator|=
literal|1
expr_stmt|;
block|}
else|else
block|{
name|strbuf_setlen
argument_list|(
operator|&
name|norm
argument_list|,
name|norm
operator|.
name|len
operator|-
literal|2
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|seg_start
argument_list|,
literal|".."
argument_list|)
condition|)
block|{
comment|/* 			 * ignore a .. segment and remove the previous segment; 			 * be careful not to remove initial '/' from path 			 */
specifier|const
name|char
modifier|*
name|prev_slash
init|=
name|norm
operator|.
name|buf
operator|+
name|norm
operator|.
name|len
operator|-
literal|3
decl_stmt|;
if|if
condition|(
name|prev_slash
operator|==
name|path_start
condition|)
block|{
comment|/* invalid .. because no previous segment to remove */
if|if
condition|(
name|out_info
condition|)
block|{
name|out_info
operator|->
name|url
operator|=
name|NULL
expr_stmt|;
name|out_info
operator|->
name|err
operator|=
name|_
argument_list|(
literal|"invalid '..' path segment"
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|norm
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
while|while
condition|(
operator|*
operator|--
name|prev_slash
operator|!=
literal|'/'
condition|)
block|{}
if|if
condition|(
name|prev_slash
operator|==
name|path_start
condition|)
block|{
name|strbuf_setlen
argument_list|(
operator|&
name|norm
argument_list|,
name|prev_slash
operator|-
name|norm
operator|.
name|buf
operator|+
literal|1
argument_list|)
expr_stmt|;
name|skip_add_slash
operator|=
literal|1
expr_stmt|;
block|}
else|else
block|{
name|strbuf_setlen
argument_list|(
operator|&
name|norm
argument_list|,
name|prev_slash
operator|-
name|norm
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
block|}
name|url_len
operator|-=
name|next_slash
operator|-
name|url
expr_stmt|;
name|url
operator|=
name|next_slash
expr_stmt|;
comment|/* if the next char is not '/' done with the path */
if|if
condition|(
operator|*
name|url
operator|!=
literal|'/'
condition|)
break|break;
name|url
operator|++
expr_stmt|;
name|url_len
operator|--
expr_stmt|;
if|if
condition|(
operator|!
name|skip_add_slash
condition|)
name|strbuf_addch
argument_list|(
operator|&
name|norm
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
block|}
name|path_len
operator|=
name|norm
operator|.
name|len
operator|-
name|path_off
expr_stmt|;
comment|/* 	 * Now simply copy the rest, if any, only normalizing %-escapes and 	 * being careful not to corrupt the URL by unescaping any delimiters. 	 */
if|if
condition|(
operator|*
name|url
condition|)
block|{
if|if
condition|(
operator|!
name|append_normalized_escapes
argument_list|(
operator|&
name|norm
argument_list|,
name|url
argument_list|,
name|url_len
argument_list|,
literal|""
argument_list|,
name|URL_RESERVED
argument_list|)
condition|)
block|{
if|if
condition|(
name|out_info
condition|)
block|{
name|out_info
operator|->
name|url
operator|=
name|NULL
expr_stmt|;
name|out_info
operator|->
name|err
operator|=
name|_
argument_list|(
literal|"invalid %XX escape sequence"
argument_list|)
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|norm
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
block|}
name|result
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|norm
argument_list|,
operator|&
name|result_len
argument_list|)
expr_stmt|;
if|if
condition|(
name|out_info
condition|)
block|{
name|out_info
operator|->
name|url
operator|=
name|result
expr_stmt|;
name|out_info
operator|->
name|err
operator|=
name|NULL
expr_stmt|;
name|out_info
operator|->
name|url_len
operator|=
name|result_len
expr_stmt|;
name|out_info
operator|->
name|scheme_len
operator|=
name|scheme_len
expr_stmt|;
name|out_info
operator|->
name|user_off
operator|=
name|user_off
expr_stmt|;
name|out_info
operator|->
name|user_len
operator|=
name|user_len
expr_stmt|;
name|out_info
operator|->
name|passwd_off
operator|=
name|passwd_off
expr_stmt|;
name|out_info
operator|->
name|passwd_len
operator|=
name|passwd_len
expr_stmt|;
name|out_info
operator|->
name|host_off
operator|=
name|host_off
expr_stmt|;
name|out_info
operator|->
name|host_len
operator|=
name|host_len
expr_stmt|;
name|out_info
operator|->
name|port_len
operator|=
name|port_len
expr_stmt|;
name|out_info
operator|->
name|path_off
operator|=
name|path_off
expr_stmt|;
name|out_info
operator|->
name|path_len
operator|=
name|path_len
expr_stmt|;
block|}
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|url_match_prefix
specifier|static
name|size_t
name|url_match_prefix
parameter_list|(
specifier|const
name|char
modifier|*
name|url
parameter_list|,
specifier|const
name|char
modifier|*
name|url_prefix
parameter_list|,
name|size_t
name|url_prefix_len
parameter_list|)
block|{
comment|/* 	 * url_prefix matches url if url_prefix is an exact match for url or it 	 * is a prefix of url and the match ends on a path component boundary. 	 * Both url and url_prefix are considered to have an implicit '/' on the 	 * end for matching purposes if they do not already. 	 * 	 * url must be NUL terminated.  url_prefix_len is the length of 	 * url_prefix which need not be NUL terminated. 	 * 	 * The return value is the length of the match in characters (including 	 * the final '/' even if it's implicit) or 0 for no match. 	 * 	 * Passing NULL as url and/or url_prefix will always cause 0 to be 	 * returned without causing any faults. 	 */
if|if
condition|(
operator|!
name|url
operator|||
operator|!
name|url_prefix
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|url_prefix_len
operator|||
operator|(
name|url_prefix_len
operator|==
literal|1
operator|&&
operator|*
name|url_prefix
operator|==
literal|'/'
operator|)
condition|)
return|return
operator|(
operator|!
operator|*
name|url
operator|||
operator|*
name|url
operator|==
literal|'/'
operator|)
condition|?
literal|1
else|:
literal|0
return|;
if|if
condition|(
name|url_prefix
index|[
name|url_prefix_len
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
name|url_prefix_len
operator|--
expr_stmt|;
if|if
condition|(
name|strncmp
argument_list|(
name|url
argument_list|,
name|url_prefix
argument_list|,
name|url_prefix_len
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|(
name|strlen
argument_list|(
name|url
argument_list|)
operator|==
name|url_prefix_len
operator|)
operator|||
operator|(
name|url
index|[
name|url_prefix_len
index|]
operator|==
literal|'/'
operator|)
condition|)
return|return
name|url_prefix_len
operator|+
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|match_urls
name|int
name|match_urls
parameter_list|(
specifier|const
name|struct
name|url_info
modifier|*
name|url
parameter_list|,
specifier|const
name|struct
name|url_info
modifier|*
name|url_prefix
parameter_list|,
name|int
modifier|*
name|exactusermatch
parameter_list|)
block|{
comment|/* 	 * url_prefix matches url if the scheme, host and port of url_prefix 	 * are the same as those of url and the path portion of url_prefix 	 * is the same as the path portion of url or it is a prefix that 	 * matches at a '/' boundary.  If url_prefix contains a user name, 	 * that must also exactly match the user name in url. 	 * 	 * If the user, host, port and path match in this fashion, the returned 	 * value is the length of the path match including any implicit 	 * final '/'.  For example, "http://me@example.com/path" is matched by 	 * "http://example.com" with a path length of 1. 	 * 	 * If there is a match and exactusermatch is not NULL, then 	 * *exactusermatch will be set to true if both url and url_prefix 	 * contained a user name or false if url_prefix did not have a 	 * user name.  If there is no match *exactusermatch is left untouched. 	 */
name|int
name|usermatched
init|=
literal|0
decl_stmt|;
name|int
name|pathmatchlen
decl_stmt|;
if|if
condition|(
operator|!
name|url
operator|||
operator|!
name|url_prefix
operator|||
operator|!
name|url
operator|->
name|url
operator|||
operator|!
name|url_prefix
operator|->
name|url
condition|)
return|return
literal|0
return|;
comment|/* check the scheme */
if|if
condition|(
name|url_prefix
operator|->
name|scheme_len
operator|!=
name|url
operator|->
name|scheme_len
operator|||
name|strncmp
argument_list|(
name|url
operator|->
name|url
argument_list|,
name|url_prefix
operator|->
name|url
argument_list|,
name|url
operator|->
name|scheme_len
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* schemes do not match */
comment|/* check the user name if url_prefix has one */
if|if
condition|(
name|url_prefix
operator|->
name|user_off
condition|)
block|{
if|if
condition|(
operator|!
name|url
operator|->
name|user_off
operator|||
name|url
operator|->
name|user_len
operator|!=
name|url_prefix
operator|->
name|user_len
operator|||
name|strncmp
argument_list|(
name|url
operator|->
name|url
operator|+
name|url
operator|->
name|user_off
argument_list|,
name|url_prefix
operator|->
name|url
operator|+
name|url_prefix
operator|->
name|user_off
argument_list|,
name|url
operator|->
name|user_len
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* url_prefix has a user but it's not a match */
name|usermatched
operator|=
literal|1
expr_stmt|;
block|}
comment|/* check the host and port */
if|if
condition|(
name|url_prefix
operator|->
name|host_len
operator|!=
name|url
operator|->
name|host_len
operator|||
name|strncmp
argument_list|(
name|url
operator|->
name|url
operator|+
name|url
operator|->
name|host_off
argument_list|,
name|url_prefix
operator|->
name|url
operator|+
name|url_prefix
operator|->
name|host_off
argument_list|,
name|url
operator|->
name|host_len
argument_list|)
condition|)
return|return
literal|0
return|;
comment|/* host names and/or ports do not match */
comment|/* check the path */
name|pathmatchlen
operator|=
name|url_match_prefix
argument_list|(
name|url
operator|->
name|url
operator|+
name|url
operator|->
name|path_off
argument_list|,
name|url_prefix
operator|->
name|url
operator|+
name|url_prefix
operator|->
name|path_off
argument_list|,
name|url_prefix
operator|->
name|url_len
operator|-
name|url_prefix
operator|->
name|path_off
argument_list|)
expr_stmt|;
if|if
condition|(
name|pathmatchlen
operator|&&
name|exactusermatch
condition|)
operator|*
name|exactusermatch
operator|=
name|usermatched
expr_stmt|;
return|return
name|pathmatchlen
return|;
block|}
end_function
begin_function
DECL|function|urlmatch_config_entry
name|int
name|urlmatch_config_entry
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
name|string_list_item
modifier|*
name|item
decl_stmt|;
name|struct
name|urlmatch_config
modifier|*
name|collect
init|=
name|cb
decl_stmt|;
name|struct
name|urlmatch_item
modifier|*
name|matched
decl_stmt|;
name|struct
name|url_info
modifier|*
name|url
init|=
operator|&
name|collect
operator|->
name|url
decl_stmt|;
specifier|const
name|char
modifier|*
name|key
decl_stmt|,
modifier|*
name|dot
decl_stmt|;
name|struct
name|strbuf
name|synthkey
init|=
name|STRBUF_INIT
decl_stmt|;
name|size_t
name|matched_len
init|=
literal|0
decl_stmt|;
name|int
name|user_matched
init|=
literal|0
decl_stmt|;
name|int
name|retval
decl_stmt|;
if|if
condition|(
operator|!
name|skip_prefix
argument_list|(
name|var
argument_list|,
name|collect
operator|->
name|section
argument_list|,
operator|&
name|key
argument_list|)
operator|||
operator|*
operator|(
name|key
operator|++
operator|)
operator|!=
literal|'.'
condition|)
block|{
if|if
condition|(
name|collect
operator|->
name|cascade_fn
condition|)
return|return
name|collect
operator|->
name|cascade_fn
argument_list|(
name|var
argument_list|,
name|value
argument_list|,
name|cb
argument_list|)
return|;
return|return
literal|0
return|;
comment|/* not interested */
block|}
name|dot
operator|=
name|strrchr
argument_list|(
name|key
argument_list|,
literal|'.'
argument_list|)
expr_stmt|;
if|if
condition|(
name|dot
condition|)
block|{
name|char
modifier|*
name|config_url
decl_stmt|,
modifier|*
name|norm_url
decl_stmt|;
name|struct
name|url_info
name|norm_info
decl_stmt|;
name|config_url
operator|=
name|xmemdupz
argument_list|(
name|key
argument_list|,
name|dot
operator|-
name|key
argument_list|)
expr_stmt|;
name|norm_url
operator|=
name|url_normalize
argument_list|(
name|config_url
argument_list|,
operator|&
name|norm_info
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|config_url
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|norm_url
condition|)
return|return
literal|0
return|;
name|matched_len
operator|=
name|match_urls
argument_list|(
name|url
argument_list|,
operator|&
name|norm_info
argument_list|,
operator|&
name|user_matched
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|norm_url
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|matched_len
condition|)
return|return
literal|0
return|;
name|key
operator|=
name|dot
operator|+
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|collect
operator|->
name|key
operator|&&
name|strcmp
argument_list|(
name|key
argument_list|,
name|collect
operator|->
name|key
argument_list|)
condition|)
return|return
literal|0
return|;
name|item
operator|=
name|string_list_insert
argument_list|(
operator|&
name|collect
operator|->
name|vars
argument_list|,
name|key
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|item
operator|->
name|util
condition|)
block|{
name|matched
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|matched
argument_list|)
argument_list|)
expr_stmt|;
name|item
operator|->
name|util
operator|=
name|matched
expr_stmt|;
block|}
else|else
block|{
name|matched
operator|=
name|item
operator|->
name|util
expr_stmt|;
comment|/* 		 * Is our match shorter?  Is our match the same 		 * length, and without user while the current 		 * candidate is with user?  Then we cannot use it. 		 */
if|if
condition|(
name|matched_len
operator|<
name|matched
operator|->
name|matched_len
operator|||
operator|(
operator|(
name|matched_len
operator|==
name|matched
operator|->
name|matched_len
operator|)
operator|&&
operator|(
operator|!
name|user_matched
operator|&&
name|matched
operator|->
name|user_matched
operator|)
operator|)
condition|)
return|return
literal|0
return|;
comment|/* Otherwise, replace it with this one. */
block|}
name|matched
operator|->
name|matched_len
operator|=
name|matched_len
expr_stmt|;
name|matched
operator|->
name|user_matched
operator|=
name|user_matched
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|synthkey
argument_list|,
name|collect
operator|->
name|section
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
operator|&
name|synthkey
argument_list|,
literal|'.'
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|synthkey
argument_list|,
name|key
argument_list|)
expr_stmt|;
name|retval
operator|=
name|collect
operator|->
name|collect_fn
argument_list|(
name|synthkey
operator|.
name|buf
argument_list|,
name|value
argument_list|,
name|collect
operator|->
name|cb
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|synthkey
argument_list|)
expr_stmt|;
return|return
name|retval
return|;
block|}
end_function
end_unit
